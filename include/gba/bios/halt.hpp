/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_BIOS_HALT_HPP
#define GBAXX_BIOS_HALT_HPP

#include <gba/interrupt/irq.hpp>

namespace gba::bios {

namespace {

    /**
     * @brief Halts the CPU until an interrupt request occurs.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-02h-gba-or-swi-06h-nds7nds9dsi7dsi9---halt">SWI 02h (GBA) or SWI 06h (NDS7/NDS9/DSi7/DSi9) - Halt</a>
     *
     * The CPU is switched into low-power mode, all other circuits (video, sound, timers, serial, keypad, system clock) are kept operating.
     *
     * @warning If interrupts are not enabled the GBA will lock up.
     */
    [[gnu::always_inline]]
    inline void Halt() noexcept {
        asm volatile inline ("swi 0x2 << ((1f - . == 4) * -16); 1:");
    }

    /**
     * @brief Halts the CPU until a specified interrupt request occurs.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-04h-gbands7nds9dsi7dsi9---intrwait-dsi7dsi9bugged">SWI 04h (GBA/NDS7/NDS9/DSi7/DSi9) - IntrWait</a>
     *
     * The CPU is switched into low-power mode, all other circuits (video, sound, timers, serial, keypad, system clock) are kept operating.
     *
     * @param clearCurrent Specifies whether to clear the current interrupt status before waiting.
     * @param flags The specific interrupt flags to wait for.
     *
     * @warning If interrupts are not enabled the GBA will lock up.
     *
     * @sa Halt()
     */
    [[gnu::always_inline]]
    inline void IntrWait(bool clearCurrent, irq flags) noexcept {
        register auto r0 asm("r0") = clearCurrent;
        register auto r1 asm("r1") = flags;
        asm volatile inline ("swi 0x4 << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1)  :: "r3");
    }

    /**
     * @brief Halts the CPU until the next vertical blank interrupt occurs.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-05h-gbands7nds9dsi7dsi9---vblankintrwait-dsi7dsi9bugged">SWI 05h (GBA/NDS7/NDS9/DSi7/DSi9) - VBlankIntrWait</a>
     *
     * The CPU is switched into low-power mode, all other circuits (video, sound, timers, serial, keypad, system clock) are kept operating.
     *
     * @warning If the VBlank interrupt is not enabled the GBA will lock up.
     *
     * @sa IntrWait()
     */
    [[gnu::always_inline]]
    inline void VBlankIntrWait() noexcept {
        asm volatile inline ("swi 0x5 << ((1f - . == 4) * -16); 1:" ::: "r0", "r1", "r3");
    }

    /**
     * @brief Halts the CPU until either Joypad, Game Pak, or General-Purpose-SIO interrupts occurs.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-03h-gba---stop>SWI 03h (GBA) - Stop</a>
     *
     * Switches the GBA into very low power mode. The CPU, System Clock, Sound, Video, SIO-Shift Clock, DMAs, and Timers are stopped.
     *
     * @warning If none of the exiting interrupts are enabled the GBA will lock up.
     *
     * @sa Halt()
     */
    [[gnu::always_inline]]
    inline void Stop() noexcept {
        asm volatile inline ("swi 0x3 << ((1f - . == 4) * -16); 1:" ::: "r0", "r1", "r3");
    }

    /**
     * @brief Calls either Halt() or Stop().
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-27h-gba-or-swi-1fh-nds7dsi7---customhalt-undocumented">SWI 27h (GBA) or SWI 1Fh (NDS7/DSi7) - CustomHalt (Undocumented)</a>
     *
     * @note This is an undocumented feature.
     *
     * @sa Halt()
     * @sa Stop()
     */
    [[gnu::always_inline]]
    inline void CustomHalt(bool stop) noexcept {
        register auto r2 asm("r2") = stop ? 0x80 : 0;
        asm volatile inline ("swi 0x27 << ((1f - . == 4) * -16); 1:" :: "r"(r2) : "r0", "r1", "r3");
    }

}

} // namespace gba::bios

#endif // define GBAXX_BIOS_HALT_HPP
