/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_SOUND_TONE_HPP
#define GBAXX_SOUND_TONE_HPP
/** @file */

#include <gba/type.hpp>

namespace gba {

    /**
     * @struct sound1cnt_l
     * @brief Shift parameters for sound channel 1 (Tone channel 1).
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000060h---sound1cnt_l-nr10---channel-1-sweep-register-rw">4000060h - SOUND1CNT_L (NR10) - Channel 1 Sweep register (R/W)</a>
     *
     * @sa mmio::TONE1_SWEEP
     */
    struct alignas(short) sound1cnt_l {
        u16 sweep_num : 3{}; /**< Number of shifts to occur during playback. */
        bool sweep_increasing : 1{}; /**< Direction of the shifts. True = increasing, false = decreasing.*/
        u16 sweep_time : 3{}; /**< Delay between shifts. This value is `N / 128KHz`, with 0 = off. */
    };

    /**
     * @struct sound1cnt_h
     * @brief Pattern parameters for sound channel 1 (Tone channel 1).
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000062h---sound1cnt_h-nr11-nr12---channel-1-dutylenenvelope-rw">4000062h - SOUND1CNT_H (NR11, NR12) - Channel 1 Duty/Len/Envelope (R/W)</a>
     *
     * @sa mmio::TONE1_PATTERN
     */
    struct alignas(short) sound1cnt_h {
        u16 length : 6{}; /**< Duration of the sound output. Calculated as `(64 - N) / 256 seconds`. */
        u16 duty : 2{}; /**< Ratio of the high-to-low state of the square wave. */
        u16 step_time : 3{}; /**< Delay between amplitude steps. This value is calculated as `N / 64 seconds`. */
        bool step_increasing : 1{}; /**< Direction of the amplitude steps. True = increasing, false = decreasing.*/
        u16 volume : 4{}; /**< Initial stating volume/amplitude. */
    };

    /**
     * @struct sound1cnt_x
     * @brief Frequency parameters for sound channel 1 (Tone channel 1).
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000064h---sound1cnt_x-nr13-nr14---channel-1-frequencycontrol-rw">4000064h - SOUND1CNT_X (NR13, NR14) - Channel 1 Frequency/Control (R/W)</a>
     *
     * @sa mmio::TONE1_FREQUENCY
     */
    struct alignas(int) sound1cnt_x {
        u32 frequency : 11{}; /**< Frequency is calcualted as `2^22 / (32 * (2048 - N))`. */
        int : 3;
        bool stop_when_expired : 1{}; /**< Stop playback when sound1cnt_h::length has expired. @sa sound1cnt_h */
        bool enabled : 1{}; /**< Write true to begin playback. */
    };

    /**
     * @struct sound2cnt_h
     * @brief Pattern parameters for sound channel 2 (Tone channel 2).
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000062h---sound1cnt_h-nr11-nr12---channel-1-dutylenenvelope-rw">4000062h - SOUND1CNT_H (NR11, NR12) - Channel 1 Duty/Len/Envelope (R/W)</a>
     *
     * @sa mmio::TONE2_PATTERN
     */
    using sound2cnt_l = sound1cnt_h;

    /**
     * @struct sound2cnt_h
     * @brief Frequency parameters for sound channel 2 (Tone channel 2).
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000064h---sound1cnt_x-nr13-nr14---channel-1-frequencycontrol-rw">4000064h - SOUND1CNT_X (NR13, NR14) - Channel 1 Frequency/Control (R/W)</a>
     *
     * @sa mmio::TONE2_FREQUENCY
     */
    using sound2cnt_h = sound1cnt_x;

} // namespace gba

#endif // define GBAXX_SOUND_TONE_HPP
