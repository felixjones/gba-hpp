/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_INTERRUPT_IRQ_HPP
#define GBAXX_INTERRUPT_IRQ_HPP

namespace gba {

    struct alignas(short) irq {
        bool vblank : 1 {};
        bool hblank : 1 {};
        bool vcounter : 1 {};
        bool timer0 : 1 {};
        bool timer1 : 1 {};
        bool timer2 : 1 {};
        bool timer3 : 1 {};
        bool serial : 1 {};
        bool dma0 : 1 {};
        bool dma1 : 1 {};
        bool dma2 : 1 {};
        bool dma3 : 1 {};
        bool keypad : 1 {};
        bool gamepak : 1 {};
    };

} // namespace gba

#endif // define GBAXX_INTERRUPT_IRQ_HPP
