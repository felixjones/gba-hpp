/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_MMIO_HPP
#define GBAXX_MMIO_HPP

#include <cstdint>

#include <gba/type.hpp>

#include <gba/video/bgxcnt.hpp>
#include <gba/video/dispcnt.hpp>
#include <gba/video/dispstat.hpp>
#include <gba/video/mosaic.hpp>
#include <gba/video/win.hpp>

namespace gba::mmio {

namespace {

    // Video

    constexpr auto DISPCNT = registral<const_ptr<volatile dispcnt>(0x4000000)>{};
    constexpr auto DISPSTAT = registral<const_ptr<volatile dispstat>(0x4000004)>{};
    constexpr auto VCOUNT = registral<const_ptr<const volatile std::uint16_t>(0x4000006)>{};

    constexpr auto BG0CNT = registral<const_ptr<volatile bgcnt>(0x4000008)>{};
    constexpr auto BG1CNT = registral<const_ptr<volatile bgcnt>(0x400000A)>{};
    constexpr auto BG2CNT = registral<const_ptr<volatile bgcnt>(0x400000C)>{};
    constexpr auto BG3CNT = registral<const_ptr<volatile bgcnt>(0x400000E)>{};

    constexpr auto BG0HOFS = registral<const_ptr<volatile std::uint16_t>(0x4000010)>{};
    constexpr auto BG0VOFS = registral<const_ptr<volatile std::uint16_t>(0x4000012)>{};

    constexpr auto BG1HOFS = registral<const_ptr<volatile std::uint16_t>(0x4000014)>{};
    constexpr auto BG1VOFS = registral<const_ptr<volatile std::uint16_t>(0x4000016)>{};

    constexpr auto BG2HOFS = registral<const_ptr<volatile std::uint16_t>(0x4000018)>{};
    constexpr auto BG2VOFS = registral<const_ptr<volatile std::uint16_t>(0x400001A)>{};

    constexpr auto BG3HOFS = registral<const_ptr<volatile std::uint16_t>(0x400001C)>{};
    constexpr auto BG3VOFS = registral<const_ptr<volatile std::uint16_t>(0x400001E)>{};

    constexpr auto BG2PA = registral<const_ptr<volatile fixedpoint<7, 8, short>>(0x4000020)>{};
    constexpr auto BG2PB = registral<const_ptr<volatile fixedpoint<7, 8, short>>(0x4000022)>{};
    constexpr auto BG2PC = registral<const_ptr<volatile fixedpoint<7, 8, short>>(0x4000024)>{};
    constexpr auto BG2PD = registral<const_ptr<volatile fixedpoint<7, 8, short>>(0x4000026)>{};
    constexpr auto BG2X = registral<const_ptr<volatile fixedpoint<19, 8, int>>(0x4000028)>{};
    constexpr auto BG2Y = registral<const_ptr<volatile fixedpoint<19, 8, int>>(0x400002C)>{};

    constexpr auto BG3PA = registral<const_ptr<volatile fixedpoint<7, 8, short>>(0x4000030)>{};
    constexpr auto BG3PB = registral<const_ptr<volatile fixedpoint<7, 8, short>>(0x4000032)>{};
    constexpr auto BG3PC = registral<const_ptr<volatile fixedpoint<7, 8, short>>(0x4000034)>{};
    constexpr auto BG3PD = registral<const_ptr<volatile fixedpoint<7, 8, short>>(0x4000036)>{};
    constexpr auto BG3X = registral<const_ptr<volatile fixedpoint<19, 8, int>>(0x4000038)>{};
    constexpr auto BG3Y = registral<const_ptr<volatile fixedpoint<19, 8, int>>(0x400003C)>{};

    constexpr auto WIN0H = registral<const_ptr<volatile u8x2>(0x4000040)>{};
    constexpr auto WIN1H = registral<const_ptr<volatile u8x2>(0x4000042)>{};
    constexpr auto WIN0V = registral<const_ptr<volatile u8x2>(0x4000044)>{};
    constexpr auto WIN1V = registral<const_ptr<volatile u8x2>(0x4000046)>{};
    constexpr auto WININ = registral<const_ptr<volatile winin>(0x4000048)>{};
    constexpr auto WINOUT = registral<const_ptr<volatile winout>(0x400004A)>{};

    constexpr auto MOSAIC = registral<const_ptr<volatile mosaic>(0x400004C)>{};

    // Video RAM

    constexpr auto VIDEO3_VRAM = const_ptr<volatile std::uint16_t[160][240]>(0x6000000);

}

} // namespace gba::mmio

#endif // define GBAXX_MMIO_HPP
