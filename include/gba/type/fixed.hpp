/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_FIXED_HPP
#define GBAXX_TYPE_FIXED_HPP

#include <cmath>
#include <concepts>

#include <gba/type/vector.hpp>

namespace gba {

    namespace detail {

        template <typename T>
        concept Primitive = std::is_fundamental_v<T> || Vector<T>;

    } // namespace detail

    template <typename T>
    consteval auto round(std::floating_point auto x) {
        return T(x + decltype(x)(0.5));
    }

    template <std::make_signed_t<std::size_t> Sh>
    constexpr auto shift_right(detail::Primitive auto x) noexcept {
        if constexpr (Sh < 0) {
            return x << -Sh;
        } else {
            return x >> Sh;
        }
    }

    template <detail::Primitive T, std::size_t F>
    struct fixed {
        explicit consteval fixed(std::floating_point auto f) requires (!Vector<T>) : m_data{round<T>(f * (1 << F))} {}

        template <std::floating_point... Args>
        explicit consteval fixed(Args&&... args) requires Vector<T> : m_data{round<typename vector_traits<T>::value_type>(std::forward<Args>(args) * (1 << F))...} {}

        fixed() = default;

        explicit constexpr fixed(std::integral auto x) noexcept requires (!Vector<T>) : m_data(x << F) {}

        template <std::integral... Args>
        explicit constexpr fixed(Args&&... args) noexcept requires Vector<T> : m_data{typename vector_traits<T>::value_type(std::forward<Args>(args) << F)...} {}

        template <detail::Primitive U, std::size_t F2>
        constexpr fixed& operator=(fixed<U, F2> f) noexcept {
            m_data = shift_right<F - F2>(f.data());
            return *this;
        }

        template <std::integral U>
        explicit constexpr operator U() const noexcept {
            return U(m_data >> F);
        }

        constexpr T data() const noexcept {
            return m_data;
        }

        T m_data{};

    protected:
        constexpr fixed(T x, std::nullptr_t) noexcept requires (!Vector<T>) : m_data{x} {}

        template <std::integral... Args>
        explicit constexpr fixed(std::nullptr_t, Args... args) noexcept requires Vector<T> : m_data{args...} {}
    public:
        static constexpr auto from_data(std::integral auto x) noexcept requires (!Vector<T>) {
            return fixed(x, nullptr);
        }

        template <std::integral... Args>
        static constexpr auto from_data(Args... args) noexcept requires Vector<T> {
            return fixed(nullptr, args...);
        }
    };

    template <std::size_t F>
    using make_fixed = fixed<int, F>;

    template <std::size_t F, std::size_t N>
    using make_fixed_vector = fixed<make_vector<int, N>, F>;

} // namespace gba

#endif // define GBAXX_TYPE_FIXED_HPP
