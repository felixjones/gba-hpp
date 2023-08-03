/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_SOUND_WAVE_HPP
#define GBAXX_SOUND_WAVE_HPP
/** @file */

#include <gba/type.hpp>

namespace gba {

    /**
     * @struct sound3cnt_l
     * @brief Sound bank parameters for sound channel 3 (Wave channel).
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000070h---sound3cnt_l-nr30---channel-3-stopwave-ram-select-rw">4000070h - SOUND3CNT_L (NR30) - Channel 3 Stop/Wave RAM select (R/W)</a>
     *
     * @sa mmio::WAVE_BANK
     */
    struct alignas(short) sound3cnt_l {
        short : 5;
        bool two_banks : 1{}; /**< Size of the banks. True = use two 32-bit banks. False = use a single 32-bit bank. */
        bool bank1 : 1{}; /**< Which bank is currently playing. The other bank will be the write destination. */
        bool enabled : 1{}; /**< Enable sound channel 3. */
    };

    /**
     * @enum wave_volume
     * @brief Volume levels for sound3cnt_h::volume.
     *
     * The available volume levels are: 0%, 25%, 50%, 75%, and 100%.
     *
     * @sa sound3cnt_h
     */
    enum class wave_volume : u16 {
        _0 = 0,
        _100 = 1,
        _50 = 2,
        _25 = 3,
        _75 = 4,
    };

    /**
     * @struct sound3cnt_h
     * @brief Length and volume parameters for sound channel 3 (Wave channel).
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000072h---sound3cnt_h-nr31-nr32---channel-3-lengthvolume-rw">4000072h - SOUND3CNT_H (NR31, NR32) - Channel 3 Length/Volume (R/W)</a>
     *
     * @sa mmio::WAVE_LEN_VOLUME
     */
    struct alignas(short) sound3cnt_h {
        u16 length : 8{}; /**< Duration of the sound output. Calculated as `(256 - N) / 256 seconds`. */
        short : 5;
        wave_volume volume : 3{}; /**< Output volume percentage. @sa wave_volume */
    };

    /**
     * @struct sound3cnt_x
     * @brief Sampling frequency parameters for sound channel 3 (Wave channel).
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000074h---sound3cnt_x-nr33-nr34---channel-3-frequencycontrol-rw">4000074h - SOUND3CNT_X (NR33, NR34) - Channel 3 Frequency/Control (R/W)</a>
     *
     * @sa mmio::WAVE_FREQ
     */
    struct alignas(int) sound3cnt_x {
        u32 sample_rate : 11{}; /**< Calculated as `2^21 / (2048 - N) Hz`. */
        int : 3;
        bool stop_when_expired : 1{}; /**< Stop playback when sound3cnt_h::length has expired. @sa sound3cnt_h */
        bool enabled : 1{}; /**< Write true to begin playback. */
    };

} // namespace gba

#endif // define GBAXX_SOUND_WAVE_HPP
