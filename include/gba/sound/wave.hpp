/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_SOUND_WAVE_HPP
#define GBAXX_SOUND_WAVE_HPP

#include <gba/type.hpp>

namespace gba {

    struct alignas(short) sound3cnt_l {
        short : 5;
        bool two_banks : 1{};
        bool bank1 : 1{};
        bool enabled : 1{};
    };

    struct alignas(short) sound3cnt_h {
        u16 length : 8{};
        short : 5;
        u16 volume : 2{};
        bool force75 : 1{};
    };

    struct alignas(int) sound3cnt_x {
        u32 sample_rate : 11{};
        int : 3;
        bool stop_when_expired : 1{};
        bool enabled : 1{};
    };

} // namespace gba

#endif // define GBAXX_SOUND_WAVE_HPP
