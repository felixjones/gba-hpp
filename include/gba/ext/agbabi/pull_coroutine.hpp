/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_PULL_COROUTINE_HPP
#define GBAXX_EXT_AGBABI_PULL_COROUTINE_HPP

#include <utility>

#include <agbabi.h>

#include <gba/ext/agbabi/push_coroutine.hpp>

#include <gba/ext/agbabi/detail/coro_context.hpp>
#include <gba/ext/agbabi/detail/stack.hpp>

namespace gba::agbabi {

template <typename T>
class push_coroutine;

template <typename T>
class pull_coroutine {
private:
    using context_type = detail::context_type<T>;
    using coro_type = detail::coro_type<T>;

    template <typename X>
    friend class push_coroutine;

    explicit pull_coroutine(context_type* ctx) noexcept : m_ctx{ctx} {}

    template <typename Fn>
    struct callable_context_type : context_type {
        explicit callable_context_type(Fn&& fn) noexcept : m_fn{fn} {}

        ~callable_context_type() noexcept override = default;

        void swap() noexcept override {
            __agbabi_coro_resume(context_type::coro);
        }

        static detail::coro_proc_type get_invoke() noexcept;
    private:
        Fn m_fn;
    };

    template <typename Fn>
    auto* make_context(void* sp, Fn&& fn) noexcept {
        auto* coro = detail::stack_emplace<coro_type>(sp);
        auto* ctx = detail::stack_emplace<callable_context_type<Fn>>(coro, std::forward<Fn>(fn));
        auto* value = detail::stack_reserve<T>(ctx);
        __agbabi_coro_make(coro, value, callable_context_type<Fn>::get_invoke());

        coro->ctx = ctx;
        coro->value = value;
        ctx->coro = coro;
        return ctx;
    }

    context_type* m_ctx;
public:
    template <typename Fn>
    pull_coroutine(detail::PointerEnd auto& stack, Fn&& fn) noexcept : m_ctx{make_context(std::end(stack), std::forward<Fn>(fn))} {
    }

    pull_coroutine(const pull_coroutine&) = delete;
    pull_coroutine& operator=(const pull_coroutine&) = delete;

    pull_coroutine(pull_coroutine&& other) noexcept : m_ctx{} {
        std::swap(m_ctx, other.m_ctx);
    }

    auto& operator=(pull_coroutine&& other) noexcept {
        std::swap(m_ctx, other.m_ctx);
        return *this;
    }

    T&& operator()() noexcept {
        m_ctx->swap();
        return std::move(*m_ctx->coro->value);
    }

    [[nodiscard]]
    explicit operator bool() const noexcept {
        return m_ctx && !m_ctx->coro->joined;
    }

    [[nodiscard]]
    bool operator!() const noexcept {
        return !m_ctx || m_ctx->coro->joined;
    }

    class iterator {
    private:
        pull_coroutine* m_coro{};

        void fetch() noexcept {
            (*m_coro)();
            if (!(*m_coro)) {
                m_coro = nullptr;
            }
        }
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = std::remove_reference_t<T>;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        using pointer_type = pointer;
        using reference_type = reference;

        iterator() noexcept = default;

        explicit iterator(pull_coroutine* coro) noexcept : m_coro{coro} {
            fetch();
        }

        bool operator==(const iterator& other) const noexcept {
            return other.m_coro == m_coro;
        }

        bool operator!=(const iterator& other) const noexcept {
            return other.m_coro != m_coro;
        }

        auto& operator++() {
            fetch();
            return *this;
        }

        void operator++(int) {
            fetch();
        }

        auto& operator*() const noexcept {
            return *m_coro->m_ctx->coro->value;
        }

        auto* operator->() const noexcept {
            return m_coro->m_ctx->coro->value;
        }
    };

    auto begin() noexcept {
        return iterator(this);
    }

    auto end() noexcept {
        return iterator();
    }
};

} // namespace gba::agbabi

#include <gba/ext/agbabi/detail/pull_coroutine_context.hpp>

#endif // define GBAXX_EXT_AGBABI_PULL_COROUTINE_HPP
