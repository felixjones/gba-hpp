/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_VIDEO_MODES_HPP
#define GBAXX_VIDEO_MODES_HPP

#include <cstddef>

#include <gba/inttype.hpp>
#include <gba/ext/agbabi.hpp>
#include <gba/util/array_traits.hpp>

#if defined(GBAXX_HAS_AGBABI)
#include <aeabi.h>
#endif

namespace gba {

template <class T, std::size_t Mode>
concept IsPixelArray = util::Array<T> &&
    (((Mode == 3 || Mode == 5) && BinaryDigits<util::array_value_type<T>, 16>) ||
    (Mode == 4 && BinaryDigits<util::array_value_type<T>, 8>));

template <class T, std::size_t Mode>
concept IsLine = IsPixelArray<T, Mode> && (util::array_size<T> % (Mode == 5 ? 128 : 240)) == 0;

template <std::size_t Mode>
struct mode;

template <>
struct mode<3> {
    static constexpr auto address = 0x6000000;

    static void clear(BinaryDigits<16> auto c) noexcept {
#if defined(GBAXX_HAS_AGBABI)
        __agbabi_wordset4(reinterpret_cast<uint16*>(address), 240 * 160 * sizeof(uint16), ((c << 16) | c));
#else
        constexpr auto end = (240 * 160) / 2;
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
        buffer[(y * 240) + x] = c;
    }

    static auto get(int x, int y) noexcept {
        auto* buffer = reinterpret_cast<const volatile uint16*>(address);
        return buffer[(y * 240) + x];
    }

    static void put_row(int y, const IsLine<3> auto& line) noexcept {
        using line_type = std::remove_reference_t<decltype(line)>;

#if defined(GBAXX_HAS_AGBABI)
        __aeabi_memcpy4(reinterpret_cast<uint16*>(address) + (y * 240), &line[0], util::array_size<line_type> * sizeof(uint16));
#else
        auto* src = reinterpret_cast<const uint32*>(&line[0]);
        auto* dst = reinterpret_cast<volatile uint32*>(address);
        dst += y * 120;

        for (int ii = 0; ii < util::array_size<line_type> / 2; ++ii) {
            dst[ii] = src[ii];
        }
#endif
    }

    static void put_row(int x, int y, const IsPixelArray<3> auto& line) noexcept {
#if defined(GBAXX_HAS_AGBABI)
        __agbabi_memcpy2(reinterpret_cast<uint16*>(address) + (y * 240) + x, &line[0], sizeof(line));
#else
        using line_type = std::remove_reference_t<decltype(line)>;
        using int16_type = util::array_value_type<line_type>;

        auto* buffer = reinterpret_cast<volatile int16_type*>(address);
        buffer += (y * 240) + x;
        for (std::size_t ii = 0; ii < util::array_size<line_type>; ++ii) {
            buffer[ii] = line[ii];
        }
#endif
    }
};

template <>
struct mode<4> {
    static constexpr auto address = 0x6000000;
    static constexpr auto offset_frame = 0xA000;

    static void clear(BinaryDigits<8> auto c, std::size_t frame) noexcept {
        const auto addr = address + (offset_frame * frame);
#if defined(GBAXX_HAS_AGBABI)
        const auto c16 = (c << 8) | c;
        __agbabi_wordset4(reinterpret_cast<uint8*>(addr), 240 * 160, ((c16 << 16) | c16));
#else
        std::memset(reinterpret_cast<uint16*>(addr), c, 240 * 160);
#endif
    }

    static void put(int x, int y, BinaryDigits<8> auto c, std::size_t frame) noexcept {
        const auto addr = address + (offset_frame * frame);
        auto* buffer16 = reinterpret_cast<volatile uint16*>(addr);

        const auto offset16 = ((y * 240) + x) / 2;
        const auto pair = buffer16[offset16];
        if (x & 1) {
            buffer16[offset16] = (pair & 0x00ff) | (c << 8);
        } else {
            buffer16[offset16] = (pair & 0xff00) | c;
        }
    }

