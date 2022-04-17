/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_IOREGISTER_HPP
#define GBAXX_IOREGISTER_HPP

#include <cstdint>
#include <type_traits>

#include <gba/util/bit_container.hpp>

namespace gba::io {

template <class Type, std::uintptr_t Address>
inline auto register_read() noexcept {
    if constexpr (std::is_fundamental_v<Type>) {
        return *reinterpret_cast<volatile Type*>(Address);
    } else if constexpr (std::is_trivially_copyable_v<Type>) {
        return static_cast<Type>(*reinterpret_cast<const volatile util::bit_container<Type>*>(Address));
    } else {
        static_assert(util::always_false<Type>);
    }
}

template <class Type, std::uintptr_t Address>
inline void register_write(const Type& value) noexcept {
    if constexpr (std::is_fundamental_v<Type>) {
        *reinterpret_cast<volatile Type*>(Address) = value;
    } else if constexpr (std::is_trivially_copyable_v<Type>) {
        *reinterpret_cast<volatile util::bit_container<Type>*>(Address) = util::bit_container<Type>{value};
    } else {
        static_assert(util::always_false<Type>);
    }
}

template <class Type, std::uintptr_t Address, typename... Args>
inline void register_emplace(Args&&... args) noexcept {
    if constexpr (std::is_fundamental_v<Type>) {
        *reinterpret_cast<volatile Type*>(Address) = Type{args...};
    } else if constexpr (std::is_constructible_v<Type, Args...>) {
        *reinterpret_cast<volatile util::bit_container<Type>*>(Address) = util::bit_container<Type>::construct(args...);
    } else if constexpr (std::is_trivially_copyable_v<Type>) {
        *reinterpret_cast<volatile util::bit_container<Type>*>(Address) = util::bit_container<Type>{Type{args...}};
    } else {
        static_assert(util::always_false<Type>);
    }
}

} // namespace gba::io

#endif // define GBAXX_IOREGISTER_HPP
