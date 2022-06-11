/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_REGISTERS_TIMER_TYPES_HPP
#define GBAXX_REGISTERS_TIMER_TYPES_HPP

#include <gba/fieldtype.hpp>
#include <gba/inttype.hpp>

namespace gba {

struct alignas(uint16) timer_type {
    uint16 freq : 2;
    bool cascade : 1;
    uint16 : 3;
    bool irq : 1;
    bool enable : 1;
};

namespace timer {

    static constexpr auto freq_1 = field_of::integral<timer_type, 0, 0x3>(0);
    static constexpr auto freq_64 = field_of::integral<timer_type, 0, 0x3>(1);
    static constexpr auto freq_256 = field_of::integral<timer_type, 0, 0x3>(2);
    static constexpr auto freq_1024 = field_of::integral<timer_type, 0, 0x3>(3);

    static constexpr auto cascade = field_of::boolean<timer_type, 2>();
    static constexpr auto irq = field_of::boolean<timer_type, 6>();
    static constexpr auto enable = field_of::boolean<timer_type, 7>();

} // namespace timer

struct tmrec_type {
    uint16 counter;
    timer_type mode;
};

} // namespace gba

#endif // define GBAXX_REGISTERS_TIMER_TYPES_HPP
