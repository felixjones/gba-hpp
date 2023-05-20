/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_MEMORY_HPP
#define GBAXX_TYPE_MEMORY_HPP

#include <cstdint>
#include <cstring>
#include <memory>
#include <utility>

namespace gba {

    template <typename T>
    constexpr auto volatile_load(const T* ptr) noexcept -> std::remove_cvref_t<T> {
        using value_type = std::remove_cvref_t<T>;
        static_assert(std::is_trivially_copyable_v<value_type>, "Volatile load can only be used with trivially copyable types");

        return __builtin_bit_cast(value_type, *const_cast<std::add_cv_t<T>*>(ptr));
    }

    template <typename T, typename U = std::remove_cvref_t<T>>
    constexpr void volatile_store(T* ptr, U&& value) noexcept {
        using value_type = std::remove_cvref_t<T>;
        static_assert(std::is_trivially_copyable_v<value_type>, "Volatile store can only be used with trivially copyable types");

        if constexpr (sizeof(value_type) == sizeof(char)) {
            asm volatile (
                "strb %[val], [%[ptr]]"
                :: [val]"l"(value), [ptr]"l"(ptr)
            );
        } else if constexpr (sizeof(value_type) == sizeof(short)) {
            asm volatile (
                "strh %[val], [%[ptr]]"
                :: [val]"l"(value), [ptr]"l"(ptr)
            );
        } else if constexpr (sizeof(value_type) == sizeof(int)) {
            asm volatile (
                "str %[val], [%[ptr]]"
                :: [val]"l"(value), [ptr]"l"(ptr)
            );
        } else if constexpr (sizeof(value_type) == 6) {
            asm volatile (
                "ldr r3, [%[src]]\n"
                "str r3, [%[dst]]\n"
                "add %[src], #4\n"
                "add %[dst], #4\n"
                "ldrh r3, [%[src]]\n"
                "strh r3, [%[dst]]"
                :: [src]"l"(&value), [dst]"l"(ptr) : "r3"
            );
        } else if constexpr (sizeof(value_type) == 8) {
            asm volatile (
                "ldr r3, [%[src]]\n"
                "str r3, [%[dst]]\n"
                "add %[src], #4\n"
                "add %[dst], #4\n"
                "ldr r3, [%[src]]\n"
                "str r3, [%[dst]]"
                :: [src]"l"(&value), [dst]"l"(ptr) : "r3"
            );
        } else {
            asm volatile ("" ::: "memory"); // Prevent optimizing out
            __builtin_memcpy(const_cast<value_type*>(ptr), &value, sizeof(value_type));
        }
    }

    template <typename T>
    constexpr void volatile_swap(T* a, T* b) noexcept {
        using value_type = std::remove_cvref_t<T>;
        static_assert(std::is_trivially_copyable_v<value_type>, "Volatile swap can only be used with trivially copyable types");

        if constexpr (sizeof(value_type) == sizeof(char)) {
            asm volatile (
                "ldrb r3, [%[a]]\n"
                "ldrb r2, [%[b]]\n"
                "strb r2, [%[a]]\n"
                "strb r3, [%[b]]"
                :: [a]"l"(a), [b]"l"(b) : "r2", "r3"
            );
        } else if constexpr (sizeof(value_type) == sizeof(short)) {
            asm volatile (
                "ldrh r3, [%[a]]\n"
                "ldrh r2, [%[b]]\n"
                "strh r2, [%[a]]\n"
                "strh r3, [%[b]]"
                :: [a]"l"(a), [b]"l"(b) : "r2", "r3"
            );
        } else if constexpr (sizeof(value_type) == sizeof(int)) {
            asm volatile (
                "ldr r3, [%[a]]\n"
                "ldr r2, [%[b]]\n"
                "str r2, [%[a]]\n"
                "str r3, [%[b]]"
                :: [a]"l"(a), [b]"l"(b) : "r2", "r3"
            );
        } else {
            const auto av = __builtin_bit_cast(value_type, *const_cast<std::add_cv_t<T>*>(a));
            const auto bv = __builtin_bit_cast(value_type, *const_cast<std::add_cv_t<T>*>(b));

            asm volatile ("" ::: "memory"); // Prevent optimizing out

            __builtin_memcpy(const_cast<value_type*>(a), &bv, sizeof(value_type));
            __builtin_memcpy(const_cast<value_type*>(b), &av, sizeof(value_type));
        }
    }

