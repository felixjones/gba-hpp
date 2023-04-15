/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_VIDEO_DISPSTAT_HPP
#define GBAXX_VIDEO_DISPSTAT_HPP

namespace gba {

    struct alignas(short) dispstat {
        const bool currently_vblank : 1 {};
        const bool currently_hblank : 1 {};
        const bool currently_vcount : 1 {};
        bool irq_vblank : 1 {};
        bool irq_hblank : 1 {};
        bool irq_vcount : 1 {};
        short : 2;
        unsigned short vcount_setting : 8 {};
    };

} // namespace gba

#endif // define GBAXX_VIDEO_DISPSTAT_HPP
