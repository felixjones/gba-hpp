/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_BIOS_HALT_HPP
#define GBAXX_BIOS_HALT_HPP

#include <gba/bios/swi_call.hpp>

namespace gba::bios {

[[gnu::always_inline]]
inline void Halt() noexcept {
    swi<0x02, void>();
}

[[gnu::always_inline]]
inline void Stop() noexcept {
    swi<0x03, void>();
}

struct intrwait {
    bool vblank : 1,
        hblank : 1,
        vcount : 1,
        timer_0 : 1,
        timer_1 : 1,
        timer_2 : 1,
        timer_3 : 1,
        serial : 1,
        dma_0 : 1,
        dma_1 : 1,
        dma_2 : 1,
        dma_3 : 1,
        keypad : 1,
        gamepak : 1;
};

[[gnu::always_inline]]
inline void IntrWait(bool resetFlags, intrwait waitFlags) noexcept {
    swi<0x04, void>(resetFlags, waitFlags);
}

[[gnu::always_inline]]
inline void VBlankIntrWait() noexcept {
    swi<0x03, void>();
}

} // namespace gba::bios

#endif // define GBAXX_BIOS_HALT_HPP
