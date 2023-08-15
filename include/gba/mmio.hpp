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

#include <gba/hardware/cartridge.hpp>
#include <gba/hardware/dma.hpp>
#include <gba/hardware/memcnt.hpp>
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

    // IRQ Handler

    inline constexpr auto IRQ_HANDLER = registral<const_ptr<void(* volatile)()>(0x03FFFFFC)>{};

    // Video

    inline constexpr auto DISPCNT = registral<const_ptr<volatile dispcnt>(0x4000000)>{};
    inline constexpr auto DISPSTAT = registral<const_ptr<volatile dispstat>(0x4000004)>{};
    inline constexpr auto VCOUNT = registral<const_ptr<const volatile u16>(0x4000006)>{};

    inline constexpr auto BG0CNT = registral<const_ptr<volatile bgcnt>(0x4000008)>{};
    inline constexpr auto BG1CNT = registral<const_ptr<volatile bgcnt>(0x400000A)>{};
    inline constexpr auto BG2CNT = registral<const_ptr<volatile bgcnt>(0x400000C)>{};
    inline constexpr auto BG3CNT = registral<const_ptr<volatile bgcnt>(0x400000E)>{};

    inline constexpr auto BG0HOFS = registral<const_ptr<volatile u16>(0x4000010)>{};
    inline constexpr auto BG0VOFS = registral<const_ptr<volatile u16>(0x4000012)>{};

    inline constexpr auto BG1HOFS = registral<const_ptr<volatile u16>(0x4000014)>{};
    inline constexpr auto BG1VOFS = registral<const_ptr<volatile u16>(0x4000016)>{};

    inline constexpr auto BG2HOFS = registral<const_ptr<volatile u16>(0x4000018)>{};
    inline constexpr auto BG2VOFS = registral<const_ptr<volatile u16>(0x400001A)>{};

    inline constexpr auto BG3HOFS = registral<const_ptr<volatile u16>(0x400001C)>{};
    inline constexpr auto BG3VOFS = registral<const_ptr<volatile u16>(0x400001E)>{};

    inline constexpr auto BG2PA = registral<const_ptr<volatile fixed<short, 8>>(0x4000020)>{};
    inline constexpr auto BG2PB = registral<const_ptr<volatile fixed<short, 8>>(0x4000022)>{};
    inline constexpr auto BG2PC = registral<const_ptr<volatile fixed<short, 8>>(0x4000024)>{};
    inline constexpr auto BG2PD = registral<const_ptr<volatile fixed<short, 8>>(0x4000026)>{};
    inline constexpr auto BG2X = registral<const_ptr<volatile fixed<int, 8>>(0x4000028)>{};
    inline constexpr auto BG2Y = registral<const_ptr<volatile fixed<int, 8>>(0x400002C)>{};

    inline constexpr auto BG3PA = registral<const_ptr<volatile fixed<short, 8>>(0x4000020)>{};
    inline constexpr auto BG3PB = registral<const_ptr<volatile fixed<short, 8>>(0x4000022)>{};
    inline constexpr auto BG3PC = registral<const_ptr<volatile fixed<short, 8>>(0x4000024)>{};
    inline constexpr auto BG3PD = registral<const_ptr<volatile fixed<short, 8>>(0x4000026)>{};
    inline constexpr auto BG3X = registral<const_ptr<volatile fixed<int, 8>>(0x4000028)>{};
    inline constexpr auto BG3Y = registral<const_ptr<volatile fixed<int, 8>>(0x400002C)>{};

    inline constexpr auto WIN0H = registral<const_ptr<volatile u8x2>(0x4000040)>{};
    inline constexpr auto WIN1H = registral<const_ptr<volatile u8x2>(0x4000042)>{};
    inline constexpr auto WIN0V = registral<const_ptr<volatile u8x2>(0x4000044)>{};
    inline constexpr auto WIN1V = registral<const_ptr<volatile u8x2>(0x4000046)>{};
    inline constexpr auto WININ = registral<const_ptr<volatile winin>(0x4000048)>{};
    inline constexpr auto WINOUT = registral<const_ptr<volatile winout>(0x400004A)>{};

    inline constexpr auto MOSAIC = registral<const_ptr<volatile mosaic>(0x400004C)>{};
    inline constexpr auto BLDCNT = registral<const_ptr<volatile bldcnt>(0x4000050)>{};
    inline constexpr auto BLDALPHA = registral<const_ptr<volatile fixed<make_vector<u8, 2>, 5>>(0x4000052)>{};
    inline constexpr auto BLDY = registral<const_ptr<volatile fixed<u16, 4>>(0x4000054)>{};

    // Sound

    inline constexpr auto TONE1_SWEEP = registral<const_ptr<volatile sound1cnt_l>(0x4000060)>{};
    inline constexpr auto TONE1_PATTERN = registral<const_ptr<volatile sound1cnt_h>(0x4000062)>{};
    inline constexpr auto TONE1_FREQUENCY = registral<const_ptr<volatile sound1cnt_x>(0x4000064)>{};

    inline constexpr auto TONE2_PATTERN = registral<const_ptr<volatile sound2cnt_l>(0x4000068)>{};
    inline constexpr auto TONE2_FREQUENCY = registral<const_ptr<volatile sound2cnt_h>(0x400006C)>{};

    inline constexpr auto WAVE_BANK = registral<const_ptr<volatile sound3cnt_l>(0x4000070)>{};
    inline constexpr auto WAVE_LEN_VOLUME = registral<const_ptr<volatile sound3cnt_h>(0x4000072)>{};
    inline constexpr auto WAVE_FREQ = registral<const_ptr<volatile sound3cnt_x>(0x4000074)>{};

    inline constexpr auto NOISE_LEN_ENV = registral<const_ptr<volatile sound4cnt_l>(0x4000078)>{};
    inline constexpr auto NOISE_FREQ = registral<const_ptr<volatile sound4cnt_h>(0x400007C)>{};

    inline constexpr auto LEFT_RIGHT_VOLUME = registral<const_ptr<volatile soundcnt_l>(0x4000080)>{};
    inline constexpr auto SOUND_MIX = registral<const_ptr<volatile soundcnt_h>(0x4000082)>{};
    inline constexpr auto SOUND_ENABLED = registral<const_ptr<volatile soundcnt_x>(0x4000084)>{};
    inline constexpr auto SOUNDBIAS = registral<const_ptr<volatile soundbias>(0x4000088)>{};

    inline constexpr auto FIFO_A = registral<const_ptr<volatile u32>(0x40000A0)>{};
    inline constexpr auto FIFO_B = registral<const_ptr<volatile u32>(0x40000A4)>{};

    // DMA

    inline constexpr auto DMA0_SRC = registral<const_ptr<const void* volatile>(0x40000B0)>{};
    inline constexpr auto DMA0_DEST = registral<const_ptr<void* volatile>(0x40000B4)>{};
    inline constexpr auto DMA0_COUNT = registral<const_ptr<volatile u16>(0x40000B8)>{};
    inline constexpr auto DMA0_CONTROL = registral<const_ptr<volatile dmacnt_h>(0x40000BA)>{};

    inline constexpr auto DMA1_SRC = registral<const_ptr<const void* volatile>(0x40000BC)>{};
    inline constexpr auto DMA1_DEST = registral<const_ptr<void* volatile>(0x40000C0)>{};
    inline constexpr auto DMA1_COUNT = registral<const_ptr<volatile u16>(0x40000C4)>{};
    inline constexpr auto DMA1_CONTROL = registral<const_ptr<volatile dmacnt_h>(0x40000C6)>{};

    inline constexpr auto DMA2_SRC = registral<const_ptr<const void* volatile>(0x40000C8)>{};
    inline constexpr auto DMA2_DEST = registral<const_ptr<void* volatile>(0x40000CC)>{};
    inline constexpr auto DMA2_COUNT = registral<const_ptr<volatile u16>(0x40000D0)>{};
    inline constexpr auto DMA2_CONTROL = registral<const_ptr<volatile dmacnt_h>(0x40000D2)>{};

    inline constexpr auto DMA3_SRC = registral<const_ptr<const void* volatile>(0x40000D4)>{};
    inline constexpr auto DMA3_DEST = registral<const_ptr<void* volatile>(0x40000D8)>{};
    inline constexpr auto DMA3_COUNT = registral<const_ptr<volatile u16>(0x40000DC)>{};
    inline constexpr auto DMA3_CONTROL = registral<const_ptr<volatile dmacnt_h>(0x40000DE)>{};

    inline constexpr auto DMA_SRC = registral_series<const_ptr<const void* volatile[4]>(0x40000B0), 12>{};
    inline constexpr auto DMA_DEST = registral_series<const_ptr<void* volatile[4]>(0x40000B4), 12>{};
    inline constexpr auto DMA_COUNT = registral_series<const_ptr<volatile u16[4]>(0x40000B8), 12>{};
    inline constexpr auto DMA_CONTROL = registral_series<const_ptr<volatile dmacnt_h[4]>(0x40000BA), 4>{};

    // Timers

    inline constexpr auto TIMER0_COUNT = registral<const_ptr<const volatile u16>(0x4000100)>{};
    inline constexpr auto TIMER0_RELOAD = registral<const_ptr<volatile short>(0x4000100)>{};
    inline constexpr auto TIMER0_CONTROL = registral<const_ptr<volatile tmcnt_h>(0x4000102)>{};

    inline constexpr auto TIMER1_COUNT = registral<const_ptr<const volatile u16>(0x4000104)>{};
    inline constexpr auto TIMER1_RELOAD = registral<const_ptr<volatile short>(0x4000104)>{};
    inline constexpr auto TIMER1_CONTROL = registral<const_ptr<volatile tmcnt_h>(0x4000106)>{};

    inline constexpr auto TIMER2_COUNT = registral<const_ptr<const volatile u16>(0x4000108)>{};
    inline constexpr auto TIMER2_RELOAD = registral<const_ptr<volatile short>(0x4000108)>{};
    inline constexpr auto TIMER2_CONTROL = registral<const_ptr<volatile tmcnt_h>(0x400010A)>{};

    inline constexpr auto TIMER3_COUNT = registral<const_ptr<const volatile u16>(0x400010C)>{};
    inline constexpr auto TIMER3_RELOAD = registral<const_ptr<volatile short>(0x400010C)>{};
    inline constexpr auto TIMER3_CONTROL = registral<const_ptr<volatile tmcnt_h>(0x400010E)>{};

    inline constexpr auto TIMER_COUNT = registral_series<const_ptr<const volatile u16[4]>(0x4000100), 4>{};
    inline constexpr auto TIMER_RELOAD = registral_series<const_ptr<volatile u16[4]>(0x4000100), 4>{};
    inline constexpr auto TIMER_CONTROL = registral_series<const_ptr<volatile tmcnt_h[4]>(0x4000102), 4>{};

    // Serial

    inline constexpr auto SIODATA32 = registral<const_ptr<volatile u32>(0x4000120)>{};
    inline constexpr auto SIOMULTI0 = registral<const_ptr<volatile u16>(0x4000120)>{};
    inline constexpr auto SIOMULTI1 = registral<const_ptr<volatile u16>(0x4000122)>{};
    inline constexpr auto SIOMULTI2 = registral<const_ptr<volatile u16>(0x4000124)>{};
    inline constexpr auto SIOMULTI3 = registral<const_ptr<volatile u16>(0x4000126)>{};
    inline constexpr auto SIOMULTI = registral_series<const_ptr<volatile u16[4]>(0x4000120)>{};

    inline constexpr auto SIOCNT_NORMAL = registral<const_ptr<volatile siocnt_normal>(0x4000128)>{};
    inline constexpr auto SIOCNT_MULTI = registral<const_ptr<volatile siocnt_multi>(0x4000128)>{};
    inline constexpr auto SIOCNT_UART = registral<const_ptr<volatile siocnt_uart>(0x4000128)>{};

    inline constexpr auto SIODATA8 = registral<const_ptr<volatile u8>(0x400012A)>{};
    inline constexpr auto SIOMLT_SEND = registral<const_ptr<volatile u16>(0x400012A)>{};

    // Keys

    inline constexpr auto KEYINPUT = registral<const_ptr<volatile keyinput>(0x4000130)>{};
    inline constexpr auto KEYCNT = registral<const_ptr<volatile keycnt>(0x4000132)>{};

    // RCNT

    inline constexpr auto RCNT_JOYBUS = registral<const_ptr<volatile rcnt_joybus>(0x4000134)>{};
    inline constexpr auto RCNT_GPIO = registral<const_ptr<volatile rcnt_gpio>(0x4000134)>{};

    // Joybus

    inline constexpr auto JOYCNT = registral<const_ptr<volatile joycnt>(0x4000140)>{};
    inline constexpr auto JOY_RECV = registral<const_ptr<volatile u32>(0x4000150)>{};
    inline constexpr auto JOY_TRANS = registral<const_ptr<volatile u32>(0x4000154)>{};
    inline constexpr auto JOYSTAT = registral<const_ptr<volatile joystat>(0x4000158)>{};

    // Interrupts

    inline constexpr auto IE = registral<const_ptr<volatile irq>(0x4000200)>{};
    inline constexpr auto IF = registral<const_ptr<volatile irq>(0x4000202)>{};
    inline constexpr auto WAITCNT = registral<const_ptr<volatile waitcnt>(0x4000204)>{};
    inline constexpr auto IME = registral<const_ptr<volatile bool>(0x4000208)>{};

    // Memory control

    inline constexpr auto MEMCNT = registral<const_ptr<volatile memcnt>(0x4000800)>{};

    // Palette RAM

    inline constexpr auto BG_PALETTE = const_ptr<volatile u16[256]>(0x5000000);
    inline constexpr auto OBJ_PALETTE = const_ptr<volatile u16[256]>(0x5000200);

    // Video RAM

    inline constexpr auto CHARBLOCK0_4BPP = const_ptr<volatile tile4bpp[512]>(0x6000000);
    inline constexpr auto CHARBLOCK1_4BPP = const_ptr<volatile tile4bpp[512]>(0x6004000);
    inline constexpr auto CHARBLOCK2_4BPP = const_ptr<volatile tile4bpp[512]>(0x6009000);
    inline constexpr auto CHARBLOCK3_4BPP = const_ptr<volatile tile4bpp[512]>(0x600C000);

    inline constexpr auto CHARBLOCK0_8BPP = const_ptr<volatile tile8bpp[256]>(0x6000000);
    inline constexpr auto CHARBLOCK1_8BPP = const_ptr<volatile tile8bpp[256]>(0x6004000);
    inline constexpr auto CHARBLOCK2_8BPP = const_ptr<volatile tile8bpp[256]>(0x6009000);
    inline constexpr auto CHARBLOCK3_8BPP = const_ptr<volatile tile8bpp[256]>(0x600C000);

    inline constexpr auto TEXT_SCREENBLOCKS = const_ptr<volatile textscreen[1024]>(0x6000000);

    inline constexpr auto VIDEO3_VRAM = const_ptr<volatile u16[160][240]>(0x6000000);
    inline constexpr auto VIDEO4_VRAM = const_ptr<volatile u8x2[160][120]>(0x6000000);
    inline constexpr auto VIDEO4_VRAM_FRAME1 = VIDEO4_VRAM + 0xa000;

    inline constexpr auto OBJ_TILES = const_ptr<volatile tile4bpp[1024]>(0x6010000);

    // Object Attribute Memory

    inline constexpr auto OBJ_ATTR0 = registral_series<const_ptr<volatile objattr0[128]>(0x7000000), 8>{};
    inline constexpr auto OBJ_ATTR1 = registral_series<const_ptr<volatile objattr1[128]>(0x7000002), 8>{};
    inline constexpr auto OBJ_ATTR2 = registral_series<const_ptr<volatile objattr2[128]>(0x7000004), 8>{};
    inline constexpr auto OBJ_ATTR = registral_series<const_ptr<volatile objattr[128]>(0x7000000), 8>{};

    inline constexpr auto OBJ_ATTR1_AFFINE = registral_series<const_ptr<volatile objattr1_affine[128]>(0x7000002), 8>{};
    inline constexpr auto OBJ_ATTR_AFFINE = registral_series<const_ptr<volatile objattr_affine[128]>(0x7000000), 8>{};

    inline constexpr auto AFFINE_PARAM_A = registral_series<const_ptr<volatile fixed<short, 8>[32]>(0x7000006), 32>{};
    inline constexpr auto AFFINE_PARAM_B = registral_series<const_ptr<volatile fixed<short, 8>[32]>(0x700000E), 32>{};
    inline constexpr auto AFFINE_PARAM_C = registral_series<const_ptr<volatile fixed<short, 8>[32]>(0x7000016), 32>{};
    inline constexpr auto AFFINE_PARAM_D = registral_series<const_ptr<volatile fixed<short, 8>[32]>(0x700001E), 32>{};

    // Cartridge IO

    inline constexpr auto IO_PORT_DATA = registral<const_ptr<volatile u16>(0x080000C4)>{};
    inline constexpr auto IO_PORT_DIRECTION = registral<const_ptr<volatile cartdirection>(0x080000C6)>{};
    inline constexpr auto IO_PORT_CONTROL = registral<const_ptr<volatile cartcontrol>(0x080000C8)>{};

} // namespace gba::mmio

#endif // define GBAXX_MMIO_HPP
