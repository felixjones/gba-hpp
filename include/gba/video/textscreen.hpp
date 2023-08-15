/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_VIDEO_TEXTSCREEN_HPP
#define GBAXX_VIDEO_TEXTSCREEN_HPP
/** @file */

#include <gba/type.hpp>

namespace gba {

    /**
     * @struct textscreen
     * @brief Tile parameters for screen entries.
     * @see <a href="https://mgba-emu.github.io/gbatek/#text-bg-screen-2-bytes-per-entry">Text BG Screen (2 bytes per entry)</a>
     *
     * @sa mmio::TEXT_SCREENBLOCKS
     */
    struct alignas(short) textscreen {
        u16 tile : 10{}; /**< Which character graphic to use for this entry. This is relative to the character block for the background this screen belongs to. @sa bgcnt::charblock */
        bool hflip : 1{}; /**< Should this tile be flipped horizontally. */
        bool vflip : 1{}; /**< Should this tile be flipped vertically. */
        u16 palbank : 4{}; /**< Which 16-color palette bank entry should be used for this tile. @sa mmio::BG_PALETTE */
    };

} // namespace gba

#endif // define GBAXX_VIDEO_TEXTSCREEN_HPP
