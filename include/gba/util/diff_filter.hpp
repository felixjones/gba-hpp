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

#include <gba/inttype.hpp>

#include <gba/bios/decompress.hpp>

#include <gba/util/constexpr.hpp>
#include <gba/util/array_traits.hpp>

namespace gba::util {

template <class T>
concept DiffableArray = Array<T> && sizeof(array_value_type<T>) <= 2;

consteval auto diff_filter(const DiffableArray auto& data) noexcept {
    using array_type = decltype(data);
    using signed_type = std::make_signed_t<array_value_type<array_type>>;

    struct filter_type : bios::uncomp_header {
        std::array<signed_type, array_size<array_type>> data;
    };

    filter_type filtered;
    filtered.type = sizeof(signed_type) == 1 ? bios::uncomp_header::type::filter_8 : bios::uncomp_header::type::filter_16;
    filtered.length = sizeof(signed_type) * array_size<array_type>;

    filtered.data[0] = data[0];
    for (std::size_t ii = 1; ii < array_size<array_type>; ++ii) {
        filtered.data[ii] = static_cast<signed_type>(data[ii] - data[ii - 1]);
    }
    return filtered;
}

} // namespace gba::util

#endif // define GBAXX_UTIL_DIFF_FILTER_HPP
