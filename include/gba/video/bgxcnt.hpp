/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_VIDEO_BGXCNT_HPP
#define GBAXX_VIDEO_BGXCNT_HPP
/** @file */

namespace gba {

    /**
     * @struct bgcnt
     * @brief Background control parameters.     *
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000008h---bg0cnt---bg0-control-rw-bg-modes-01-only">4000008h - BG0CNT - BG0 Control (R/W) (BG Modes 0,1 only)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#400000ah---bg1cnt---bg1-control-rw-bg-modes-01-only">400000Ah - BG1CNT - BG1 Control (R/W) (BG Modes 0,1 only)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#400000ch---bg2cnt---bg2-control-rw-bg-modes-012-only">400000Ch - BG2CNT - BG2 Control (R/W) (BG Modes 0,1,2 only)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#400000eh---bg3cnt---bg3-control-rw-bg-modes-02-only">400000Eh - BG3CNT - BG3 Control (R/W) (BG Modes 0,2 only)</a>
     *
     * @sa mmio::BG0CNT
     * @sa mmio::BG1CNT
     * @sa mmio::BG2CNT
     * @sa mmio::BG3CNT
     */
    struct alignas(short) bgcnt {
        unsigned short priority : 2{}; /**< Priority where 0 = highest, 3 = lowest. */
        unsigned short charblock : 2{}; /**< Which 16KiB block contains the character graphics. */
        short : 2;
        bool mosaic : 1{}; /**< Should mosaic effects apply to this background. @sa mmio::MOSAIC */
        bool bpp8 : 1{}; /**< Bit-depth of the character graphics. True = 256 colors, false = 16 colors. @sa tile4bpp @sa tile8bpp */
        unsigned short screenblock : 5{}; /**< Which 2KiB block contains the screen layout data. */
        bool is_affine_wrapping : 1{}; /**< When true, the texturing of affine backgrounds will be repeated off the edge of the background. @warning Only available for backgrounds 2 & 3 with affine mode. */
        unsigned short size : 2{}; /**< Size & shape of the background. This behaves differently for affine mode backgrounds. <table><thead><tr><th>Value</th><th>Text Mode</th><th>Affine Mode</th></tr></thead><tbody><tr><td>0</td><td>256x256 (2KiB)</td><td>128x128 (256 bytes)</td></tr><tr><td>1</td><td>512x256 (4KiB)</td><td>256x256 (1KiB)</td></tr><tr><td>2</td><td>256x512 (4KiB)</td><td>512x512 (4KiB)</td></tr><tr><td>3</td><td>512x512 (8KiB)</td><td>1024x1024 (16KiB)</td></tr></tbody></table> */
    };

} // namespace gba

#endif // define GBAXX_VIDEO_BGXCNT_HPP