    template <typename T, typename... Args>
    constexpr T volatile_emplace(T* ptr, Args&&... args) noexcept {
        using value_type = std::remove_cvref_t<T>;
        static_assert(std::is_trivially_copyable_v<value_type>, "Volatile emplace can only be used with trivially copyable types");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion" // Bizarre bug with std::forward...not forwarding?
        const auto value = value_type{std::forward<Args>(args)...};
#pragma GCC diagnostic pop

        if constexpr (sizeof(value_type) == sizeof(char)) {
            asm volatile (
                "strb %[val], [%[ptr]]"
                :: [val]"l"(value), [ptr]"l"(ptr)
            );
        } else if constexpr (sizeof(value_type) == sizeof(short)) {
            asm volatile (
                "strh %[val], [%[ptr]]"
                :: [val]"l"(value), [ptr]"l"(ptr)
            );
        } else if constexpr (sizeof(value_type) == sizeof(int)) {
            asm volatile (
                "str %[val], [%[ptr]]"
                :: [val]"l"(value), [ptr]"l"(ptr)
            );
        } else if constexpr (sizeof(value_type) == 6) {
            asm volatile (
                "ldr r3, [%[src]]\n"
                "str r3, [%[dst]]\n"
                "add %[src], #4\n"
                "add %[dst], #4\n"
                "ldrh r3, [%[src]]\n"
                "strh r3, [%[dst]]"
                :: [src]"l"(&value), [dst]"l"(ptr) : "r3"
            );
        } else if constexpr (sizeof(value_type) == 8) {
            asm volatile (
                "ldr r3, [%[src]]\n"
                "str r3, [%[dst]]\n"
                "add %[src], #4\n"
                "add %[dst], #4\n"
                "ldr r3, [%[src]]\n"
                "str r3, [%[dst]]"
                :: [src]"l"(&value), [dst]"l"(ptr) : "r3"
            );
        } else {
            asm volatile ("" ::: "memory"); // Prevent optimizing out
            __builtin_memcpy(const_cast<value_type*>(ptr), &value, sizeof(value_type));
        }

        return value;
    }

    template <typename T>
    struct const_ptr {
        using pointer = T*;
        using element_type = T;

        constexpr explicit const_ptr(std::uintptr_t p) noexcept : m_ptr{p} {}

        constexpr pointer get() const noexcept {
            return reinterpret_cast<pointer>(m_ptr);
        }

        constexpr std::add_lvalue_reference_t<T> operator*() const noexcept {
            return *get();
        }

        constexpr pointer operator->() const noexcept {
            return get();
        }

        constexpr std::remove_extent_t<T>& operator[](std::integral auto i) const noexcept requires std::is_array_v<T> {
            return this->operator*()[i];
        }

        constexpr auto* operator&() const noexcept {
            return reinterpret_cast<std::remove_volatile_t<std::remove_all_extents_t<T>>*>(m_ptr);
        }

        constexpr auto size() const noexcept requires std::is_array_v<T> {
            return sizeof(T);
        }

        std::uintptr_t m_ptr;
    };

    template <typename T>
    constexpr auto operator+(const_ptr<T> reg, std::integral auto offset) noexcept -> const_ptr<T> {
        return const_ptr<T>(reg.m_ptr + offset);
    }

    template <typename T>
    concept ConstPtr = std::is_same_v<std::remove_cvref_t<T>, const_ptr<typename T::element_type>>;

    template <auto Ptr> requires ConstPtr<decltype(Ptr)>
    struct registral {
        using value_type = std::remove_reference_t<typename decltype(Ptr)::element_type>;

        constexpr auto& operator=(const std::remove_cvref_t<value_type>& value) const noexcept requires(!std::is_const_v<value_type>) {
            volatile_store(Ptr.get(), std::remove_cvref_t<value_type>(value));
            return *this;
        }

        constexpr value_type* operator&() const noexcept {
            return Ptr.get();
        }

        constexpr value_type* operator->() const noexcept {
            return Ptr.get();
        }

        constexpr std::remove_cvref_t<value_type> operator*() const noexcept {
            return value();
        }

        constexpr std::remove_cvref_t<value_type> value() const noexcept {
            return volatile_load(Ptr.get());
        }

