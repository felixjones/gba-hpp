/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_OVERCLOCK_HPP
#define GBAXX_EXT_AGBABI_OVERCLOCK_HPP

#include <agbabi.h>

#include <gba/mmio.hpp>

namespace gba::agbabi {

namespace {

    inline bool try_overclock_ewram() noexcept {
        if (__agbabi_poll_ewram()) {
            mmio::MEMCNT = memcnt_fast_ewram;
            return true;
        }
        return false;
    }

}

} // namespace gba::agbabi

#endif // define GBAXX_EXT_AGBABI_OVERCLOCK_HPP
