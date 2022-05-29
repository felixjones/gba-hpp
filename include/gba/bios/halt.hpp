/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_BIOS_HALT_HPP
#define GBAXX_BIOS_HALT_HPP

#include <gba/fieldtype.hpp>

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

struct intrwait_type {
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

namespace intrwait {

    static constexpr auto vblank = field_of::boolean<intrwait_type, 0>();
    static constexpr auto hblank = field_of::boolean<intrwait_type, 1>();
    static constexpr auto vcount = field_of::boolean<intrwait_type, 2>();
    static constexpr auto timer_0 = field_of::boolean<intrwait_type, 3>();
    static constexpr auto timer_1 = field_of::boolean<intrwait_type, 4>();
    static constexpr auto timer_2 = field_of::boolean<intrwait_type, 5>();
    static constexpr auto timer_3 = field_of::boolean<intrwait_type, 6>();
    static constexpr auto serial = field_of::boolean<intrwait_type, 7>();
    static constexpr auto dma_0 = field_of::boolean<intrwait_type, 8>();
    static constexpr auto dma_1 = field_of::boolean<intrwait_type, 9>();
    static constexpr auto dma_2 = field_of::boolean<intrwait_type, 10>();
    static constexpr auto dma_3 = field_of::boolean<intrwait_type, 11>();
    static constexpr auto keypad = field_of::boolean<intrwait_type, 12>();
    static constexpr auto gamepak = field_of::boolean<intrwait_type, 13>();

} // namespace intrwait

[[gnu::always_inline]]
inline void IntrWait(bool resetFlags, intrwait_type waitFlags) noexcept {
    swi<0x04, void>(resetFlags, waitFlags);
}

[[gnu::always_inline]]
inline void VBlankIntrWait() noexcept {
    swi<0x05, void>();
}

} // namespace gba::bios

#endif // define GBAXX_BIOS_HALT_HPP
