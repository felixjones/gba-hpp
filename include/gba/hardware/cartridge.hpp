/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_HARDWARE_CARTRIDGE_HPP
#define GBAXX_HARDWARE_CARTRIDGE_HPP

#include <gba/type.hpp>

namespace gba {

struct alignas(int) waitcnt {
    u32 sram : 2{};
    u32 ws0_first : 2{};
    u32 ws0_second : 1{};
    u32 ws1_first : 2{};
    u32 ws1_second : 1{};
    u32 ws2_first : 2{};
    u32 ws2_second : 1{};
    u32 phi : 2{};
    int : 1;
    bool prefetch : 1{};
    bool cgb : 1{};
};

static constexpr auto waitcnt_default = waitcnt{.sram = 3, .ws0_first = 1, .ws0_second = 1, .ws2_first = 3, .prefetch = true};

struct alignas(short) cartdata {
    u16 data : 4{};
};

struct alignas(short) cartdirection {
    u16 direction : 4{};
};

struct alignas(short) cartcontrol {
    bool enabled : 1{};
};

} // namespace gba

#endif // define GBAXX_HARDWARE_CARTRIDGE_HPP
