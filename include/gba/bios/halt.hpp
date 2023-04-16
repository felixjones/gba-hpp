/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_BIOS_HALT_HPP
#define GBAXX_BIOS_HALT_HPP

namespace gba::bios {

namespace {

    [[gnu::always_inline]]
    inline void VBlankIntrWait() noexcept {
        asm volatile inline ("swi 0x5 << ((1f - . == 4) * -16); 1:" ::: "r0", "r1", "r3");
    }

}

} // namespace gba::bios

#endif // define GBAXX_BIOS_HALT_HPP
