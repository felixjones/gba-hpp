/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_UTIL_HPP
#define GBAXX_TYPE_UTIL_HPP
/** @file */

#include <concepts>
#include <cstdint>

#include <gba/type/vector.hpp>

namespace gba {

    /**
     * @brief Concept for a "fundamental" type.
     *
     * A fundamental type either conforms to std::is_fundamental_v, or is a GNU vector of fundamental types.
     *
     * @tparam T Type for checking the concept.
     */
    template <typename T>
    concept Fundamental = std::is_fundamental_v<T> || Vector<T>;

    /**
     * @brief Rounds a floating-point number to the nearest integer.
     *
     * Only available in a constexpr context.
     *
     * @tparam T The type to convert the resulting integer to.
     * @param x The floating-point number to be rounded.
     * @return The rounded integer value.
     *
     * @note If the fractional part of the input is exactly 0.5, the rounding is performed towards the nearest even number.
     * For example, 1.5 will be rounded to 2, while -1.5 will be rounded to -2.
     */
    template <typename T>
    consteval auto round_float(std::floating_point auto x) {
        return T(x + decltype(x)(0.5));
    }

    /**
     * @brief Shifts the given value by `Sh` bits.
     *
     * @tparam Sh The number of bits to shift right by. Negative values will shift left.
     * @param x The value to be shifted.
     * @return The result of the shift operation.
     */
    template <std::int64_t Sh>
    constexpr auto shift_right(Fundamental auto x) noexcept {
        if constexpr (Sh < 0) {
            return x << -Sh;
        } else if constexpr (Sh > 0) {
            return x >> Sh;
        } else {
            return x;
        }
    }

    /**
     * @brief Shifts a fixed-point integer from its current decimal bits to a target.
     *
     * This is used mostly in the fixed type when converting between two fixed-point formats.
     *
     * @tparam From Bit precision of the input variable.
     * @tparam To Desired bit precision of the result variable.
     * @param x The variable to be shifted.
     * @return The variables shifted right `From - To` bits.
     *
     * @sa shift_right()
     * @sa fixed
     */
    template <std::int64_t From, std::int64_t To>
    constexpr auto shift_to(Fundamental auto x) noexcept {
        return shift_right<From - To>(x);
    }

    /**
     * @brief Extends the sign of an integer type to a given number of bits.
     *
     * @tparam Bits Number of desired low bits of x.
     * @param x The variable to be sign extended.
     * @return The variable sign extended.
     */
    template <std::size_t Bits>
    constexpr auto sign_extend(std::integral auto x) noexcept {
        constexpr auto shift = (sizeof(x) * 8) - Bits;
        return (x << shift) >> shift;
    }

    /**
     * @brief Utility to copy the sign from one type to another.
     *
     * @tparam Sign The source type to copy the sign from.
     * @tparam Target The destination type to receive the Sign.
     *
     * @sa std::copysign()
     */
    template <typename Sign, typename Target>
    using copy_sign = std::conditional_t<std::is_signed_v<Sign>,
                std::make_signed_t<Target>,
                std::make_unsigned_t<Target>
            >;

    /**
     * @brief Increases the size of an integer or GNU vector.
     *
     * char will become short, short will become int, int will become long long. The sign will be preserved.
     *
     * @tparam T Integral or Vector type to make bigger.
     */
    template <typename T>
    struct make_bigger;

    /**
     * @brief std::integral specialized make_bigger.
     *
     * char will become short, short will become int, int will become long long. The sign will be preserved.
     *
     * @tparam T Integer type to make bigger.
     */
    template <std::integral T>
    struct make_bigger<T> {
        using type = copy_sign<T, std::conditional_t<std::is_same_v<std::make_signed_t<T>, char>,
                short,
                std::conditional_t<std::is_same_v<std::make_signed_t<T>, short>,
                        int,
                        long long // No larger integer type
                >>>;
    };

    /**
     * @brief Vector specialized make_bigger.
     *
     * This will apply make_bigger to the element type of the GNU vector.
     *
     * @tparam T Vector type to make bigger.
     */
    template <Vector T>
    struct make_bigger<T> {
        using type = make_vector<typename make_bigger<typename vector_traits<T>::value_type>::type, vector_traits<T>::size>;
    };

    template <typename T, typename I>
    struct n_tuple_helper {};

    template <typename T, std::size_t... I>
    struct n_tuple_helper<T, std::index_sequence<I...>> {
        using type = std::tuple<std::enable_if_t<I || true, T>...>;
    };

    /**
     * @brief Creates an std::tuple of type T repeated N times.
     *
     * Useful for implementing std::tuple_element specializations for custom structured binding.
     *
     * @tparam T Type to repeat in the tuple.
     * @tparam N Number of elements to repeat in the tuple.
     */
    template <typename T, std::size_t N>
    using n_tuple = typename n_tuple_helper<T, std::make_index_sequence<N>>::type;

} // namespace gba

#endif // define GBAXX_TYPE_UTIL_HPP
