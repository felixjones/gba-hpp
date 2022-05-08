/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_IRQ_HPP
#define GBAXX_EXT_AGBABI_IRQ_HPP

#include <agbabi.h>

namespace gba::agbabi::irq {

    static constexpr auto empty = __agbabi_irq_empty;

    inline auto user(void(*uproc)(short)) noexcept {
        __agbabi_irq_uproc = uproc;
        return __agbabi_irq_user;
    }

} // namespace gba::agbabi::irq

#endif // define GBAXX_EXT_AGBABI_IRQ_HPP
