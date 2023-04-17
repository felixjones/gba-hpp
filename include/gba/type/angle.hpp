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
        constexpr auto pi = std::numbers::pi_v<decltype(radian)>;
        return radian / (2 * pi);
    }

    template <std::integral T, std::size_t B>
    struct angle {
        using data_type = T;
        static constexpr auto bits = B;

        explicit consteval angle(std::floating_point auto radian) : m_data{round<T>(radians_to_turns(radian) * (1 << B))} {}

        angle() = default;

        explicit constexpr angle(T&& data) noexcept : m_data{data} {}

        template <std::integral U, std::size_t B2>
        explicit constexpr angle(angle<U, B2> a) noexcept : m_data(shift_to<B2, B>(a.data())) {}

        template <std::integral U, std::size_t B2>
        angle& operator=(angle<U, B2>&& rhs) noexcept {
            m_data = shift_to<B2, B>(rhs.data());
            return *this;
        }

        constexpr T data() const noexcept {
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

} // namespace gba

#endif // define GBAXX_TYPE_ANGLE_HPP
