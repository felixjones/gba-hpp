/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_UTIL_TUPLE_TRAITS_HPP
#define GBAXX_UTIL_TUPLE_TRAITS_HPP

#include <cstddef>
#include <tuple>
#include <type_traits>

#include <gba/tupletype.hpp>

#include <gba/util/array_traits.hpp>

namespace gba::util {
namespace detail {

    template <typename>
    struct is_tuple : std::false_type {};

    template <typename... Ts>
    struct is_tuple<tuple<Ts...>> : std::true_type {};

    template <typename... Ts>
    struct is_tuple<std::tuple<Ts...>> : std::true_type {};

    template <std::size_t, typename, typename, std::size_t>
    struct make_tuple_impl;

    template <std::size_t Idx, typename T, typename... Ts, std::size_t Num>
    struct make_tuple_impl<Idx, tuple<Ts...>, T, Num> : make_tuple_impl<Idx + 1, tuple<Ts..., std::tuple_element_t<Idx, T>>, T, Num> {};

    template <std::size_t Num, typename T, typename... Ts>
    struct make_tuple_impl<Num, tuple<Ts...>, T, Num> {
        using type = tuple<Ts...>;
    };

    template <typename T>
    struct make_tuple : make_tuple_impl<0, tuple<>, std::remove_cvref_t<T>, std::tuple_size<std::remove_cvref_t<T>>::value> {};

    template <typename...>
    struct combine_tuples;

    template <>
    struct combine_tuples<> {
        using type = tuple<>;
    };

    template <typename... Ts>
    struct combine_tuples<tuple<Ts...>> {
        using type = tuple<Ts...>;
    };

    template <typename... T1s, typename... T2s, typename... Rest>
    struct combine_tuples<tuple<T1s...>, tuple<T2s...>, Rest...> {
        using type = typename combine_tuples<tuple<T1s..., T2s...>, Rest...>::type;
    };

} // namespace detail

template <typename T>
concept Tuple = detail::is_tuple<T>::value;

template <typename... Ts> requires std::conjunction_v<detail::is_tuple<Ts>...>
using tuple_cat_type = typename detail::combine_tuples<typename detail::make_tuple<Ts>::type...>::type;

namespace detail {
    template <class T, std::size_t ElemIdx = 0u> requires Tuple<T> && Array<std::tuple_element_t<ElemIdx, T>>
    consteval auto tuple_elements() noexcept {
        constexpr auto element_size = array_size<std::tuple_element_t<ElemIdx, T>>;

        if constexpr (ElemIdx + 1u < std::tuple_size_v<T>) {
            return element_size + tuple_elements<T, ElemIdx + 1u>();
        } else {
            return element_size;
        }
    }

    template <class T, std::size_t Idx, std::size_t Count = 0u, std::size_t ElemIdx = 0u, std::size_t ArrIdx = 0u>
    consteval auto tuple_element_at_index() noexcept {
        if constexpr (Idx == Count) {
            return ElemIdx;
        } else {
            constexpr auto element_size = array_size<std::tuple_element_t<ElemIdx, T>>;

            if constexpr (ArrIdx + 1u == element_size) {
                return tuple_element_at_index<T, Idx, Count + 1u, ElemIdx + 1u>();
            } else {
                return tuple_element_at_index<T, Idx, Count + 1u, ElemIdx, ArrIdx + 1u>();
            }
        }
    }

    template <class T, std::size_t Idx, std::size_t Count = 0u, std::size_t ElemIdx = 0u, std::size_t ArrIdx = 0u>
    consteval auto tuple_array_index_at_index() noexcept {
        if constexpr (Idx == Count) {
            return ArrIdx;
        } else {
            constexpr auto element_size = array_size<std::tuple_element_t<ElemIdx, T>>;

            if constexpr (ArrIdx + 1u == element_size) {
                return tuple_array_index_at_index<T, Idx, Count + 1u, ElemIdx + 1u>();
            } else {
                return tuple_array_index_at_index<T, Idx, Count + 1u, ElemIdx, ArrIdx + 1u>();
            }
        }
    }

    template <std::size_t Idx, typename First, typename... Rest>
    constexpr void tuple_split(Tuple auto& result, First&& first, Rest&&... rest) noexcept {
        using tuple_type = std::remove_reference_t<decltype(result)>;

        constexpr auto tuple_element = tuple_element_at_index<tuple_type, Idx>();
        constexpr auto array_index = tuple_array_index_at_index<tuple_type, Idx>();

        std::get<tuple_element>(result)[array_index] = first;

        if constexpr (Idx + 1u < tuple_elements<tuple_type>() && sizeof...(Rest) > 0) {
            tuple_split<Idx + 1u>(result, rest...);
        }
    }

    template <class T, std::size_t N, std::size_t Begin, class Result = tuple<std::tuple_element_t<Begin, T>>, std::size_t Counter = 1u> requires Tuple<T>
    using tuple_slicer_type = std::conditional_t<Counter < N, tuple_cat_type<Result, tuple<std::tuple_element_t<Begin + Counter, T>>>, Result>;

