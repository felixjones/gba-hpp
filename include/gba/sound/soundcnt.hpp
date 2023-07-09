/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_SOUND_SOUNDCNT_HPP
#define GBAXX_SOUND_SOUNDCNT_HPP

#include <gba/type.hpp>

namespace gba {

    struct alignas(short) soundcnt_l {
        u16 right_volume : 3{};
        short : 1;
        u16 left_volume : 3{};
        short : 1;
        bool tone1_right : 1{};
        bool tone2_right : 1{};
        bool wave_right : 1{};
        bool noise_right : 1{};
        bool tone1_left : 1{};
        bool tone2_left : 1{};
        bool wave_left : 1{};
        bool noise_left : 1{};
    };

    struct alignas(short) soundcnt_h {
        u16 psg : 2{};
        bool sound_a_full : 1{};
        bool sound_b_full : 1{};
        short : 3;
        bool sound_a_right : 1{};
        bool sound_a_left : 1{};
        bool sound_a_timer : 1{};
        bool sound_a_reset : 1{};
        bool sound_b_right : 1{};
        bool sound_b_left : 1{};
        bool sound_b_timer : 1{};
        bool sound_b_reset : 1{};
    };

    struct alignas(char) soundcnt_x {
        bool tone1_playing : 1{};
        bool tone2_playing : 1{};
        bool wave_playing : 1{};
        bool noise_playing : 1{};
        char : 3;
        bool enabled : 1{};
    };

    enum class sample_cycle : u16 {
        _9bit = 0,
        _8bit = 1,
        _7bit = 2,
        _6bit = 3,
    };

    struct alignas(short) soundbias {
        short : 1;
        u16 bias_level : 9{};
        short : 4;
        gba::sample_cycle sample_cycle : 2{};
    };

} // namespace gba

#endif // define GBAXX_SOUND_SOUNDCNT_HPP