        constexpr auto& operator[](std::integral auto i) const noexcept requires(!std::is_const_v<value_type>) {
            using element_type = std::remove_reference_t<decltype(value_type()[0])>;
            return reinterpret_cast<volatile element_type*>(Ptr.get())[i];
        }

        constexpr auto operator[](std::integral auto i) const noexcept requires(std::is_const_v<value_type>) {
            using element_type = std::remove_reference_t<decltype(value_type()[0])>;
            return volatile_load(reinterpret_cast<const volatile element_type*>(Ptr.get()) + i);
        }

        template <auto T>
        constexpr void swap(const registral<T>&) const noexcept requires(!std::is_const_v<value_type>) {
            volatile_swap(Ptr.get(), T.get());
        }

        template <typename T = value_type>
        constexpr void swap(T&& value) const noexcept requires(!std::is_const_v<value_type>) {
            auto prev = volatile_load(Ptr.get());
            volatile_store(Ptr.get(), static_cast<value_type&&>(value));
            value = static_cast<value_type&&>(prev);
        }

        constexpr void reset() const noexcept requires(!std::is_const_v<value_type>) {
            volatile_emplace(Ptr.get());
        }

        template <typename... Args>
        constexpr std::remove_cvref_t<value_type> emplace(Args&&... args) const noexcept requires(!std::is_const_v<value_type>) {
            return volatile_emplace(Ptr.get(), std::forward<Args>(args)...);
        }

        struct scoped_ref : std::remove_cvref_t<value_type> {
            scoped_ref() noexcept = default;

            constexpr ~scoped_ref() noexcept {
                if (!m_owner) {
                    return;
                }
                *m_owner = *(std::remove_cvref_t<value_type>*) this;
            }

            constexpr scoped_ref& operator=(scoped_ref&& other) noexcept {
                m_owner = other.m_owner;
                *(std::remove_cvref_t<value_type>*) this = other;
                other.m_owner = nullptr;
                return *this;
            }

            constexpr explicit operator bool() const noexcept {
                return !std::is_const_v<value_type> && m_owner;
            }

        private:
            friend registral;
            constexpr explicit scoped_ref(const registral* owner) noexcept : m_owner{owner}, std::remove_cvref_t<value_type>{owner->value()} {}

            const registral* m_owner;
        };

        constexpr auto acquire() const noexcept {
            return scoped_ref{this};
        }
    };

    template <auto Ptr, typename T>
    constexpr bool operator==(const registral<Ptr>& reg, const T& value) noexcept {
        return reg.value() == value;
    }

    template <typename T, auto Ptr>
    constexpr bool operator==(const T& value, const registral<Ptr>& reg) noexcept {
        return value == reg.value();
    }

    template <auto Ptr, typename T>
    constexpr bool operator!=(const registral<Ptr>& reg, const T& value) noexcept {
        return reg.value() != value;
    }

    template <typename T, auto Ptr>
    constexpr bool operator!=(const T& value, const registral<Ptr>& reg) noexcept {
        return value != reg.value();
    }

    template <auto Ptr, typename T>
    constexpr bool operator<(const registral<Ptr>& reg, const T& value) noexcept {
        return reg.value() < value;
    }

    template <typename T, auto Ptr>
    constexpr bool operator<(const T& value, const registral<Ptr>& reg) noexcept {
        return value < reg.value();
    }

    template <auto Ptr, typename T>
    constexpr bool operator<=(const registral<Ptr>& reg, const T& value) noexcept {
        return reg.value() <= value;
    }

    template <typename T, auto Ptr>
    constexpr bool operator<=(const T& value, const registral<Ptr>& reg) noexcept {
        return value <= reg.value();
    }

    template <auto Ptr, typename T>
    constexpr bool operator>(const registral<Ptr>& reg, const T& value) noexcept {
        return reg.value() > value;
    }

    template <typename T, auto Ptr>
    constexpr bool operator>(const T& value, const registral<Ptr>& reg) noexcept {
        return value > reg.value();
    }

    template <auto Ptr, typename T>
    constexpr bool operator>=(const registral<Ptr>& reg, const T& value) noexcept {
        return reg.value() >= value;
    }

    template <typename T, auto Ptr>
    constexpr bool operator>=(const T& value, const registral<Ptr>& reg) noexcept {
        return value >= reg.value();
    }

