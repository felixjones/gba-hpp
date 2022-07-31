/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_DETAIL_PUSH_COROUTINE_CONTEXT_HPP
#define GBAXX_EXT_AGBABI_DETAIL_PUSH_COROUTINE_CONTEXT_HPP

#include <agbabi.h>

#include <gba/ext/agbabi/pull_coroutine.hpp>
#include <gba/ext/agbabi/push_coroutine.hpp>

#include <gba/ext/agbabi/detail/coro_context.hpp>

namespace gba::agbabi {

template <typename T>
template <typename Fn>
detail::coro_proc_type push_coroutine<T>::callable_context_type<Fn>::get_invoke() noexcept {
    return reinterpret_cast<detail::coro_proc_type>(+[](coro_type* coro) {
        auto* ctx = reinterpret_cast<callable_context_type*>(coro->ctx);
        auto y = detail::yield_context_type{coro};
        auto f = pull_coroutine<T>(&y);
        ctx->m_fn(f);
        return 0;
    });
}

} // namespace gba::agbabi

#endif // define GBAXX_EXT_AGBABI_DETAIL_PUSH_COROUTINE_CONTEXT_HPP
