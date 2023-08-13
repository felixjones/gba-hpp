/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_STACK_HPP
#define GBAXX_TYPE_STACK_HPP
/** @file */

#include <concepts>
#include <cstdint>
#include <new>
#include <utility>

namespace gba::stack {

    /**
     * @brief Concept for checking if a type returns a pointer with the std::end() function.
     *
     * @tparam T The type to be checked.
     */
    template <typename T>
    concept PointerEnd = requires(T a) {
        { std::end(a) } -> std::same_as<std::remove_cvref_t<decltype(a[0])>*>;
    };

    /**
     * @brief Reserve stack space for a type on the given stack.
     *
     * @tparam T Type to reserve allocation space for on the stack.
     * @param stack A pointer to the top of the stack to be reserved.
     * @return Pointer to the space reserved for type T on the stack.
     */
    template <typename T>
    auto* reserve(void* stack) noexcept {
        static constexpr auto aligned = ((sizeof(T) + 7u) >> 3u) << 3u;
        auto pointer = (reinterpret_cast<std::uintptr_t>(stack) & ~0x7u) - aligned;

        return reinterpret_cast<T*>(pointer);
    }

    /**
     * @brief Reserve stack space for, and construct, a type on the given stack.
     *
     * @tparam T Type to reserve allocation space for & construct on the stack.
     * @tparam Args Types for the arguments.
     * @param stack A pointer to the top of the stack to emplace the type onto.
     * @param args List of arguments to be forwarded to the type constructor.
     * @return Pointer to the constructed type on the stack.
     */
    template <typename T, typename... Args>
    auto* emplace(void* stack, Args&&... args) noexcept {
        static constexpr auto aligned = ((sizeof(T) + 7u) >> 3u) << 3u;
        auto pointer = (reinterpret_cast<std::uintptr_t>(stack) & ~0x7u) - aligned;

        return new(reinterpret_cast<void*>(pointer)) T(std::forward<Args>(args)...);
    }

    /**
     * @brief Pushes elements onto the stack.
     *
     * This function is used to add elements onto the stack. It takes any number of elements as parameters and adds them
     * to the stack in the order they are passed.
     *
     * @param stack A pointer to the stack object.
     * @param first The first element to be added to the stack.
     * @param rest Any additional elements to be added to the stack.
     */
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
