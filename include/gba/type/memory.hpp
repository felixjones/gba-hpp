/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_MEMORY_HPP
#define GBAXX_TYPE_MEMORY_HPP
/** @file */

#include <cstdint>
#include <cstring>
#include <memory>
#include <utility>

namespace gba {

    /**
     * @brief Returns the value pointed to by *ptr in a register-safe manner, ensuring a volatile load.
     * @note This function does not modify the pointer itself.
     *
     * This function loads from the pointer with const volatile qualifications, meaning that the compiler is prevented
     * from optimizing away or reordering the access to memory.
     *
     * @tparam T The type of the value pointed to by *ptr.
     * @param ptr A pointer to the value to be loaded.
     * @return The value pointed to by *ptr.
     *
     * @sa volatile_store()
     */
    template <typename T>
    constexpr auto volatile_load(const T* ptr) noexcept -> std::remove_cvref_t<T> {
        using value_type = std::remove_cvref_t<T>;
        static_assert(std::is_trivially_copyable_v<value_type>, "Volatile load can only be used with trivially copyable types");

        return __builtin_bit_cast(value_type, *const_cast<std::add_cv_t<T>*>(ptr));
    }

    /**
     * @brief Stores a value in a register-safe manner, ensuring a volatile store.
     *
     * This function stores the value into the pointer with volatile qualifications, meaning that the compiler is
     * prevented from optimizing away or reordering the access to memory.
     *
     * @tparam T The type of the memory location.
     * @tparam U The type of the value to be stored.
     * @param ptr A pointer to the memory location to store the value in.
     * @param value The value to be stored in the memory location.
     *
     * @see volatile_load()
     */
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

    /**
     * @brief Swaps the values of two variables using volatile pointers.
     *
     * This function swaps the values of two variables by utilizing pointers with volatile qualifications, meaning that
     * the compiler is prevented from optimizing away or reordering the access to memory.
     *
     * @tparam T The type of the variables to be swapped.
     * @param a Pointer to the first variable.
     * @param b Pointer to the second variable.
     *
     * @note Variable `a` will always be loaded before `b`, and `a` will always be stored before `b` (but after `b` has
     *        been loaded).
     */
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

