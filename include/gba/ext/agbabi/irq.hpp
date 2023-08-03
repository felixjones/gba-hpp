/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_IRQ_HPP
#define GBAXX_EXT_AGBABI_IRQ_HPP
/** @file */

#include <agbabi.h>

#include <gba/interrupt/irq.hpp>

#include <bit>
#include <concepts>
#include <functional>
#include <utility>

namespace gba::agbabi {

    namespace detail {

        template <typename T, typename U>
        concept NotSameAs = !std::same_as<T, U>;

        template <typename T>
        concept ConvertsToFunction = requires(T t) {
            {+t};
        };

    } // namespace detail

namespace {

    /**
     * @brief Empty IRQ handler.
     *
     * This IRQ handler simply acknowledges raised IRQs and returns. This is useful if you need to use BIOS functions
     * that rely on an IRQ, but you do not wish to implement an IRQ handler.
     *
     * @section Setting up VBlank interrupt with an empty IRQ handler:
     * @code{cpp}
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     mmio::IRQ_HANDLER = agbabi::irq_empty;
     *
     *     mmio::DISPSTAT = {.irq_vblank = true};
     *     mmio::IE = {.vblank = true};
     *     mmio::IME = true;
     *
     *     while (true) {
     *         bios::VBlankIntrWait();
     *     }
     * }
     * @endcode
     */
    const auto irq_empty = __agbabi_irq_empty;

    /**
     * @brief Register a user-defined interrupt handler function.
     *
     * This function allows the user to register a function that will be called when an interrupt occurs.
     * The provided function will be called with an integer argument containing the raised IRQ flags.
     *
     * @param func Pointer to the user-defined interrupt handler function.
     * @return func again (useful for chaining calls)
     *
     * @section Setting a user-defined interrupt handler:
     * @code{cpp}
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     mmio::IRQ_HANDLER = irq_user([](int irqFlags) {
     *         if (irqFlags & 0x1) {
     *             // Handle VBlank interrupt
     *         }
     *     });
     *
     *     mmio::DISPSTAT = {.irq_vblank = true};
     *     mmio::IE = {.vblank = true};
     *     mmio::IME = true;
     *
     *     while (true) {
     *         bios::VBlankIntrWait();
     *     }
     * }
     * @endcode
     */
    inline auto irq_user(void(*func)(int)) noexcept -> decltype(&__agbabi_irq_user) {
        __agbabi_irq_user_fn = func;
        return __agbabi_irq_user;
    }

    /**
     * @brief Register a user-defined interrupt handler function.
     *
     * This function allows the user to register a function that will be called when an interrupt occurs.
     * The provided function will be called with an irq struct argument containing the raised IRQ flags.
     *
     * @param func Pointer to the user-defined interrupt handler function.
     * @return func again (useful for chaining calls)
     *
     * @section Setting a user-defined interrupt handler:
     * @code{cpp}
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     mmio::IRQ_HANDLER = irq_user([](irq flags) {
     *         if (flags.vblank) {
     *             // Handle VBlank interrupt
     *         }
     *     });
     *
     *     mmio::DISPSTAT = {.irq_vblank = true};
     *     mmio::IE = {.vblank = true};
     *     mmio::IME = true;
     *
     *     while (true) {
     *         bios::VBlankIntrWait();
     *     }
     * }
     * @endcode
     */
    template <std::same_as<void(*)(irq)> Pointer>
    inline auto irq_user(Pointer&& func) noexcept -> decltype(&__agbabi_irq_user) {
        auto f = std::forward<Pointer>(func);
        __agbabi_irq_user_fn = std::bit_cast<decltype(__agbabi_irq_user_fn)>(f);
        return __agbabi_irq_user;
    }

    /**
     * @brief Register a user-defined interrupt handler function.
     *
     * This variant of irq_user can wrap a lambda function to allow for lambda captures to be passed into the user
     * IRQ handler.
     *
     * @param func Pointer to the user-defined interrupt handler function.
     * @return func again (useful for chaining calls)
     *
     * @section Setting a user-defined interrupt handler with a lambda capture:
     * @code{cpp}
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     int counter = 0;
     *     mmio::IRQ_HANDLER = irq_user([&](irq flags) {
     *         if (flags.vblank) {
     *             counter += 1; // Counts up once per vblank
     *         }
     *     });
     *
     *     mmio::DISPSTAT = {.irq_vblank = true};
     *     mmio::IE = {.vblank = true};
     *     mmio::IME = true;
     *
     *     while (true) {
     *         bios::VBlankIntrWait();
     *         if (counter == 600) {
     *             break; // Break after roughly 10 seconds
     *         }
     *     }
     * }
     * @endcode
     */
    template <detail::NotSameAs<void(*)(irq)> Pointer>
    inline auto irq_user(Pointer&& func) noexcept -> decltype(&__agbabi_irq_user) {
        if constexpr (detail::ConvertsToFunction<Pointer>) {
            decltype(+func) f = +func;
            __agbabi_irq_user_fn = std::bit_cast<decltype(__agbabi_irq_user_fn)>(f);
        } else {
            static std::function<void(irq flags)> f = std::forward<Pointer>(func);
            __agbabi_irq_user_fn = [](int irqFlags) {
                f(std::bit_cast<irq>(short(irqFlags)));
            };
        }
        return __agbabi_irq_user;
    }

}

} // namespace gba::agbabi

#endif // define GBAXX_EXT_AGBABI_IRQ_HPP
