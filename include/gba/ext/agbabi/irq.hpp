/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_IRQ_HPP
#define GBAXX_EXT_AGBABI_IRQ_HPP

#include <bit>

#include <agbabi.h>

#include <gba/registers/irq_types.hpp>

namespace gba::agbabi::irq {

    static constexpr auto empty = __agbabi_irq_empty;

    inline auto user(void(*uproc)(irq_type)) noexcept {
        __agbabi_irq_uproc = std::bit_cast<void(*)(short)>(uproc);
        return __agbabi_irq_user;
    }

} // namespace gba::agbabi::irq

#endif // define GBAXX_EXT_AGBABI_IRQ_HPP