    /**
     * @brief Constructs a value and copies it into the pointer.
     *
     * @tparam T The type of value to be constructed.
     * @tparam Args The types of the arguments to be forwarded.
     * @param ptr Pointer to where the value will be constructed.
     * @param args The arguments to be forwarded to construct the value.
     * @return A copy of the newly constructed value.
     *
     * @note This does not perform placement new on the ptr due to volatility concerns. The intention of this function
     *       is to construct values as close to the volatile store as possible.
     */
    template <typename T, typename... Args>
    constexpr std::remove_cvref_t<T> volatile_emplace(T* ptr, Args&&... args) noexcept {
        using value_type = std::remove_cvref_t<T>;
        static_assert(std::is_trivially_copyable_v<value_type>, "Volatile emplace can only be used with trivially copyable types");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion" // Bizarre bug with std::forward...not forwarding?
        const auto value = value_type(std::forward<Args>(args)...);
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

    /**
     * @struct const_ptr
     * @brief A lightweight wrapper class for a constant pointer.
     *
     * The const_ptr class provides a simple and efficient way to handle constant pointers
     * in C++ code. It allows for transparent access to the underlying pointer and provides
     * convenient member functions for dereferencing, accessing array elements, and obtaining
     * the pointer's address.
     *
     * @tparam T The type of the pointer.
     */
    template <typename T>
    struct const_ptr {
        using pointer = T*;
        using element_type = T;

        /**
         * @brief Convert address to const_ptr.
         *
         * @param p Address of the pointer.
         */
        constexpr explicit const_ptr(std::uintptr_t p) noexcept : m_ptr{p} {}

        /**
         * @brief Get a pointer from the underlying address.
         *
         * @return A usable pointer.
         */
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

        /**
         * @brief sizeof() helper.
         *
         * @return sizeof(T).
         */
        constexpr auto size() const noexcept requires std::is_array_v<T> {
            return sizeof(T);
        }

        std::uintptr_t m_ptr;
    };

    template <typename T>
    constexpr auto operator+(const_ptr<T> reg, std::integral auto offset) noexcept -> const_ptr<T> {
        return const_ptr<T>(reg.m_ptr + std::uintptr_t(offset));
    }

    template <typename T>
    concept ConstPtr = std::is_same_v<std::remove_cvref_t<T>, const_ptr<typename T::element_type>>;

    /**
     * @struct registral
     * @brief Memory mapped register accessor type.
     *
     * @tparam Ptr Constant pointer that this registral will operate on.
     *
     * @note The name "registral" is a portmanteau of "register" and "optional", named as such because of the API
     *       similarities with std::optional.
     *
     * @sa const_ptr
     */
    template <auto Ptr> requires ConstPtr<decltype(Ptr)>
    struct registral {
        using value_type = std::remove_reference_t<typename decltype(Ptr)::element_type>;

        constexpr auto& operator=(const std::remove_cvref_t<value_type>& value) const noexcept requires(!std::is_const_v<value_type>) {
            volatile_store(Ptr.get(), std::remove_cvref_t<value_type>(value));
            return *this;
        }

        /**
         * @brief The address operator has been overloaded to provide convenience for functions such as std::memcpy().
         *
         * @return Pointer to value_type.
         */
        constexpr value_type* operator&() const noexcept {
            return Ptr.get();
        }

        constexpr value_type* operator->() const noexcept {
            return Ptr.get();
        }

        constexpr std::remove_cvref_t<value_type> operator*() const noexcept {
            return value();
        }

        /**
         * @brief Value read access.
         *
         * @return The current value at this memory mapped register.
         *
         * @sa volatile_load()
         */
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

        /**
         * @brief Swaps the values of two similar registrals.
         *
         * @tparam T const_ptr value of the other registral to swap with.
         *
         * @note The value_type of the registrals must match.
         *
         * @sa volatile_swap()
         */
        template <auto T>
        constexpr void swap(const registral<T>&) const noexcept requires(!std::is_const_v<value_type>) {
            volatile_swap(Ptr.get(), T.get());
        }

        /**
         * @brief Swaps this registral with a given value.
         *
         * This first loads the previous value, stores the new value, and then sets the new value to the previous value.
         *
         * @tparam T The type of value.
         *
         * @note This does not make use of volatile_swap().
         *
         * @sa volatile_load()
         * @sa volatile_store()
         */
        template <typename T = value_type>
        constexpr void swap(T&& value) const noexcept requires(!std::is_const_v<value_type>) {
            auto prev = volatile_load(Ptr.get());
            volatile_store(Ptr.get(), static_cast<value_type&&>(value));
            value = static_cast<value_type&&>(prev);
        }

        /**
         * @brief Clears the registral's value.
         *
         * Calls volatile_emplace() with no arguments, essentially calling the default constructor of value_type.
         *
         * @sa volatile_emplace()
         */
        constexpr void reset() const noexcept requires(!std::is_const_v<value_type>) {
            volatile_emplace(Ptr.get());
        }

        /**
         * @brief Constructs the registral's value.
         *
         * @tparam Args Type of args.
         * @param args Arguments to be forwarded to the constructor.
         * @return A copy of the newly constructed value.
         *
         * @sa volatile_emplace()
         */
        template <typename... Args>
        constexpr std::remove_cvref_t<value_type> emplace(Args&&... args) const noexcept requires(!std::is_const_v<value_type>) {
            return volatile_emplace(Ptr.get(), std::forward<Args>(args)...);
        }

        /**
         * @struct scoped_ref
         * @brief Uses RAII to temporarily read the registral for modification before storing the updated value.
         *
         * @sa acquire()
         */
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
            constexpr explicit scoped_ref(const registral* owner) noexcept : std::remove_cvref_t<value_type>{owner->value()}, m_owner{owner} {}

            const registral* m_owner;
        };

        /**
         * @brief Acquire a temporary scoped reference for reading & modifying the registral's value.
         *
         * @return Scoped reference to the data stored at the registral.
         *
         * @section Reading and writing a registral with acquire():
         * @code{cpp}
         * #include <gba/gba.hpp>
         *
         * int main() {
         *     using namespace gba;
         *
         *     struct my_type { int a; int b; };
         *
         *     static constexpr auto MY_REGISTRAL = registral<const_ptr<my_type>(0x4000000)>{};
         *
         *     if (auto ref = MY_REGISTRAL.acquire()) {
         *         std::swap(ref.a, ref.b);
         *         // ref is written back to MY_REGISTRAL on scope exit
         *     }
         * }
         * @endcode
         *
         * @sa scoped_ref
         */
        constexpr auto acquire() const noexcept requires (!Fundamental<value_type>) {
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

    /**
     * @struct registral_series
     * @brief Similar to registral, but for a series of memory mapped elements.
     *
     * @tparam Ptr The address of the first element in the series.
     * @tparam Stride The number of bytes between elements. If Stride is 0, the elements are assumed to be tightly packed.
     *
     * @sa registral
     */
    template <auto Ptr, std::ptrdiff_t Stride = 0> requires ConstPtr<decltype(Ptr)> && std::is_array_v<typename decltype(Ptr)::element_type>
    struct registral_series {
        using element_type = std::remove_extent_t<std::remove_reference_t<typename decltype(Ptr)::element_type>>;
        using ptr_type = decltype(Ptr.m_ptr);

        static constexpr auto stride = Stride ? Stride : sizeof(element_type);

        constexpr auto& operator=(const std::remove_cvref_t<typename decltype(Ptr)::element_type>& value) const noexcept requires(!std::is_const_v<typename decltype(Ptr)::element_type>) {
            volatile_store(Ptr.get(), std::remove_cvref_t<typename decltype(Ptr)::element_type>(value));
            return *this;
        }

        /**
         * @brief The address operator has been overloaded to provide convenience for functions such as std::memcpy().
         *
         * @return Pointer to the first value_type element.
         */
        constexpr auto* operator&() const noexcept {
            using decayed_type = std::decay_t<typename decltype(Ptr)::element_type>;
            return reinterpret_cast<decayed_type>(Ptr.get());
        }

        constexpr auto& operator[](ptr_type i) const noexcept requires(!std::is_const_v<element_type>) {
            return *reinterpret_cast<std::remove_volatile_t<element_type>*>(Ptr.m_ptr + i * stride);
        }

        constexpr auto operator[](ptr_type i) const noexcept requires(std::is_const_v<element_type>) {
            return volatile_load(reinterpret_cast<element_type*>(Ptr.m_ptr + i * stride));
        }

        /**
         * @brief Read value at `i`.
         *
         * @param i Index of the element to load.
         * @return Value stored at index `i`.
         *
         * @note This returns a copy of the value at `i`, not a reference.
         */
        constexpr auto get(std::size_t i) const noexcept {
            return volatile_load(reinterpret_cast<std::remove_volatile_t<element_type>*>(Ptr.m_ptr + i * stride));
        }

        /**
         * @brief Write value to `i`.
         *
         * @tparam T Type of value.
         * @param i Index to put the value.
         * @param value Value to put at index.
         */
        template <typename T = std::remove_volatile_t<element_type>>
        constexpr void set(std::size_t i, T&& value) const noexcept {
            volatile_store(reinterpret_cast<element_type*>(Ptr.m_ptr + i * stride), static_cast<element_type&&>(value));
        }

        /**
         * @brief Calls the default constructor for the element at the given index.
         *
         * @param i Index of the element to reset.
         *
         * @sa registral::reset()
         */
        constexpr void reset(std::size_t i) const noexcept requires(!std::is_const_v<element_type>) {
            volatile_emplace(reinterpret_cast<element_type*>(Ptr.m_ptr + i * stride));
        }

        /**
         * @brief Constructs the value into the given index.
         *
         * @tparam Args Argument types.
         * @param i Element index to construct the value at.
         * @param args Arguments to be passed to the value constructor.
         * @return A copy of the constructed value.
         *
         * @sa registral::emplace()
         */
        template <typename... Args>
        constexpr auto emplace(std::size_t i, Args&&... args) const noexcept -> std::remove_cvref_t<element_type> requires(!std::is_const_v<element_type>) {
            return volatile_emplace(reinterpret_cast<element_type*>(Ptr.m_ptr + i * stride), std::forward<Args>(args)...);
        }

        /**
         * @struct scoped_ref
         * @brief Uses RAII to temporarily read a registral_series entry for modification before storing the updated
         *        value.
         *
         * @sa acquire()
         */
        struct scoped_ref : std::remove_cvref_t<element_type> {
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

        /**
         * @brief Acquire a temporary scoped reference for reading & modifying the registral_series' element at `i`.
         *
         * @return Scoped reference to the data stored at index `i`.
         *
         * @section Reading and writing a registral_series entry with acquire():
         * @code{cpp}
         * #include <gba/gba.hpp>
         *
         * int main() {
         *     using namespace gba;
         *
         *     struct my_type { int a; int b; };
         *
         *     static constexpr auto MY_REGISTRAL = registral_series<const_ptr<my_type[4]>(0x4000000), 8>{};
         *
         *     for (int ii = 0; ii < 4; ++ii) {
         *         if (auto ref = MY_REGISTRAL.acquire(ii)) {
         *             std::swap(ref.a, ref.b);
         *             // ref is written back to MY_REGISTRAL[ii] on scope exit
         *         }
         *     }
         * }
         * @endcode
         *
         * @sa scoped_ref
         */
        constexpr auto acquire(std::size_t i) const noexcept {
            return scoped_ref{Ptr.m_ptr + i * stride};
        }
    };

} // namespace gba

namespace std {

    template <auto Lhs, auto Rhs> requires
        std::is_same_v<std::remove_cvref_t<decltype(Lhs)>, gba::const_ptr<typename decltype(Lhs)::element_type>> &&
        std::is_same_v<std::remove_cvref_t<decltype(Rhs)>, gba::const_ptr<typename decltype(Rhs)::element_type>>
    constexpr void swap(const gba::registral<Lhs>& lhs, const gba::registral<Rhs>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }

} // namespace std

#endif // define GBAXX_TYPE_MEMORY_HPP
