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

    template <bool Signed>
    consteval auto radians_to_turns(std::floating_point auto radian) {
        constexpr auto pi = std::numbers::pi_v<decltype(radian)>;

//        if constexpr (Signed) {
//            return (radian / pi) - 1; //TODO: Fix this
//        } else {
            return radian / (2 * pi);
//        }
    }

    template <std::integral T, std::size_t B>
    struct angle {
        using data_type = T;
        static constexpr auto bits = B;

        explicit consteval angle(std::floating_point auto radian) : m_data{round<T>(radians_to_turns<std::is_signed_v<T>>(radian) * (1 << B))} {}

        angle() = default;

        explicit constexpr angle(T&& data) noexcept : m_data{data} {}

        template <std::integral U, std::size_t B2>
        explicit constexpr angle(angle<U, B2> a) noexcept : m_data(shift_right<int(B2) - int(B)>(a.data())) {}

        constexpr T data() const noexcept {
            return m_data;
        }

        T m_data{};
    };

    template <typename T>
    concept Angle = std::same_as<angle<typename T::data_type, T::bits>, T>;

} // namespace gba

#endif // define GBAXX_TYPE_ANGLE_HPP
