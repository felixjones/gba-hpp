/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_VIDEO_DISPSTAT_HPP
#define GBAXX_VIDEO_DISPSTAT_HPP
/** @file */

#include <gba/type.hpp>

namespace gba {

    /**
     * @struct dispstat
     * @brief Display hardware state parameters.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000004h---dispstat---general-lcd-status-readwrite">4000004h - DISPSTAT - General LCD Status (Read/Write)</a>
     *
     * @sa mmio::DISPSTAT
     */
    struct alignas(short) dispstat {
        bool currently_vblank : 1{}; /**< Read only. True when the display hardware is in VBlank. */
        bool currently_hblank : 1{}; /**< Read only. True when the display hardware is in HBlank. */
        bool currently_vcount : 1{}; /**< Read only. True when the display hardware is rendering the current line set in vcount_setting. @sa mmio::VCOUNT */
        bool irq_vblank : 1{}; /**< Set the display hardware to emit an IRQ signal when in VBlank. */
        bool irq_hblank : 1{}; /**< Set the display hardware to emit IRQ signals when in HBlank. */
        bool irq_vcount : 1{}; /**< Set the display hardware to emit an IRQ signal when vcount_setting matches the current rendering line. @sa mmio::VCOUNT */
        short : 2;
        u16 vcount_setting : 8{}; /**< Which line should trigger currently_vcount and/or irq_vcount when the display hardware is at that line. @sa mmio::VCOUNT */
    };

} // namespace gba

#endif // define GBAXX_VIDEO_DISPSTAT_HPP
