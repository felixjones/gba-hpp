/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_COROUTINE_HPP
#define GBAXX_EXT_AGBABI_COROUTINE_HPP

#include <gba/ext/agbabi/fiber.hpp>
#include <gba/ext/agbabi/pull_coroutine.hpp>
#include <gba/ext/agbabi/push_coroutine.hpp>

namespace gba::agbabi {

template <typename T>
struct coroutine {
    using pull_type = pull_coroutine<T>;
    using push_type = push_coroutine<T>;
};

template <>
struct coroutine<void> {
    using pull_type = fiber;
    using push_type = fiber;
};

} // namespace gba::agbabi

#endif // define GBAXX_EXT_AGBABI_COROUTINE_HPP
