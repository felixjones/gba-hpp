/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_VIDEO_WIN_HPP
#define GBAXX_VIDEO_WIN_HPP
/** @file */

namespace gba {

    /**
     * @struct winin
     * @brief Parameters for behavior when an element is inside a given window.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000048h---winin---control-of-inside-of-windows-rw">4000048h - WININ - Control of Inside of Window(s) (R/W)</a>
     *
     * @sa mmio::WININ
     */
    struct alignas(short) winin {
        bool win0_bg0 : 1{}; /**< Clip background 0 to window 0. */
        bool win0_bg1 : 1{}; /**< Clip background 1 to window 0. */
        bool win0_bg2 : 1{}; /**< Clip background 2 to window 0. */
        bool win0_bg3 : 1{}; /**< Clip background 3 to window 0. */
        bool win0_obj : 1{}; /**< Clip objects to window 0. */
        bool win0_effect : 1{}; /**< Apply color special effects to elements within window 0. */
        short : 2;
        bool win1_bg0 : 1{}; /**< Clip background 0 to window 1. */
        bool win1_bg1 : 1{}; /**< Clip background 1 to window 1. */
        bool win1_bg2 : 1{}; /**< Clip background 2 to window 1. */
        bool win1_bg3 : 1{}; /**< Clip background 3 to window 1. */
        bool win1_obj : 1{}; /**< Clip objects to window 1. */
        bool win1_effect : 1{}; /**< Apply color special effects to elements within window 1. */
    };

    /**
     * @struct winout
     * @brief Parameters for behavior when an element is outside a given window, as well as parameters for object & window effects.
     * @see <a href="https://mgba-emu.github.io/gbatek/#400004ah---winout---control-of-outside-of-windows--inside-of-obj-window-rw">400004Ah - WINOUT - Control of Outside of Windows & Inside of OBJ Window (R/W)</a>
     *
     * @sa mmio::WINOUT
     */
    struct alignas(short) winout {
        bool outside_bg0 : 1{}; /**< Clip background 0 to the outside of both windows. */
        bool outside_bg1 : 1{}; /**< Clip background 1 to the outside of both windows. */
        bool outside_bg2 : 1{}; /**< Clip background 2 to the outside of both windows. */
        bool outside_bg3 : 1{}; /**< Clip background 3 to the outside of both windows. */
        bool outside_obj : 1{}; /**< Clip objects to the outside of both windows. */
        bool outside_effect : 1{}; /**< Apply color special effects to elements outside both windows. */
        short : 2;
        bool obj_win_bg0 : 1{}; /**< Use windowing objects to mask background 0. */
        bool obj_win_bg1 : 1{}; /**< Use windowing objects to mask background 1. */
        bool obj_win_bg2 : 1{}; /**< Use windowing objects to mask background 2. */
        bool obj_win_bg3 : 1{}; /**< Use windowing objects to mask background 3. */
        bool obj_win_obj : 1{}; /**< Use windowing objects to mask other objects. */
        bool obj_win_effect : 1{}; /**< Apply color special effects to elements within windowing objects. */
    };

} // namespace gba

#endif // define GBAXX_VIDEO_WIN_HPP
