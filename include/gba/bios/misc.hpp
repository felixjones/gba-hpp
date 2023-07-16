/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_BIOS_MISC_HPP
#define GBAXX_BIOS_MISC_HPP

namespace gba::bios {

namespace {

    [[gnu::always_inline, gnu::const]]
    inline int GetBiosChecksum() noexcept {
        register int i asm("r0");
        asm volatile inline ("swi 0xD << ((1f - . == 4) * -16); 1:" : "=r"(i) :: "r1", "r3");
        return i;
    }

}

} // namespace gba::bios

#endif // define GBAXX_BIOS_MISC_HPP
