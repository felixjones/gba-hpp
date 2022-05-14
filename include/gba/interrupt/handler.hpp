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

#include <gba/registers/irq.hpp>
#include <gba/registers/irq_types.hpp>

namespace gba::irq {

#ifndef GBAXX_HAS_AGBABI

namespace detail {

inline static auto*& irq_uproc() noexcept {
    static void(*uproc)(irq_type);
    return uproc;
}

[[gnu::target("arm"), gnu::naked]]
inline static void irq_empty() noexcept {
    asm(R"(
        mov     r0, #0x4000000

        // r1 = REG_IE & REG_IF, r0 = &REG_IE_IF
        ldr     r1, [r0, #0x200]!
        and     r1, r1, r1, lsr #16

        // r2 = REG_BIOSIF | r1
        ldr     r2, [r0, #-0x208]
        orr     r2, r2, r1

        // Acknowledge REG_IF and REG_BIOSIF
        strh    r1, [r0, #0x2]
        str     r2, [r0, #-0x208]

        bx      lr
    )");
}

[[gnu::target("arm"), gnu::naked]]
inline static void irq_user_arm_naked(std::remove_reference_t<decltype(irq_uproc())>) noexcept {
    asm(R"(
        mov     r1, #0x4000000

        // r3 = REG_IE & REG_IF, r1 = &REG_IE_IF
        ldr     r3, [r1, #0x200]!
        and     r3, r3, r3, lsr #16

        // r2 = REG_BIOSIF | r3
        ldr     r2, [r1, #-0x208]
        orr     r2, r2, r3

        // Acknowledge REG_IF and REG_BIOSIF
        strh    r3, [r1, #0x2]
        str     r2, [r1, #-0x208]

        // Clear handled from REG_IE
        ldrh    r2, [r1]
        bic     r2, r2, r3
        strh    r2, [r1]

        // Disable REG_IME
        // Use r1/REG_BASE because lowest bit is clear
        str     r1, [r1, #0x8]

        // Change to user mode
        mrs     r2, cpsr
        bic     r2, r2, #0xdf
        orr     r2, r2, #0x1f
        msr     cpsr, r2

        push    {r1, r3-r11, lr}

        // Call uproc
        mov     lr, pc
        bx      r0

        pop     {r1, r3-r11, lr}

        // Disable REG_IME again
        // Use r1/REG_BASE because lowest bit is clear
        str     r1, [r1, #0x8]

        // Change to irq mode
        mrs     r2, cpsr
        bic     r2, r2, #0xdf
        orr     r2, r2, #0x92
        msr     cpsr, r2

        // Restore REG_IE
        ldrh    r2, [r1]
        orr     r2, r2, r3
        strh    r2, [r1]

        // Enable REG_IME
        mov     r2, #1
        str     r2, [r1, #0x8]

        bx      lr
    )");
}

[[gnu::target("arm")]]
inline static void irq_user() noexcept {
    irq_user_arm_naked(irq_uproc());
}

} // namespace detail

#endif

struct handler_arm {
    static constexpr auto address = 0x3007FFC;

    static void set(void(*handler)()) noexcept {
        io::register_write<decltype(handler), address>(handler);
    }

    static auto get() noexcept {
        return io::register_read<void(*)(), address>();
    }
};

struct handler {
    static constexpr auto address = 0x3007FFC;

    static void set(std::nullptr_t) noexcept {
#ifdef GBAXX_HAS_AGBABI
        __agbabi_irq_uproc = nullptr;
        io::register_write<void(*)(), address>(agbabi::irq::empty);
#else
        detail::irq_uproc() = nullptr;
        io::register_write<decltype(&detail::irq_empty), address>(detail::irq_empty);
#endif
    }

    static void set(void(*handler)(irq_type)) noexcept {
#ifdef GBAXX_HAS_AGBABI
        io::register_write<void(*)(), address>(agbabi::irq::user(handler));
#else
        detail::irq_uproc() = handler;
        io::register_write<decltype(&detail::irq_user), address>(detail::irq_user);
#endif
    }

    static auto get() noexcept {
#ifdef GBAXX_HAS_AGBABI
        return std::bit_cast<void(*)(irq_type)>(__agbabi_irq_uproc);
#else
        return detail::irq_uproc();
#endif
    }
};

} // namespace gba::irq

#endif // define GBAXX_INTERRUPT_HANDLER_HPP
