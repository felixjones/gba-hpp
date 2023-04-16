/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_VIDEO_WIN_HPP
#define GBAXX_VIDEO_WIN_HPP

namespace gba {

    struct alignas(short) winin {
        bool win0_bg0 : 1 {};
        bool win0_bg1 : 1 {};
        bool win0_bg2 : 1 {};
        bool win0_bg3 : 1 {};
        bool win0_obj : 1 {};
        bool win0_effect : 1 {};
        short : 2;
        bool win1_bg0 : 1 {};
        bool win1_bg1 : 1 {};
        bool win1_bg2 : 1 {};
        bool win1_bg3 : 1 {};
        bool win1_obj : 1 {};
        bool win1_effect : 1 {};
    };

    struct alignas(short) winout {
        bool outside_bg0 : 1 {};
        bool outside_bg1 : 1 {};
        bool outside_bg2 : 1 {};
        bool outside_bg3 : 1 {};
        bool outside_obj : 1 {};
        bool outside_effect : 1 {};
        short : 2;
        bool obj_win_bg0 : 1 {};
        bool obj_win_bg1 : 1 {};
        bool obj_win_bg2 : 1 {};
        bool obj_win_bg3 : 1 {};
        bool obj_win_obj : 1 {};
        bool obj_win_effect : 1 {};
    };

} // namespace gba

#endif // define GBAXX_VIDEO_WIN_HPP
