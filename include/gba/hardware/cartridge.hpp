/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_HARDWARE_CARTRIDGE_HPP
#define GBAXX_HARDWARE_CARTRIDGE_HPP
/** @file */

#include <gba/type.hpp>

namespace gba {

    /**
     * @struct waitcnt
     * @brief Structure representing the configuration of game pak access timings.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000204h---waitcnt---waitstate-control-rw">4000204h - WAITCNT - Waitstate Control (R/W)</a>
     *
     * The game pak ROM is mirrored to three address regions at 08000000h, 0A000000h, and 0C000000h, these areas are
     * called Wait State 0-2. Different access timings may be assigned to each area (this might be useful in case that a
     * game pak contains several ROM chips with different access times each).
     *
     * <table>
     * <tr><td> <b>Member</b> </td><td> <b>Explanation</b>         </td><td> <b>Value = Cycles</b>    </td></tr>
     * <tr><td> sram          </td><td> SRAM Wait Control          </td><td> 0,1,2,3 = 4,3,2,8 cycles </td></tr>
     * <tr><td> ws0_first     </td><td> Wait State 0 First Access  </td><td> 0,1,2,3 = 4,3,2,8 cycles </td></tr>
     * <tr><td> ws0_second    </td><td> Wait State 0 Second Access </td><td> 0,1 = 2,1 cycles         </td></tr>
     * <tr><td> ws1_first     </td><td> Wait State 1 First Access  </td><td> 0,1,2,3 = 4,3,2,8 cycles </td></tr>
     * <tr><td> ws1_second    </td><td> Wait State 1 Second Access </td><td> 0,1 = 4,1 cycles         </td></tr>
     * <tr><td> ws2_first     </td><td> Wait State 2 First Access  </td><td> 0,1,2,3 = 4,3,2,8 cycles </td></tr>
     * <tr><td> ws2_second    </td><td> Wait State 2 Second Access </td><td> 0,1 = 8,1 cycles         </td></tr>
     * </table>
     *
     * @sa mmio::WAITCNT
     */
    struct alignas(int) waitcnt {
        u32 sram: 2{};
        u32 ws0_first: 2{};
        u32 ws0_second: 1{};
        u32 ws1_first: 2{};
        u32 ws1_second: 1{};
        u32 ws2_first: 2{};
        u32 ws2_second: 1{};
        u32 phi: 2{};
        int : 1;
        bool prefetch: 1{}; /**< When prefetch is enabled, the GBA attempts to read opcodes from Game Pak ROM during
                                 periods when the CPU is not using the bus. Memory access is then performed with 0 Waits
                                 if the CPU requests data which is already stored in the buffer. The prefetch buffer
                                 stores up to eight 16bit values. */
        bool cgb: 1{}; /**< Is the console in CGB mode? Always false. */
    };

    /**
     * @var waitcnt_default
     * @brief The default waitcnt configuration.
     *
     * @sa waitcnt
     * @sa mmio::WAITCNT
     */
    static constexpr auto waitcnt_default = waitcnt{.sram = 3, .ws0_first = 1, .ws0_second = 1, .ws2_first = 3, .prefetch = true};

    /**
     * @struct cartdirection
     * @brief Bitmask represents the control of GPIO data direction.
     * @see <a href="https://mgba-emu.github.io/gbatek/#80000c6h---io-port-direction-for-above-data-port-selectable-w-or-rw">80000C6h - I/O Port Direction (for above Data Port) (selectable W or R/W)</a>
     *
     * 0 bit (low) = input (read mmio::IO_PORT_DATA), 1 bit (high) = output (write mmio::IO_PORT_DATA)
     *
     * @section Reading, and then writing, a nibble:
     * @code{cpp}
     * #include <gba/gba.hpp>
     *
     * int read_and_write_nibble(int input) {
     *     using namespace gba;
     *
     *     mmio::IO_PORT_DIRECTION = {.direction = 0b0000}; // Set bits 0-3 to low (read)
     *     const auto output = *mmio::IO_PORT_DATA; // Read nibble
     *
     *     mmio::IO_PORT_DIRECTION = {.direction = 0b1111}; // Set bits 0-3 to high (write)
     *     mmio::IO_PORT_DATA = input & 0xF; // Write nibble
     *
     *     return output;
     * }
     * @endcode
     *
     * @sa mmio::IO_PORT_DIRECTION
     * @sa mmio::IO_PORT_DATA
    */
    struct alignas(short) cartdirection {
        u16 direction: 4{};
    };

    /**
     * @struct cartcontrol
     * @brief Represents the enabled state for GPIO.
     * @see <a href="https://mgba-emu.github.io/gbatek/#80000c8h---io-port-control-selectable-w-or-rw">80000C8h - I/O Port Control (selectable W or R/W)</a>
     *
     * The 'enabled' member is a bit-field with a size of 1 bit, aligned on a short boundary.
     *
     * @sa mmio::IO_PORT_CONTROL
     * @sa mmio::IO_PORT_DIRECTION
     * @sa mmio::IO_PORT_DATA
     */
    struct alignas(short) cartcontrol {
        bool enabled: 1{};
    };

} // namespace gba

#endif // define GBAXX_HARDWARE_CARTRIDGE_HPP
