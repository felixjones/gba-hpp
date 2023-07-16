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

#include <gba/hardware/dma.hpp>
#include <gba/hardware/serial.hpp>
#include <gba/hardware/timer.hpp>

#include <gba/input/key.hpp>

#include <gba/interrupt/irq.hpp>

#include <gba/sound/noise.hpp>
#include <gba/sound/soundcnt.hpp>
#include <gba/sound/tone.hpp>
#include <gba/sound/wave.hpp>

#include <gba/video/bgxcnt.hpp>
#include <gba/video/bld.hpp>
#include <gba/video/dispcnt.hpp>
#include <gba/video/dispstat.hpp>
#include <gba/video/mosaic.hpp>
#include <gba/video/objattr.hpp>
#include <gba/video/textscreen.hpp>
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

    // Sound

    constexpr auto TONE1_SWEEP = registral<const_ptr<volatile sound1cnt_l>(0x4000060)>{};
    constexpr auto TONE1_PATTERN = registral<const_ptr<volatile sound1cnt_h>(0x4000062)>{};
    constexpr auto TONE1_FREQUENCY = registral<const_ptr<volatile sound1cnt_x>(0x4000064)>{};

    constexpr auto TONE2_PATTERN = registral<const_ptr<volatile sound2cnt_l>(0x4000068)>{};
    constexpr auto TONE2_FREQUENCY = registral<const_ptr<volatile sound2cnt_h>(0x400006C)>{};

    constexpr auto WAVE_BANK = registral<const_ptr<volatile sound3cnt_l>(0x4000070)>{};
    constexpr auto WAVE_LEN_VOLUME = registral<const_ptr<volatile sound3cnt_h>(0x4000072)>{};
    constexpr auto WAVE_FREQ = registral<const_ptr<volatile sound3cnt_x>(0x4000074)>{};

    constexpr auto NOISE_LEN_ENV = registral<const_ptr<volatile sound4cnt_l>(0x4000078)>{};
    constexpr auto NOISE_FREQ = registral<const_ptr<volatile sound4cnt_h>(0x400007C)>{};

    constexpr auto LEFT_RIGHT_VOLUME = registral<const_ptr<volatile soundcnt_l>(0x4000080)>{};
    constexpr auto SOUND_MIX = registral<const_ptr<volatile soundcnt_h>(0x4000082)>{};
    constexpr auto SOUND_ENABLED = registral<const_ptr<volatile soundcnt_x>(0x4000084)>{};
    constexpr auto SOUNDBIAS = registral<const_ptr<volatile soundbias>(0x4000088)>{};

    constexpr auto FIFO_A = registral<const_ptr<volatile u32>(0x40000A0)>{};
    constexpr auto FIFO_B = registral<const_ptr<volatile u32>(0x40000A4)>{};

    // DMA

    constexpr auto DMA0_SRC = registral<const_ptr<const void* volatile>(0x40000B0)>{};
    constexpr auto DMA0_DEST = registral<const_ptr<void* volatile>(0x40000B4)>{};
    constexpr auto DMA0_COUNT = registral<const_ptr<volatile u16>(0x40000B8)>{};
    constexpr auto DMA0_CONTROL = registral<const_ptr<volatile dmacnt_h>(0x40000BA)>{};

    constexpr auto DMA1_SRC = registral<const_ptr<const void* volatile>(0x40000BC)>{};
    constexpr auto DMA1_DEST = registral<const_ptr<void* volatile>(0x40000C0)>{};
    constexpr auto DMA1_COUNT = registral<const_ptr<volatile u16>(0x40000C4)>{};
    constexpr auto DMA1_CONTROL = registral<const_ptr<volatile dmacnt_h>(0x40000C6)>{};

    constexpr auto DMA2_SRC = registral<const_ptr<const void* volatile>(0x40000C8)>{};
    constexpr auto DMA2_DEST = registral<const_ptr<void* volatile>(0x40000CC)>{};
    constexpr auto DMA2_COUNT = registral<const_ptr<volatile u16>(0x40000D0)>{};
    constexpr auto DMA2_CONTROL = registral<const_ptr<volatile dmacnt_h>(0x40000D2)>{};

    constexpr auto DMA3_SRC = registral<const_ptr<const void* volatile>(0x40000D4)>{};
    constexpr auto DMA3_DEST = registral<const_ptr<void* volatile>(0x40000D8)>{};
    constexpr auto DMA3_COUNT = registral<const_ptr<volatile u16>(0x40000DC)>{};
    constexpr auto DMA3_CONTROL = registral<const_ptr<volatile dmacnt_h>(0x40000DE)>{};

    constexpr auto DMA_SRC = registral_series<const_ptr<const void* volatile[4]>(0x40000B0), 12>{};
    constexpr auto DMA_DEST = registral_series<const_ptr<void* volatile[4]>(0x40000B4), 12>{};
    constexpr auto DMA_COUNT = registral_series<const_ptr<volatile u16[4]>(0x40000B8), 12>{};
    constexpr auto DMA_CONTROL = registral_series<const_ptr<volatile dmacnt_h[4]>(0x40000BA), 4>{};

    // Timers

    constexpr auto TIMER0_COUNT = registral<const_ptr<const volatile u16>(0x4000100)>{};
    constexpr auto TIMER0_RELOAD = registral<const_ptr<volatile short>(0x4000100)>{};
    constexpr auto TIMER0_CONTROL = registral<const_ptr<volatile tmcnt_h>(0x4000102)>{};

    constexpr auto TIMER1_COUNT = registral<const_ptr<const volatile u16>(0x4000104)>{};
    constexpr auto TIMER1_RELOAD = registral<const_ptr<volatile short>(0x4000104)>{};
    constexpr auto TIMER1_CONTROL = registral<const_ptr<volatile tmcnt_h>(0x4000106)>{};

    constexpr auto TIMER2_COUNT = registral<const_ptr<const volatile u16>(0x4000108)>{};
    constexpr auto TIMER2_RELOAD = registral<const_ptr<volatile short>(0x4000108)>{};
    constexpr auto TIMER2_CONTROL = registral<const_ptr<volatile tmcnt_h>(0x400010A)>{};

    constexpr auto TIMER3_COUNT = registral<const_ptr<const volatile u16>(0x400010C)>{};
    constexpr auto TIMER3_RELOAD = registral<const_ptr<volatile short>(0x400010C)>{};
    constexpr auto TIMER3_CONTROL = registral<const_ptr<volatile tmcnt_h>(0x400010E)>{};

    constexpr auto TIMER_COUNT = registral_series<const_ptr<const volatile u16[4]>(0x4000100), 4>{};
    constexpr auto TIMER_RELOAD = registral_series<const_ptr<volatile u16[4]>(0x4000100), 4>{};
    constexpr auto TIMER_CONTROL = registral_series<const_ptr<volatile tmcnt_h[4]>(0x4000102), 4>{};

    // Serial

    constexpr auto SIODATA32 = registral<const_ptr<volatile u32>(0x4000120)>{};
    constexpr auto SIOMULTI0 = registral<const_ptr<volatile u16>(0x4000120)>{};
    constexpr auto SIOMULTI1 = registral<const_ptr<volatile u16>(0x4000122)>{};
    constexpr auto SIOMULTI2 = registral<const_ptr<volatile u16>(0x4000124)>{};
    constexpr auto SIOMULTI3 = registral<const_ptr<volatile u16>(0x4000126)>{};
    constexpr auto SIOMULTI = registral_series<const_ptr<volatile u16[4]>(0x4000120)>{};

    constexpr auto SIOCNT_NORMAL = registral<const_ptr<volatile siocnt_normal>(0x4000128)>{};
    constexpr auto SIOCNT_MULTI = registral<const_ptr<volatile siocnt_multi>(0x4000128)>{};
    constexpr auto SIOCNT_UART = registral<const_ptr<volatile siocnt_uart>(0x4000128)>{};

    constexpr auto SIODATA8 = registral<const_ptr<volatile u8>(0x400012A)>{};
    constexpr auto SIOMLT_SEND = registral<const_ptr<volatile u16>(0x400012A)>{};

    // Keys

    constexpr auto KEYINPUT = registral<const_ptr<volatile keyinput>(0x4000130)>{};
    constexpr auto KEYCNT = registral<const_ptr<volatile keycnt>(0x4000132)>{};

    // RCNT

    constexpr auto RCNT_JOYBUS = registral<const_ptr<volatile rcnt_joybus>(0x4000134)>{};
    constexpr auto RCNT_GPIO = registral<const_ptr<volatile rcnt_gpio>(0x4000134)>{};

    // Joybus

    constexpr auto JOYCNT = registral<const_ptr<volatile joycnt>(0x4000140)>{};
    constexpr auto JOY_RECV = registral<const_ptr<volatile u32>(0x4000150)>{};
    constexpr auto JOY_TRANS = registral<const_ptr<volatile u32>(0x4000154)>{};
    constexpr auto JOYSTAT = registral<const_ptr<volatile joystat>(0x4000158)>{};

    // Interrupts

    constexpr auto IE = registral<const_ptr<volatile irq>(0x4000200)>{};
    constexpr auto IF = registral<const_ptr<volatile irq>(0x4000202)>{};
    constexpr auto WAITCNT = registral<const_ptr<volatile u32>(0x4000204)>{};
    constexpr auto IME = registral<const_ptr<volatile bool>(0x4000208)>{};

    // Palette RAM

    constexpr auto BG_PALETTE = const_ptr<volatile u16[256]>(0x5000000);
    constexpr auto OBJ_PALETTE = const_ptr<volatile u16[256]>(0x5000200);

    // Video RAM

    constexpr auto CHARBLOCK0_4BPP = const_ptr<volatile tile4bpp[512]>(0x6000000);
    constexpr auto CHARBLOCK1_4BPP = const_ptr<volatile tile4bpp[512]>(0x6004000);
    constexpr auto CHARBLOCK2_4BPP = const_ptr<volatile tile4bpp[512]>(0x6009000);
    constexpr auto CHARBLOCK3_4BPP = const_ptr<volatile tile4bpp[512]>(0x600C000);

    constexpr auto CHARBLOCK0_8BPP = const_ptr<volatile tile8bpp[256]>(0x6000000);
    constexpr auto CHARBLOCK1_8BPP = const_ptr<volatile tile8bpp[256]>(0x6004000);
    constexpr auto CHARBLOCK2_8BPP = const_ptr<volatile tile8bpp[256]>(0x6009000);
    constexpr auto CHARBLOCK3_8BPP = const_ptr<volatile tile8bpp[256]>(0x600C000);

    constexpr auto TEXT_SCREENBLOCKS = const_ptr<volatile textscreen[1024]>(0x6000000);

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
