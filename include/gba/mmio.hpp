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

namespace gba::mmio {

namespace {

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

}

} // namespace gba::mmio

#endif // define GBAXX_MMIO_HPP
