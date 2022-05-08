/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_INTERRUPT_HANDLER_HPP
#define GBAXX_INTERRUPT_HANDLER_HPP

#include <gba/ioregister.hpp>
#include <gba/ext/agbabi.hpp>

namespace gba::irq {

struct handler_arm {
    static constexpr auto address = 0x3007FFC;

    static void set(void(*handler)()) noexcept {
        io::register_write<void(*)(), address>(handler);
    }

    static auto get() noexcept {
        return io::register_read<void(*)(), address>();
    }
};

#ifdef GBAXX_HAS_AGBABI

struct handler {
    static constexpr auto address = 0x3007FFC;

    static void set(std::nullptr_t) noexcept {
        __agbabi_irq_uproc = nullptr;
        io::register_write<void(*)(), address>(agbabi::irq::empty);
    }

    static void set(void(*handler)(short)) noexcept {
        io::register_write<void(*)(), address>(agbabi::irq::user(handler));
    }

    static auto get() noexcept {
        return __agbabi_irq_uproc;
    }
};

#endif

} // namespace gba::irq

#endif // define GBAXX_INTERRUPT_HANDLER_HPP
