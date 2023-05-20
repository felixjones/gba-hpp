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

#include <gba/input/key.hpp>

#include <gba/interrupt/irq.hpp>

#include <gba/video/bgxcnt.hpp>
#include <gba/video/bld.hpp>
#include <gba/video/dispcnt.hpp>
#include <gba/video/dispstat.hpp>
#include <gba/video/mosaic.hpp>
#include <gba/video/objattr.hpp>
#include <gba/video/win.hpp>

namespace gba::mmio {

namespace {

    // IRQ Handler

    constexpr auto IRQ_HANDLER = registral<const_ptr<void(* volatile)()>(0x03FFFFFC)>{};

    // Video

    constexpr auto DISPCNT = registral<const_ptr<volatile dispcnt>(0x4000000)>{};
    constexpr auto DISPSTAT = registral<const_ptr<volatile dispstat>(0x4000004)>{};
    constexpr auto VCOUNT = registral<const_ptr<const volatile u16>(0x4000006)>{};

    constexpr auto BG0CNT = registral<const_ptr<volatile bgcnt>(0x4000008)>{};
    constexpr auto BG1CNT = registral<const_ptr<volatile bgcnt>(0x400000A)>{};
    constexpr auto BG2CNT = registral<const_ptr<volatile bgcnt>(0x400000C)>{};
    constexpr auto BG3CNT = registral<const_ptr<volatile bgcnt>(0x400000E)>{};

    constexpr auto BG0HOFS = registral<const_ptr<volatile u16>(0x4000010)>{};
    constexpr auto BG0VOFS = registral<const_ptr<volatile u16>(0x4000012)>{};

    constexpr auto BG1HOFS = registral<const_ptr<volatile u16>(0x4000014)>{};
    constexpr auto BG1VOFS = registral<const_ptr<volatile u16>(0x4000016)>{};

    constexpr auto BG2HOFS = registral<const_ptr<volatile u16>(0x4000018)>{};
    constexpr auto BG2VOFS = registral<const_ptr<volatile u16>(0x400001A)>{};

    constexpr auto BG3HOFS = registral<const_ptr<volatile u16>(0x400001C)>{};
    constexpr auto BG3VOFS = registral<const_ptr<volatile u16>(0x400001E)>{};

    constexpr auto BG2PA = registral<const_ptr<volatile fixed<short, 8>>(0x4000020)>{};
    constexpr auto BG2PB = registral<const_ptr<volatile fixed<short, 8>>(0x4000022)>{};
    constexpr auto BG2PC = registral<const_ptr<volatile fixed<short, 8>>(0x4000024)>{};
    constexpr auto BG2PD = registral<const_ptr<volatile fixed<short, 8>>(0x4000026)>{};
    constexpr auto BG2X = registral<const_ptr<volatile fixed<int, 8>>(0x4000028)>{};
    constexpr auto BG2Y = registral<const_ptr<volatile fixed<int, 8>>(0x400002C)>{};

    constexpr auto BG3PA = registral<const_ptr<volatile fixed<short, 8>>(0x4000020)>{};
    constexpr auto BG3PB = registral<const_ptr<volatile fixed<short, 8>>(0x4000022)>{};
    constexpr auto BG3PC = registral<const_ptr<volatile fixed<short, 8>>(0x4000024)>{};
    constexpr auto BG3PD = registral<const_ptr<volatile fixed<short, 8>>(0x4000026)>{};
    constexpr auto BG3X = registral<const_ptr<volatile fixed<int, 8>>(0x4000028)>{};
    constexpr auto BG3Y = registral<const_ptr<volatile fixed<int, 8>>(0x400002C)>{};

    constexpr auto WIN0H = registral<const_ptr<volatile u8x2>(0x4000040)>{};
    constexpr auto WIN1H = registral<const_ptr<volatile u8x2>(0x4000042)>{};
    constexpr auto WIN0V = registral<const_ptr<volatile u8x2>(0x4000044)>{};
    constexpr auto WIN1V = registral<const_ptr<volatile u8x2>(0x4000046)>{};
    constexpr auto WININ = registral<const_ptr<volatile winin>(0x4000048)>{};
    constexpr auto WINOUT = registral<const_ptr<volatile winout>(0x400004A)>{};

    constexpr auto MOSAIC = registral<const_ptr<volatile mosaic>(0x400004C)>{};
    constexpr auto BLDCNT = registral<const_ptr<volatile bldcnt>(0x4000050)>{};
    constexpr auto BLDALPHA = registral<const_ptr<volatile fixed<make_vector<u8, 2>, 5>>(0x4000052)>{};
    constexpr auto BLDY = registral<const_ptr<volatile fixed<u16, 4>>(0x4000054)>{};

    // Keys

    constexpr auto KEYINPUT = registral<const_ptr<volatile keyinput>(0x4000130)>{};

    // Interrupts

    constexpr auto IE = registral<const_ptr<volatile irq>(0x4000200)>{};
    constexpr auto IF = registral<const_ptr<volatile irq>(0x4000202)>{};
    constexpr auto WAITCNT = registral<const_ptr<volatile u32>(0x4000204)>{};
    constexpr auto IME = registral<const_ptr<volatile bool>(0x4000208)>{};

    // Palette RAM

    constexpr auto BG_PALETTE = const_ptr<volatile u16[256]>(0x5000000);
    constexpr auto OBJ_PALETTE = const_ptr<volatile u16[256]>(0x5000200);

    // Video RAM

    constexpr auto VIDEO3_VRAM = const_ptr<volatile u16[160][240]>(0x6000000);
    constexpr auto VIDEO4_VRAM = const_ptr<volatile u8x2[160][120]>(0x6000000);
    constexpr auto VIDEO4_VRAM_FRAME1 = VIDEO4_VRAM + 0xa000;

    constexpr auto OBJ_TILES = const_ptr<volatile tile4bpp[1024]>(0x6010000);

    // Object Attribute Memory

    constexpr auto OBJ_ATTR0 = registral_series<const_ptr<volatile objattr0[128]>(0x7000000), 8>{};
    constexpr auto OBJ_ATTR1 = registral_series<const_ptr<volatile objattr1[128]>(0x7000002), 8>{};
    constexpr auto OBJ_ATTR2 = registral_series<const_ptr<volatile objattr2[128]>(0x7000004), 8>{};
    constexpr auto OBJ_ATTR = registral_series<const_ptr<volatile objattr[128]>(0x7000000), 8>{};

    constexpr auto OBJ_ATTR1_AFFINE = registral_series<const_ptr<volatile objattr1_affine[128]>(0x7000002), 8>{};
    constexpr auto OBJ_ATTR_AFFINE = registral_series<const_ptr<volatile objattr_affine[128]>(0x7000000), 8>{};

    constexpr auto AFFINE_PARAM_A = registral_series<const_ptr<volatile fixed<short, 8>[32]>(0x7000006), 32>{};
    constexpr auto AFFINE_PARAM_B = registral_series<const_ptr<volatile fixed<short, 8>[32]>(0x700000E), 32>{};
    constexpr auto AFFINE_PARAM_C = registral_series<const_ptr<volatile fixed<short, 8>[32]>(0x7000016), 32>{};
    constexpr auto AFFINE_PARAM_D = registral_series<const_ptr<volatile fixed<short, 8>[32]>(0x700001E), 32>{};

}

} // namespace gba::mmio

#endif // define GBAXX_MMIO_HPP