    static auto get(int x, int y, std::size_t frame) noexcept -> uint8 {
        const auto addr = address + (offset_frame * frame);
        auto* buffer16 = reinterpret_cast<const volatile uint16*>(addr);

        const auto offset16 = (y * 240) + (x / 2);
        const auto pair = buffer16[offset16];
        if (x & 1) {
            return pair >> 8;
        } else {
            return pair & 0xff;
        }
    }

    static void put_row(int y, const IsLine<4> auto& line, std::size_t frame) noexcept {
        using line_type = std::remove_reference_t<decltype(line)>;

        const auto addr = address + (offset_frame * frame);
#if defined(GBAXX_HAS_AGBABI)
        __aeabi_memcpy4(reinterpret_cast<uint8*>(addr) + (y * 240), &line[0], util::array_size<line_type>);
#else
        auto* src = reinterpret_cast<const uint32*>(&line[0]);
        auto* dst = reinterpret_cast<volatile uint32*>(addr);
        dst += y * 60;

        for (int ii = 0; ii < util::array_size<line_type> / 4; ++ii) {
            dst[ii] = src[ii];
        }
#endif
    }
};

template <>
struct mode<5> {
    static constexpr auto address = 0x6000000;
    static constexpr auto offset_frame = 0xA000;

    static void clear(BinaryDigits<16> auto c, std::size_t frame) noexcept {
        const auto addr = address + (offset_frame * frame);
#if defined(GBAXX_HAS_AGBABI)
        __agbabi_wordset4(reinterpret_cast<uint16*>(addr), 160 * 128 * sizeof(uint16), ((c << 16) | c));
#else
        constexpr auto end = (160 * 128) / 2;
        auto* buffer = reinterpret_cast<volatile uint32*>(addr);
        const auto cc = uint32((c << 16) | c);
        for (int ii = 0; ii < end; ++ii) {
            buffer[ii] = cc;
        }
#endif
    }

    static void put(int x, int y, BinaryDigits<16> auto c, std::size_t frame) noexcept {
        const auto addr = address + (offset_frame * frame);
        using int16_type = decltype(c);

        auto* buffer = reinterpret_cast<volatile int16_type*>(addr);
        buffer[(y * 160) + x] = c;
    }

    static auto get(int x, int y, std::size_t frame) noexcept {
        const auto addr = address + (offset_frame * frame);
        auto* buffer = reinterpret_cast<const volatile uint16*>(addr);
        return buffer[(y * 160) + x];
    }

    static void put_row(int y, const IsLine<3> auto& line, std::size_t frame) noexcept {
        using line_type = std::remove_reference_t<decltype(line)>;

        const auto addr = address + (offset_frame * frame);
#if defined(GBAXX_HAS_AGBABI)
        __aeabi_memcpy4(reinterpret_cast<uint16*>(addr) + (y * 160), &line[0], util::array_size<line_type> * sizeof(uint16));
#else
        auto* src = reinterpret_cast<const uint32*>(&line[0]);
        auto* dst = reinterpret_cast<volatile uint32*>(addr);
        dst += y * 80;

        for (int ii = 0; ii < util::array_size<line_type> / 2; ++ii) {
            dst[ii] = src[ii];
        }
#endif
    }

    static void put_row(int x, int y, const IsPixelArray<3> auto& line, std::size_t frame) noexcept {
        const auto addr = address + (offset_frame * frame);
#if defined(GBAXX_HAS_AGBABI)
        __agbabi_memcpy2(reinterpret_cast<uint16*>(addr) + (y * 160) + x, &line[0], sizeof(line));
#else
        using line_type = std::remove_reference_t<decltype(line)>;
        using int16_type = util::array_value_type<line_type>;

        auto* buffer = reinterpret_cast<volatile int16_type*>(addr);
        buffer += (y * 160) + x;
        for (std::size_t ii = 0; ii < util::array_size<line_type>; ++ii) {
            buffer[ii] = line[ii];
        }
#endif
    }
};

} // namespace gba

#endif // define GBAXX_VIDEO_MODES_HPP
