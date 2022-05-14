/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_REGISTERS_IRQ_TYPES_HPP
#define GBAXX_REGISTERS_IRQ_TYPES_HPP

#include <bit>

#include <gba/fieldtype.hpp>
#include <gba/inttype.hpp>

namespace gba {

struct alignas(uint16) irq_type {
    bool vblank : 1;
    bool hblank : 1;
    bool vcount : 1;
    bool timer0 : 1;
    bool timer1 : 1;
    bool timer2 : 1;
    bool timer3 : 1;
    bool serial : 1;
    bool dma0 : 1;
    bool dma1 : 1;
    bool dma2 : 1;
    bool dma3 : 1;
    bool keypad : 1;
    bool gamepak : 1;
};

namespace irq {

    static constexpr auto vblank = field_of::boolean<irq_type, 0>();
    static constexpr auto hblank = field_of::boolean<irq_type, 1>();
    static constexpr auto vcount = field_of::boolean<irq_type, 2>();
    static constexpr auto timer0 = field_of::boolean<irq_type, 3>();
    static constexpr auto timer1 = field_of::boolean<irq_type, 4>();
    static constexpr auto timer2 = field_of::boolean<irq_type, 5>();
    static constexpr auto timer3 = field_of::boolean<irq_type, 6>();
    static constexpr auto serial = field_of::boolean<irq_type, 7>();
    static constexpr auto dma0 = field_of::boolean<irq_type, 8>();
    static constexpr auto dma1 = field_of::boolean<irq_type, 9>();
    static constexpr auto dma2 = field_of::boolean<irq_type, 10>();
    static constexpr auto dma3 = field_of::boolean<irq_type, 11>();
    static constexpr auto keypad = field_of::boolean<irq_type, 12>();
    static constexpr auto gamepak = field_of::boolean<irq_type, 13>();

} // namespace irq
} // namespace gba

#endif // define GBAXX_REGISTERS_IRQ_TYPES_HPP
