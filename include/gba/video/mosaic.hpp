/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_VIDEO_MOSAIC_HPP
#define GBAXX_VIDEO_MOSAIC_HPP
/** @file */

#include <gba/type.hpp>

namespace gba {

    /**
     * @struct mosaic
     * @brief Mosaic effect parameters.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400004ch---mosaic---mosaic-size-w">400004Ch - MOSAIC - Mosaic Size (W)</a>
     *
     * @sa mmio::MOSAIC
     */
    struct alignas(int) mosaic {
        u32 bg_h_extra : 4{}; /**< Number of extra horizontal pixels to repeat when rendering mosaic backgrounds. */
        u32 bg_v_extra : 4{}; /**< Number of extra vertical pixels to repeat when rendering mosaic backgrounds. */
        u32 obj_h_extra : 4{}; /**< Number of extra horizontal pixels to repeat when rendering mosaic objects. */
        u32 obj_v_extra : 4{}; /**< Number of extra vertical pixels to repeat when rendering mosaic objects. */
    };

} // namespace gba

#endif // define GBAXX_VIDEO_MOSAIC_HPP
