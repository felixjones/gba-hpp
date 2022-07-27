/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_DETAIL_STACK_HPP
#define GBAXX_EXT_AGBABI_DETAIL_STACK_HPP

#include <concepts>
#include <cstdint>
#include <utility>

#include <agbabi.h>

#include <gba/util/array_traits.hpp>

namespace gba::agbabi::detail {

    template <class T>
    concept PointerEnd = requires(T& a) {
        { std::end(a) } -> std::same_as<util::array_value_type<T>*>;
    };

    template <typename T>
    auto* stack_reserve(void* stack) noexcept {
        static constexpr auto aligned = ((sizeof(T) + 7u) >> 3u) << 3u;
        auto pointer = (reinterpret_cast<std::uintptr_t>(stack) & ~0x7u) - aligned;

        return reinterpret_cast<T*>(pointer);
    }

    template <typename T, typename... Args>
    auto* stack_emplace(void* stack, Args&&... args) noexcept {
        static constexpr auto aligned = ((sizeof(T) + 7u) >> 3u) << 3u;
        auto pointer = (reinterpret_cast<std::uintptr_t>(stack) & ~0x7u) - aligned;

        return new(reinterpret_cast<void*>(pointer)) T(std::forward<Args>(args)...);
    }

    template <typename T, typename... Ts>
    auto* stack_push(void* stack, T&& first, Ts&&... rest) noexcept {
        static constexpr auto aligned = ((sizeof(T) + 7u) >> 3u) << 3u;
        auto pointer = (reinterpret_cast<std::uintptr_t>(stack) & ~0x7u) - aligned;

        *reinterpret_cast<T*>(pointer) = std::forward<T>(first);

        if constexpr (sizeof...(rest) == 0) {
            return reinterpret_cast<T*>(pointer);
        } else {
            return stack_push(reinterpret_cast<void*>(pointer), std::forward<Ts>(rest)...);
        }
    }

    static inline auto** self_on_coro_stack(void* stack) noexcept {
        return reinterpret_cast<agbabi_coro_t**>(reinterpret_cast<std::uintptr_t>(stack) & ~0x7) - 2;
    }

} // namespace gba::agbabi::detail

#endif // define GBAXX_EXT_AGBABI_DETAIL_STACK_HPP
