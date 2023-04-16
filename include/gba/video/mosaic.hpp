/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_VIDEO_MOSAIC_HPP
#define GBAXX_VIDEO_MOSAIC_HPP

namespace gba {

    struct alignas(int) mosaic {
        unsigned int bg_h_extra : 4 {};
        unsigned int bg_v_extra : 4 {};
        unsigned int obj_h_extra : 4 {};
        unsigned int obj_v_extra : 4 {};
    };

} // namespace gba

#endif // define GBAXX_VIDEO_MOSAIC_HPP
