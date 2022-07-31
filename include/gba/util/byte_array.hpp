/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_UTIL_BYTE_ARRAY_HPP
#define GBAXX_UTIL_BYTE_ARRAY_HPP

#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>

#include <gba/util/array_traits.hpp>

namespace gba::util {

template <typename T>
concept IntegerArray = requires {
    requires Array<T>;
    requires std::is_integral_v<array_value_type<T>>;
};

template <typename T>
concept ByteArray = requires {
    requires Array<T>;
    requires std::is_same_v<std::byte, array_value_type<T>>;
};

template <typename T>
concept StringLiteral = std::is_same_v<T,
        std::add_lvalue_reference_t<
            const char[std::extent_v<std::remove_reference_t<T>>]
        >
    >;

consteval auto to_char_array(StringLiteral auto&& data) noexcept {
    constexpr auto size = array_size<decltype(data)> - 1u;

    std::array<char, size> result{};
    std::copy_n(std::cbegin(data), size, std::begin(result));
    return result;
}

consteval ByteArray auto to_byte_array(const IntegerArray auto& data) noexcept {
    using value_type = array_value_type<decltype(data)>;
    constexpr auto size = array_size<decltype(data)>;
    constexpr auto sizeof_type = sizeof(value_type);

    std::array<std::byte, sizeof(value_type) * size> result {};
    std::size_t index = 0;

    for (std::size_t ii = 0; ii < size; ++ii) {
        auto span = data[ii];
        for (std::size_t jj = 0; jj < sizeof_type; ++jj) {
            result[index++] = std::byte(span & 0xff);
            span >>= 8;
        }
    }
    return result;
}

consteval ByteArray auto to_byte_array(std::integral auto data) noexcept {
    std::array<std::byte, sizeof(data)> result {};
    std::size_t index = 0;

    for (std::size_t ii = 0; ii < sizeof(data); ++ii) {
        result[index++] = std::byte(data & 0xff);
        data >>= 8;
    }
    return result;
}

} // namespace gba::util

#endif // define GBAXX_UTIL_BYTE_ARRAY_HPP
