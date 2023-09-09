/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_MMIO_HPP
#define GBAXX_MMIO_HPP
/** @file */

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

    /**
     * @brief Pointer to the IRQ handler function.
     * @see <a href="https://mgba-emu.github.io/gbatek/#default-memory-usage-at-03007fxx-and-mirrored-to-03ffffxx">Default memory usage at 03007FXX (and mirrored to 03FFFFXX)</a>
     *
     * The IRQ handler function is expected to read the raised interrupts via the mmio::IF register, handle the
     * interrupts, and then acknowledged the handled interrupts by writing back to the mmio::IF register.
     *
     * @note The handler function type is `void(*)()`, meaning it has no parameters and returns `void`.
     * @note This "register" is actually located within IWRAM (0x03007FFC).
     * @warning The handler function must be implemented with 32-bit ARM instructions.
     *
     * @code{cpp}
     * // Handle VBlank and keypad interrupts
     * #include <gba/gba.hpp>
     *
     * [[gnu::interrupt("IRQ"), gnu::target("arm")]]
     * void my_handler() {
     *     using namespace gba;
     *
     *     const auto irq = *mmio::IF; // Read the raised IRQs
     *
     *     if (irq.vblank) {
     *         // Handle vertical blank
     *     }
     *     if (irq.keypad) {
     *         // Handle key input
     *     }
     *     // handle other irq flags, etc, etc ...
     *
     *     mmio::IF = irq; // Acknowledge the raised IRQs
     * }
     *
     * int main() {
     *     using namespace gba;
     *
     *     mmio::IRQ_HANDLER = my_handler; // With C++23 my_handler could be inlined as a lambda with the correct attributes
     *
     *     // Enable VBlank and keypad IRQs
     *     mmio::DISPSTAT = {.irq_vblank = true};
     *     mmio::KEYCNT = {.irq_enabled = true};
     *     mmio::IE = {.vblank = true, .keypad = true};
     *
     *     mmio::IME = true; // Activate IRQ interrupt handling
     *
     *     while (true) {
     *         bios::VBlankIntrWait(); // With VBlank interrupt handled, we can now wait for vertical-sync
     *     }
     * }
     * @endcode
     *
     * @sa IF
     * @sa IE
     * @sa IME
     * @sa irq
     */
    inline constexpr auto IRQ_HANDLER = registral<const_ptr<void(* volatile)()>(0x03FFFFFC)>{};

    // Video

    /**
     * @brief Display control register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000000h---dispcnt---lcd-control-readwrite">4000000h - DISPCNT - LCD Control (Read/Write)</a>
     *
     * Controls the current display mode, and which layers (backgrounds, object) are visible, as well as which windows
     * are in use.
     *
     * @code{cpp}
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     mmio::DISPCNT = {.video_mode = 3, .show_bg2 = true};
     *
     *     // Write some pixels to the bitmap
     *     mmio::VIDEO3_VRAM[80][120] = 0x001f;
     *     mmio::VIDEO3_VRAM[80][136] = 0x03e0;
     *     mmio::VIDEO3_VRAM[96][120] = 0x7c00;
     *
     *     while (true);
     * }
     * @endcode
     *
     * @sa dispcnt
     */
    inline constexpr auto DISPCNT = registral<const_ptr<volatile dispcnt>(0x4000000)>{};

    /**
     * @brief Green swap flag.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000002h---undocumented---green-swap-rw">4000002h - Undocumented - Green Swap (R/W)</a>
     *
     * @note This is an undocumented feature.
     */
    inline constexpr auto GREEN_SWAP = registral<const_ptr<volatile bool>(0x4000002)>{};

    /**
     * @brief Display status register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000004h---dispstat---general-lcd-status-readwrite">4000004h - DISPSTAT - General LCD Status (Read/Write)</a>
     *
     * Contains the interrupt status of the display, as well as controls if the display should emit interrupts.
     *
     * @code{cpp}
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     mmio::DISPSTAT = {.irq_vblank = true};
     *
     *     mmio::IRQ_HANDLER = agbabi::irq_empty;
     *     mmio::IE = {.vblank = true};
     *     mmio::IME = true;
     *
     *     while (true) {
     *         bios::VBlankIntrWait();
     *
     *         // Inside vertical blank
     *
     *         while (mmio::DISPSTAT->currently_vblank); // Trap until we exit vblank
     *
     *         // Outside vertical blank
     *     }
     * }
     * @endcode
     *
     * @sa dispstat
     * @sa IRQ_HANDLER
     */
    inline constexpr auto DISPSTAT = registral<const_ptr<volatile dispstat>(0x4000004)>{};

    /**
     * @brief Scanline currently being rendered.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000006h---vcount---vertical-counter-read-only">4000006h - VCOUNT - Vertical Counter (Read only)</a>
     *
     * Values in range from 160..227 indicate ‘hidden’ scanlines within VBlank area.
     *
     * @code{cpp}
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     while (true) {
     *         while (mmio::VCOUNT < 160); // Busy wait for vertical blank (line 160)
     *
     *         // Inside vertical blank
     *
     *         while (*mmio::VCOUNT); // Busy wait for vertical draw (line 0)
     *
     *         // Outside vertical blank
     *     }
     * }
     * @endcode
     *
     * @sa dispstat
     */
    inline constexpr auto VCOUNT = registral<const_ptr<const volatile u16>(0x4000006)>{};

    /**
     * @brief Background 0 control register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000008h---bg0cnt---bg0-control-rw-bg-modes-01-only">4000008h - BG0CNT - BG0 Control (R/W) (BG Modes 0,1 only)</a>
     *
     * @note Background 0 is only available with display modes 0 and 1.
     *
     * @sa bgcnt
     * @sa BG1CNT
     * @sa BG2CNT
     * @sa BG3CNT
     * @sa BGCNT
     */
    inline constexpr auto BG0CNT = registral<const_ptr<volatile bgcnt>(0x4000008)>{};

    /**
     * @brief Background 1 control register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400000ah---bg1cnt---bg1-control-rw-bg-modes-01-only">400000Ah - BG1CNT - BG1 Control (R/W) (BG Modes 0,1 only)</a>
     *
     * @note Background 1 is only available with display modes 0 and 1.
     *
     * @sa bgcnt
     * @sa BG0CNT
     * @sa BG2CNT
     * @sa BG3CNT
     * @sa BGCNT
     */
    inline constexpr auto BG1CNT = registral<const_ptr<volatile bgcnt>(0x400000A)>{};

    /**
     * @brief Background 2 control register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400000ch---bg2cnt---bg2-control-rw-bg-modes-012-only">400000Ch - BG2CNT - BG2 Control (R/W) (BG Modes 0,1,2 only)</a>
     *
     * @note Background 2 is available with all display modes, however the character/tile map is note used with bitmap
     *       graphic modes (modes 3, 4, or 5).
     *
     * @sa bgcnt
     * @sa BG0CNT
     * @sa BG1CNT
     * @sa BG3CNT
     * @sa BGCNT
     */
    inline constexpr auto BG2CNT = registral<const_ptr<volatile bgcnt>(0x400000C)>{};

    /**
     * @brief Background 3 control register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400000eh---bg3cnt---bg3-control-rw-bg-modes-02-only">400000Eh - BG3CNT - BG3 Control (R/W) (BG Modes 0,2 only)</a>
     *
     * @note Background 3 is only available with display modes 0 and 2.
     *
     * @sa bgcnt
     * @sa BG0CNT
     * @sa BG1CNT
     * @sa BG2CNT
     * @sa BGCNT
     */
    inline constexpr auto BG3CNT = registral<const_ptr<volatile bgcnt>(0x400000E)>{};

    /**
     * @brief Background control registers.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000008h---bg0cnt---bg0-control-rw-bg-modes-01-only">4000008h - BG0CNT - BG0 Control (R/W) (BG Modes 0,1 only)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#400000ah---bg1cnt---bg1-control-rw-bg-modes-01-only">400000Ah - BG1CNT - BG1 Control (R/W) (BG Modes 0,1 only)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#400000ch---bg2cnt---bg2-control-rw-bg-modes-012-only">400000Ch - BG2CNT - BG2 Control (R/W) (BG Modes 0,1,2 only)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#400000eh---bg3cnt---bg3-control-rw-bg-modes-02-only">400000Eh - BG3CNT - BG3 Control (R/W) (BG Modes 0,2 only)</a>
     *
     * The four background registers as a registral_series. This allows for treating the background registers as if they
     * are an array.
     *
     * @sa bgcnt
     * @sa BG0CNT
     * @sa BG1CNT
     * @sa BG2CNT
     * @sa BG3CNT
     */
    inline constexpr auto BGCNT = registral_series<const_ptr<volatile bgcnt[4]>(0x4000008)>{};

    /**
     * @brief Background 0 horizontal offset register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000010h---bg0hofs---bg0-x-offset-w">4000010h - BG0HOFS - BG0 X-Offset (W)</a>
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa BG0VOFS
     * @sa BGOFS
     */
    inline constexpr auto BG0HOFS = registral<const_ptr<volatile u16>(0x4000010)>{};

    /**
     * @brief Background 0 vertical offset register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000012h---bg0vofs---bg0-y-offset-w">4000012h - BG0VOFS - BG0 Y-Offset (W)</a>
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa BG0HOFS
     * @sa BGOFS
     */
    inline constexpr auto BG0VOFS = registral<const_ptr<volatile u16>(0x4000012)>{};

    /**
     * @brief Background 1 horizontal offset register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000014h---bg1hofs---bg1-x-offset-w">4000014h - BG1HOFS - BG1 X-Offset (W)</a>
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa BG1VOFS
     * @sa BGOFS
     */
    inline constexpr auto BG1HOFS = registral<const_ptr<volatile u16>(0x4000014)>{};

    /**
     * @brief Background 1 vertical offset register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000016h---bg1vofs---bg1-y-offset-w">4000016h - BG1VOFS - BG1 Y-Offset (W)</a>
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa BG1HOFS
     * @sa BGOFS
     */
    inline constexpr auto BG1VOFS = registral<const_ptr<volatile u16>(0x4000016)>{};

    /**
     * @brief Background 2 horizontal offset register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000018h---bg2hofs---bg2-x-offset-w">4000018h - BG2HOFS - BG2 X-Offset (W)</a>
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa BG2VOFS
     * @sa BGOFS
     */
    inline constexpr auto BG2HOFS = registral<const_ptr<volatile u16>(0x4000018)>{};

    /**
     * @brief Background 2 vertical offset register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400001ah---bg2vofs---bg2-y-offset-w">400001Ah - BG2VOFS - BG2 Y-Offset (W)</a>
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa BG2HOFS
     * @sa BGOFS
     */
    inline constexpr auto BG2VOFS = registral<const_ptr<volatile u16>(0x400001A)>{};

    /**
     * @brief Background 3 horizontal offset register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400001ch---bg3hofs---bg3-x-offset-w">400001Ch - BG3HOFS - BG3 X-Offset (W)</a>
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa BG3VOFS
     * @sa BGOFS
     */
    inline constexpr auto BG3HOFS = registral<const_ptr<volatile u16>(0x400001C)>{};

    /**
     * @brief Background 3 vertical offset register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400001eh---bg3vofs---bg3-y-offset-w">400001Eh - BG3VOFS - BG3 Y-Offset (W)</a>
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa BG3HOFS
     * @sa BGOFS
     */
    inline constexpr auto BG3VOFS = registral<const_ptr<volatile u16>(0x400001E)>{};

    /**
     * @brief Background offset registers.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000010h---bg0hofs---bg0-x-offset-w">4000010h - BG0HOFS - BG0 X-Offset (W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000012h---bg0vofs---bg0-y-offset-w">4000012h - BG0VOFS - BG0 Y-Offset (W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000014h---bg1hofs---bg1-x-offset-w">4000014h - BG1HOFS - BG1 X-Offset (W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000016h---bg1vofs---bg1-y-offset-w">4000016h - BG1VOFS - BG1 Y-Offset (W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000018h---bg2hofs---bg2-x-offset-w">4000018h - BG2HOFS - BG2 X-Offset (W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#400001ah---bg2vofs---bg2-y-offset-w">400001Ah - BG2VOFS - BG2 Y-Offset (W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#400001ch---bg3hofs---bg3-x-offset-w">400001Ch - BG3HOFS - BG3 X-Offset (W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#400001eh---bg3vofs---bg3-y-offset-w">400001Eh - BG3VOFS - BG3 Y-Offset (W)</a>
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa BG0HOFS
     * @sa BG0VOFS
     * @sa BG1HOFS
     * @sa BG1VOFS
     * @sa BG2HOFS
     * @sa BG2VOFS
     * @sa BG3HOFS
     * @sa BG3VOFS
     */
    inline constexpr auto BGOFS = registral_series<const_ptr<volatile make_vector<u16, 2>[4]>(0x4000010)>{};

    /**
     * @brief Background 2 affine matrix "A" register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000020h---bg2pa---bg2-rotationscaling-parameter-a-alias-dx-w">4000020h - BG2PA - BG2 Rotation/Scaling Parameter A (alias dx) (W)</a>
     * @see <a href="https://en.wikipedia.org/wiki/Affine_transformation">Affine transformation - Wikipedia</a>
     *
     * The first element of background 2's affine matrix.
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa BG2PB
     * @sa BG2PC
     * @sa BG2PD
     * @sa BG2X
     * @sa BG2Y
     */
    inline constexpr auto BG2PA = registral<const_ptr<volatile fixed<short, 8>>(0x4000020)>{};

    /**
     * @brief Background 2 affine matrix "B" register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000022h---bg2pb---bg2-rotationscaling-parameter-b-alias-dmx-w">4000022h - BG2PB - BG2 Rotation/Scaling Parameter B (alias dmx) (W)</a>
     * @see <a href="https://en.wikipedia.org/wiki/Affine_transformation">Affine transformation - Wikipedia</a>
     *
     * The second element of background 2's affine matrix.
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa BG2PA
     * @sa BG2PC
     * @sa BG2PD
     * @sa BG2X
     * @sa BG2Y
     */
    inline constexpr auto BG2PB = registral<const_ptr<volatile fixed<short, 8>>(0x4000022)>{};

    /**
     * @brief Background 2 affine matrix "C" register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000024h---bg2pc---bg2-rotationscaling-parameter-c-alias-dy--w">4000024h - BG2PC - BG2 Rotation/Scaling Parameter C (alias dy) (W)</a>
     * @see <a href="https://en.wikipedia.org/wiki/Affine_transformation">Affine transformation - Wikipedia</a>
     *
     * The third element of background 2's affine matrix.
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa BG2PA
     * @sa BG2PB
     * @sa BG2PD
     * @sa BG2X
     * @sa BG2Y
     */
    inline constexpr auto BG2PC = registral<const_ptr<volatile fixed<short, 8>>(0x4000024)>{};

    /**
     * @brief Background 2 affine matrix "D" register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000026h---bg2pd---bg2-rotationscaling-parameter-d-alias-dmy-w">4000026h - BG2PD - BG2 Rotation/Scaling Parameter D (alias dmy) (W)</a>
     * @see <a href="https://en.wikipedia.org/wiki/Affine_transformation">Affine transformation - Wikipedia</a>
     *
     * The fourth element of background 2's affine matrix.
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa BG2PA
     * @sa BG2PB
     * @sa BG2PC
     * @sa BG2X
     * @sa BG2Y
     */
    inline constexpr auto BG2PD = registral<const_ptr<volatile fixed<short, 8>>(0x4000026)>{};

    /**
     * @brief Background 2 affine matrix "X" register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000028h---bg2x_l---bg2-reference-point-x-coordinate-lower-16-bit-w">4000028h - BG2X_L - BG2 Reference Point X-Coordinate, lower 16 bit (W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#400002ah---bg2x_h---bg2-reference-point-x-coordinate-upper-12-bit-w">400002Ah - BG2X_H - BG2 Reference Point X-Coordinate, upper 12 bit (W)</a>
     * @see <a href="https://en.wikipedia.org/wiki/Affine_transformation">Affine transformation - Wikipedia</a>
     *
     * The horizontal translation of the matrix.
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa BG2PA
     * @sa BG2PB
     * @sa BG2PC
     * @sa BG2PD
     * @sa BG2Y
     */
    inline constexpr auto BG2X = registral<const_ptr<volatile fixed<int, 8>>(0x4000028)>{};

    /**
     * @brief Background 2 affine matrix "Y" register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400002ch---bg2y_l---bg2-reference-point-y-coordinate-lower-16-bit-w">400002Ch - BG2Y_L - BG2 Reference Point Y-Coordinate, lower 16 bit (W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#400002eh---bg2y_h---bg2-reference-point-y-coordinate-upper-12-bit-w">400002Eh - BG2Y_H - BG2 Reference Point Y-Coordinate, upper 12 bit (W)</a>
     * @see <a href="https://en.wikipedia.org/wiki/Affine_transformation">Affine transformation - Wikipedia</a>
     *
     * The vertical translation of the matrix.
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa BG2PA
     * @sa BG2PB
     * @sa BG2PC
     * @sa BG2PD
     * @sa BG2X
     */
    inline constexpr auto BG2Y = registral<const_ptr<volatile fixed<int, 8>>(0x400002C)>{};

    /**
     * @brief Background 3 affine matrix "A" register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400003xh---bg3x_lh-bg3y_lh-bg3pa-d---bg3-rotationscaling-parameters">400003Xh - BG3X_L/H, BG3Y_L/H, BG3PA-D - BG3 Rotation/Scaling Parameters</a>
     * @see <a href="https://en.wikipedia.org/wiki/Affine_transformation">Affine transformation - Wikipedia</a>
     *
     * The first element of background 3's affine matrix.
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa BG3PB
     * @sa BG3PC
     * @sa BG3PD
     * @sa BG3X
     * @sa BG3Y
     */
    inline constexpr auto BG3PA = registral<const_ptr<volatile fixed<short, 8>>(0x4000030)>{};

    /**
     * @brief Background 3 affine matrix "B" register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400003xh---bg3x_lh-bg3y_lh-bg3pa-d---bg3-rotationscaling-parameters">400003Xh - BG3X_L/H, BG3Y_L/H, BG3PA-D - BG3 Rotation/Scaling Parameters</a>
     * @see <a href="https://en.wikipedia.org/wiki/Affine_transformation">Affine transformation - Wikipedia</a>
     *
     * The second element of background 3's affine matrix.
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa BG3PA
     * @sa BG3PC
     * @sa BG3PD
     * @sa BG3X
     * @sa BG3Y
     */
    inline constexpr auto BG3PB = registral<const_ptr<volatile fixed<short, 8>>(0x4000032)>{};

    /**
     * @brief Background 3 affine matrix "C" register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400003xh---bg3x_lh-bg3y_lh-bg3pa-d---bg3-rotationscaling-parameters">400003Xh - BG3X_L/H, BG3Y_L/H, BG3PA-D - BG3 Rotation/Scaling Parameters</a>
     * @see <a href="https://en.wikipedia.org/wiki/Affine_transformation">Affine transformation - Wikipedia</a>
     *
     * The third element of background 3's affine matrix.
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa BG3PA
     * @sa BG3PB
     * @sa BG3PD
     * @sa BG3X
     * @sa BG3Y
     */
    inline constexpr auto BG3PC = registral<const_ptr<volatile fixed<short, 8>>(0x4000034)>{};

    /**
     * @brief Background 3 affine matrix "D" register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400003xh---bg3x_lh-bg3y_lh-bg3pa-d---bg3-rotationscaling-parameters">400003Xh - BG3X_L/H, BG3Y_L/H, BG3PA-D - BG3 Rotation/Scaling Parameters</a>
     * @see <a href="https://en.wikipedia.org/wiki/Affine_transformation">Affine transformation - Wikipedia</a>
     *
     * The fourth element of background 3's affine matrix.
     *
     * @sa BG3PA
     * @sa BG3PB
     * @sa BG3PC
     * @sa BG3X
     * @sa BG3Y
     */
    inline constexpr auto BG3PD = registral<const_ptr<volatile fixed<short, 8>>(0x4000036)>{};

    /**
     * @brief Background 3 affine matrix "X" register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400003xh---bg3x_lh-bg3y_lh-bg3pa-d---bg3-rotationscaling-parameters">400003Xh - BG3X_L/H, BG3Y_L/H, BG3PA-D - BG3 Rotation/Scaling Parameters</a>
     * @see <a href="https://en.wikipedia.org/wiki/Affine_transformation">Affine transformation - Wikipedia</a>
     *
     * The horizontal translation of the matrix.
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa BG3PA
     * @sa BG3PB
     * @sa BG3PC
     * @sa BG3PD
     * @sa BG3Y
     */
    inline constexpr auto BG3X = registral<const_ptr<volatile fixed<int, 8>>(0x4000038)>{};

    /**
     * @brief Background 3 affine matrix "Y" register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400003xh---bg3x_lh-bg3y_lh-bg3pa-d---bg3-rotationscaling-parameters">400003Xh - BG3X_L/H, BG3Y_L/H, BG3PA-D - BG3 Rotation/Scaling Parameters</a>
     * @see <a href="https://en.wikipedia.org/wiki/Affine_transformation">Affine transformation - Wikipedia</a>
     *
     * The vertical translation of the matrix.
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa BG3PA
     * @sa BG3PB
     * @sa BG3PC
     * @sa BG3PD
     * @sa BG3X
     */
    inline constexpr auto BG3Y = registral<const_ptr<volatile fixed<int, 8>>(0x400003C)>{};

    /**
     * @brief Window 0 horizontal dimensions register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000040h---win0h---window-0-horizontal-dimensions-w">4000040h - WIN0H - Window 0 Horizontal Dimensions (W)</a>
     *
     * The second element is the right-most edge of the window (exclusive, so +1, with a maximum of 240).
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa WIN0V
     * @sa WININ
     * @sa WINOUT
     */
    inline constexpr auto WIN0H = registral<const_ptr<volatile u8x2>(0x4000040)>{};

    /**
     * @brief Window 1 horizontal dimensions register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000042h---win1h---window-1-horizontal-dimensions-w">4000042h - WIN1H - Window 1 Horizontal Dimensions (W)</a>
     *
     * The second element is the right-most edge of the window (exclusive, so +1, with a maximum of 240).
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa WIN1V
     * @sa WININ
     * @sa WINOUT
     */
    inline constexpr auto WIN1H = registral<const_ptr<volatile u8x2>(0x4000042)>{};

    /**
     * @brief Window 0 vertical dimensions register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000044h---win0v---window-0-vertical-dimensions-w">4000044h - WIN0V - Window 0 Vertical Dimensions (W)</a>
     *
     * The second element is the bottom-most edge of the window (exclusive, so +1, with a maximum of 160).
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa WIN0H
     * @sa WININ
     * @sa WINOUT
     */
    inline constexpr auto WIN0V = registral<const_ptr<volatile u8x2>(0x4000044)>{};

    /**
     * @brief Window 1 vertical dimensions register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000046h---win1v---window-1-vertical-dimensions-w">4000046h - WIN1V - Window 1 Vertical Dimensions (W)</a>
     *
     * The second element is the bottom-most edge of the window (exclusive, so +1, with a maximum of 160).
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @sa WIN1H
     * @sa WININ
     * @sa WINOUT
     */
    inline constexpr auto WIN1V = registral<const_ptr<volatile u8x2>(0x4000046)>{};

    /**
     * @brief Window "inside" control register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000048h---winin---control-of-inside-of-windows-rw">4000048h - WININ - Control of Inside of Window(s) (R/W)</a>
     *
     * Controls the graphical effects for elements considered "inside" a window.
     *
     * @code{cpp}
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     mmio::DISPCNT = {.video_mode = 0, .show_bg0 = true, .enable_win0 = true};
     *
     *     // Clip a 32 pixel border around the display
     *     mmio::WIN0H = u8x2{32, gba::screen_width - 32};
     *     mmio::WIN0V = u8x2{32, gba::screen_height - 32};
     *
     *     mmio::WININ = {.win0_bg0 = true};
     *
     *     // Note: Some graphics on background 0 is required for this sample to be effective
     *     while (true);
     * }
     * @endcode
     *
     * @sa winin
     * @sa WIN0H
     * @sa WIN1H
     * @sa WIN0V
     * @sa WIN1V
     * @sa WINOUT
     */
    inline constexpr auto WININ = registral<const_ptr<volatile winin>(0x4000048)>{};

    /**
     * @brief Window "outside" control register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400004ah---winout---control-of-outside-of-windows--inside-of-obj-window-rw">400004Ah - WINOUT - Control of Outside of Windows & Inside of OBJ Window (R/W)</a>
     *
     * Controls the graphical effects for elements considered "outside" a window, as well as the object layer.
     *
     * @code{cpp}
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     mmio::DISPCNT = {.video_mode = 0, .show_bg0 = true, .enable_win0 = true, .enable_win1 = true};
     *
     *     // Mask the upper left of the display
     *     mmio::WIN0H = u8x2{0, gba::screen_width / 2};
     *     mmio::WIN0V = u8x2{0, gba::screen_height / 2};
     *
     *     // Also mask the lower right of the display
     *     mmio::WIN1H = u8x2{gba::screen_width / 2, gba::screen_width};
     *     mmio::WIN1V = u8x2{gba::screen_height / 2, gba::screen_height};
     *
     *     mmio::WINOUT = {.outside_bg0 = true};
     *
     *     // Note: Some graphics on background 0 is required for this sample to be effective
     *     while (true);
     * }
     * @endcode
     *
     * @sa winout
     * @sa WIN0H
     * @sa WIN1H
     * @sa WIN0V
     * @sa WIN1V
     * @sa WININ
     */
    inline constexpr auto WINOUT = registral<const_ptr<volatile winout>(0x400004A)>{};

    /**
     * @brief Mosaic control register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400004ch---mosaic---mosaic-size-w">400004Ch - MOSAIC - Mosaic Size (W)</a>
     *
     * The mosaic effect repeats pixels N times over to create "chunky pixels". The common use for this is to simulate a
     * crude "blur" visual effect (such as when a game character is receiving damage, or losing consciousness).
     *
     * @warning This is a write-only register, a separate shadow variable should be used for read & modify operations
     *          before being written to this register.
     *
     * @code{cpp}
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     mmio::DISPCNT = {.video_mode = 0, .show_bg0 = true};
     *     mmio::BG0CNT = {.mosaic = true};
     *
     *     auto effect = mosaic{}; // Shadow variable for tracking the current mosaic level
     *
     *     while (true) {
     *         mmio::MOSAIC = effect; // Write the current mosaic effect
     *
     *         // Grow the mosaic
     *         effect.bg_h_extra += 1;
     *         effect.bg_v_extra += 1;
     *
     *         while (mmio::VCOUNT < 160); // Busy-wait for vertical blank
     *     }
     * }
     * @endcode
     *
     * @sa mosaic
     */
    inline constexpr auto MOSAIC = registral<const_ptr<volatile mosaic>(0x400004C)>{};

    /**
     * @brief Blend effect control register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000050h---bldcnt---color-special-effects-selection-rw">4000050h - BLDCNT - Color Special Effects Selection (R/W)</a>
     *
     * @code{cpp}
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     mmio::DISPCNT = {.video_mode = 0, .show_bg0 = true};
     *
     *     mmio::BLDCNT = bldcnt{
     *         .target1_bg0 = true,
     *         .mode = gba::color_effect::brighten
     *     };
     *
     *     auto gamma = decltype(mmio::BLDY)::value_type{}; // Shadow variable for tracking the current gamma
     *
     *     keystate keys{};
     *     while (true) {
     *         gamma.data() += (keys = *mmio::KEYINPUT).yaxis();
     *
     *         mmio::BLDY = gamma;
     *     }
     * }
     * @endcode
     *
     * @sa bldcnt
     */
    inline constexpr auto BLDCNT = registral<const_ptr<volatile bldcnt>(0x4000050)>{};

    /**
     * @brief Blend effect alpha value register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000052h---bldalpha---alpha-blending-coefficients-rw-not-w">4000052h - BLDALPHA - Alpha Blending Coefficients (R/W) (not W)</a>
     *
     * Used when bldcnt::mode is set to color_effect::alpha_blend.
     *
     * @note Some documentation refers to the two elements of this register as EVA and EVB, possibly meaning "exposure
     *       value A/B" in reference to a double-exposure in photography.
     *
     * @code{cpp}
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     mmio::DISPCNT = {.video_mode = 0, .show_bg0 = true};
     *
     *     mmio::BLDCNT = bldcnt{
     *         .target1_backdrop = true,
     *         .mode = gba::color_effect::alpha_blend,
     *         .target2_bg0 = true
     *     };
     *
     *     auto alpha = fixed<int, 5>{}; // Shadow variable for tracking the current alpha
     *
     *     keystate keys{};
     *     while (true) {
     *         alpha.data() += (keys = *mmio::KEYINPUT).yaxis();
     *
     *         // EVA = alpha, EVB = 1 - alpha
     *         mmio::BLDALPHA = fixed<u8x2, 5>{alpha, 1 - alpha};
     *     }
     * }
     * @endcode
     *
     * @sa bldcnt
     */
    inline constexpr auto BLDALPHA = registral<const_ptr<volatile fixed<u8x2, 5>>(0x4000052)>{};

    /**
     * @brief Blend effect gamma register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000054h---bldy---brightness-fade-inout-coefficient-w-not-rw">4000054h - BLDY - Brightness (Fade-In/Out) Coefficient (W) (not R/W)</a>
     *
     * @note Some documentation refers to the element of this register as EVY, possibly meaning "exposure gamma" (where
     *       the "Y" resembles a Greek gamma) in reference to increasing/decreasing exposure in photography.
     *
     * @code{cpp}
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     mmio::DISPCNT = {.video_mode = 0, .show_bg0 = true};
     *
     *     mmio::BLDCNT = bldcnt{
     *         .target1_bg0 = true,
     *         .mode = gba::color_effect::brighten
     *     };
     *
     *     auto gamma = decltype(mmio::BLDY)::value_type{}; // Shadow variable for tracking the current gamma
     *
     *     keystate keys{};
     *     while (true) {
     *         gamma.data() += (keys = *mmio::KEYINPUT).yaxis();
     *
     *         mmio::BLDY = gamma;
     *     }
     * }
     * @endcode
     *
     * @sa bldcnt
     */
    inline constexpr auto BLDY = registral<const_ptr<volatile fixed<u16, 4>>(0x4000054)>{};

    // Sound

    /**
     * @brief Sound tone 1 sweep register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000060h---sound1cnt_l-nr10---channel-1-sweep-register-rw">4000060h - SOUND1CNT_L (NR10) - Channel 1 Sweep register (R/W)</a>
     *
     * @sa TONE1_PATTERN
     * @sa TONE1_FREQUENCY
     * @sa sound1cnt_l
     */
    inline constexpr auto TONE1_SWEEP = registral<const_ptr<volatile sound1cnt_l>(0x4000060)>{};

    /**
     * @brief Sound tone 1 pattern register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000062h---sound1cnt_h-nr11-nr12---channel-1-dutylenenvelope-rw">4000062h - SOUND1CNT_H (NR11, NR12) - Channel 1 Duty/Len/Envelope (R/W)</a>
     *
     * @sa TONE1_SWEEP
     * @sa TONE1_FREQUENCY
     * @sa sound1cnt_h
     */
    inline constexpr auto TONE1_PATTERN = registral<const_ptr<volatile sound1cnt_h>(0x4000062)>{};

    /**
     * @brief Sound tone 1 frequency register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000064h---sound1cnt_x-nr13-nr14---channel-1-frequencycontrol-rw">4000064h - SOUND1CNT_X (NR13, NR14) - Channel 1 Frequency/Control (R/W)</a>
     *
     * @sa TONE1_SWEEP
     * @sa TONE1_PATTERN
     * @sa sound1cnt_x
     */
    inline constexpr auto TONE1_FREQUENCY = registral<const_ptr<volatile sound1cnt_x>(0x4000064)>{};

    /**
     * @brief Sound tone 2 pattern register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000068h---sound2cnt_l-nr21-nr22---channel-2-dutylengthenvelope-rw">4000068h - SOUND2CNT_L (NR21, NR22) - Channel 2 Duty/Length/Envelope (R/W)</a>
     *
     * @sa TONE2_FREQUENCY
     * @sa sound2cnt_l
     */
    inline constexpr auto TONE2_PATTERN = registral<const_ptr<volatile sound2cnt_l>(0x4000068)>{};

    /**
     * @brief Sound tone 2 frequency register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400006ch---sound2cnt_h-nr23-nr24---channel-2-frequencycontrol-rw">400006Ch - SOUND2CNT_H (NR23, NR24) - Channel 2 Frequency/Control (R/W)</a>
     *
     * @sa TONE2_PATTERN
     * @sa sound2cnt_h
     */
    inline constexpr auto TONE2_FREQUENCY = registral<const_ptr<volatile sound2cnt_h>(0x400006C)>{};

    /**
     * @brief Sound wave bank register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000070h---sound3cnt_l-nr30---channel-3-stopwave-ram-select-rw">4000070h - SOUND3CNT_L (NR30) - Channel 3 Stop/Wave RAM select (R/W)</a>
     *
     * @sa WAVE_LEN_VOLUME
     * @sa WAVE_FREQ
     * @sa sound3cnt_l
     */
    inline constexpr auto WAVE_BANK = registral<const_ptr<volatile sound3cnt_l>(0x4000070)>{};

    /**
     * @brief Sound wave length & volume register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000072h---sound3cnt_h-nr31-nr32---channel-3-lengthvolume-rw">4000072h - SOUND3CNT_H (NR31, NR32) - Channel 3 Length/Volume (R/W)</a>
     *
     * @sa WAVE_BANK
     * @sa WAVE_FREQ
     * @sa sound3cnt_h
     */
    inline constexpr auto WAVE_LEN_VOLUME = registral<const_ptr<volatile sound3cnt_h>(0x4000072)>{};

    /**
     * @brief Sound wave frequency register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000074h---sound3cnt_x-nr33-nr34---channel-3-frequencycontrol-rw">4000074h - SOUND3CNT_X (NR33, NR34) - Channel 3 Frequency/Control (R/W)</a>
     *
     * @sa WAVE_BANK
     * @sa WAVE_LEN_VOLUME
     * @sa sound3cnt_x
     */
    inline constexpr auto WAVE_FREQ = registral<const_ptr<volatile sound3cnt_x>(0x4000074)>{};

    /**
     * @brief Sound noise length & envelope register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000078h---sound4cnt_l-nr41-nr42---channel-4-lengthenvelope-rw">4000078h - SOUND4CNT_L (NR41, NR42) - Channel 4 Length/Envelope (R/W)</a>
     *
     * @sa NOISE_FREQ
     * @sa sound4cnt_l
     */
    inline constexpr auto NOISE_LEN_ENV = registral<const_ptr<volatile sound4cnt_l>(0x4000078)>{};

    /**
     * @brief Sound noise frequency register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400007ch---sound4cnt_h-nr43-nr44---channel-4-frequencycontrol-rw">400007Ch - SOUND4CNT_H (NR43, NR44) - Channel 4 Frequency/Control (R/W)</a>
     *
     * @sa NOISE_LEN_ENV
     * @sa sound4cnt_h
     */
    inline constexpr auto NOISE_FREQ = registral<const_ptr<volatile sound4cnt_h>(0x400007C)>{};

    /**
     * @brief Stereo volume register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000080h---soundcnt_l-nr50-nr51---channel-lr-volumeenable-rw">4000080h - SOUNDCNT_L (NR50, NR51) - Channel L/R Volume/Enable (R/W)</a>
     *
     * @sa SOUND_MIX
     * @sa SOUND_ENABLED
     * @sa SOUNDBIAS
     * @sa soundcnt_l
     */
    inline constexpr auto LEFT_RIGHT_VOLUME = registral<const_ptr<volatile soundcnt_l>(0x4000080)>{};

    /**
     * @brief Sound channel mixing register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000082h---soundcnt_h-gba-only---dma-sound-controlmixing-rw">4000082h - SOUNDCNT_H (GBA only) - DMA Sound Control/Mixing (R/W)</a>
     *
     * @sa LEFT_RIGHT_VOLUME
     * @sa SOUND_ENABLED
     * @sa SOUNDBIAS
     * @sa soundcnt_h
     */
    inline constexpr auto SOUND_MIX = registral<const_ptr<volatile soundcnt_h>(0x4000082)>{};

    /**
     * @brief Sound enable & status register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000084h---soundcnt_x-nr52---sound-onoff-rw">4000084h - SOUNDCNT_X (NR52) - Sound on/off (R/W)</a>
     *
     * @sa LEFT_RIGHT_VOLUME
     * @sa SOUND_MIX
     * @sa SOUNDBIAS
     * @sa soundcnt_x
     */
    inline constexpr auto SOUND_ENABLED = registral<const_ptr<volatile soundcnt_x>(0x4000084)>{};

    /**
     * @brief Sound output hardware control register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000088h---soundbias---sound-pwm-control-rw-see-below">4000088h - SOUNDBIAS - Sound PWM Control (R/W, see below)</a>
     *
     * @sa LEFT_RIGHT_VOLUME
     * @sa SOUND_MIX
     * @sa SOUND_ENABLED
     * @sa soundbias
     */
    inline constexpr auto SOUNDBIAS = registral<const_ptr<volatile soundbias>(0x4000088)>{};

    /**
     * @brief Sound FIFO buffer A.
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000a0h---fifo_a_l---sound-a-fifo-data-0-and-data-1-w">40000A0h - FIFO_A_L - Sound A FIFO, Data 0 and Data 1 (W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000a2h---fifo_a_h---sound-a-fifo-data-2-and-data-3-w">40000A2h - FIFO_A_H - Sound A FIFO, Data 2 and Data 3 (W)</a>
     *
     * @sa FIFO_B
     */
    inline constexpr auto FIFO_A = registral<const_ptr<volatile u32>(0x40000A0)>{};

    /**
     * @brief Sound FIFO buffer B.
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000a4h---fifo_b_l---sound-b-fifo-data-0-and-data-1-w">40000A4h - FIFO_B_L - Sound B FIFO, Data 0 and Data 1 (W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000a6h---fifo_b_h---sound-b-fifo-data-2-and-data-3-w">40000A6h - FIFO_B_H - Sound B FIFO, Data 2 and Data 3 (W)</a>
     *
     * @sa FIFO_A
     */
    inline constexpr auto FIFO_B = registral<const_ptr<volatile u32>(0x40000A4)>{};

    // DMA

    /**
     * @brief Source address for DMA 0.
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000b0h0b2h---dma0sad---dma-0-source-address-w-internal-memory">40000B0h,0B2h - DMA0SAD - DMA 0 Source Address (W) (internal memory)</a>
     *
     * @warning Cannot transfer from ROM.
     *
     * @sa DMA0_DEST
     * @sa DMA0_COUNT
     * @sa DMA0_CONTROL
     * @sa DMA1_SRC
     * @sa DMA2_SRC
     * @sa DMA3_SRC
     * @sa DMA_SRC
     */
    inline constexpr auto DMA0_SRC = registral<const_ptr<const void* volatile>(0x40000B0)>{};

    /**
     * @brief Destination address for DMA 0.
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000b4h0b6h---dma0dad---dma-0-destination-address-w-internal-memory">40000B4h,0B6h - DMA0DAD - DMA 0 Destination Address (W) (internal memory)</a>
     *
     * @warning Cannot transfer to ROM.
     *
     * @sa DMA0_SRC
     * @sa DMA0_COUNT
     * @sa DMA0_CONTROL
     * @sa DMA1_DEST
     * @sa DMA2_DEST
     * @sa DMA3_DEST
     * @sa DMA_DEST
     */
    inline constexpr auto DMA0_DEST = registral<const_ptr<void* volatile>(0x40000B4)>{};

    /**
     * @brief Transfer count register for DMA 0.
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000b8h---dma0cnt_l---dma-0-word-count-w-14-bit-14000h">40000B8h - DMA0CNT_L - DMA 0 Word Count (W) (14 bit, 1..4000h)</a>
     *
     * This is the number of units to be transferred: 16 or 32 bits, depending on the value of dmacnt_h::transfer_32bit.
     *
     * @warning Only the lower 14 bits are used.
     *
     * @sa DMA0_SRC
     * @sa DMA0_DEST
     * @sa DMA0_CONTROL
     * @sa DMA1_COUNT
     * @sa DMA2_COUNT
     * @sa DMA3_COUNT
     * @sa DMA_COUNT
     */
    inline constexpr auto DMA0_COUNT = registral<const_ptr<volatile u16>(0x40000B8)>{};

    /**
     * @brief Control register for DMA 0.
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000bah---dma0cnt_h---dma-0-control-rw">40000BAh - DMA0CNT_H - DMA 0 Control (R/W)</a>
     *
     * @sa DMA0_SRC
     * @sa DMA0_DEST
     * @sa DMA0_COUNT
     * @sa DMA1_CONTROL
     * @sa DMA2_CONTROL
     * @sa DMA3_CONTROL
     * @sa DMA_CONTROL
     * @sa dmacnt_h
     */
    inline constexpr auto DMA0_CONTROL = registral<const_ptr<volatile dmacnt_h>(0x40000BA)>{};

    /**
     * @brief Source address for DMA 1.
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000bch0beh---dma1sad---dma-1-source-address-w-any-memory">40000BCh,0BEh - DMA1SAD - DMA 1 Source Address (W) (any memory)</a>
     *
     * @sa DMA1_DEST
     * @sa DMA1_COUNT
     * @sa DMA1_CONTROL
     * @sa DMA0_SRC
     * @sa DMA2_SRC
     * @sa DMA3_SRC
     * @sa DMA_SRC
     */
    inline constexpr auto DMA1_SRC = registral<const_ptr<const void* volatile>(0x40000BC)>{};

    /**
     * @brief Destination address for DMA 1.
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000c0h0c2h---dma1dad---dma-1-destination-address-w-internal-memory">40000C0h,0C2h - DMA1DAD - DMA 1 Destination Address (W) (internal memory)</a>
     *
     * @warning Cannot transfer to ROM.
     *
     * @sa DMA1_SRC
     * @sa DMA1_COUNT
     * @sa DMA1_CONTROL
     * @sa DMA0_DEST
     * @sa DMA2_DEST
     * @sa DMA3_DEST
     * @sa DMA_DEST
     */
    inline constexpr auto DMA1_DEST = registral<const_ptr<void* volatile>(0x40000C0)>{};

    /**
     * @brief Transfer count register for DMA 1.
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000c4h---dma1cnt_l---dma-1-word-count-w-14-bit-14000h">40000C4h - DMA1CNT_L - DMA 1 Word Count (W) (14 bit, 1..4000h)</a>
     *
     * This is the number of units to be transferred: 16 or 32 bits, depending on the value of dmacnt_h::transfer_32bit.
     *
     * @warning Only the lower 14 bits are used.
     *
     * @sa DMA1_SRC
     * @sa DMA1_DEST
     * @sa DMA1_CONTROL
     * @sa DMA0_COUNT
     * @sa DMA2_COUNT
     * @sa DMA3_COUNT
     * @sa DMA_COUNT
     */
    inline constexpr auto DMA1_COUNT = registral<const_ptr<volatile u16>(0x40000C4)>{};

    /**
     * @brief Control register for DMA 1.
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000c6h---dma1cnt_h---dma-1-control-rw">40000C6h - DMA1CNT_H - DMA 1 Control (R/W)</a>
     *
     * @sa DMA1_SRC
     * @sa DMA1_DEST
     * @sa DMA1_COUNT
     * @sa DMA0_CONTROL
     * @sa DMA2_CONTROL
     * @sa DMA3_CONTROL
     * @sa DMA_CONTROL
     * @sa dmacnt_h
     */
    inline constexpr auto DMA1_CONTROL = registral<const_ptr<volatile dmacnt_h>(0x40000C6)>{};

    /**
     * @brief Source address for DMA 2.
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000c8h0cah---dma2sad---dma-2-source-address-w-any-memory">40000C8h,0CAh - DMA2SAD - DMA 2 Source Address (W) (any memory)</a>
     *
     * @sa DMA2_DEST
     * @sa DMA2_COUNT
     * @sa DMA2_CONTROL
     * @sa DMA0_SRC
     * @sa DMA1_SRC
     * @sa DMA3_SRC
     * @sa DMA_SRC
     */
    inline constexpr auto DMA2_SRC = registral<const_ptr<const void* volatile>(0x40000C8)>{};

    /**
     * @brief Destination address for DMA 2.
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000cch0ceh---dma2dad---dma-2-destination-address-w-internal-memory">40000CCh,0CEh - DMA2DAD - DMA 2 Destination Address (W) (internal memory)</a>
     *
     * @warning Cannot transfer to ROM.
     *
     * @sa DMA2_SRC
     * @sa DMA2_COUNT
     * @sa DMA2_CONTROL
     * @sa DMA0_DEST
     * @sa DMA1_DEST
     * @sa DMA3_DEST
     * @sa DMA_DEST
     */
    inline constexpr auto DMA2_DEST = registral<const_ptr<void* volatile>(0x40000CC)>{};

    /**
     * @brief Transfer count register for DMA 2.
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000d0h---dma2cnt_l---dma-2-word-count-w-14-bit-14000h">40000D0h - DMA2CNT_L - DMA 2 Word Count (W) (14 bit, 1..4000h)</a>
     *
     * This is the number of units to be transferred: 16 or 32 bits, depending on the value of dmacnt_h::transfer_32bit.
     *
     * @warning Only the lower 14 bits are used.
     *
     * @sa DMA2_SRC
     * @sa DMA2_DEST
     * @sa DMA2_CONTROL
     * @sa DMA0_COUNT
     * @sa DMA1_COUNT
     * @sa DMA3_COUNT
     * @sa DMA_COUNT
     */
    inline constexpr auto DMA2_COUNT = registral<const_ptr<volatile u16>(0x40000D0)>{};

    /**
     * @brief Control register for DMA 2.
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000d2h---dma2cnt_h---dma-2-control-rw">40000D2h - DMA2CNT_H - DMA 2 Control (R/W)</a>
     *
     * @sa DMA2_SRC
     * @sa DMA2_DEST
     * @sa DMA2_COUNT
     * @sa DMA0_CONTROL
     * @sa DMA1_CONTROL
     * @sa DMA3_CONTROL
     * @sa DMA_CONTROL
     * @sa dmacnt_h
     */
    inline constexpr auto DMA2_CONTROL = registral<const_ptr<volatile dmacnt_h>(0x40000D2)>{};

    /**
     * @brief Source address for DMA 3.
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000d4h0d6h---dma3sad---dma-3-source-address-w-any-memory">40000D4h,0D6h - DMA3SAD - DMA 3 Source Address (W) (any memory)</a>
     *
     * @sa DMA3_DEST
     * @sa DMA3_COUNT
     * @sa DMA3_CONTROL
     * @sa DMA0_SRC
     * @sa DMA1_SRC
     * @sa DMA2_SRC
     * @sa DMA_SRC
     */
    inline constexpr auto DMA3_SRC = registral<const_ptr<const void* volatile>(0x40000D4)>{};

    /**
     * @brief Destination address for DMA 3.
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000d8h0dah---dma3dad---dma-3-destination-address-w-any-memory">40000D8h,0DAh - DMA3DAD - DMA 3 Destination Address (W) (any memory)</a>
     *
     * @sa DMA3_SRC
     * @sa DMA3_COUNT
     * @sa DMA3_CONTROL
     * @sa DMA0_DEST
     * @sa DMA1_DEST
     * @sa DMA2_DEST
     * @sa DMA_DEST
     */
    inline constexpr auto DMA3_DEST = registral<const_ptr<void* volatile>(0x40000D8)>{};

    /**
     * @brief Transfer count register for DMA 3.
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000dch---dma3cnt_l---dma-3-word-count-w-16-bit-110000h">40000DCh - DMA3CNT_L - DMA 3 Word Count (W) (16 bit, 1..10000h)</a>
     *
     * This is the number of units to be transferred: 16 or 32 bits, depending on the value of dmacnt_h::transfer_32bit.
     *
     * @warning Only the lower 14 bits are used.
     *
     * @sa DMA3_SRC
     * @sa DMA3_DEST
     * @sa DMA3_CONTROL
     * @sa DMA0_COUNT
     * @sa DMA1_COUNT
     * @sa DMA2_COUNT
     * @sa DMA_COUNT
     */
    inline constexpr auto DMA3_COUNT = registral<const_ptr<volatile u16>(0x40000DC)>{};

    /**
     * @brief Control register for DMA 3.
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000deh---dma3cnt_h---dma-3-control-rw">40000DEh - DMA3CNT_H - DMA 3 Control (R/W)</a>
     *
     * @note DMA3 is the most appropriate DMA register for general purpose transfers.
     *
     * @code{cpp}
     * #include <gba/gba.hpp>
     *
     * extern const std::array<int, 1024> some_data;
     *
     * int main() {
     *     using namespace gba;
     *
     *     std::array<int, 1024> destination{};
     *
     *     mmio::DMA3_SRC = some_data.data();
     *     mmio::DMA3_DEST = destination.data();
     *     mmio::DMA3_COUNT = destination.size(); // Number of 32-bit transfers
     *
     *     // Begin 32-bit transfer
     *     mmio::DMA3_CONTROL = dmacnt_h{
     *         .transfer_32bit = true,
     *         .enabled = true
     *     };
     *
     *     while (true);
     * }
     * @endcode
     *
     * @sa DMA3_SRC
     * @sa DMA3_DEST
     * @sa DMA3_COUNT
     * @sa DMA0_CONTROL
     * @sa DMA1_CONTROL
     * @sa DMA2_CONTROL
     * @sa DMA_CONTROL
     * @sa dmacnt_h
     */
    inline constexpr auto DMA3_CONTROL = registral<const_ptr<volatile dmacnt_h>(0x40000DE)>{};

    /**
     * @brief DMA source registers.
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000b0h0b2h---dma0sad---dma-0-source-address-w-internal-memory">40000B0h,0B2h - DMA0SAD - DMA 0 Source Address (W) (internal memory)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000bch0beh---dma1sad---dma-1-source-address-w-any-memory">40000BCh,0BEh - DMA1SAD - DMA 1 Source Address (W) (any memory)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000c8h0cah---dma2sad---dma-2-source-address-w-any-memory">40000C8h,0CAh - DMA2SAD - DMA 2 Source Address (W) (any memory)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000d4h0d6h---dma3sad---dma-3-source-address-w-any-memory">40000D4h,0D6h - DMA3SAD - DMA 3 Source Address (W) (any memory)</a>
     *
     * The four DMA source registers as a registral_series.
     *
     * @sa DMA0_SRC
     * @sa DMA1_SRC
     * @sa DMA2_SRC
     * @sa DMA3_SRC
     */
    inline constexpr auto DMA_SRC = registral_series<const_ptr<const void* volatile[4]>(0x40000B0), 12>{};

    /**
     * @brief DMA destination registers.
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000b4h0b6h---dma0dad---dma-0-destination-address-w-internal-memory">40000B4h,0B6h - DMA0DAD - DMA 0 Destination Address (W) (internal memory)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000c0h0c2h---dma1dad---dma-1-destination-address-w-internal-memory">40000C0h,0C2h - DMA1DAD - DMA 1 Destination Address (W) (internal memory)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000cch0ceh---dma2dad---dma-2-destination-address-w-internal-memory">40000CCh,0CEh - DMA2DAD - DMA 2 Destination Address (W) (internal memory)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000d8h0dah---dma3dad---dma-3-destination-address-w-any-memory">40000D8h,0DAh - DMA3DAD - DMA 3 Destination Address (W) (any memory)</a>
     *
     * The four DMA destination registers as a registral_series.
     *
     * @sa DMA0_DEST
     * @sa DMA1_DEST
     * @sa DMA2_DEST
     * @sa DMA3_DEST
     */
    inline constexpr auto DMA_DEST = registral_series<const_ptr<void* volatile[4]>(0x40000B4), 12>{};

    /**
     * @brief DMA transfer count registers.
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000b8h---dma0cnt_l---dma-0-word-count-w-14-bit-14000h">40000B8h - DMA0CNT_L - DMA 0 Word Count (W) (14 bit, 1..4000h)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000c4h---dma1cnt_l---dma-1-word-count-w-14-bit-14000h">40000C4h - DMA1CNT_L - DMA 1 Word Count (W) (14 bit, 1..4000h)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000d0h---dma2cnt_l---dma-2-word-count-w-14-bit-14000h">40000D0h - DMA2CNT_L - DMA 2 Word Count (W) (14 bit, 1..4000h)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000dch---dma3cnt_l---dma-3-word-count-w-16-bit-110000h">40000DCh - DMA3CNT_L - DMA 3 Word Count (W) (16 bit, 1..10000h)</a>
     *
     * The four DMA transfer count registers as a registral_series.
     *
     * @sa DMA0_COUNT
     * @sa DMA1_COUNT
     * @sa DMA2_COUNT
     * @sa DMA3_COUNT
     */
    inline constexpr auto DMA_COUNT = registral_series<const_ptr<volatile u16[4]>(0x40000B8), 12>{};

    /**
     * @brief DMA control registers.
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000bah---dma0cnt_h---dma-0-control-rw">40000BAh - DMA0CNT_H - DMA 0 Control (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000c6h---dma1cnt_h---dma-1-control-rw">40000C6h - DMA1CNT_H - DMA 1 Control (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000d2h---dma2cnt_h---dma-2-control-rw">40000D2h - DMA2CNT_H - DMA 2 Control (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000deh---dma3cnt_h---dma-3-control-rw">40000DEh - DMA3CNT_H - DMA 3 Control (R/W)</a>
     *
     * The four DMA control registers as a registral_series.
     *
     * @sa DMA0_CONTROL
     * @sa DMA1_CONTROL
     * @sa DMA2_CONTROL
     * @sa DMA3_CONTROL
     * @sa dmacnt_h
     */
    inline constexpr auto DMA_CONTROL = registral_series<const_ptr<volatile dmacnt_h[4]>(0x40000BA), 4>{};

    // Timers

    /**
     * @brief Timer 0 counter register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000100h---tm0cnt_l---timer-0-counterreload-rw">4000100h - TM0CNT_L - Timer 0 Counter/Reload (R/W)</a>
     *
     * @note This is a read-only alias of TIMER0_RELOAD.
     *
     * @sa TIMER0_RELOAD
     * @sa TIMER0_CONTROL
     * @sa TIMER1_COUNT
     * @sa TIMER2_COUNT
     * @sa TIMER3_COUNT
     * @sa TIMER_COUNT
     */
    inline constexpr auto TIMER0_COUNT = registral<const_ptr<const volatile u16>(0x4000100)>{};

    /**
     * @brief Timer 0 counter & reload register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000100h---tm0cnt_l---timer-0-counterreload-rw">4000100h - TM0CNT_L - Timer 0 Counter/Reload (R/W)</a>
     *
     * @note Writing to this register sets the value to start counting from when the timer overflows/ticks.
     *
     * @sa TIMER0_COUNT
     * @sa TIMER0_CONTROL
     * @sa TIMER1_RELOAD
     * @sa TIMER2_RELOAD
     * @sa TIMER3_RELOAD
     * @sa TIMER_RELOAD
     */
    inline constexpr auto TIMER0_RELOAD = registral<const_ptr<volatile short>(0x4000100)>{};

    /**
     * @brief Timer 0 control register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000102h---tm0cnt_h---timer-0-control-rw">4000102h - TM0CNT_H - Timer 0 Control (R/W)</a>
     *
     * @sa TIMER0_COUNT
     * @sa TIMER0_RELOAD
     * @sa TIMER1_CONTROL
     * @sa TIMER2_CONTROL
     * @sa TIMER3_CONTROL
     * @sa TIMER_CONTROL
     */
    inline constexpr auto TIMER0_CONTROL = registral<const_ptr<volatile tmcnt_h>(0x4000102)>{};

    /**
     * @brief Timer 1 counter register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000104h---tm1cnt_l---timer-1-counterreload-rw">4000104h - TM1CNT_L - Timer 1 Counter/Reload (R/W)</a>
     *
     * @note This is a read-only alias of TIMER1_RELOAD.
     *
     * @sa TIMER1_RELOAD
     * @sa TIMER1_CONTROL
     * @sa TIMER0_COUNT
     * @sa TIMER2_COUNT
     * @sa TIMER3_COUNT
     * @sa TIMER_COUNT
     */
    inline constexpr auto TIMER1_COUNT = registral<const_ptr<const volatile u16>(0x4000104)>{};

    /**
     * @brief Timer 1 counter & reload register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000104h---tm1cnt_l---timer-1-counterreload-rw">4000104h - TM1CNT_L - Timer 1 Counter/Reload (R/W)</a>
     *
     * @note Writing to this register sets the value to start counting from when the timer overflows/ticks.
     *
     * @sa TIMER1_COUNT
     * @sa TIMER1_CONTROL
     * @sa TIMER0_RELOAD
     * @sa TIMER2_RELOAD
     * @sa TIMER3_RELOAD
     * @sa TIMER_RELOAD
     */
    inline constexpr auto TIMER1_RELOAD = registral<const_ptr<volatile short>(0x4000104)>{};

    /**
     * @brief Timer 1 control register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000106h---tm1cnt_h---timer-1-control-rw">4000106h - TM1CNT_H - Timer 1 Control (R/W)</a>
     *
     * @sa TIMER1_COUNT
     * @sa TIMER1_RELOAD
     * @sa TIMER0_CONTROL
     * @sa TIMER2_CONTROL
     * @sa TIMER3_CONTROL
     * @sa TIMER_CONTROL
     */
    inline constexpr auto TIMER1_CONTROL = registral<const_ptr<volatile tmcnt_h>(0x4000106)>{};

    /**
     * @brief Timer 2 counter register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000108h---tm2cnt_l---timer-2-counterreload-rw">4000108h - TM2CNT_L - Timer 2 Counter/Reload (R/W)</a>
     *
     * @note This is a read-only alias of TIMER2_RELOAD.
     *
     * @sa TIMER2_RELOAD
     * @sa TIMER2_CONTROL
     * @sa TIMER0_COUNT
     * @sa TIMER1_COUNT
     * @sa TIMER3_COUNT
     * @sa TIMER_COUNT
     */
    inline constexpr auto TIMER2_COUNT = registral<const_ptr<const volatile u16>(0x4000108)>{};

    /**
     * @brief Timer 2 counter & reload register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000108h---tm2cnt_l---timer-2-counterreload-rw">4000108h - TM2CNT_L - Timer 2 Counter/Reload (R/W)</a>
     *
     * @note Writing to this register sets the value to start counting from when the timer overflows/ticks.
     *
     * @sa TIMER2_COUNT
     * @sa TIMER2_CONTROL
     * @sa TIMER0_RELOAD
     * @sa TIMER1_RELOAD
     * @sa TIMER3_RELOAD
     * @sa TIMER_RELOAD
     */
    inline constexpr auto TIMER2_RELOAD = registral<const_ptr<volatile short>(0x4000108)>{};

    /**
     * @brief Timer 2 control register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400010ah---tm2cnt_h---timer-2-control-rw">400010Ah - TM2CNT_H - Timer 2 Control (R/W)</a>
     *
     * @sa TIMER2_COUNT
     * @sa TIMER2_RELOAD
     * @sa TIMER0_CONTROL
     * @sa TIMER1_CONTROL
     * @sa TIMER3_CONTROL
     * @sa TIMER_CONTROL
     */
    inline constexpr auto TIMER2_CONTROL = registral<const_ptr<volatile tmcnt_h>(0x400010A)>{};

    /**
     * @brief Timer 3 counter register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400010ch---tm3cnt_l---timer-3-counterreload-rw">400010Ch - TM3CNT_L - Timer 3 Counter/Reload (R/W)</a>
     *
     * @note This is a read-only alias of TIMER3_RELOAD.
     *
     * @sa TIMER3_RELOAD
     * @sa TIMER3_CONTROL
     * @sa TIMER0_COUNT
     * @sa TIMER1_COUNT
     * @sa TIMER2_COUNT
     * @sa TIMER_COUNT
     */
    inline constexpr auto TIMER3_COUNT = registral<const_ptr<const volatile u16>(0x400010C)>{};

    /**
     * @brief Timer 3 counter & reload register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400010ch---tm3cnt_l---timer-3-counterreload-rw">400010Ch - TM3CNT_L - Timer 3 Counter/Reload (R/W)</a>
     *
     * @note Writing to this register sets the value to start counting from when the timer overflows/ticks.
     *
     * @sa TIMER3_COUNT
     * @sa TIMER3_CONTROL
     * @sa TIMER0_RELOAD
     * @sa TIMER1_RELOAD
     * @sa TIMER2_RELOAD
     * @sa TIMER_RELOAD
     */
    inline constexpr auto TIMER3_RELOAD = registral<const_ptr<volatile short>(0x400010C)>{};

    /**
     * @brief Timer 3 control register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400010eh---tm3cnt_h---timer-3-control-rw">400010Eh - TM3CNT_H - Timer 3 Control (R/W)</a>
     *
     * @sa TIMER3_COUNT
     * @sa TIMER3_RELOAD
     * @sa TIMER0_CONTROL
     * @sa TIMER1_CONTROL
     * @sa TIMER2_CONTROL
     * @sa TIMER_CONTROL
     */
    inline constexpr auto TIMER3_CONTROL = registral<const_ptr<volatile tmcnt_h>(0x400010E)>{};

    /**
     * @brief Timer count registers.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000100h---tm0cnt_l---timer-0-counterreload-rw">4000100h - TM0CNT_L - Timer 0 Counter/Reload (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000104h---tm1cnt_l---timer-1-counterreload-rw">4000104h - TM1CNT_L - Timer 1 Counter/Reload (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000108h---tm2cnt_l---timer-2-counterreload-rw">4000108h - TM2CNT_L - Timer 2 Counter/Reload (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#400010ch---tm3cnt_l---timer-3-counterreload-rw">400010Ch - TM3CNT_L - Timer 3 Counter/Reload (R/W)</a>
     *
     * The four timer count registers as a registral_series.
     *
     * @sa TIMER0_COUNT
     * @sa TIMER1_COUNT
     * @sa TIMER2_COUNT
     * @sa TIMER3_COUNT
     */
    inline constexpr auto TIMER_COUNT = registral_series<const_ptr<const volatile u16[4]>(0x4000100), 4>{};

    /**
     * @brief Timer reload registers.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000100h---tm0cnt_l---timer-0-counterreload-rw">4000100h - TM0CNT_L - Timer 0 Counter/Reload (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000104h---tm1cnt_l---timer-1-counterreload-rw">4000104h - TM1CNT_L - Timer 1 Counter/Reload (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000108h---tm2cnt_l---timer-2-counterreload-rw">4000108h - TM2CNT_L - Timer 2 Counter/Reload (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#400010ch---tm3cnt_l---timer-3-counterreload-rw">400010Ch - TM3CNT_L - Timer 3 Counter/Reload (R/W)</a>
     *
     * The four timer reload registers as a registral_series.
     *
     * @sa TIMER0_RELOAD
     * @sa TIMER1_RELOAD
     * @sa TIMER2_RELOAD
     * @sa TIMER3_RELOAD
     */
    inline constexpr auto TIMER_RELOAD = registral_series<const_ptr<volatile u16[4]>(0x4000100), 4>{};

    /**
     * @brief Timer control registers.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000102h---tm0cnt_h---timer-0-control-rw">4000102h - TM0CNT_H - Timer 0 Control (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000106h---tm1cnt_h---timer-1-control-rw">4000106h - TM1CNT_H - Timer 1 Control (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#400010ah---tm2cnt_h---timer-2-control-rw">400010Ah - TM2CNT_H - Timer 2 Control (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#400010eh---tm3cnt_h---timer-3-control-rw">400010Eh - TM3CNT_H - Timer 3 Control (R/W)</a>
     *
     * The four timer control registers as a registral_series.
     *
     * @code{cpp}
     * // Create a 1 second timer
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     mmio::TIMER2_RELOAD = -0x4000;
     *     mmio::TIMER2_CONTROL = tmcnt_h{.scale = timer_scale::_1024};
     *
     *     mmio::TIMER3_CONTROL = tmcnt_h{.cascade = true, .enabled = true};
     *
     *     int prev = -1;
     *     while (true) {
     *         // Detect that at least one second has elapsed
     *         if (u16 curr = *mmio::TIMER3_COUNT; curr != prev) {
     *             // Calculate hours, minutes, seconds elapsed
     *             const int hours = curr / 3600;
     *             const int minutes = (curr % 3600) / 60;
     *             const int seconds = curr % 60;
     *
     *             prev = curr;
     *         }
     *     }
     * }
     * @endcode
     *
     * @sa TIMER0_CONTROL
     * @sa TIMER1_CONTROL
     * @sa TIMER2_CONTROL
     * @sa TIMER3_CONTROL
     */
    inline constexpr auto TIMER_CONTROL = registral_series<const_ptr<volatile tmcnt_h[4]>(0x4000102), 4>{};

    // Serial

    /**
     * @brief Serial IO 32-bit data read/write register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000120h---siodata32_l---sio-normal-communication-lower-16bit-data-rw">4000120h - SIODATA32_L - SIO Normal Communication lower 16bit data (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000122h---siodata32_h---sio-normal-communication-upper-16bit-data-rw">4000122h - SIODATA32_H - SIO Normal Communication upper 16bit data (R/W)</a>
     *
     * @sa SIOCNT_NORMAL
     * @sa SIODATA8
     */
    inline constexpr auto SIODATA32 = registral<const_ptr<volatile u32>(0x4000120)>{};

    /**
     * @brief Serial IO 16-bit data read/write register for multiplayer client 0.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000120h---siomulti0---sio-multi-player-data-0-parent-rw">4000120h - SIOMULTI0 - SIO Multi-Player Data 0 (Parent) (R/W)</a>
     *
     * Incoming 16-bit data received from multiplayer client 0.
     *
     * @sa SIOCNT_MULTI
     * @sa SIOMULTI1
     * @sa SIOMULTI2
     * @sa SIOMULTI3
     * @sa SIOMULTI
     */
    inline constexpr auto SIOMULTI0 = registral<const_ptr<volatile u16>(0x4000120)>{};

    /**
     * @brief Serial IO 16-bit data read/write register for multiplayer client 1.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000122h---siomulti1---sio-multi-player-data-1-parent-rw">4000122h - SIOMULTI1 - SIO Multi-Player Data 1 (Parent) (R/W)</a>
     *
     * Incoming 16-bit data received from multiplayer client 1.
     *
     * @sa SIOCNT_MULTI
     * @sa SIOMULTI0
     * @sa SIOMULTI2
     * @sa SIOMULTI3
     * @sa SIOMULTI
     */
    inline constexpr auto SIOMULTI1 = registral<const_ptr<volatile u16>(0x4000122)>{};

    /**
     * @brief Serial IO 16-bit data read/write register for multiplayer client 2.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000124h---siomulti2---sio-multi-player-data-2-parent-rw">4000124h - SIOMULTI2 - SIO Multi-Player Data 2 (Parent) (R/W)</a>
     *
     * Incoming 16-bit data received from multiplayer client 2.
     *
     * @sa SIOCNT_MULTI
     * @sa SIOMULTI0
     * @sa SIOMULTI1
     * @sa SIOMULTI3
     * @sa SIOMULTI
     */
    inline constexpr auto SIOMULTI2 = registral<const_ptr<volatile u16>(0x4000124)>{};

    /**
     * @brief Serial IO 16-bit data read/write register for multiplayer client 3.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000126h---siomulti3---sio-multi-player-data-3-parent-rw">4000126h - SIOMULTI3 - SIO Multi-Player Data 3 (Parent) (R/W)</a>
     *
     * Incoming 16-bit data received from multiplayer client 3.
     *
     * @sa SIOCNT_MULTI
     * @sa SIOMULTI0
     * @sa SIOMULTI1
     * @sa SIOMULTI2
     * @sa SIOMULTI
     */
    inline constexpr auto SIOMULTI3 = registral<const_ptr<volatile u16>(0x4000126)>{};

    /**
     * @brief Registral series for 16-bit data read/write multiplayer clients.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000120h---siomulti0---sio-multi-player-data-0-parent-rw">4000120h - SIOMULTI0 - SIO Multi-Player Data 0 (Parent) (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000122h---siomulti1---sio-multi-player-data-1-parent-rw">4000122h - SIOMULTI1 - SIO Multi-Player Data 1 (Parent) (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000124h---siomulti2---sio-multi-player-data-2-parent-rw">4000124h - SIOMULTI2 - SIO Multi-Player Data 2 (Parent) (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000126h---siomulti3---sio-multi-player-data-3-parent-rw">4000126h - SIOMULTI3 - SIO Multi-Player Data 3 (Parent) (R/W)</a>
     *
     * @sa SIOCNT_MULTI
     * @sa SIOMULTI0
     * @sa SIOMULTI1
     * @sa SIOMULTI2
     * @sa SIOMULTI3
     */
    inline constexpr auto SIOMULTI = registral_series<const_ptr<volatile u16[4]>(0x4000120)>{};

    /**
     * @brief Serial IO control register for Normal mode.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000128h---siocnt---sio-control-usage-in-normal-mode-rw">4000128h - SIOCNT - SIO Control, usage in NORMAL Mode (R/W)</a>
     *
     * @sa SIODATA8
     * @sa SIODATA32
     * @sa SIOCNT_MULTI
     * @sa SIOCNT_UART
     * @sa RCNT
     * @sa siocnt_normal
     */
    inline constexpr auto SIOCNT_NORMAL = registral<const_ptr<volatile siocnt_normal>(0x4000128)>{};

    /**
     * @brief Serial IO control register for Multiplayer mode.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000128h---siocnt---sio-control-usage-in-multi-player-mode-rw">4000128h - SIOCNT - SIO Control, usage in MULTI-PLAYER Mode (R/W)</a>
     *
     * Incoming 16-bit data received from the other multiplayer clients.
     *
     * @sa SIOMLT_SEND
     * @sa SIOMULTI0
     * @sa SIOMULTI1
     * @sa SIOMULTI2
     * @sa SIOMULTI3
     * @sa SIOMULTI
     * @sa SIOCNT_NORMAL
     * @sa SIOCNT_UART
     * @sa RCNT
     * @sa siocnt_multi
     */
    inline constexpr auto SIOCNT_MULTI = registral<const_ptr<volatile siocnt_multi>(0x4000128)>{};

    /**
     * @brief Serial IO control register for UART mode.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000128h---sccnt_l---sio-control-usage-in-uart-mode-rw">4000128h - SCCNT_L - SIO Control, usage in UART Mode (R/W)</a>
     *
     * @sa SIODATA8
     * @sa SIOCNT_NORMAL
     * @sa SIOCNT_MULTI
     * @sa RCNT
     * @sa siocnt_multi
     */
    inline constexpr auto SIOCNT_UART = registral<const_ptr<volatile siocnt_uart>(0x4000128)>{};

    /**
     * @brief Serial IO 8-bit data read/write register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400012ah---siodata8---sio-normal-communication-8bit-data-rw">400012Ah - SIODATA8 - SIO Normal Communication 8bit Data (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#400012ah---siodata8---usage-in-uart-mode-rw">400012Ah - SIODATA8 - usage in UART Mode (R/W)</a>
     *
     * @sa SIOCNT_NORMAL
     * @sa SIOCNT_UART
     * @sa SIODATA32
     */
    inline constexpr auto SIODATA8 = registral<const_ptr<volatile u8>(0x400012A)>{};

    /**
     * @brief Serial IO 16-bit data write register for Multiplayer mode.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400012ah---siomlt_send---data-send-register-rw">400012Ah - SIOMLT_SEND - Data Send Register (R/W)</a>
     *
     * Outgoing 16-bit data to be sent to the other multiplayer clients.
     *
     * @sa SIOMULTI0
     * @sa SIOMULTI1
     * @sa SIOMULTI2
     * @sa SIOMULTI3
     * @sa SIOMULTI
     * @sa SIOCNT_MULTI
     */
    inline constexpr auto SIOMLT_SEND = registral<const_ptr<volatile u16>(0x400012A)>{};

    // Keys

    /**
     * @brief Read-only keypad input register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000130h---keyinput---key-status-r">4000130h - KEYINPUT - Key Status (R)</a>
     *
     * Reads the current state of the keypad keys. The buttons are pulled-low when pressed, resulting in a pressed
     * button being in a `0` state, and a released button being in a `1` state.
     *
     * @code{cpp}
     * // Read the keypad
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     while (true) {
     *         const auto keys = *mmio::KEYINPUT;
     *
     *         // Invert the buttons for more logical boolean states
     *         const auto a = !keys.i_a; // button A
     *         const auto b = !keys.i_b; // button B
     *         const auto select = !keys.i_select; // button Select
     *         const auto start = !keys.i_start; // button Start
     *
     *         // The ::*axis() member functions convert dpad directions to a signed integer
     *         const auto xaxis = keys.xaxis(); // dpad left/right
     *         const auto yaxis = keys.yaxis(); // dpad down/up
     *
     *         const auto r = !keys.i_r; // button R
     *         const auto l = !keys.i_l; // button L
     *     }
     * }
     * @endcode
     *
     * @note It is recommended to handle key input via the keystate helper struct.
     *
     * @sa KEYCNT
     * @sa keyinput
     */
    inline constexpr auto KEYINPUT = registral<const_ptr<volatile keyinput>(0x4000130)>{};

    /**
     * @brief Keypad interrupt control register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000132h---keycnt---key-interrupt-control-rw">4000132h - KEYCNT - Key Interrupt Control (R/W)</a>
     *
     * @code{cpp}
     * // Enable keypad IRQ
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     mmio::IRQ_HANDLER = agbabi::irq_empty;
     *
     *     // Enable keypad IRQs
     *     mmio::KEYCNT = {.irq_enabled = true};
     *     mmio::IE = {.keypad = true};
     *
     *     mmio::IME = true;
     *
     *     while (true) {
     *         bios::IntrWait(true, irq{.keypad = true}); // Wait for any button to be pressed
     *     }
     * }
     * @endcode
     *
     * @sa IE
     * @sa keycnt
     */
    inline constexpr auto KEYCNT = registral<const_ptr<volatile keycnt>(0x4000132)>{};

    // RCNT

    /**
     * @brief Mode selection register for Normal/Multiplayer/UART.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000134h---rcnt-r---mode-selection-in-normalmultiplayeruart-modes-rw">4000134h - RCNT (R) - Mode Selection, in Normal/Multiplayer/UART modes (R/W)</a>
     *
     * @warning This register must be set to zero before Normal/Multiplayer/UART serial modes can be used.
     *
     * @code{cpp}
     * // Reset RCNT to zero for Normal/Multiplayer/UART modes.
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     mmio::RCNT.reset();
     *     // SIOCNT_NORMAL, SIOCNT_MULTI, SIOCNT_UART can now be used
     *
     *     while (true);
     * }
     * @endcode
     *
     * @sa SIOCNT_NORMAL
     * @sa SIOCNT_MULTI
     * @sa SIOCNT_UART
     */
    inline constexpr auto RCNT = registral<const_ptr<volatile u16>(0x4000134)>{};

    /**
     * @brief Mode selection register for JOY BUS.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000134h---rcnt-r---mode-selection-in-joy-bus-mode-rw">4000134h - RCNT (R) - Mode Selection, in JOY BUS mode (R/W)</a>
     *
     * @warning This register must be set to the default constructor of rcnt_joybus to use JOY BUS serial mode.
     *
     * @code{cpp}
     * // Reset RCNT_JOYBUS to zero for JOY BUS mode.
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     mmio::RCNT_JOYBUS.reset();
     *     // JOY BUS can now be used
     *
     *     while (true);
     * }
     * @endcode
     *
     * @sa RCNT
     * @sa JOYCNT
     * @sa JOY_RECV
     * @sa JOY_TRANS
     * @sa JOYSTAT
     */
    inline constexpr auto RCNT_JOYBUS = registral<const_ptr<volatile rcnt_joybus>(0x4000134)>{};

    /**
     * @brief Mode selection register for GPIO.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000134h---rcnt-r---sio-mode-usage-in-general-purpose-mode-rw">4000134h - RCNT (R) - SIO Mode, usage in GENERAL-PURPOSE Mode (R/W)</a>
     *
     * @warning This register must be set to the default constructor of rcnt_gpio to use GPIO serial mode.
     *
     * @code{cpp}
     * // Reset RCNT_GPIO to zero for GPIO mode.
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     mmio::RCNT_GPIO.reset();
     *     // GPIO can now be used
     *
     *     while (true);
     * }
     * @endcode
     *
     * @sa RCNT
     */
    inline constexpr auto RCNT_GPIO = registral<const_ptr<volatile rcnt_gpio>(0x4000134)>{};

    // Joybus

    /**
     * @brief Serial IO control register for JOY BUS mode.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000140h---joycnt---joy-bus-control-register-rw">4000140h - JOYCNT - JOY BUS Control Register (R/W)</a>
     *
     * @sa JOY_RECV
     * @sa JOY_TRANS
     * @sa JOYSTAT
     * @sa RCNT_JOYBUS
     * @sa joycnt
     */
    inline constexpr auto JOYCNT = registral<const_ptr<volatile joycnt>(0x4000140)>{};

    /**
     * @brief JOY BUS receive register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000150h---joy_recv_l---receive-data-register-low-rw">4000150h - JOY_RECV_L - Receive Data Register low (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000152h---joy_recv_h---receive-data-register-high-rw">4000152h - JOY_RECV_H - Receive Data Register high (R/W)</a>
     *
     * @sa JOYCNT
     * @sa JOY_TRANS
     * @sa JOYSTAT
     * @sa RCNT_JOYBUS
     */
    inline constexpr auto JOY_RECV = registral<const_ptr<volatile u32>(0x4000150)>{};

    /**
     * @brief JOY BUS transfer/send register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000154h---joy_trans_l---send-data-register-low-rw">4000154h - JOY_TRANS_L - Send Data Register low (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000156h---joy_trans_h---send-data-register-high-rw">4000156h - JOY_TRANS_H - Send Data Register high (R/W)</a>
     *
     * @sa JOYCNT
     * @sa JOY_RECV
     * @sa JOYSTAT
     * @sa RCNT_JOYBUS
     */
    inline constexpr auto JOY_TRANS = registral<const_ptr<volatile u32>(0x4000154)>{};

    /**
     * @brief JOY BUS receive status register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000158h---joystat---receive-status-register-rw">4000158h - JOYSTAT - Receive Status Register (R/W)</a>
     *
     * @sa JOYCNT
     * @sa JOY_RECV
     * @sa JOYSTAT
     * @sa RCNT_JOYBUS
     * @sa joystat
     */
    inline constexpr auto JOYSTAT = registral<const_ptr<volatile joystat>(0x4000158)>{};

    // Interrupts

    /**
     * @brief Interrupt enable parameters.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000200h---ie---interrupt-enable-register-rw">4000200h - IE - Interrupt Enable Register (R/W)</a>
     *
     * @code{cpp}
     * // Enable VBlank and keypad interrupts
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     mmio::IRQ_HANDLER = agbabi::irq_empty;
     *
     *     mmio::DISPSTAT = {.irq_vblank = true};
     *     mmio::KEYCNT = {.irq_enabled = true};
     *
     *     // Enable VBlank and keypad IRQs
     *     mmio::IE = {.vblank = true, .keypad = true};
     *
     *     mmio::IME = true; // Activate IRQ interrupt handling
     *
     *     while (true) {
     *         bios::VBlankIntrWait();
     *         bios::IntrWait(true, irq{.keypad=true}); // Wait for keypad press
     *     }
     * }
     * @endcode
     *
     * @sa IF
     * @sa IME
     * @sa irq
     */
    inline constexpr auto IE = registral<const_ptr<volatile irq>(0x4000200)>{};

    /**
     * @brief Acknowledge/inspect raised IRQ flags.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000202h---if---interrupt-request-flags--irq-acknowledge-rw-see-below">4000202h - IF - Interrupt Request Flags / IRQ Acknowledge (R/W, see below)</a>
     *
     * @warning Interrupts must be manually acknowledged by writing to this register.
     *
     * @code{cpp}
     * // Read and acknowledge interrupts
     * #include <gba/gba.hpp>
     *
     * [[gnu::interrupt("IRQ"), gnu::target("arm")]]
     * void my_handler() {
     *     using namespace gba;
     *
     *     const auto irq = *mmio::IF; // Read the raised IRQs
     *
     *     // Deal with raised IRQs
     *     if (irq.vblank) {
     *     } else if (irq.keypad) {
     *     }
     *
     *     mmio::IF = irq; // Acknowledge the raised IRQs
     * }
     *
     * int main() {
     *     using namespace gba;
     *
     *     mmio::IRQ_HANDLER = my_handler;
     *
     *     // Enable VBlank and keypad IRQs
     *     mmio::DISPSTAT = {.irq_vblank = true};
     *     mmio::KEYCNT = {.irq_enabled = true};
     *     mmio::IE = {.vblank = true, .keypad = true};
     *
     *     mmio::IME = true;
     *
     *     while (true);
     * }
     * @endcode
     *
     * @sa IE
     * @sa IME
     * @sa irq
     */
    inline constexpr auto IF = registral<const_ptr<volatile irq>(0x4000202)>{};

    /**
     * @brief Waitstate control register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000204h---waitcnt---waitstate-control-rw">4000204h - WAITCNT - Waitstate Control (R/W)</a>
     *
     * Use to configure gamepak waitstate access timings.
     *
     * @code{cpp}
     * // Setup default waitstate
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     mmio::WAITCNT = waitcnt_default; // Set default waitstate control
     *
     *     while (true);
     * }
     * @endcode
     *
     * @sa waitcnt
     */
    inline constexpr auto WAITCNT = registral<const_ptr<volatile waitcnt>(0x4000204)>{};

    /**
     * @brief Main interrupt enable flag.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000208h---ime---interrupt-master-enable-register-rw">4000208h - IME - Interrupt Master Enable Register (R/W)</a>
     *
     * @code{cpp}
     * // Enable VBlank interrupts
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     mmio::IRQ_HANDLER = agbabi::irq_empty;
     *
     *     mmio::DISPSTAT = {.irq_vblank = true};
     *     mmio::IE = {.vblank = true};
     *
     *     mmio::IME = true; // Activate IRQ interrupt handling
     *
     *     while (true) {
     *         bios::VBlankIntrWait();
     *     }
     * }
     * @endcode
     *
     * @sa IE
     * @sa IF
     */
    inline constexpr auto IME = registral<const_ptr<volatile bool>(0x4000208)>{};

    // Post flag

    /**
     * @brief Hardware reset flag.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000300h---postflg---byte---undocumented---post-boot--debug-control-rw">4000300h - POSTFLG - BYTE - Undocumented - Post Boot / Debug Control (R/W)</a>
     *
     * Returns false when the device has gone through the hardware BIOS startup, otherwise true if it has soft reset.
     *
     * @note This is an undocumented feature.
     */
    inline constexpr auto POSTFLG = registral<const_ptr<volatile bool>(0x4000300)>{};

    // Memory control

    /**
     * @brief Memory control register.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000800h---32bit---undocumented---internal-memory-control-rw">4000800h - 32bit - Undocumented - Internal Memory Control (R/W)</a>
     *
     * @note This is an undocumented feature.
     *
     * @sa memcnt
     */
    inline constexpr auto MEMCNT = registral<const_ptr<volatile memcnt>(0x4000800)>{};

    // Palette RAM

    /**
     * @brief Background palette memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#color-palette-ram">Color Palette RAM</a>
     *
     * @sa OBJ_PALETTE
     */
    inline constexpr auto BG_PALETTE = const_ptr<volatile u16[256]>(0x5000000);

    /**
     * @brief Object palette memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#color-palette-ram">Color Palette RAM</a>
     *
     * @sa BG_PALETTE
     */
    inline constexpr auto OBJ_PALETTE = const_ptr<volatile u16[256]>(0x5000200);

    // Video RAM

    /**
     * @brief Character block 0 4-bpp image memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4bit-depth-16-colors-16-palettes">4bit depth (16 colors, 16 palettes)</a>
     *
     * Each tile is 32 bytes. There are 512 tiles.
     *
     * @sa CHARBLOCK1_4BPP
     * @sa CHARBLOCK2_4BPP
     * @sa CHARBLOCK3_4BPP
     * @sa CHARBLOCKS_4BPP
     * @sa tile4bpp
     */
    inline constexpr auto CHARBLOCK0_4BPP = const_ptr<volatile tile4bpp[512]>(0x6000000);

    /**
     * @brief Character block 1 4-bpp image memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4bit-depth-16-colors-16-palettes">4bit depth (16 colors, 16 palettes)</a>
     *
     * Each tile is 32 bytes. There are 512 tiles.
     *
     * @sa CHARBLOCK0_4BPP
     * @sa CHARBLOCK2_4BPP
     * @sa CHARBLOCK3_4BPP
     * @sa CHARBLOCKS_4BPP
     * @sa tile4bpp
     */
    inline constexpr auto CHARBLOCK1_4BPP = const_ptr<volatile tile4bpp[512]>(0x6004000);

    /**
     * @brief Character block 2 4-bpp image memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4bit-depth-16-colors-16-palettes">4bit depth (16 colors, 16 palettes)</a>
     *
     * Each tile is 32 bytes. There are 512 tiles.
     *
     * @sa CHARBLOCK0_4BPP
     * @sa CHARBLOCK1_4BPP
     * @sa CHARBLOCK3_4BPP
     * @sa CHARBLOCKS_4BPP
     * @sa tile4bpp
     */
    inline constexpr auto CHARBLOCK2_4BPP = const_ptr<volatile tile4bpp[512]>(0x6009000);

    /**
     * @brief Character block 3 4-bpp image memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4bit-depth-16-colors-16-palettes">4bit depth (16 colors, 16 palettes)</a>
     *
     * Each tile is 32 bytes. There are 512 tiles.
     *
     * @sa CHARBLOCK0_4BPP
     * @sa CHARBLOCK1_4BPP
     * @sa CHARBLOCK2_4BPP
     * @sa CHARBLOCKS_4BPP
     * @sa tile4bpp
     */
    inline constexpr auto CHARBLOCK3_4BPP = const_ptr<volatile tile4bpp[512]>(0x600C000);

    /**
     * @brief Character block 0 8-bpp image memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#8bit-depth-256-colors-1-palette">8bit depth (256 colors, 1 palette)</a>
     *
     * Each tile is 64 bytes. There are 256 tiles.
     *
     * @sa CHARBLOCK1_8BPP
     * @sa CHARBLOCK2_8BPP
     * @sa CHARBLOCK3_8BPP
     * @sa CHARBLOCKS_8BPP
     * @sa tile8bpp
     */
    inline constexpr auto CHARBLOCK0_8BPP = const_ptr<volatile tile8bpp[256]>(0x6000000);

    /**
     * @brief Character block 1 8-bpp image memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#8bit-depth-256-colors-1-palette">8bit depth (256 colors, 1 palette)</a>
     *
     * Each tile is 64 bytes. There are 256 tiles.
     *
     * @sa CHARBLOCK0_8BPP
     * @sa CHARBLOCK2_8BPP
     * @sa CHARBLOCK3_8BPP
     * @sa CHARBLOCKS_8BPP
     * @sa tile8bpp
     */
    inline constexpr auto CHARBLOCK1_8BPP = const_ptr<volatile tile8bpp[256]>(0x6004000);

    /**
     * @brief Character block 2 8-bpp image memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#8bit-depth-256-colors-1-palette">8bit depth (256 colors, 1 palette)</a>
     *
     * Each tile is 64 bytes. There are 256 tiles.
     *
     * @sa CHARBLOCK0_8BPP
     * @sa CHARBLOCK1_8BPP
     * @sa CHARBLOCK3_8BPP
     * @sa CHARBLOCKS_8BPP
     * @sa tile8bpp
     */
    inline constexpr auto CHARBLOCK2_8BPP = const_ptr<volatile tile8bpp[256]>(0x6009000);

    /**
     * @brief Character block 3 8-bpp image memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#8bit-depth-256-colors-1-palette">8bit depth (256 colors, 1 palette)</a>
     *
     * Each tile is 64 bytes. There are 256 tiles.
     *
     * @sa CHARBLOCK0_8BPP
     * @sa CHARBLOCK1_8BPP
     * @sa CHARBLOCK2_8BPP
     * @sa CHARBLOCKS_8BPP
     * @sa tile8bpp
     */
    inline constexpr auto CHARBLOCK3_8BPP = const_ptr<volatile tile8bpp[256]>(0x600C000);

    /**
     * @brief Character 4-bpp image memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4bit-depth-16-colors-16-palettes">4bit depth (16 colors, 16 palettes)</a>
     *
     * Each tile is 32 bytes. There are 512 tiles. There are 4 blocks.
     *
     * @sa CHARBLOCK0_4BPP
     * @sa CHARBLOCK1_4BPP
     * @sa CHARBLOCK2_4BPP
     * @sa CHARBLOCK3_4BPP
     * @sa tile4bpp
     */
    inline constexpr auto CHARBLOCKS_4BPP = const_ptr<tile4bpp[4][512]>(0x6000000);

    /**
     * @brief Character 8-bpp image memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#8bit-depth-256-colors-1-palette">8bit depth (256 colors, 1 palette)</a>
     *
     * Each tile is 64 bytes. There are 256 tiles. There are 4 blocks.
     *
     * @sa CHARBLOCK0_8BPP
     * @sa CHARBLOCK1_8BPP
     * @sa CHARBLOCK2_8BPP
     * @sa CHARBLOCK3_8BPP
     * @sa tile8bpp
     */
    inline constexpr auto CHARBLOCKS_8BPP = const_ptr<tile8bpp[4][256]>(0x6000000);

    /**
     * @brief Screen/background tilemap memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#text-bg-screen-2-bytes-per-entry">Text BG Screen (2 bytes per entry)</a>
     *
     * Each screen has 1024 tiles (32x32). There are 32 screens.
     *
     * @sa textscreen
     */
    inline constexpr auto TEXT_SCREENBLOCKS = const_ptr<textscreen[32][1024]>(0x6000000);

    /**
     * @brief Screen/background tilemap memory for affine backgrounds.
     * @see <a href="https://mgba-emu.github.io/gbatek/#rotationscaling-bg-screen-1-byte-per-entry">Rotation/Scaling BG Screen (1 byte per entry)</a>
     *
     * Each tile is an 8-bit entry into a 8-bpp character block. Largest map size is 128x128 tiles.
     *
     * @sa CHARBLOCKS_8BPP
     */
    inline constexpr auto AFFINE_SCREENBLOCKS = const_ptr<u8[32][16384]>(0x6000000);

    /**
     * @brief Mode 3 bitmap background memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#bg-mode-3---240x160-pixels-32768-colors">BG Mode 3 - 240x160 pixels, 32768 colors</a>
     *
     * Bitmap is 16-bpp with a resolution of 240x160.
     */
    inline constexpr auto VIDEO3_VRAM = const_ptr<volatile u16[160][240]>(0x6000000);

    /**
     * @brief Mode 4 bitmap background memory (first page).
     * @see <a href="https://mgba-emu.github.io/gbatek/#bg-mode-4---240x160-pixels-256-colors-out-of-32768-colors">BG Mode 4 - 240x160 pixels, 256 colors (out of 32768 colors)</a>
     *
     * Bitmap is 8-bpp with a resolution of 240x160. The page is selected with dispcnt::page.
     *
     * @sa VIDEO4_VRAM_FRAME1
     * @sa BG_PALETTE
     * @sa dispcnt
     */
    inline constexpr auto VIDEO4_VRAM = const_ptr<volatile u8x2[160][120]>(0x6000000);

    /**
     * @brief Mode 4 bitmap background memory (second page).
     * @see <a href="https://mgba-emu.github.io/gbatek/#bg-mode-4---240x160-pixels-256-colors-out-of-32768-colors">BG Mode 4 - 240x160 pixels, 256 colors (out of 32768 colors)</a>
     *
     * Bitmap is 8-bpp with a resolution of 240x160. The page is selected with dispcnt::page.
     *
     * @sa VIDEO4_VRAM
     * @sa BG_PALETTE
     * @sa dispcnt
     */
    inline constexpr auto VIDEO4_VRAM_FRAME1 = VIDEO4_VRAM + 0xA000;

    /**
     * @brief Mode 5 bitmap background memory (first page).
     * @see <a href="https://mgba-emu.github.io/gbatek/#bg-mode-5---160x128-pixels-32768-colors">BG Mode 5 - 160x128 pixels, 32768 colors</a>
     *
     * Bitmap is 16-bpp with a resolution of 160x128. The page is selected with dispcnt::page.
     *
     * @sa VIDEO5_VRAM_FRAME1
     * @sa dispcnt
     */
    inline constexpr auto VIDEO5_VRAM = const_ptr<volatile u16[128][160]>(0x6000000);

    /**
     * @brief Mode 5 bitmap background memory (second page).
     * @see <a href="https://mgba-emu.github.io/gbatek/#bg-mode-5---160x128-pixels-32768-colors">BG Mode 5 - 160x128 pixels, 32768 colors</a>
     *
     * Bitmap is 16-bpp with a resolution of 160x128. The page is selected with dispcnt::page.
     *
     * @sa VIDEO5_VRAM
     * @sa dispcnt
     */
    inline constexpr auto VIDEO5_VRAM_FRAME1 = VIDEO5_VRAM + 0xA000;

    /**
     * @brief Object tile 4-bpp image memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#vram---character-data">VRAM - Character Data</a>
     *
     * Each tile is 32 bytes. There are 1024 tiles.
     *
     * @sa tile4bpp
     */
    inline constexpr auto OBJ_TILES = const_ptr<volatile tile4bpp[1024]>(0x6010000);

    // Object Attribute Memory

    /**
     * @brief Object attribute memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#obj-attribute-0-rw">OBJ Attribute 0 (R/W)</a>
     *
     * Includes the Y position and shape of objects.
     *
     * @sa OBJ_ATTR
     * @sa objattr0
     */
    inline constexpr auto OBJ_ATTR0 = registral_series<const_ptr<volatile objattr0[128]>(0x7000000), 8>{};

    /**
     * @brief Object attribute memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#obj-attribute-1-rw">OBJ Attribute 1 (R/W)</a>
     *
     * Includes the X position and size of objects.
     *
     * @sa OBJ_ATTR1_AFFINE
     * @sa OBJ_ATTR
     * @sa objattr1
     */
    inline constexpr auto OBJ_ATTR1 = registral_series<const_ptr<volatile objattr1[128]>(0x7000002), 8>{};

    /**
     * @brief Object attribute memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#obj-attribute-2-rw">OBJ Attribute 2 (R/W)</a>
     *
     * Includes the tile ID and palette bank of objects.
     *
     * @sa OBJ_ATTR
     * @sa objattr2
     */
    inline constexpr auto OBJ_ATTR2 = registral_series<const_ptr<volatile objattr2[128]>(0x7000004), 8>{};

    /**
     * @brief Object attribute memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#obj-attribute-0-rw">OBJ Attribute 0 (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#obj-attribute-1-rw">OBJ Attribute 1 (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#obj-attribute-2-rw">OBJ Attribute 2 (R/W)</a>
     *
     * Combines all the object attributes for regular objects.
     *
     * @sa OBJ_ATTR_AFFINE
     * @sa objattr
     */
    inline constexpr auto OBJ_ATTR = registral_series<const_ptr<volatile objattr[128]>(0x7000000), 8>{};

    /**
     * @brief Object attribute memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#obj-attribute-1-rw">OBJ Attribute 1 (R/W)</a>
     *
     * Objects attribute 1 for affine objects.
     *
     * @sa OBJ_ATTR1
     * @sa OBJ_ATTR_AFFINE
     * @sa objattr1_affine
     */
    inline constexpr auto OBJ_ATTR1_AFFINE = registral_series<const_ptr<volatile objattr1_affine[128]>(0x7000002), 8>{};

    /**
     * @brief Object attribute memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#obj-attribute-0-rw">OBJ Attribute 0 (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#obj-attribute-1-rw">OBJ Attribute 1 (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#obj-attribute-2-rw">OBJ Attribute 2 (R/W)</a>
     *
     * Combines all the object attributes for affine objects.
     *
     * @sa OBJ_ATTR
     * @sa objattr_affine
     */
    inline constexpr auto OBJ_ATTR_AFFINE = registral_series<const_ptr<volatile objattr_affine[128]>(0x7000000), 8>{};

    /**
     * @brief Affine object matrix memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#obj-rotationscaling-papbpcpd-parameters-rw">OBJ Rotation/Scaling PA,PB,PC,PD Parameters (R/W)</a>
     *
     * @sa AFFINE_PARAM_B
     * @sa AFFINE_PARAM_C
     * @sa AFFINE_PARAM_D
     */
    inline constexpr auto AFFINE_PARAM_A = registral_series<const_ptr<volatile fixed<short, 8>[32]>(0x7000006), 32>{};

    /**
     * @brief Affine object matrix memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#obj-rotationscaling-papbpcpd-parameters-rw">OBJ Rotation/Scaling PA,PB,PC,PD Parameters (R/W)</a>
     *
     * @sa AFFINE_PARAM_A
     * @sa AFFINE_PARAM_C
     * @sa AFFINE_PARAM_D
     */
    inline constexpr auto AFFINE_PARAM_B = registral_series<const_ptr<volatile fixed<short, 8>[32]>(0x700000E), 32>{};

    /**
     * @brief Affine object matrix memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#obj-rotationscaling-papbpcpd-parameters-rw">OBJ Rotation/Scaling PA,PB,PC,PD Parameters (R/W)</a>
     *
     * @sa AFFINE_PARAM_A
     * @sa AFFINE_PARAM_B
     * @sa AFFINE_PARAM_D
     */
    inline constexpr auto AFFINE_PARAM_C = registral_series<const_ptr<volatile fixed<short, 8>[32]>(0x7000016), 32>{};

    /**
     * @brief Affine object matrix memory.
     * @see <a href="https://mgba-emu.github.io/gbatek/#obj-rotationscaling-papbpcpd-parameters-rw">OBJ Rotation/Scaling PA,PB,PC,PD Parameters (R/W)</a>
     *
     * @sa AFFINE_PARAM_A
     * @sa AFFINE_PARAM_B
     * @sa AFFINE_PARAM_C
     */
    inline constexpr auto AFFINE_PARAM_D = registral_series<const_ptr<volatile fixed<short, 8>[32]>(0x700001E), 32>{};

    // Cartridge IO

    /**
     * @brief Gamepak IO port data bits.
     * @see <a href="https://mgba-emu.github.io/gbatek/#80000c4h---io-port-data-selectable-w-or-rw">80000C4h - I/O Port Data (selectable W or R/W)</a>
     *
     * @sa IO_PORT_DIRECTION
     * @sa IO_PORT_CONTROL
     */
    inline constexpr auto IO_PORT_DATA = registral<const_ptr<volatile u16>(0x080000C4)>{};

    /**
     * @brief Gamepak IO port data direction.
     * @see <a href="https://mgba-emu.github.io/gbatek/#80000c6h---io-port-direction-for-above-data-port-selectable-w-or-rw">80000C6h - I/O Port Direction (for above Data Port) (selectable W or R/W)</a>
     *
     * 0 for read, 1 for write.
     *
     * @sa IO_PORT_DATA
     * @sa IO_PORT_CONTROL
     * @sa cartdirection
     */
    inline constexpr auto IO_PORT_DIRECTION = registral<const_ptr<volatile cartdirection>(0x080000C6)>{};

    /**
     * @brief Gamepak IO port control.
     * @see <a href="https://mgba-emu.github.io/gbatek/#80000c8h---io-port-control-selectable-w-or-rw">80000C8h - I/O Port Control (selectable W or R/W)</a>
     *
     * Contains the GPIO enable boolean flag.
     *
     * @sa IO_PORT_DATA
     * @sa IO_PORT_DIRECTION
     * @sa cartcontrol
     */
    inline constexpr auto IO_PORT_CONTROL = registral<const_ptr<volatile cartcontrol>(0x080000C8)>{};

} // namespace gba::mmio

#endif // define GBAXX_MMIO_HPP
