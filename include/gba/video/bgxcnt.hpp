/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_VIDEO_BGXCNT_HPP
#define GBAXX_VIDEO_BGXCNT_HPP

namespace gba {

    struct alignas(short) bgcnt {
        unsigned short priority : 2 {};
        unsigned short charblock : 2 {};
        bool mosaic : 1 {};
        bool bpp8 : 1 {};
        unsigned short screenblock : 5 {};
        bool is_affine_wrapping : 1 {};
        unsigned short size : 2 {};
    };

} // namespace gba

#endif // define GBAXX_VIDEO_BGXCNT_HPP
