/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_ANGLE_HPP
#define GBAXX_TYPE_ANGLE_HPP
/** @file */

#include <concepts>
#include <numbers>

#include <gba/type/util.hpp>

namespace gba {

    /**
     * @brief Converts the given value from radians to turns.
     *
     * This function takes a floating-point value in radians and converts it
     * to turns. A turn is a complete revolution, which is equal to 2 Pi radians.
     *
     * @param radian The value in radians to be converted.
     * @return The converted value in turns.
     */
    consteval auto radians_to_turns(std::floating_point auto radian) {
        constexpr auto two_pi = 2 * std::numbers::pi_v<decltype(radian)>;
        return radian / two_pi;
    }

    /**
     * @struct angle
     * @brief Binary angle measurement type.
     * @see <a href="https://en.wikipedia.org/wiki/Binary_angular_measurement">Binary angular measurement - Wikipedia</a>
     *
     * The binary precision is determined by the template parameter `B`, which specifies the number of bits used to
     * represent the angle.
     *
     * @tparam T The underlying data type used to store the angle value.
     * @tparam B The number of bits used to represent the angle.
     */
    template <std::integral T, std::size_t B = sizeof(T) * 8>
    struct angle {
        using data_type = T;
        static constexpr auto bits = B;
        static constexpr auto mask = B == sizeof(T) * 8 ? T(std::make_signed_t<T>(-1)) : (T{1} << B) - 1;

        /**
         * @brief Convert angle from radians to a binary angle measurement.
         *
         * @param radian The angle in radians.
         */
        consteval angle(std::floating_point auto radian) : m_data{T(round_float<T>(radians_to_turns(radian) * (1LL << B)) & mask)} {}

        angle() = default;

        /**
         * @brief Directly initialize the underlying data for a binary angle.
         *
         * @tparam U Integer data type to convert from.
         * @param data Binary angle in raw integer format.
         */
        template <std::integral U>
        constexpr angle(U data) noexcept : m_data(static_cast<T>(data)) {}

        /**
         * @brief Convert from a binary angle of another format.
         *
         * @tparam U Other internal storage type.
         * @tparam B2 Other bit precision.
         * @param a Other binary angle to convert from.
         */
        template <std::integral U, std::size_t B2>
        constexpr angle(angle<U, B2> a) noexcept : m_data(static_cast<T>(shift_to<B2, B>(a.data()))) {}

#ifdef _DEBUG
        /**
         * @brief Convert to floating point radians.
         *
         * @warning Operating with floating point types can be slow, for this reason this conversion is only available
         *          when _DEBUG is defined.
         *
         * @tparam U Destination floating point type
         * @return Angle in radians.
         */
        template <std::floating_point U>
        explicit constexpr operator U() const noexcept {
            constexpr auto two_pi = 2 * std::numbers::pi_v<U>;

            return (two_pi * (m_data & mask)) / U(1LLU << B);
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

    /**
     * @brief Binary angle type concept.
     *
     * @tparam T Expected to be angle.
     *
     * @sa angle
     */
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
        using data_type = decltype(typename Lhs::data_type() + typename Rhs::data_type());

        return sign_extend<bits>(shift_to<Lhs::bits, bits>(data_type(lhs.data()))) <=> sign_extend<bits>(shift_to<Rhs::bits, bits>(data_type(rhs.data())));
    }

    template <Angle Lhs, Angle Rhs>
    constexpr bool operator==(Lhs lhs, Rhs rhs) noexcept {
        constexpr auto bits = (Lhs::bits + Rhs::bits) / 2;

        return shift_to<Lhs::bits, bits>(lhs.data()) == shift_to<Rhs::bits, bits>(rhs.data());
    }

    template <Angle Lhs, std::integral Rhs>
    constexpr auto operator>>(Lhs lhs, Rhs rhs) noexcept {
        return Lhs(lhs.data() >> rhs);
    }

    template <Angle Lhs, std::integral Rhs>
    constexpr auto operator<<(Lhs lhs, Rhs rhs) noexcept {
        return Lhs(lhs.data() << rhs);
    }

} // namespace gba

#endif // define GBAXX_TYPE_ANGLE_HPP
