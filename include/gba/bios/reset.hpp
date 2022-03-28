/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_BIOS_RESET_HPP
#define GBAXX_BIOS_RESET_HPP

#include <gba/bios/swi_call.hpp>

namespace gba::bios {

[[gnu::always_inline]]
inline void SoftReset() noexcept {
    swi<0x00, void>();
    __builtin_unreachable();
}

struct reset {
    bool ewram : 1,
        iwram : 1,
        palette : 1,
        vram : 1,
        oam : 1,
        reg_sio : 1,
        reg_sound : 1,
        reg : 1;
};

[[gnu::always_inline]]
inline void RegisterRamReset(reset flags) noexcept {
    swi<0x01, void>(flags);
}

namespace undocumented {

[[gnu::always_inline]]
inline void HardReset() noexcept {
    swi<0x26, void>();
    __builtin_unreachable();
}

} // namespace undocumented
} // namespace gba::bios

#endif // define GBAXX_BIOS_RESET_HPP
