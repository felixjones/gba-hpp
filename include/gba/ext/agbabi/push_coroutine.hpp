/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_PUSH_COROUTINE_HPP
#define GBAXX_EXT_AGBABI_PUSH_COROUTINE_HPP

#include <utility>

#include <agbabi.h>

#include <gba/ext/agbabi/pull_coroutine.hpp>

#include <gba/ext/agbabi/detail/coro_context.hpp>
#include <gba/ext/agbabi/detail/stack.hpp>

namespace gba::agbabi {

template <typename T, typename U>
concept IsReferenceTo = (std::is_same_v<U, std::remove_cvref_t<T>>);

template <typename T>
class pull_coroutine;

template <typename T>
class push_coroutine {
private:
    using context_type = detail::context_type<T>;
    using coro_type = detail::coro_type<T>;

    template <typename X>
    friend class pull_coroutine;

    explicit push_coroutine(context_type* ctx) noexcept : m_ctx{ctx} {}

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
    push_coroutine(detail::PointerEnd auto& stack, Fn&& fn) noexcept : m_ctx{make_context(std::end(stack), std::forward<Fn>(fn))} {
        m_ctx->swap(); // Execute until a push is requested
    }

    push_coroutine(const push_coroutine&) = delete;
    push_coroutine& operator=(const push_coroutine&) = delete;

    push_coroutine(push_coroutine&& other) noexcept : m_ctx{} {
        std::swap(m_ctx, other.m_ctx);
    }

    auto& operator=(push_coroutine&& other) noexcept {
        std::swap(m_ctx, other.m_ctx);
        return *this;
    }

    void operator()(IsReferenceTo<T> auto&& value) noexcept {
        *m_ctx->coro->value = value;
        m_ctx->swap();
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
        push_coroutine* m_coro{};
    public:
        using iterator_category = std::output_iterator_tag;
        using value_type = void;
        using difference_type = void;
        using pointer = void;
        using reference = void;

        iterator() noexcept = default;

        explicit iterator(push_coroutine* coro) noexcept : m_coro{coro} {
        }

        auto& operator=(T t) noexcept {
            (*m_coro)(t);
            if (!(*m_coro)) {
                m_coro = nullptr;
            }
            return *this;
        }

        bool operator==(const iterator& other) const noexcept {
            return other.m_coro == m_coro;
        }

        bool operator!=(const iterator& other) const noexcept {
            return other.m_coro != m_coro;
        }

        auto& operator*() noexcept {
            return *this;
        }

        auto& operator++() noexcept {
            return *this;
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

#include <gba/ext/agbabi/detail/push_coroutine_context.hpp>

#endif // define GBAXX_EXT_AGBABI_PUSH_COROUTINE_HPP
