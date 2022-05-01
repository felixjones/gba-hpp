/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_VIDEO_PALETTES_HPP
#define GBAXX_VIDEO_PALETTES_HPP

#include <cstddef>

#include <gba/inttype.hpp>
#include <gba/video/modes.hpp>

namespace gba {

template <class T>
concept IsColorArray = util::Array<T> && BinaryDigits<util::array_value_type<T>, 16>;

template <class T>
concept IsPaletteEntry = IsColorArray<T> && (util::array_size<T> % 8) == 0;

template <std::uintptr_t Address>
struct palette {
    static constexpr auto address = Address;

    static void clear(BinaryDigits<16> auto c) noexcept {
#if defined(GBAXX_HAS_AGBABI)
        __agbabi_wordset4(reinterpret_cast<uint16*>(address), (8 * 8) * sizeof(uint16), ((c << 16) | c));
#else
        constexpr auto end = (8 * 8) / 2;
        auto* buffer = reinterpret_cast<volatile uint32*>(address);
        const auto cc = uint32((c << 16) | c);
        for (int ii = 0; ii < end; ++ii) {
            buffer[ii] = cc;
        }
#endif
    }

    static void put(int x, int y, BinaryDigits<16> auto c) noexcept {
        using int16_type = decltype(c);

        auto* buffer = reinterpret_cast<volatile int16_type*>(address);
        buffer[(y * 8) + x] = c;
    }

    static auto get(int x, int y) noexcept {
        auto* buffer = reinterpret_cast<const volatile uint16*>(address);
        return buffer[(y * 8) + x];
    }

    static void put_row(int y, const IsPaletteEntry auto line) noexcept {
#if defined(GBAXX_HAS_AGBABI)
        __aeabi_memcpy4(reinterpret_cast<uint16*>(address) + (y * 8), &line[0], util::array_size<decltype(line)> * sizeof(uint16));
#else
        auto* src = reinterpret_cast<const uint32*>(&line[0]);
        auto* dst= reinterpret_cast<volatile uint32*>(address);
        dst += y * 8;

        for (int ii = 0; ii < util::array_size<decltype(line)> / 2; ++ii) {
            dst[ii] = src[ii];
        }
#endif
    }

    static void put_row(int x, int y, const IsColorArray auto line) noexcept {
#if defined(GBAXX_HAS_AGBABI)
        __agbabi_memcpy2(reinterpret_cast<uint16*>(address) + (y * 8) + x, &line[0], sizeof(line));
#else
        using int16_type = util::array_value_type<decltype(line)>;

        auto* buffer = reinterpret_cast<volatile int16_type*>(address);
        buffer += (y * 8) + x;
        for (std::size_t ii = 0; ii < util::array_size<decltype(line)>; ++ii) {
            buffer[ii] = line[ii];
        }
#endif
    }
};

using bg_palette = palette<0x5000000>;
using obj_palette = palette<0x5000200>;

} // namespace gba

#endif // define GBAXX_VIDEO_PALETTES_HPP
