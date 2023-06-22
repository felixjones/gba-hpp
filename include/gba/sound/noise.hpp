/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_SOUND_NOISE_HPP
#define GBAXX_SOUND_NOISE_HPP

#include <gba/type.hpp>

namespace gba {

    struct alignas(int) sound4cnt_l {
        u16 length : 6 {};
        short : 2;
        u16 step_time : 3 {};
        bool step_increasing : 1 {};
        u16 volume : 4 {};
    };

    struct alignas(int) sound4cnt_h {
        u16 r : 3 {};
        bool counter7 : 1 {};
        u16 s : 4 {};
        int : 6;
        bool stop_when_expired : 1 {};
        bool enabled : 1 {};
    };

} // namespace gba

#endif // define GBAXX_SOUND_NOISE_HPP
