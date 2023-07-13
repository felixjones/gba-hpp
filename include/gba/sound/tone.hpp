/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_SOUND_TONE_HPP
#define GBAXX_SOUND_TONE_HPP

#include <gba/type.hpp>

namespace gba {

    struct alignas(short) sound1cnt_l {
        u16 sweep_num : 3{};
        bool sweep_increasing : 1{};
        u16 sweep_time : 3{};
    };

    struct alignas(short) sound1cnt_h {
        u16 length : 6{};
        u16 duty : 2{};
        u16 step_time : 3{};
        bool step_increasing : 1{};
        u16 volume : 4{};
    };

    struct alignas(int) sound1cnt_x {
        u32 frequency : 11{};
        int : 3;
        bool stop_when_expired : 1{};
        bool enabled : 1{};
    };

    using sound2cnt_l = sound1cnt_h;
    using sound2cnt_h = sound1cnt_x;

} // namespace gba

#endif // define GBAXX_SOUND_TONE_HPP
