/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_VIDEO_BLD_HPP
#define GBAXX_VIDEO_BLD_HPP

namespace gba {

    enum class color_effect : unsigned short {
        no_effect = 0,
        alpha_blend = 1,
        brighten = 2,
        darken = 3
    };

    struct alignas(short) bldcnt {
        bool target1_bg0 : 1;
        bool target1_bg1 : 1;
        bool target1_bg2 : 1;
        bool target1_bg3 : 1;
        bool target1_obj : 1;
        bool target1_backdrop : 1;
        color_effect mode : 2;
        bool target2_bg0 : 1;
        bool target2_bg1 : 1;
        bool target2_bg2 : 1;
        bool target2_bg3 : 1;
        bool target2_obj : 1;
        bool target2_backdrop : 1;
    };

} // namespace gba

#endif // define GBAXX_VIDEO_BLD_HPP
