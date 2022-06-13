/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_UTIL_ARRAY_TRAITS_HPP
#define GBAXX_UTIL_ARRAY_TRAITS_HPP

#include <array>
#include <cstddef>
#include <type_traits>

namespace gba::util {
namespace detail {

    template <typename T, std::size_t N>
    auto array_size_stub(const std::array<T, N>&) -> std::integral_constant<std::size_t, N>;

    template <typename T, std::size_t N>
    auto array_size_stub(T(&)[N]) -> std::integral_constant<std::size_t, N>;

    template <typename T, std::size_t N>
    auto array_type_stub(const std::array<T, N>&) -> T;

    template <typename T, std::size_t N>
    auto array_type_stub(T(&)[N]) -> T;

    template <std::size_t... Indices>
    consteval auto fill_array(auto value, std::index_sequence<Indices...>) noexcept {
        return std::array<decltype(value), sizeof...(Indices)>{{(static_cast<void>(Indices), value)...}};
    }

} // namespace detail

template <class T>
constexpr auto array_size = decltype(detail::array_size_stub(std::declval<const T&>()))::value;

template <class T>
using array_value_type = decltype(detail::array_type_stub(std::declval<const T&>()));

template <class T>
concept Array = std::is_array_v<T> || std::is_same_v<T, std::array<array_value_type<T>, array_size<T>>>;

template <class T, class E>
concept ArrayOf = Array<T> && std::is_same_v<E, array_value_type<T>>;

namespace detail {

    template <class... Ts>
    consteval void join_into(auto iter, const Array auto& lhs, const Ts&... rhs) noexcept {
        iter = std::copy(std::cbegin(lhs), std::cend(lhs), iter);

        if constexpr (sizeof...(rhs) > 0) {
            join_into(iter, rhs...);
        }
    }

    template <std::size_t Size, class T, class... Ts>
    consteval auto arrays_match_size() noexcept requires Array<T> {
        if constexpr (array_size<T> != Size) {
            return false;
        } else if constexpr (sizeof...(Ts) > 0) {
            return arrays_match_size<Size, Ts...>();
        } else {
            return true;
        }
    }

    template <class T, class... Ts>
    constexpr static bool arrays_same_size = arrays_match_size<array_size<T>, T, Ts...>();

    template <class E, class T, class... Ts>
    consteval auto arrays_match_value_type() noexcept requires Array<T> {
        if constexpr (!std::is_same_v<array_value_type<T>, E>) {
            return false;
        } else if constexpr (sizeof...(Ts) > 0) {
            return arrays_match_value_type<E, Ts...>();
        } else {
            return true;
        }
    }

    template <class T, class... Ts>
    constexpr static bool arrays_same_value_type = arrays_match_value_type<array_value_type<T>, T, Ts...>();

    template <std::size_t Idx, typename First, typename... Rest>
    constexpr void array_split(Array auto& result, First&& first, Rest&&... rest) noexcept {
        using array_type = std::remove_reference_t<decltype(result)>;

        constexpr auto depth = array_size<array_value_type<array_type>>;

        std::get<Idx / depth>(result)[Idx % depth] = first;

        if constexpr (Idx + 1u < (depth * array_size<array_type>) && sizeof...(Rest) > 0) {
            array_split<Idx + 1u>(result, rest...);
        }
    }

} // namespace detail

template <std::size_t N>
consteval auto fill_array(auto value) noexcept {
    return detail::fill_array(value, std::make_index_sequence<N>());
}

template <class... Ts>
consteval auto join_array(const Array auto& lhs, const Ts&... rhs) noexcept {
    const auto size = (array_size<decltype(lhs)> + ... + array_size<Ts>);

    std::array<array_value_type<decltype(lhs)>, size> result{};
    detail::join_into(result.begin(), lhs, rhs...);
    return result;
}

consteval auto flatten_array(const Array auto& lhs) noexcept {
    using array_type = decltype(lhs);
    using value_type = array_value_type<decltype(lhs)>;

    if constexpr (!Array<value_type>) {
        return lhs;
    } else {
        auto result = std::array<array_value_type<value_type>, array_size<array_type> * array_size<value_type>>{};

        auto iter = result.begin();
        for (const auto& inner : lhs) {
            iter = std::copy(std::cbegin(inner), std::cend(inner), iter);
        }

        return result;
    }
}

template <class... Ts>
concept ArraysSameSize = detail::arrays_same_size<Ts...>;

template <class... Ts>
concept ArraysSameValueType = detail::arrays_same_value_type<Ts...>;

template <typename T, std::size_t N, typename... Ts>
constexpr auto array_split(Ts&&... elems) noexcept {
    auto result = std::array<std::array<T, N>, (sizeof...(Ts) + (N - 1)) / N>{};
    detail::array_split<0>(result, std::forward<Ts>(elems)...);
    return result;
}

template <class T, std::size_t N> requires Array<T>
using array_slice_type = std::array<array_value_type<T>, N>;

} // namespace gba::util

#endif // define GBAXX_UTIL_ARRAY_TRAITS_HPP
