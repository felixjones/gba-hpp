/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_STACK_HPP
#define GBAXX_TYPE_STACK_HPP

#include <concepts>
#include <cstdint>
#include <new>
#include <utility>

namespace gba::stack {

    template <typename T>
    concept PointerEnd = requires(T a) {
        { std::end(a) } -> std::same_as<std::remove_cvref_t<decltype(a[0])>*>;
    };

    template <typename T>
    auto* reserve(void* stack) noexcept {
        static constexpr auto aligned = ((sizeof(T) + 7u) >> 3u) << 3u;
        auto pointer = (reinterpret_cast<std::uintptr_t>(stack) & ~0x7u) - aligned;

        return reinterpret_cast<T*>(pointer);
    }

    template <typename T, typename... Args>
    auto* emplace(void* stack, Args&&... args) noexcept {
        static constexpr auto aligned = ((sizeof(T) + 7u) >> 3u) << 3u;
        auto pointer = (reinterpret_cast<std::uintptr_t>(stack) & ~0x7u) - aligned;

        return new(reinterpret_cast<void*>(pointer)) T(std::forward<Args>(args)...);
    }

    template <typename T, typename... Ts>
    auto* push(void* stack, T&& first, Ts&&... rest) noexcept {
        static constexpr auto aligned = ((sizeof(T) + 7u) >> 3u) << 3u;
        auto pointer = (reinterpret_cast<std::uintptr_t>(stack) & ~0x7u) - aligned;

        *reinterpret_cast<T*>(pointer) = std::forward<T>(first);

        if constexpr (sizeof...(rest) == 0) {
            return reinterpret_cast<T*>(pointer);
        } else {
            return stack_push(reinterpret_cast<void*>(pointer), std::forward<Ts>(rest)...);
        }
    }

} // namespace gba::stack

#endif // define GBAXX_TYPE_STACK_HPP
