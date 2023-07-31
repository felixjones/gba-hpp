/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_HARDWARE_MEMCNT_HPP
#define GBAXX_HARDWARE_MEMCNT_HPP

#include <gba/type.hpp>

namespace gba {

    /**
     * @brief Represents hardware memory control.
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000800h---32bit---undocumented---internal-memory-control-rw">4000800h - 32bit - Undocumented - Internal Memory Control (R/W)</a>
     *
     * @sa mmio::MEMCNT
     */
    struct alignas(int) memcnt {
        bool swap_bios : 1{};
        int : 2;
        bool disable_cgb_bios : 1{};
        int : 1;
        bool ewram : 1{};
        int : 18;
        u32 ws_ewram : 4{};
    };

    /**
     * @brief Default value for mmio::MEMCNT.
     *
     * Initialised to 0D000020h by hardware.
     *
     * @sa memcnt
     * @sa mmio::MEMCNT
     */
    static constexpr auto memcnt_default = memcnt{.ewram = true, .ws_ewram = 0xd};

    /**
     * @brief Fast EWRAM memory counter options for memcnt structure.
     *
     * @warning Fast EWRAM memory is not compatible with OXY, DS, or any other devices after AGS101.
     *
     * @sa memcnt
     * @sa mmio::MEMCNT
     */
    static constexpr auto memcnt_fast_ewram = memcnt{.ewram = true, .ws_ewram = 0xe};

} // namespace gba

#endif // define GBAXX_HARDWARE_MEMCNT_HPP
