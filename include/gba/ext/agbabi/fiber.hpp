/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_FIBER_HPP
#define GBAXX_EXT_AGBABI_FIBER_HPP

#include <array>
#include <concepts>
#include <functional>
#include <memory>
#include <type_traits>

#include <agbabi.h>

#include <gba/util/array_traits.hpp>

namespace gba::agbabi {
namespace detail {

    template <class T>
    concept PointerEnd = requires(T& a) {
        { std::end(a) } -> std::same_as<util::array_value_type<T>*>;
    };

    template <class Type, typename... Args>
    auto* stack_emplace(std::ptrdiff_t& stackEnd, Args... args) noexcept {
        stackEnd -= sizeof(Type);
        stackEnd &= ~0x3;
        return std::construct_at<Type>(reinterpret_cast<Type*>(stackEnd), std::forward<Args>(args)...);
    }

    template <typename Type>
    auto* stack_put(std::ptrdiff_t& stackEnd, const Type& type) noexcept {
        stackEnd -= sizeof(type);
        stackEnd &= ~0x3;
        return std::construct_at<Type>(reinterpret_cast<Type*>(stackEnd), type);
    }

} // namespace detail

class fiber : public agbabi_coro_t {
public:
    class yield_type {
        friend fiber;
    public:
        explicit yield_type(agbabi_coro_t* owner) noexcept : m_owner{owner} {}

        void operator()() noexcept {
            m_hasYielded = true;
            __agbabi_coro_yield(m_owner, 0);
        }
    private:
        yield_type(yield_type&& other) noexcept : m_owner{other.m_owner}, m_hasYielded{other.m_hasYielded} {
            other.m_owner = nullptr;
        }

        agbabi_coro_t* m_owner{};
        bool m_hasYielded{false};
    };

    using function_type = std::function<void(yield_type&)>;

    template <class Stack> requires detail::PointerEnd<Stack>
    fiber(Stack& stack, function_type&& function) noexcept : agbabi_coro_t{} {
        auto stackEnd = reinterpret_cast<std::ptrdiff_t>(std::end(stack));
        m_yield = detail::stack_emplace<yield_type>(stackEnd, this);
        m_function = detail::stack_put(stackEnd, function);
        __agbabi_coro_make(this, reinterpret_cast<void*>(stackEnd), invoke);
    }

    fiber(fiber&& other) noexcept : agbabi_coro_t{other.arm_sp, other.joined}, m_yield{other.m_yield}, m_function{other.m_function} {
        other.arm_sp = other.joined = 0;
        other.m_yield = nullptr;
        other.m_function = nullptr;
        m_yield->m_owner = this;
    }

    void operator()() noexcept {
        m_yield->m_hasYielded = false;
        __agbabi_coro_resume(this);
    }

    [[nodiscard]]
    explicit operator auto() const noexcept {
        return m_yield->m_hasYielded;
    }
private:
    static int invoke(agbabi_coro_t* self) noexcept {
        auto* f = reinterpret_cast<fiber*>(self);
        std::invoke(*f->m_function, *f->m_yield);
        return 0;
    }

    yield_type* m_yield{};
    function_type* m_function{};
};

} // namespace gba::agbabi

#endif // define GBAXX_EXT_AGBABI_FIBER_HPP
