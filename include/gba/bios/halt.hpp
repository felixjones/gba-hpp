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

    [[gnu::always_inline]]
    inline void Halt() noexcept {
        asm volatile inline ("swi 0x2 << ((1f - . == 4) * -16); 1:" ::: "r0", "r1", "r3");
    }

    [[gnu::always_inline]]
    inline void IntrWait(bool clearCurrent, irq flags) noexcept {
        register auto r0 asm("r0") = clearCurrent;
        register auto r1 asm("r1") = flags;
        asm volatile inline ("swi 0x4 << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1)  :: "r3");
    }

    [[gnu::always_inline]]
    inline void VBlankIntrWait() noexcept {
        asm volatile inline ("swi 0x5 << ((1f - . == 4) * -16); 1:" ::: "r0", "r1", "r3");
    }

    [[gnu::always_inline]]
    inline void Stop() noexcept {
        asm volatile inline ("swi 0x3 << ((1f - . == 4) * -16); 1:" ::: "r0", "r1", "r3");
    }

    [[gnu::always_inline]]
    inline void CustomHalt(bool stop) noexcept {
        register auto r2 asm("r2") = stop ? 0x80 : 0;
        asm volatile inline ("swi 0x27 << ((1f - . == 4) * -16); 1:" :: "r"(r2) : "r0", "r1", "r3");
    }

}

} // namespace gba::bios

#endif // define GBAXX_BIOS_HALT_HPP
