/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_HARDWARE_TIMER_HPP
#define GBAXX_HARDWARE_TIMER_HPP
/** @file */

#include <gba/type.hpp>

namespace gba {

    /**
     * @enum timer_scale
     * @brief Divisor for the system clock (~16.78MHz).
     *
     * @sa tmcnt_h::scale
     */
    enum class timer_scale : u16 {
        _1 = 0,
        _64 = 1,
        _256 = 2,
        _1024 = 3
    };

    /**
     * @struct tmcnt_h
     * @brief Parameters for timers.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000102h---tm0cnt_h---timer-0-control-rw">4000102h - TM0CNT_H - Timer 0 Control (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000106h---tm1cnt_h---timer-1-control-rw">4000102h - TM1CNT_H - Timer 1 Control (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#400010ah---tm2cnt_h---timer-2-control-rw">4000102h - TM2CNT_H - Timer 2 Control (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#400010eh---tm3cnt_h---timer-3-control-rw">4000102h - TM3CNT_H - Timer 3 Control (R/W)</a>
     *
     * @sa mmio::TIMER0_CONTROL
     * @sa mmio::TIMER1_CONTROL
     * @sa mmio::TIMER2_CONTROL
     * @sa mmio::TIMER3_CONTROL
     * @sa mmio::TIMER_CONTROL
     */
    struct alignas(short) tmcnt_h {
        timer_scale scale : 2{}; /**< The frequency of the timer ticks. @sa timer_scale */
        bool cascade : 1{}; /**< When true the next timer's counter will increment when this timer's counter overflows. @sa mmio::TIMER0_COUNT @sa mmio::TIMER1_COUNT @sa mmio::TIMER2_COUNT @sa mmio::TIMER3_COUNT */
        short : 3;
        bool overflow_irq : 1{}; /**< When true an interrupt will occur when this timer overflows. @sa irq::timer0 @sa irq::timer1 @sa irq::timer2 @sa irq::timer3 */
        bool enabled : 1{}; /**< Enable/start timer. */
    };

} // namespace gba

#endif // define GBAXX_HARDWARE_TIMER_HPP