    template <auto Ptr, std::three_way_comparable_with<std::remove_cvref_t<typename decltype(Ptr)::element_type>> T>
    constexpr auto operator<=>(const registral<Ptr>& reg, const T& value) noexcept {
        return reg.value() <=> value;
    }

    template <auto Ptr, std::ptrdiff_t Stride> requires ConstPtr<decltype(Ptr)> && std::is_array_v<typename decltype(Ptr)::element_type>
    struct registral_series {
        using element_type = std::remove_extent_t<std::remove_reference_t<typename decltype(Ptr)::element_type>>;

        constexpr auto& operator=(const std::remove_cvref_t<typename decltype(Ptr)::element_type>& value) const noexcept requires(!std::is_const_v<typename decltype(Ptr)::element_type>) {
            volatile_store(Ptr.get(), std::remove_cvref_t<typename decltype(Ptr)::element_type>(value));
            return *this;
        }

        constexpr auto operator&() const noexcept -> typename decltype(Ptr)::element_type* {
            return Ptr.get();
        }

        constexpr auto& operator[](std::integral auto i) const noexcept requires(!std::is_const_v<element_type>) {
            return *reinterpret_cast<std::remove_volatile_t<element_type>*>(Ptr.m_ptr + i * Stride);
        }

        constexpr auto operator[](std::integral auto i) const noexcept requires(std::is_const_v<element_type>) {
            return volatile_load(reinterpret_cast<element_type*>(Ptr.m_ptr + i * Stride));
        }

        constexpr auto get(std::size_t i) const noexcept {
            return volatile_load(reinterpret_cast<std::remove_volatile_t<element_type>*>(Ptr.m_ptr + i * Stride));
        }

        template <typename T = std::remove_volatile_t<element_type>>
        constexpr void set(std::size_t i, T&& value) const noexcept {
            volatile_store(reinterpret_cast<element_type*>(Ptr.m_ptr + i * Stride), static_cast<element_type&&>(value));
        }

        constexpr void reset(std::size_t i) const noexcept requires(!std::is_const_v<element_type>) {
            volatile_emplace(reinterpret_cast<element_type*>(Ptr.m_ptr + i * Stride));
        }

        template <typename... Args>
        constexpr auto emplace(std::size_t i, Args&&... args) const noexcept -> std::remove_cvref_t<element_type> requires(!std::is_const_v<element_type>) {
            return volatile_emplace(reinterpret_cast<element_type*>(Ptr.m_ptr + i * Stride), std::forward<Args>(args)...);
        }

        struct scoped_ref : std::remove_cvref_t<element_type> {
            scoped_ref() noexcept = default;

            constexpr ~scoped_ref() noexcept {
                if (!m_address) {
                    return;
                }
                volatile_store(
                    reinterpret_cast<element_type*>(m_address),
                    *(std::remove_cvref_t<element_type>*) this
                );
            }

            constexpr scoped_ref& operator=(scoped_ref&& other) noexcept {
                m_address = other.m_address;
                *(std::remove_cvref_t<element_type>*) this = other;
                other.m_address = 0;
                return *this;
            }

            constexpr explicit operator bool() const noexcept {
                return !std::is_const_v<element_type> && m_address;
            }

        private:
            friend registral_series;

            constexpr explicit scoped_ref(const std::uintptr_t address) noexcept : m_address{address},
                std::remove_cvref_t<element_type>{volatile_load(reinterpret_cast<element_type*>(address))} {}

            std::uintptr_t m_address;
        };

        constexpr auto acquire(std::size_t i) const noexcept {
            return scoped_ref{Ptr.m_ptr + i * Stride};
        }
    };

} // namespace gba

namespace std {

    template <auto Lhs, auto Rhs> requires
        std::is_same_v<std::remove_cvref_t<decltype(Lhs)>, gba::const_ptr < typename decltype(Lhs)::element_type>> &&
        std::is_same_v<std::remove_cvref_t<decltype(Rhs)>, gba::const_ptr < typename decltype(Rhs)::element_type>>
    constexpr void swap(const gba::registral<Lhs>& lhs, const gba::registral<Rhs>& rhs ) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }

} // namespace std

#endif // define GBAXX_TYPE_MEMORY_HPP
