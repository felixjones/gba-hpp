/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_OVERCLOCK_HPP
#define GBAXX_EXT_AGBABI_OVERCLOCK_HPP
/** @file */

#include <agbabi.h>

#include <gba/mmio.hpp>

namespace gba::agbabi {

    /**
     * @brief Attempts to overclock the EWRAM (reduces wait-states).
     * @see <a href="https://mgba-emu.github.io/gbatek/#4000800h---32bit---undocumented---internal-memory-control-rw">4000800h - 32bit - Undocumented - Internal Memory Control (R/W)</a>
     *
     * A section of EWRAM is tested to check if overclocking was successful before applying the overclock and returning
     * the result.
     *
     * @warning Not all GBA models support overclocked EWRAM.
     */
    inline bool try_overclock_ewram() noexcept {
        if (__agbabi_poll_ewram()) {
            mmio::MEMCNT = memcnt_fast_ewram;
            return true;
        }
        return false;
    }

} // namespace gba::agbabi

#endif // define GBAXX_EXT_AGBABI_OVERCLOCK_HPP
