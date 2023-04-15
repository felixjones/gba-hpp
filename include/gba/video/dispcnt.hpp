/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_VIDEO_DISPCNT_HPP
#define GBAXX_VIDEO_DISPCNT_HPP

namespace gba {

struct alignas(int) dispcnt {
    unsigned int video_mode : 3 {};
    bool : 1;
    bool show_frame1 : 1 {};
    bool hblank_oam_free : 1 {};
    bool obj_vram_1d : 1 {};
    bool forced_blank : 1 {};
    bool show_bg0 : 1 {};
    bool show_bg1 : 1 {};
    bool show_bg2 : 1 {};
    bool show_bg3 : 1 {};
    bool show_obj : 1 {};
    bool enable_win0 : 1 {};
    bool enable_win1 : 1 {};
    bool enable_obj_win : 1 {};
};

} // namespace gba

#endif // define GBAXX_VIDEO_DISPCNT_HPP
