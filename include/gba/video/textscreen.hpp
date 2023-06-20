/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_VIDEO_TEXTSCREEN_HPP
#define GBAXX_VIDEO_TEXTSCREEN_HPP

#include <gba/type.hpp>

namespace gba {

    struct alignas(short) textscreen {
        u16 tile : 10 {};
        bool hflip : 1 {};
        bool vflip : 1 {};
        u16 palbank : 4 {};
    };

} // namespace gba

#endif // define GBAXX_VIDEO_TEXTSCREEN_HPP