    template <class T, std::size_t Counter = 0u> requires Tuple<T>
    struct tuple_elements_same {
        static consteval auto value() noexcept {
            constexpr auto same_as_first = std::is_same_v<std::tuple_element_t<0, T>, std::tuple_element_t<Counter, T>> ;

            if constexpr (same_as_first == false) {
                return false;
            } else if constexpr (Counter + 1u < std::tuple_size_v<T>) {
                return tuple_elements_same<T, Counter + 1u>::value();
            } else {
                return true;
            }
        }

        static consteval auto is_array() noexcept {
            if constexpr (!Array<std::tuple_element_t<Counter, T>>) {
                return false;
            } else if constexpr (Counter + 1u < std::tuple_size_v<T>) {
                return tuple_elements_same<T, Counter + 1u>::is_array();
            } else {
                return true;
            }
        }
    };

    template <class, typename...>
    struct tuple_from_array_type_impl {};

    template <class T, typename... Ts> requires (std::tuple_size_v<tuple<Ts...>> == array_size<T>)
    struct tuple_from_array_type_impl<T, tuple<Ts...>> {
        using type = tuple<Ts...>;
    };

    template <class T, typename... Ts> requires (std::tuple_size_v<tuple<Ts...>> < array_size<T>)
    struct tuple_from_array_type_impl<T, tuple<Ts...>> {
        using type = typename tuple_from_array_type_impl<T, tuple_cat_type<tuple<Ts...>, tuple<array_value_type<T>>>>::type;
    };

    template <typename>
    struct tuple_from_array_type {};

    template <typename... Ts> requires Tuple<tuple<Ts...>>
    struct tuple_from_array_type<tuple<Ts...>> {
        using type = tuple<Ts...>;
    };

    template <typename T, std::size_t N> requires Array<std::array<T, N>>
    struct tuple_from_array_type<std::array<T, N>> {
        using type = typename tuple_from_array_type_impl<std::array<T, N>, tuple<T>>::type;
    };

    template <typename>
    struct is_multi_dimensional {};

    template <typename... Ts> requires Tuple<tuple<Ts...>>
    struct is_multi_dimensional<tuple<Ts...>> {
        static constexpr auto value = detail::tuple_elements_same<tuple<Ts...>>::is_array();
    };

    template <typename T, std::size_t N> requires Array<std::array<T, N>>
    struct is_multi_dimensional<std::array<T, N>> {
        static constexpr auto value = Array<T>;
    };

} // namespace detail

template <typename T, typename... Ts> requires Tuple<T>
constexpr auto tuple_split(Ts&&... elems) noexcept {
    static_assert(sizeof...(Ts) <= detail::tuple_elements<T>());

    auto result = T{};
    detail::tuple_split<0u>(result, elems...);
    return result;
}

template <class T, std::size_t N, std::size_t Begin = 0u> requires Tuple<T> && (Begin + N <= std::tuple_size_v<T>) && (N >= 1)
using tuple_slice_type = detail::tuple_slicer_type<T, N, Begin>;

template <class T> requires Tuple<T> && (detail::tuple_elements_same<T>::value())
using tuple_to_array_type = std::array<std::tuple_element_t<0, T>, std::tuple_size_v<T>>;

template <class T>
using tuple_from_array_type = typename detail::tuple_from_array_type<T>::type;

template <class T> requires Tuple<T>
using tuple_normalize_type = std::conditional_t<detail::tuple_elements_same<T>::value(), std::array<std::tuple_element_t<0, T>, std::tuple_size_v<T>>, T>;

template <class T>
static constexpr auto is_multi_dimensional = detail::is_multi_dimensional<T>::value;

namespace detail {

    template <class, std::size_t, std::size_t>
    struct tuple_or_array_slice_type {};

    template <class T, std::size_t N, std::size_t Begin> requires Array<T>
    struct tuple_or_array_slice_type<T, N, Begin> {
        using type = array_slice_type<T, N>;
    };

    template <class T, std::size_t N, std::size_t Begin> requires Tuple<T>
    struct tuple_or_array_slice_type<T, N, Begin> {
        using type = tuple_slice_type<T, N, Begin>;
    };

    template <std::size_t, class>
    struct tuple_slice_inner_type {};

    template <std::size_t N, typename... Ts>
    struct tuple_slice_inner_type<N, tuple<Ts...>> {
        using type = tuple<array_slice_type<Ts, N>...>;
    };

} // namespace detail

template <class T, std::size_t N, std::size_t Begin = 0u>
using tuple_or_array_slice_type = typename detail::tuple_or_array_slice_type<T, N, Begin>::type;

template <class T, std::size_t N, std::size_t Begin = 0u> requires is_multi_dimensional<T>
using tuple_or_array_slice_inner_type = typename detail::tuple_slice_inner_type<N, tuple_from_array_type<T>>::type;

} // namespace gba::util

#endif // define GBAXX_UTIL_TUPLE_TRAITS_HPP
