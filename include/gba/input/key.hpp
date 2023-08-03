/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_INPUT_KEY_HPP
#define GBAXX_INPUT_KEY_HPP
/** @file */

#include <gba/type/memory.hpp>

namespace gba {

    /**
     * @struct keyinput
     * @brief Read only bitfield of the device key/button state.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000130h---keyinput---key-status-r">4000130h - KEYINPUT - Key Status (R)</a>
     *
     * It is recommended to handle key input via the keystate helper struct.
     *
     * @note The `i_` prefix denotes an inverted key. Because pressed keys are pulled "low" to ground (pressed = 0,
     * released = 1) the boolean state of the keys are inverted (pressed = false, released = true).
     *
     * @sa keystate
     * @sa mmio::KEYINPUT
     */
    struct alignas(short) keyinput {
        bool i_a : 1{true}; /**< Inverted (false = pressed) button A. */
        bool i_b : 1{true}; /**< Inverted (false = pressed) button B. */
        bool i_select : 1{true}; /**< Inverted (false = pressed) button Select. */
        bool i_start : 1{true}; /**< Inverted (false = pressed) button Start. */
        bool i_right : 1{true}; /**< Inverted (false = pressed) button Right directional pad. */
        bool i_left : 1{true}; /**< Inverted (false = pressed) button Left directional pad. */
        bool i_up : 1{true}; /**< Inverted (false = pressed) button Up directional pad. */
        bool i_down : 1{true}; /**< Inverted (false = pressed) button Down directional pad. */
        bool i_r : 1{true}; /**< Inverted (false = pressed) button R shoulder. */
        bool i_l : 1{true}; /**< Inverted (false = pressed) button L shoulder. */

        /**
         * @brief Converts the left/right directional pad buttons to -1, 0, +1.
         *
         * @return The x-axis value from -1 to +1.
         */
        [[nodiscard]]
        constexpr int xaxis() const noexcept {
            int keys = __builtin_bit_cast(short, *this);
            keys = (keys << 26) >> 30; // Sign extend
#if defined(__thumb__)
            return (0 - keys) >> 1; // Optimize for register pressure
#else
            return (keys >> 1) - keys; // Optimize for code density
#endif
        }

        /**
         * @brief Converts the down/up directional pad buttons to -1, 0, +1.
         *
         * @note This function returns -1 for "down", +1 for "up": opposite to screen-space where -1 is "up".
         *
         * @return The y-axis value from -1 to +1.
         */
        [[nodiscard]]
        constexpr int yaxis() const noexcept {
            int keys = __builtin_bit_cast(short, *this);
            keys = (keys << 24) >> 30; // Sign extend
#if defined(__thumb__)
            return (0 - keys) >> 1; // Optimize for register pressure
#else
            return (keys >> 1) - keys; // Optimize for code density
#endif
        }

        /**
         * @brief Return xaxis(), but inverted so right is -1, left is +1.
         *
         * @return The inverted x-axis value from -1 to +1.
         *
         * @sa xaxis()
         */
        [[nodiscard]]
        constexpr int i_xaxis() const noexcept {
            int keys = __builtin_bit_cast(short, *this);
            keys = (keys << 26) >> 30; // Sign extend
#if defined(__thumb__)
            return (keys + 1) >> 1; // Optimize for register pressure
#else
            return keys - (keys >> 1); // Optimize for code density
#endif
        }

        /**
         * @brief Return yaxis(), but inverted so up is -1, down is +1.
         *
         * @note Unlike yaxis(), -1 is "up" which matches screen-space where -1 is also "up".
         *
         * @return The inverted y-axis value from -1 to +1.
         *
         * @sa yaxis()
         */
        [[nodiscard]]
        constexpr int i_yaxis() const noexcept {
            int keys = __builtin_bit_cast(short, *this);
            keys = (keys << 24) >> 30; // Sign extend
#if defined(__thumb__)
            return (keys + 1) >> 1; // Optimize for register pressure
#else
            return keys - (keys >> 1); // Optimize for code density
#endif
        }
    };

    /**
     * @struct keycnt
     * @brief Parameters for the keypad IRQ.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000132h---keycnt---key-interrupt-control-rw">4000132h - KEYCNT - Key Interrupt Control (R/W)</a>
     *
     * True = key is selected for IRQ. False = key is ignored for IRQ.
     *
     * @sa irq::keypad
     * @sa mmio::KEYCNT
     */
    struct alignas(short) keycnt {
        bool a : 1{}; /**< Button A. */
        bool b : 1{}; /**< Button B. */
        bool select : 1{}; /**< Button Select. */
        bool start : 1{}; /**< Button Start. */
        bool right : 1{}; /**< Button Right directional pad. */
        bool left : 1{}; /**< Button Left directional pad. */
        bool up : 1{}; /**< Button Up directional pad. */
        bool down : 1{}; /**< Button Down directional pad. */
        bool r : 1{}; /**< Button R shoulder. */
        bool l : 1{}; /**< Button L shoulder. */
        short : 4;
        bool irq_enabled : 1{}; /**< Enable the keypad to send IRQ interrupts when a key is pressed. */
        bool irq_all : 1{}; /**< The behavior of the keypad IRQ. True = IRQ when all selected buttons are pressed. False = IRQ when any selected button is pressed. */
    };

} // namespace gba

#endif // define GBAXX_INPUT_KEY_HPP
