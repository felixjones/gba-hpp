/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_HARDWARE_MEMCNT_HPP
#define GBAXX_HARDWARE_MEMCNT_HPP

#include <gba/type.hpp>

namespace gba {

    struct alignas(int) memcnt {
        bool swap_bios : 1{};
        int : 2;
        bool disable_cgb_bios : 1{};
        int : 1;
        bool ewram : 1{};
        int : 18;
        u32 ws_ewram : 4{};
    };

    static constexpr auto memcnt_default = memcnt{.ewram = true, .ws_ewram = 0xd};
    static constexpr auto memcnt_fast_ewram = memcnt{.ewram = true, .ws_ewram = 0xe};

} // namespace gba

#endif // define GBAXX_HARDWARE_MEMCNT_HPP
