/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_PUSH_COROUTINE_HPP
#define GBAXX_EXT_AGBABI_PUSH_COROUTINE_HPP

#include <functional>

#include <agbabi.h>

#include <gba/ext/agbabi/fiber.hpp>

namespace gba::agbabi {

template <class Type>
class push_coroutine : public agbabi_coro_t {
public:
    using value_type = Type;

    class pull_type {
        friend push_coroutine;
    private:
        struct sentinel {};
    public:
        class iterator {
        public:
            explicit iterator(pull_type& pull) noexcept : m_pull{&pull} {
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
            pull_type* const m_pull{};
        };

        explicit pull_type(agbabi_coro_t* owner) noexcept : m_owner{owner} {}

        auto& operator()() noexcept {
            m_value.reset();
            __agbabi_coro_yield(m_owner, 0);
            return m_value.value();
        }

        [[nodiscard]]
        explicit operator auto() const noexcept {
            return m_value.has_value();
        }

        [[nodiscard]]
        auto& get() const noexcept {
            return m_value.value();
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
        pull_type(pull_type&& other) noexcept : m_owner{other.m_owner}, m_value{std::move(other.m_value)} {
            other.m_owner = nullptr;
            other.m_value.reset();
        }

        agbabi_coro_t* m_owner{};
        std::optional<value_type> m_value{};
    };

    using function_type = std::function<void(pull_type&)>;

    template <class Stack> requires detail::PointerEnd<Stack>
    push_coroutine(Stack& stack, function_type&& function) noexcept : agbabi_coro_t{} {
        auto stackEnd = reinterpret_cast<std::ptrdiff_t>(std::end(stack));
        m_pull = detail::stack_emplace<pull_type>(stackEnd, this);
        m_function = detail::stack_put(stackEnd, function);
        __agbabi_coro_make(this, reinterpret_cast<void*>(stackEnd), invoke);
    }

    push_coroutine(push_coroutine&& other) noexcept : agbabi_coro_t{other.arm_sp, other.joined}, m_pull{other.m_pull}, m_function{other.m_function} {
        other.arm_sp = other.joined = 0;
        other.m_yield = nullptr;
        other.m_function = nullptr;
        m_pull->m_owner = this;
    }

    void operator()(const value_type& value) noexcept {
        if (m_function) {
            __agbabi_coro_resume(this);
        }
        m_pull->m_value.emplace(value);
        __agbabi_coro_resume(this);
    }

    void operator()(value_type&& value) noexcept {
        if (m_function) {
            __agbabi_coro_resume(this);
        }
        m_pull->m_value.emplace(std::move(value));
        __agbabi_coro_resume(this);
    }

    [[nodiscard]]
    explicit operator bool() const noexcept {
        return joined == 0;
    }
private:
    static int invoke(agbabi_coro_t* self) noexcept {
        auto* f = reinterpret_cast<push_coroutine*>(self);
        auto* function = f->m_function;
        f->m_function = nullptr;
        std::invoke(*function, *f->m_pull);
        f->m_function = function;
        return 0;
    }

    pull_type* m_pull{};
    function_type* m_function{};
};

} // namespace gba::agbabi

#endif // define GBAXX_EXT_AGBABI_PUSH_COROUTINE_HPP
