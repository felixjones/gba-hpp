/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_REGISTERS_IRQ_HPP
#define GBAXX_REGISTERS_IRQ_HPP

#include <gba/ioregister.hpp>

#include <gba/registers/irq_types.hpp>

namespace gba::reg {

struct ime : io::register_ptr<bool> {
    static constexpr auto address = 0x4000208;

    consteval ime() noexcept : io::register_ptr<bool>{address} {}

    static void reset() noexcept {
        io::register_emplace<bool, address>();
    }

    static void set(bool value) noexcept {
        io::register_write<decltype(value), address>(value);
    }

    static auto get() noexcept {
        return io::register_read<bool, address>();
    }
};

struct ie : io::register_ptr<irq_type> {
    static constexpr auto address = 0x4000200;

    consteval ie() noexcept : io::register_ptr<irq_type>{address} {}

    static void reset() noexcept {
        io::register_emplace<irq_type, address>();
    }

    static void set(irq_type value) noexcept {
        io::register_write<decltype(value), address>(value);
    }

    static auto get() noexcept {
        return io::register_read<irq_type, address>();
    }
};

struct if_ : io::register_ptr<irq_type> {
    static constexpr auto address = 0x4000202;

    consteval if_() noexcept : io::register_ptr<irq_type>{address} {}

    static void reset() noexcept {
        io::register_emplace<uint16, address>(uint16{0xffff});
    }

    static void set(irq_type value) noexcept {
        io::register_write<decltype(value), address>(value);
    }

    static auto get() noexcept {
        return io::register_read<irq_type, address>();
    }
};

} // namespace gba::reg

#endif // define GBAXX_REGISTERS_IRQ_HPP
