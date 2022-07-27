/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_FIBER_HPP
#define GBAXX_EXT_AGBABI_FIBER_HPP

#include <utility>

#include <agbabi.h>

#include <gba/ext/agbabi/detail/coro_context.hpp>
#include <gba/ext/agbabi/detail/stack.hpp>

namespace gba::agbabi {

class fiber {
private:
    using context_type = detail::context_type<void>;
    using coro_type = detail::coro_type<void>;

    explicit fiber(context_type* ctx) noexcept : m_ctx{ctx} {}

    template <typename Fn>
    struct callable_context_type : context_type {
        explicit callable_context_type(Fn&& fn) noexcept : m_fn{fn} {}

        ~callable_context_type() noexcept override = default;

        void swap() noexcept override {
            __agbabi_coro_resume(coro);
        }

        static auto get_invoke() noexcept {
            using proc_type = int(*)(agbabi_coro_t*);
            return reinterpret_cast<proc_type>(+[](coro_type* coro) {
                auto* ctx = reinterpret_cast<callable_context_type*>(coro->ctx);
                auto y = detail::yield_context_type{coro};
                auto f = fiber(&y);
                ctx->m_fn(f);
                return 0;
            });
        }
    private:
        Fn m_fn;
    };

    template <typename Fn>
    auto* make_context(void* sp, Fn&& fn) noexcept {
        auto* coro = detail::stack_emplace<coro_type>(sp);
        auto* ctx = detail::stack_emplace<callable_context_type<Fn>>(coro, std::forward<Fn>(fn));
        __agbabi_coro_make(coro, ctx, callable_context_type<Fn>::get_invoke());

        coro->ctx = ctx;
        ctx->coro = coro;
        return ctx;
    }

    context_type* m_ctx;
public:
    template <typename Fn>
    fiber(detail::PointerEnd auto& stack, Fn&& fn) noexcept : m_ctx{make_context(std::end(stack), std::forward<Fn>(fn))} {
    }

    fiber(const fiber&) = delete;
    fiber& operator=(const fiber&) = delete;

    fiber(fiber&& other) noexcept : m_ctx{} {
        std::swap(m_ctx, other.m_ctx);
    }

    auto& operator=(fiber&& other) noexcept {
        std::swap(m_ctx, other.m_ctx);
        return *this;
    }

    void operator()() noexcept {
        m_ctx->swap();
    }

    [[nodiscard]]
    explicit operator bool() const noexcept {
        return m_ctx && !m_ctx->coro->joined;
    }
};

} // namespace gba::agbabi

#endif // define GBAXX_EXT_AGBABI_FIBER_HPP
