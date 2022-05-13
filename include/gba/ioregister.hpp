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
    if constexpr (std::is_fundamental_v<Type> || std::is_pointer_v<Type>) {
        return *reinterpret_cast<const volatile Type*>(Address);
    } else if constexpr (std::is_trivially_copyable_v<Type>) {
        return static_cast<Type>(*reinterpret_cast<const volatile util::bit_container<Type>*>(Address));
    } else {
        static_assert(util::always_false<Type>);
    }
}

template <class Type, std::uintptr_t Address>
inline void register_write(const Type& value) noexcept {
    if constexpr (std::is_fundamental_v<Type> || std::is_pointer_v<Type>) {
        *reinterpret_cast<volatile Type*>(Address) = value;
    } else if constexpr (std::is_trivially_copyable_v<Type>) {
        reinterpret_cast<volatile util::bit_container<Type>*>(Address)->copy_from(value);
    } else {
        static_assert(util::always_false<Type>);
    }
}

template <class Type, std::uintptr_t Address, typename... Args>
inline void register_emplace(Args&&... args) noexcept {
    if constexpr (std::is_fundamental_v<Type> || std::is_pointer_v<Type>) {
        *reinterpret_cast<volatile Type*>(Address) = Type{args...};
    } else if constexpr (std::is_constructible_v<Type, Args...>) {
        reinterpret_cast<volatile util::bit_container<Type>*>(Address)->copy_from(util::bit_container<Type>::construct(args...));
    } else if constexpr (std::is_trivially_copyable_v<Type>) {
        reinterpret_cast<volatile util::bit_container<Type>*>(Address)->copy_from(util::bit_container<Type>{Type{args...}});
    } else {
        static_assert(util::always_false<Type>);
    }
}

template <typename Type>
class register_ptr {
public:
    using element_type = std::remove_extent_t<Type>;

    class reference_type {
        friend register_ptr;
    public:
        reference_type(const reference_type&) = delete;
        reference_type& operator=(const reference_type&) = delete;

        auto& operator=(const element_type& value) const&& noexcept {
            if constexpr (std::is_const_v<element_type>) {
                static_assert(util::always_false<element_type>);
            }
            if constexpr (std::is_fundamental_v<Type> || std::is_pointer_v<Type>) {
                *reinterpret_cast<volatile element_type*>(m_address) = value;
            } else if constexpr (std::is_trivially_copyable_v<Type>) {
                reinterpret_cast<volatile util::bit_container<element_type>*>(m_address)->copy_from(value);
            } else {
                static_assert(util::always_false<element_type>);
            }
            return value;
        }

        operator element_type() const&& noexcept {
            if constexpr (std::is_fundamental_v<element_type> || std::is_pointer_v<Type>) {
                return *reinterpret_cast<const volatile element_type*>(m_address);
            } else if constexpr (std::is_trivially_copyable_v<element_type>) {
                return static_cast<element_type>(*reinterpret_cast<const volatile util::bit_container<element_type>*>(m_address));
            } else {
                static_assert(util::always_false<element_type>);
            }
        }
    protected:
        constexpr reference_type() noexcept = default;

        [[nodiscard]]
        static reference_type construct() noexcept {
            return reference_type{};
        }
    private:
        std::uintptr_t m_address{};
    };

    constexpr register_ptr() noexcept = default;
    constexpr register_ptr(const register_ptr&) noexcept = default;
    constexpr register_ptr& operator=(const register_ptr&) noexcept = default;
    constexpr auto operator<=>(const register_ptr& rhs) const noexcept  = default;

    constexpr explicit register_ptr(std::uintptr_t address) noexcept : m_address{address} {}
    constexpr explicit register_ptr(std::nullptr_t) noexcept {}
    explicit register_ptr(element_type* pointer) noexcept : m_address{reinterpret_cast<std::uintptr_t>(pointer)} {}
    explicit register_ptr(volatile element_type* pointer) noexcept : m_address{reinterpret_cast<std::uintptr_t>(pointer)} {}

    [[nodiscard]]
    volatile auto* to_underlying() const noexcept {
        return reinterpret_cast<volatile element_type*>(m_address);
    }

    [[nodiscard]]
    auto&& operator[](std::ptrdiff_t idx) const noexcept {
        return make_ref(m_address + sizeof(element_type) * idx);
    }

    [[nodiscard]]
    auto&& operator*() const noexcept {
        return make_ref(m_address);
    }

    [[nodiscard]]
    volatile auto* operator->() const noexcept {
        return to_underlying();
    }

    [[nodiscard]]
    constexpr explicit operator bool() const noexcept {
        return m_address;
    }
private:
    static auto&& make_ref(std::uintptr_t address, reference_type&& ref = reference_type::construct()) noexcept {
        ref.m_address = address;
        return std::move(ref);
    }

    std::uintptr_t m_address{};
};

} // namespace gba::io

#endif // define GBAXX_IOREGISTER_HPP
