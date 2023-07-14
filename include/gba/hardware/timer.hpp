/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_HARDWARE_TIMER_HPP
#define GBAXX_HARDWARE_TIMER_HPP

#include <gba/type.hpp>

namespace gba {

    enum class timer_scale : u16 {
        _1 = 0,
        _64 = 1,
        _256 = 2,
        _1024 = 3
    };

    struct alignas(short) tmcnt_h {
        timer_scale scale : 2{};
        bool cascade : 1{};
        short : 3;
        bool overflow_irq : 1{};
        bool enabled : 1{};
    };

} // namespace gba

#endif // define GBAXX_HARDWARE_TIMER_HPP
