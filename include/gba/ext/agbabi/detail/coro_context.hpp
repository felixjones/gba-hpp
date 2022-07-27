/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_DETAIL_CORO_HPP
#define GBAXX_EXT_AGBABI_DETAIL_CORO_HPP

#include <agbabi.h>

namespace gba::agbabi::detail {

    using coro_proc_type = int(*)(agbabi_coro_t*);

    template <typename T>
    struct context_type;

    template <typename T>
    struct coro_type : agbabi_coro_t {
        context_type<T>* ctx;
        T* value;
    };

    template <>
    struct coro_type<void> : agbabi_coro_t {
        context_type<void>* ctx;
    };

    template <typename T>
    struct context_type {
        context_type() noexcept = default;
        explicit context_type(detail::coro_type<T>* co) noexcept : coro{co} {}

        virtual ~context_type() noexcept = default;
        virtual void swap() noexcept = 0;

        detail::coro_type<T>* coro;
    };

    template <typename T>
    struct yield_context_type : context_type<T> {
        explicit yield_context_type(detail::coro_type<T>* co) noexcept : context_type<T>{co} {}

        ~yield_context_type() noexcept override = default;

        void swap() noexcept override {
            __agbabi_coro_yield(context_type<T>::coro, 0);
        }
    };

} // namespace gba::agbabi::detail

#endif // define GBAXX_EXT_AGBABI_DETAIL_CORO_HPP
