/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_FIBER_HPP
#define GBAXX_EXT_AGBABI_FIBER_HPP
/** @file */

#include <utility>

#include <agbabi.h>

#include <gba/type/stack.hpp>

#include <gba/ext/agbabi/detail/coro_context.hpp>

namespace gba::agbabi {

    /**
     * @class fiber
     * @brief The `fiber` class represents a thread of execution.
     *
     * A fiber is a cooperative multitasking entity that can be used to switch between different contexts of execution
     * within a single thread.
     *
     * The `fiber` class is move-only, meaning it cannot be copied. It can, however, be moved. Moving a fiber transfers
     * its context and execution state to another `fiber` object, allowing for easy transfer of control between
     * different fibers.
     *
     * @note Fibers are created using a stack, which defines the memory used for the fiber's execution. The stack is allocated
     * outside the `fiber` class and passed as an argument in the constructor.
     * @warning The lifetime of the stack must be longer than the lifetime of the fiber.
     */
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
                using proc_type = int(*)(__agbabi_coro_t*);
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
            auto* coro = stack::emplace<coro_type>(sp);
            auto* ctx = stack::emplace<callable_context_type<Fn>>(coro, std::forward<Fn>(fn));
            __agbabi_coro_make(coro, ctx, callable_context_type<Fn>::get_invoke());

            coro->ctx = ctx;
            ctx->coro = coro;
            return ctx;
        }

        context_type* m_ctx;
    public:
        /**
         * @brief Constructs a fiber object that encapsulates a stack and a function.
         * @tparam Fn The type of the function to be executed by the fiber.
         * @param stack A reference to a stack container that supports std::end().
         * @param fn The function to be executed by the fiber.
         * @warning The fiber object does not take ownership of the stack or the function. It is the responsibility
         *          of the caller to ensure that the stack and function are valid and remain valid during the lifetime
         *          of the fiber object.
         */
        template <typename Fn>
        fiber(stack::PointerEnd auto& stack, Fn&& fn) noexcept : m_ctx{make_context(std::end(stack), std::forward<Fn>(fn))} {
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

        /**
         * @brief Calls the fiber.
         *
         * This function swaps the context of execution between the calling context and the fiber context. The execution
         * context includes the program counter, stack pointer, and other register values.
         */
        void operator()() noexcept {
            m_ctx->swap();
        }

        /**
         * @brief Checks if the fiber has returned from execution.
         *
         * @return True if the fiber is returned, false otherwise.
         *
         * @note A fiber returning is not the same as a fiber yielding.
         */
        [[nodiscard]]
        explicit operator bool() const noexcept {
            return m_ctx && !m_ctx->coro->joined;
        }
    };

} // namespace gba::agbabi

#endif // define GBAXX_EXT_AGBABI_FIBER_HPP
