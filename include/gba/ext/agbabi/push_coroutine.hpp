/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_PUSH_COROUTINE_HPP
#define GBAXX_EXT_AGBABI_PUSH_COROUTINE_HPP

#include <iterator>
#include <utility>

#include <agbabi.h>

#include <gba/type/stack.hpp>

#include <gba/ext/agbabi/pull_coroutine.hpp>

#include <gba/ext/agbabi/detail/coro_context.hpp>

namespace gba::agbabi {

    template <typename T>
    class pull_coroutine;

    /**
     * @brief A class that represents a push-style coroutine.
     *
     * This class allows the user to define a coroutine that can be iterated over to push values to a consumer.
     * The coroutine is implemented using a stack-based context and can be used by calling operator() to push the next
     * value to the coroutine.
     */
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
            auto* coro = stack::emplace<coro_type>(sp);
            auto* ctx = stack::emplace<callable_context_type<Fn>>(coro, std::forward<Fn>(fn));
            auto* value = stack::reserve<T>(ctx);
            __agbabi_coro_make(coro, value, callable_context_type<Fn>::get_invoke());

            coro->ctx = ctx;
            coro->value = value;
            ctx->coro = coro;
            return ctx;
        }

        context_type* m_ctx;
    public:
        /**
         * @brief Constructs a push_coroutine object that encapsulates a stack and a function.
         * @tparam Fn The type of the function to be executed by the push_coroutine.
         * @param stack A reference to a stack container that supports std::end().
         * @param fn The function to be executed by the push_coroutine.
         * @warning The push_coroutine object does not take ownership of the stack or the function. It is the
         *          responsibility of the caller to ensure that the stack and function are valid and remain valid during
         *          the lifetime of the push_coroutine object.
         */
        template <typename Fn>
        push_coroutine(stack::PointerEnd auto& stack, Fn&& fn) noexcept : m_ctx{make_context(std::end(stack), std::forward<Fn>(fn))} {
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

        /**
         * @brief Calls the push_coroutine.
         *
         * This function swaps the context of execution between the calling context and the push_coroutine context. The
         * execution context includes the program counter, stack pointer, and other register values.
         *
         * @tparam T The type of values produced by the coroutine.
         * @tparam Args The types of the arguments to be pushed to the coroutine.
         * @param args The arguments to be pushed to the coroutine.
         */
        template <typename... Args>
        void operator()(Args&&... args) noexcept {
            *m_ctx->coro->value = T(std::forward<Args>(args)...);
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

        /**
         * @brief An iterator class for a push-based coroutine.
         *
         * This iterator class is used to iterate over the values to send to a push-based coroutine.
         * It provides the necessary interfaces and operators required for iteration.
         *
         * @tparam T The type of values received by the coroutine.
         */
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
