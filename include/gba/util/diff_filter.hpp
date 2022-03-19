/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_UTIL_DIFF_FILTER_HPP
#define GBAXX_UTIL_DIFF_FILTER_HPP

#include <array>
#include <cstddef>
#include <type_traits>

#include <gba/bios/decompress.hpp>

namespace gba::util {

template <typename T, std::size_t Sz> requires std::is_integral_v<T> && (sizeof(T) == 1 || sizeof(T) == 2)
consteval auto diff_filter(const std::array<T, Sz>& data) noexcept {
    using signed_type = std::make_signed_t<T>;

    struct filter_type : bios::uncomp_header {
        std::array<signed_type, Sz> data;
    };

    filter_type filtered;
    filtered.type = sizeof(T) == 1 ? bios::uncomp_header::type::filter_8 : bios::uncomp_header::type::filter_16;
    filtered.length = sizeof(T) * Sz;

    filtered.data[0] = data[0];
    for (std::size_t ii = 1; ii < Sz; ++ii) {
        filtered.data[ii] = static_cast<signed_type>(data[ii] - data[ii - 1]);
    }
    return filtered;
}

} // namespace gba::util

#endif // define GBAXX_UTIL_DIFF_FILTER_HPP
