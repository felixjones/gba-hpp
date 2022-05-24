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

#include <gba/util/array_traits.hpp>

namespace gba::util {
namespace detail {

    template <typename>
    struct is_tuple : std::false_type{};

    template <typename... Ts>
    struct is_tuple<std::tuple<Ts...>> : std::true_type{};

} // namespace detail

template <typename T>
concept Tuple = detail::is_tuple<T>::value;

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

} // namespace detail

template <typename T, typename... Ts> requires Tuple<T>
constexpr auto tuple_split(Ts&&... elems) noexcept {
    static_assert(sizeof...(Ts) <= detail::tuple_elements<T>());

    auto result = T{};
    detail::tuple_split<0u>(result, elems...);
    return result;
}

template <typename... Ts> requires std::conjunction_v<detail::is_tuple<Ts>...>
using tuple_cat_type = decltype(std::tuple_cat(std::declval<Ts>()...));

namespace detail {

    template <class T, std::size_t N, std::size_t Begin, class Result = std::tuple<std::tuple_element_t<Begin, T>>, std::size_t Counter = 1u> requires Tuple<T>
    using tuple_slicer_type = std::conditional_t<Counter < N, tuple_cat_type<Result, std::tuple<std::tuple_element_t<Begin + Counter, T>>>, Result>;

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
    };

} // namespace detail

template <class T, std::size_t N, std::size_t Begin = 0u> requires Tuple<T> && (Begin + N <= std::tuple_size_v<T>) && (N >= 1)
using tuple_slice_type = detail::tuple_slicer_type<T, N, Begin>;

template <class T> requires Tuple<T> && (detail::tuple_elements_same<T>::value())
using tuple_to_array_type = std::array<std::tuple_element_t<0, T>, std::tuple_size_v<T>>;

} // namespace gba::util

#endif // define GBAXX_UTIL_TUPLE_TRAITS_HPP
