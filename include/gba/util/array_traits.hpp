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


template <std::size_t N>
consteval auto fill_array(auto value) noexcept {
    return detail::fill_array(value, std::make_index_sequence<N>());
}

} // namespace gba::util

#endif // define GBAXX_UTIL_ARRAY_TRAITS_HPP
