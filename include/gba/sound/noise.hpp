/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_SOUND_NOISE_HPP
#define GBAXX_SOUND_NOISE_HPP
/** @file */

#include <gba/type.hpp>

namespace gba {

    /**
     * @struct sound4cnt_l
     * @brief Length and volume parameters for sound channel 4 (Noise channel).
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000078h---sound4cnt_l-nr41-nr42---channel-4-lengthenvelope-rw">4000078h - SOUND4CNT_L (NR41, NR42) - Channel 4 Length/Envelope (R/W)</a>
     *
     * @sa mmio::NOISE_LEN_ENV
     */
    struct alignas(int) sound4cnt_l {
        u16 length : 6{}; /**< Length/duration. */
        short : 2;
        u16 step_time : 3{}; /**< Frequency of the envelope step. */
        bool step_increasing : 1{}; /**< True = envelope increases each step. False = envelope decreases. */
        u16 volume : 4{}; /**< Starting volume of the envelope. */
    };

    /**
     * @struct sound4cnt_h
     * @brief Frequency parameters for sound channel 4 (Noise channel).
     * @see <a href="https://mgba-emu.github.io/gbatek/#400007ch---sound4cnt_h-nr43-nr44---channel-4-frequencycontrol-rw">400007Ch - SOUND4CNT_H (NR43, NR44) - Channel 4 Frequency/Control (R/W)</a>
     *
     * @sa mmio::NOISE_FREQ
     */
    struct alignas(int) sound4cnt_h {
        u16 r : 3{}; /**< Frequency division ratio. */
        bool counter7 : 1{}; /**< Counter step size. True = 7-bit, False = 15-bit. */
        u16 s : 4{}; /**< Shift clock frequency. */
        int : 6;
        bool stop_when_expired : 1{}; /**< Stop audio output when sound4cnt_l::length expires. */
        bool enabled : 1{}; /**< True = Restart and begin audio output. */
    };

} // namespace gba

#endif // define GBAXX_SOUND_NOISE_HPP
