/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_COROUTINE_HPP
#define GBAXX_EXT_AGBABI_COROUTINE_HPP

#include <gba/ext/agbabi/fiber.hpp>
#include <gba/ext/agbabi/pull_coroutine.hpp>
#include <gba/ext/agbabi/push_coroutine.hpp>

namespace gba::agbabi {

    /**
     * @brief Represents a coroutine object.
     *
     * This struct provides a convenience interface to work with coroutines using pull and push types. The pull type is
     * used to pull values from the coroutine, while the push type is used to push values into the coroutine.
     *
     * @tparam T The type of values passed between the coroutine and the caller
     */
    template <typename T>
    struct coroutine {
        using pull_type = pull_coroutine<T>;
        using push_type = push_coroutine<T>;
    };

    /**
     * @brief Specialization of coroutine where pull_type and push_type are both fiber types.
     *
     * Unlike pull_coroutine or push_coroutine, fiber does not pass values between the coroutine and the caller.
     *
     * @sa coroutine
     */
    template <>
    struct coroutine<void> {
        using pull_type = fiber;
        using push_type = fiber;
    };

} // namespace gba::agbabi

#endif // define GBAXX_EXT_AGBABI_COROUTINE_HPP
