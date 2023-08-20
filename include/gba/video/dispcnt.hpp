/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_VIDEO_DISPCNT_HPP
#define GBAXX_VIDEO_DISPCNT_HPP
/** @file */

#include <gba/type.hpp>

namespace gba {

    /**
     * @struct dispcnt
     * @brief Parameters for display control.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000000h---dispcnt---lcd-control-readwrite">4000000h - DISPCNT - LCD Control (Read/Write)</a>
     *
     * <table><thead><tr><th>Video Mode</th><th>Text Backgrounds</th><th>Affine Backgrounds</th></tr></thead><tbody><tr><td>0</td><td>0123</td><td>xxxx</td></tr><tr><td>1</td><td>01xx</td><td>xx2x</td></tr><tr><td>2</td><td>xxxx</td><td>xx23</td></tr></tbody></table>
     *
     * <table><thead><tr><th>Video Mode</th><th>Resolution</th><th>Bits-per-pixel</th><th>Pages</th></tr></thead><tbody><tr><td>3</td><td>240x160</td><td>16</td><td>1</td></tr><tr><td>4</td><td>240x160</td><td>8</td><td>2</td></tr><tr><td>5</td><td>160x128</td><td>16</td><td>2</td></tr></tbody></table>
     *
     * @sa mmio::DISPCNT
     */
     struct alignas(int) dispcnt {
        unsigned int video_mode : 3{}; /**< 0, 1, 2 are tiled modes. 3, 4, 5 are bitmap modes. @warning 6 and 7 are invalid values. */
        int : 1;
        u32 page : 1{}; /**< Current bitmap page to be displayed. @warning Only available with bitmap modes 4 & 5. */
        bool hblank_oam_free : 1{}; /**< Enable reading/writing OAM during HBlank. @note This flag is poorly understood as OAM is seemingly always available, regardless of this setting. */
        bool obj_vram_1d : 1{}; /**< Sprite character graphics mapping mode. 2D mapping treats the VRAM as a large bitmap texture. 1D mapping will map subsequent characters based on the object shape/size. */
        bool forced_blank : 1{}; /**< Turns off the display. */
        bool show_bg0 : 1{}; /**< Enable background layer 0. */
        bool show_bg1 : 1{}; /**< Enable background layer 1. */
        bool show_bg2 : 1{}; /**< Enable background layer 2. */
        bool show_bg3 : 1{}; /**< Enable background layer 3. */
        bool show_obj : 1{}; /**< Enable object layer. */
        bool enable_win0 : 1{}; /**< Enable Window 0 for windowing effects. @sa mmio::WIN0H @sa mmio::WIN0V */
        bool enable_win1 : 1{}; /**< Enable Window 1 for windowing effects. @sa mmio::WIN1H @sa mmio::WIN1V */
        bool enable_obj_win : 1{}; /**< Enable windowing effects on the object layer. */
    };

} // namespace gba

#endif // define GBAXX_VIDEO_DISPCNT_HPP
