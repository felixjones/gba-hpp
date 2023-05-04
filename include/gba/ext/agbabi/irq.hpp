/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_IRQ_HPP
#define GBAXX_EXT_AGBABI_IRQ_HPP

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

    using irq_function = std::function<void(irq flags)>;

    [[maybe_unused]]
    void (*irq_empty)() = __agbabi_irq_empty;

    inline auto irq_user(void(*func)(int)) noexcept -> decltype(&__agbabi_irq_user) {
        __agbabi_irq_user_fn = func;
        return __agbabi_irq_user;
    }

    template <std::same_as<void(*)(irq)> Pointer>
    inline auto irq_user(Pointer&& func) noexcept -> decltype(&__agbabi_irq_user) {
        auto f = std::forward<Pointer>(func);
        __agbabi_irq_user_fn = std::bit_cast<decltype(__agbabi_irq_user_fn)>(f);
        return __agbabi_irq_user;
    }

    template <detail::NotSameAs<void(*)(irq)> Pointer>
    inline auto irq_user(Pointer&& func) noexcept -> decltype(&__agbabi_irq_user) {
        if constexpr (detail::ConvertsToFunction<Pointer>) {
            decltype(+func) f = +func;
            __agbabi_irq_user_fn = std::bit_cast<decltype(__agbabi_irq_user_fn)>(f);
        } else {
            static irq_function f = std::forward<Pointer>(func);
            __agbabi_irq_user_fn = [](int irqFlags) {
                f(std::bit_cast<irq>(short(irqFlags)));
            };
        }
        return __agbabi_irq_user;
    }

}

} // namespace gba::agbabi

#endif // define GBAXX_EXT_AGBABI_IRQ_HPP
