/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_UTIL_TILES_HPP
#define GBAXX_UTIL_TILES_HPP

#include <array>
#include <cstddef>

#include <gba/vectype.hpp>

#include <gba/util/array_traits.hpp>
#include <gba/util/byte_array.hpp>

namespace gba::util {
namespace detail {

    consteval auto tile_next_row(auto x) noexcept {
        return (x + 7) & -8;
    }

    template <std::size_t Bits> requires IsPowerOfTwo<Bits> && (Bits <= 8)
    consteval auto make_tile(const ArrayOf<char> auto& palette, const ArrayOf<char> auto& bitmap) noexcept {
        constexpr auto palLen = util::array_size<decltype(palette)> - 1; // Exclude null byte
        static_assert(palLen <= (1u << Bits));

        std::size_t len = 0;
        std::array<std::byte, 8 * 8> tile{};

        for (const auto& pixel : bitmap) {
            if (pixel == 0) {
                break;
            }

            if (pixel == '\n') {
                len = detail::tile_next_row(len);
                continue;
            }

            for (std::size_t i = 0; i < palLen; ++i) {
                if (palette[i] == 0) {
                    break;
                }
                if (palette[i] == pixel) {
                    tile[len++] = std::byte(i);
                    break;
                }
            }
        }

        if constexpr (Bits == 8) {
            return tile;
        }

        const auto steps = 8u / Bits;

        std::array<std::byte, Bits * 8> data{};
        for (std::size_t i = 0; i < data.size(); ++i) {
            std::byte accum{};
            for (std::size_t j = 0; j < steps; ++j) {
                accum |= tile[i * steps + j] << (j * Bits);
            }
            data[i] = accum;
        }
        return data;
    }

} // namespace detail

consteval auto make_tile_1bpp(const ArrayOf<char> auto& palette, const ArrayOf<char> auto& bitmap) noexcept {
    return detail::make_tile<1>(palette, bitmap);
}

consteval auto make_tile_2bpp(const ArrayOf<char> auto& palette, const ArrayOf<char> auto& bitmap) noexcept {
    return detail::make_tile<2>(palette, bitmap);
}

consteval auto make_tile_4bpp(const ArrayOf<char> auto& palette, const ArrayOf<char> auto& bitmap) noexcept {
    return detail::make_tile<4>(palette, bitmap);
}

consteval auto make_tile_8bpp(const ArrayOf<char> auto& palette, const ArrayOf<char> auto& bitmap) noexcept {
    return detail::make_tile<8>(palette, bitmap);
}

} // namespace gba::util

#endif // define GBAXX_UTIL_TILES_HPP
