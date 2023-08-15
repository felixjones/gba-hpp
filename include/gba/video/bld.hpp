/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_VIDEO_BLD_HPP
#define GBAXX_VIDEO_BLD_HPP
/** @file */

#include <gba/type.hpp>

namespace gba {

    /**
     * @enum color_effect
     * @brief Blend effect modes.
     *
     * @sa bldcnt
     */
    enum class color_effect : u16 {
        no_effect = 0, /**< Normal rendering/blending disabled. */
        alpha_blend = 1, /**< Alpha blending/double exposure. @sa mmio::BLDALPHA */
        brighten = 2, /**< Gamma/brightness increase. @sa mmio::BLDY */
        darken = 3, /**< Gamma/brightness decrease. @sa mmio::BLDY */
    };

    /**
     * @struct bldcnt
     * @brief Parameters for blending control.
     *
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000050h---bldcnt---color-special-effects-selection-rw">4000050h - BLDCNT - Color Special Effects Selection (R/W)</a>
     *
     * @sa mmio::BLDCNT
     */
     struct alignas(short) bldcnt {
        bool target1_bg0 : 1{}; /**< Color A from background 0. */
        bool target1_bg1 : 1{}; /**< Color A from background 1. */
        bool target1_bg2 : 1{}; /**< Color A from background 2. */
        bool target1_bg3 : 1{}; /**< Color A from background 3. */
        bool target1_obj : 1{}; /**< Color A from the sprite/object layer. */
        bool target1_backdrop : 1{}; /**< Color A from the backdrop color (Palette entry 0). @sa mmio::BG_PALETTE */
        color_effect mode : 2{}; /**< Operation to apply between color A and color B. @sa color_effect */
        bool target2_bg0 : 1{}; /**< Color B from background 0. */
        bool target2_bg1 : 1{}; /**< Color B from background 1. */
        bool target2_bg2 : 1{}; /**< Color B from background 2. */
        bool target2_bg3 : 1{}; /**< Color B from background 3. */
        bool target2_obj : 1{}; /**< Color B from the sprite/object layer. */
        bool target2_backdrop : 1{}; /**< Color B from the backdrop color (Palette entry 0). @sa mmio::BG_PALETTE */
    };

} // namespace gba

#endif // define GBAXX_VIDEO_BLD_HPP
