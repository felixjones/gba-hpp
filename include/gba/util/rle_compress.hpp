/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_UTIL_RLE_COMPRESS_HPP
#define GBAXX_UTIL_RLE_COMPRESS_HPP

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <type_traits>

#include <gba/inttype.hpp>

#include <gba/bios/decompress.hpp>

#include <gba/util/byte_array.hpp>
#include <gba/util/constexpr.hpp>

namespace gba::util {
namespace detail::rle {

consteval auto compress_32mib(const ByteArray auto& data) noexcept {
    struct compressed_type {
        std::array<std::byte, 0x2000000> data;
        std::size_t uncompressed_length;
        std::size_t compressed_length;
    };

    constexpr auto size = array_size<decltype(data)>;

    compressed_type compressed{};
    compressed.uncompressed_length = array_size<decltype(data)>;
    compressed.compressed_length = 0;

    auto& result = compressed.data;
    auto& resultLength = compressed.compressed_length;

    for (std::size_t ii = 0; ii < size; ++ii) {
        std::size_t count = 1;

        while (ii < size - 1 && data[ii] == data[ii + 1]) {
            count++;
            ii++;
        }

        if (count >= 3) {
            while (count >= 130) {
                result[resultLength++] = std::byte(0xff); // 127 (+3) | 0x80
                result[resultLength++] = data[ii];
                count -= 130;
            }

            if (count >= 3) {
                result[resultLength++] = std::byte((count - 3) | 0x80);
                result[resultLength++] = data[ii];
                continue;
            }
        }

        std::size_t begin = ii - count + 1;
        while (ii < size - 1 && data[ii] != data[ii + 1]) {
            count++;
            ii++;
        }

        while (count >= 128) {
            result[resultLength++] = std::byte(0x7f); // 127 (+1)
            std::copy(std::next(std::cbegin(data), begin), std::next(std::cbegin(data), begin + 128), std::next(std::begin(result), resultLength));
            resultLength += 128;
            begin += 128;
            count -= 128;
        }

        if (count) {
            result[resultLength++] = std::byte(count - 1);
            std::copy(std::next(std::cbegin(data), begin), std::next(std::cbegin(data), begin + count), std::next(std::begin(result), resultLength));
            resultLength += count;
        }
    }

    // Pad to multiple of 4
    compressed.compressed_length = ((compressed.compressed_length + 3) >> 2) << 2;
    return compressed;
}

consteval auto compress(auto callable) noexcept {
    constexpr auto& data_32mib = make_static<rle::compress_32mib(callable())>;

    struct rle_type : bios::uncomp_header {
        std::array<std::byte, data_32mib.compressed_length> data;
    };

    rle_type compressed{};
    compressed.type = bios::uncomp_header::type::rle;
    compressed.length = data_32mib.uncompressed_length;

    std::copy(data_32mib.data.cbegin(), std::next(data_32mib.data.cbegin(), data_32mib.compressed_length), compressed.data.begin());

    return compressed;
}

} // namespace detail::rle

consteval auto& rle_compress(auto callable) noexcept {
    return make_static<detail::rle::compress(callable)>;
}

} // namespace gba::util

#endif // define GBAXX_UTIL_RLE_COMPRESS_HPP
