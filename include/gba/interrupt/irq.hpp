/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_INTERRUPT_IRQ_HPP
#define GBAXX_INTERRUPT_IRQ_HPP
/** @file */

namespace gba {

    /**
     * @struct irq
     * @brief Interrupt mask.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000200h---ie---interrupt-enable-register-rw">4000200h - IE - Interrupt Enable Register (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000202h---if---interrupt-request-flags--irq-acknowledge-rw-see-below">4000202h - IF - Interrupt Request Flags / IRQ Acknowledge (R/W, see below)</a>
     *
     * This is used as both read state of the currently raised interrupts, as well as the write state for enabling IRQs.
     *
     * @sa bios::IntrWait()
     * @sa mmio::IRQ_HANDLER
     * @sa mmio::IE
     * @sa mmio::IF
     */
    struct alignas(short) irq {
        bool vblank : 1{}; /**< IRQ when display enters Vertical Blanking period. @sa mmio::DISPSTAT */
        bool hblank : 1{}; /**< IRQ when display enters Horizontal Blanking period. Occurs 160 times per frame. @sa mmio::DISPSTAT */
        bool vcounter : 1{}; /**< IRQ when PPU renders horziontal line matching counter. @sa mmio::VCOUNT */
        bool timer0 : 1{}; /**< IRQ when timer 0 count overflows. @sa mmio::TIMER0_CONTROL */
        bool timer1 : 1{}; /**< IRQ when timer 1 count overflows. @sa mmio::TIMER1_CONTROL */
        bool timer2 : 1{}; /**< IRQ when timer 2 count overflows. @sa mmio::TIMER2_CONTROL */
        bool timer3 : 1{}; /**< IRQ when timer 3 count overflows. @sa mmio::TIMER3_CONTROL */
        bool serial : 1{}; /**< IRQ related to serial IO transfers. @sa mmio::SIOCNT_NORMAL @sa mmio::SIOCNT_MULTI @sa mmio::SIOCNT_UART @sa mmio::RCNT_JOYBUS @sa mmio::RCNT_GPIO */
        bool dma0 : 1{}; /**< IRQ when DMA 0 completes. @sa mmio::DMA0_CONTROL */
        bool dma1 : 1{}; /**< IRQ when DMA 1 completes. @sa mmio::DMA1_CONTROL */
        bool dma2 : 1{}; /**< IRQ when DMA 2 completes. @sa mmio::DMA2_CONTROL */
        bool dma3 : 1{}; /**< IRQ when DMA 3 completes. @sa mmio::DMA3_CONTROL */
        bool keypad : 1{}; /**< IRQ when keypad keys are pressed. @sa mmio::KEYCNT */
        bool gamepak : 1{}; /**< IRQ when gamepak triggers the DRQ pin. By default, this occurs when a gamepak is removed, but some cartridges may trigger this at other times. */
    };

} // namespace gba

#endif // define GBAXX_INTERRUPT_IRQ_HPP
