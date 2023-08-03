/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_SOUND_SOUNDCNT_HPP
#define GBAXX_SOUND_SOUNDCNT_HPP
/** @file */

#include <gba/type.hpp>

namespace gba {

    /**
     * @struct soundcnt_l
     * @brief Stereo volume parameters and sound channel enable flags.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000080h---soundcnt_l-nr50-nr51---channel-lr-volumeenable-rw">4000080h - SOUNDCNT_L (NR50, NR51) - Channel L/R Volume/Enable (R/W)</a>
     *
     * @sa mmio::LEFT_RIGHT_VOLUME
     */
    struct alignas(short) soundcnt_l {
        u16 right_volume : 3{}; /**< Right-channel stereo volume. */
        short : 1;
        u16 left_volume : 3{}; /**< Left-channel stereo volume. */
        short : 1;
        bool tone1_right : 1{}; /**< Enable tone1 output on right-channel. @sa sound1cnt_l @sa sound1cnt_h @sa sound1cnt_x */
        bool tone2_right : 1{}; /**< Enable tone2 output on right-channel. @sa sound2cnt_l @sa sound2cnt_h */
        bool wave_right : 1{}; /**< Enable wave output on right-channel. @sa sound3cnt_l @sa sound3cnt_h @sa sound3cnt_x */
        bool noise_right : 1{}; /**< Enable noise output on right-channel. @sa sound4cnt_l @sa sound4cnt_h */
        bool tone1_left : 1{}; /**< Enable tone1 output on left-channel. @sa sound1cnt_l @sa sound1cnt_h @sa sound1cnt_x */
        bool tone2_left : 1{}; /**< Enable tone2 output on left-channel. @sa sound2cnt_l @sa sound2cnt_h */
        bool wave_left : 1{}; /**< Enable wave output on left-channel. @sa sound3cnt_l @sa sound3cnt_h @sa sound3cnt_x */
        bool noise_left : 1{}; /**< Enable noise output on left-channel. @sa sound4cnt_l @sa sound4cnt_h */
    };

    /**
     * @enum volume
     * @brief Volume levels for soundcnt_h::volume.
     *
     * The available volume levels are: 25%, 75%, and 100%.
     *
     * @sa soundcnt_h
     */
    enum class volume : u16 {
        _25 = 0,
        _75 = 1,
        _100 = 2,
    };

    /**
     * @struct soundcnt_h
     * @brief Parameters for PCM sound channels A and B.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000080h---soundcnt_l-nr50-nr51---channel-lr-volumeenable-rw">4000080h - SOUNDCNT_L (NR50, NR51) - Channel L/R Volume/Enable (R/W)</a>
     *
     * @sa mmio::SOUND_MIX
     */
    struct alignas(short) soundcnt_h {
        gba::volume volume : 2{}; /**< Volume level percentage. @sa volume */
        bool sound_a_full : 1{}; /**< PCM channel A sound volume. True = 100%, false = 50%. */
        bool sound_b_full : 1{}; /**< PCM channel B sound volume. True = 100%, false = 50%. */
        short : 3;
        bool sound_a_right : 1{}; /**< Enable PCM channel A to output on the right stereo channel. */
        bool sound_a_left : 1{}; /**< Enable PCM channel A to output on the left stereo channel. */
        u16 sound_a_timer : 1{}; /**< Timer to be used for PCM channel A frequency. Only timers 0 and 1 are available for PCM playback. @sa mmio::TIMER0_CONTROL @sa mmio::TIMER1_CONTROL */
        bool sound_a_reset : 1{}; /**< When true, the FIFO pipeline is cleared before playback begins, clearing any previous PCM output. */
        bool sound_b_right : 1{}; /**< Enable PCM channel B to output on the right stereo channel. */
        bool sound_b_left : 1{}; /**< Enable PCM channel B to output on the left stereo channel. */
        u16 sound_b_timer : 1{};/**< Timer to be used for PCM channel B frequency. Only timers 0 and 1 are available for PCM playback. @sa mmio::TIMER0_CONTROL @sa mmio::TIMER1_CONTROL */
        bool sound_b_reset : 1{}; /**< When true, the FIFO pipeline is cleared before playback begins, clearing any previous PCM output. */
    };

    /**
     * @struct soundcnt_x
     * @brief Parameters for enabling the various sound channels.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000084h---soundcnt_x-nr52---sound-onoff-rw">4000084h - SOUNDCNT_X (NR52) - Sound on/off (R/W)</a>
     *
     * @sa mmio::SOUND_ENABLED
     */
    struct alignas(char) soundcnt_x {
        bool tone1_playing : 1{}; /**< Read-only indicator of the playback status for tone1. */
        bool tone2_playing : 1{}; /**< Read-only indicator of the playback status for tone2. */
        bool wave_playing : 1{}; /**< Read-only indicator of the playback status for wave. */
        bool noise_playing : 1{}; /**< Read-only indicator of the playback status for noise. */
        char : 3;
        bool enabled : 1{}; /**< Enable flag for all sound output. */
    };

    /**
     * @enum sample
     * @brief Sound amplitude resolution/bit-depth.
     */
    enum class sample : u16 {
        _9bit = 0,
        _8bit = 1,
        _7bit = 2,
        _6bit = 3,
    };

    /**
     * @struct soundbias
     * @brief Final sound output resolution.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000088h---soundbias---sound-pwm-control-rw-see-below">4000088h - SOUNDBIAS - Sound PWM Control (R/W, see below)</a>
     *
     * @sa mmio::SOUNDBIAS
     */
    struct alignas(short) soundbias {
        short : 1;
        u16 bias_level : 9{}; /**< Default value is 0x100. */
        short : 4;
        sample cycle : 2{}; /**< Default value is 9bit: ideal for PCM channels A and B playback. 6bit is ideal for tone1/tone2/wave/noise channels. */
    };

} // namespace gba

#endif // define GBAXX_SOUND_SOUNDCNT_HPP
