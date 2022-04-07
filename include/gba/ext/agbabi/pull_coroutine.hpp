/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_PULL_COROUTINE_HPP
#define GBAXX_EXT_AGBABI_PULL_COROUTINE_HPP

#include <functional>

#include <agbabi.h>

#include <gba/ext/agbabi/fiber.hpp>

namespace gba::agbabi {

template <class Type>
class pull_coroutine : public agbabi_coro_t {
private:
    struct sentinel {};
public:
    using value_type = Type;

    class iterator {
    public:
        explicit iterator(pull_coroutine& pull) noexcept : m_pull{&pull} {
            std::invoke(*m_pull);
        }

        [[nodiscard]]
        auto operator!=(sentinel) const noexcept {
            return bool{*m_pull};
        }

        [[nodiscard]]
        auto operator*() const noexcept {
            return m_pull->get();
        }

        auto& operator++() noexcept {
            std::invoke(*m_pull);
            return *this;
        }
    private:
        pull_coroutine* const m_pull{};
    };

    class push_type {
        friend pull_coroutine;
    public:
        explicit push_type(agbabi_coro_t* owner) noexcept : m_owner{owner} {}

        push_type& operator()(const value_type& value) noexcept {
            m_value.emplace(value);
            __agbabi_coro_yield(m_owner, 0);
            return *this;
        }

        push_type& operator()(value_type&& value) noexcept {
            m_value.emplace(std::move(value));
            __agbabi_coro_yield(m_owner, 0);
            return *this;
        }
    private:
        push_type(push_type&& other) noexcept : m_owner{other.m_owner}, m_value{other.m_value} {
            other.m_owner = nullptr;
        }

        agbabi_coro_t* m_owner{};
        std::optional<value_type> m_value{};
    };

    using function_type = std::function<void(push_type&)>;

    template <class Stack> requires detail::PointerEnd<Stack>
    pull_coroutine(Stack& stack, function_type&& function) noexcept : agbabi_coro_t{} {
        auto stackEnd = reinterpret_cast<std::ptrdiff_t>(std::end(stack));
        m_push = detail::stack_emplace<push_type>(stackEnd, this);
        m_function = detail::stack_put(stackEnd, function);
        __agbabi_coro_make(this, reinterpret_cast<void*>(stackEnd), invoke);
    }

    pull_coroutine(pull_coroutine&& other) noexcept : agbabi_coro_t{other.arm_sp, other.joined}, m_push{other.m_push}, m_function{other.m_function} {
        other.arm_sp = other.joined = 0;
        other.m_yield = nullptr;
        other.m_function = nullptr;
        m_push->m_owner = this;
    }

    void operator()() noexcept {
        __agbabi_coro_resume(this);
    }

    [[nodiscard]]
    explicit operator auto() const noexcept {
        return m_push->m_value.has_value();
    }

    [[nodiscard]]
    auto& get() const noexcept {
        return m_push->m_value.value();
    }

    [[nodiscard]]
    auto begin() noexcept {
        return iterator(*this);
    }

    [[nodiscard]]
    auto end() const noexcept {
        return sentinel{};
    }
private:
    static int invoke(agbabi_coro_t* self) noexcept {
        auto* f = reinterpret_cast<pull_coroutine*>(self);
        std::invoke(*f->m_function, *f->m_push);
        f->m_push->m_value.reset();
        return 0;
    }

    push_type* m_push{};
    function_type* m_function{};
};

} // namespace gba::agbabi

#endif // define GBAXX_EXT_AGBABI_PULL_COROUTINE_HPP
