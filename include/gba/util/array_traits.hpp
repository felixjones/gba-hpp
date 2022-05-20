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

} // namespace gba::util

#endif // define GBAXX_UTIL_ARRAY_TRAITS_HPP
