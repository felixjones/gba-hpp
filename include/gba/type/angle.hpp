/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_ANGLE_HPP
#define GBAXX_TYPE_ANGLE_HPP

#include <concepts>
#include <numbers>

#include <gba/type/util.hpp>

namespace gba {

    consteval auto radians_to_turns(std::floating_point auto radian) {
        constexpr auto two_pi = 2 * std::numbers::pi_v<decltype(radian)>;
        return radian / two_pi;
    }

    template <std::integral T, std::size_t B>
    struct angle {
        using data_type = T;
        static constexpr auto bits = B;

        explicit consteval angle(std::floating_point auto radian) : m_data{round<T>(radians_to_turns(radian) * (1 << B))} {}

        angle() = default;

        template <std::integral U>
        explicit constexpr angle(U data) noexcept : m_data{T(data)} {}

        template <std::integral U, std::size_t B2>
        explicit constexpr angle(angle<U, B2> a) noexcept : m_data(shift_to<B2, B>(a.data())) {}

#ifdef _DEBUG
        template <std::floating_point U>
        explicit constexpr operator U() const noexcept {
            constexpr auto two_pi = 2 * std::numbers::pi_v<U>;
            constexpr auto mask = (1 << B) - 1;
            return (two_pi * (m_data & mask)) / U(1 << B);
        }
#endif

        template <std::integral U, std::size_t B2>
        constexpr angle& operator=(angle<U, B2> rhs) noexcept {
            m_data = shift_to<B2, B>(rhs.data());
            return *this;
        }

        template <std::integral U, std::size_t B2>
        constexpr angle& operator+=(angle<U, B2> rhs) noexcept {
            m_data += shift_to<B2, B>(rhs.data());
            return *this;
        }

        template <std::integral U, std::size_t B2>
        constexpr angle& operator-=(angle<U, B2> rhs) noexcept {
            m_data -= shift_to<B2, B>(rhs.data());
            return *this;
        }

        constexpr angle operator-() const noexcept {
            return angle(-m_data);
        }

        constexpr T data() const noexcept {
            return m_data;
        }

        constexpr T& data() noexcept {
            return m_data;
        }

        T m_data{};
    };

    template <typename T>
    concept Angle = std::same_as<angle<typename T::data_type, T::bits>, T>;

    template <Angle Lhs, Angle Rhs>
    constexpr auto operator+(Lhs lhs, Rhs rhs) noexcept {
        using data_type = decltype(typename Lhs::data_type() + typename Rhs::data_type());

        constexpr auto bits = (Lhs::bits + Rhs::bits) / 2;

        return angle<data_type, bits>(shift_to<Lhs::bits, bits>(lhs.data()) + shift_to<Rhs::bits, bits>(rhs.data()));
    }

    template <Angle Lhs, Angle Rhs>
    constexpr auto operator-(Lhs lhs, Rhs rhs) noexcept {
        using data_type = decltype(typename Lhs::data_type() - typename Rhs::data_type());

        constexpr auto bits = (Lhs::bits + Rhs::bits) / 2;

        return angle<data_type, bits>(shift_to<Lhs::bits, bits>(lhs.data()) - shift_to<Rhs::bits, bits>(rhs.data()));
    }

    template <Angle Lhs, std::integral Rhs>
    constexpr auto operator/(Lhs lhs, Rhs rhs) noexcept {
        return Lhs(lhs.data() / rhs);
    }

    template <Angle Lhs, Angle Rhs>
    constexpr auto operator<=>(Lhs lhs, Rhs rhs) noexcept {
        constexpr auto bits = (Lhs::bits + Rhs::bits) / 2;
        constexpr auto mask = (1 << bits) - 1;

        return (shift_to<Lhs::bits, bits>(lhs.data()) & mask) <=> (shift_to<Rhs::bits, bits>(rhs.data()) & mask);
    }

    template <Angle Lhs, Angle Rhs>
    constexpr bool operator==(Lhs lhs, Rhs rhs) noexcept {
        constexpr auto bits = (Lhs::bits + Rhs::bits) / 2;
        constexpr auto mask = (1 << bits) - 1;

        return (shift_to<Lhs::bits, bits>(lhs.data()) & mask) == (shift_to<Rhs::bits, bits>(rhs.data()) & mask);
    }

} // namespace gba

#endif // define GBAXX_TYPE_ANGLE_HPP
