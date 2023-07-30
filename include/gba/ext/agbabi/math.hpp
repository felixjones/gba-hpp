/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_MATH_HPP
#define GBAXX_EXT_AGBABI_MATH_HPP

#include <agbabi.h>

#include <gba/type.hpp>

namespace gba::agbabi {

namespace {

    /**
     * Calculates the sine of the given binary angle.
     *
     * @param bam The binary angle for which the sine value will be calculated.
     * @return The signed fixed-point sine value of the given binary angle.
     *
     * @note The binary angle is converted to 15 bits of precision before calculating the sine.
     * @note The return fixed-point precision is always 29 bits.
     *
     * @sa cos()
     */
    [[gnu::const]]
    auto sin(Angle auto bam) noexcept {
        return fixed<int, 29>::from_data(__agbabi_sin(angle<int, 15>(bam).data()));
    }

    /**
     * Calculates the cosine of the given binary angle.
     *
     * @param bam The binary angle for which the cosine value will be calculated.
     * @return The signed fixed-point cosine value of the given binary angle.
     *
     * @note The binary angle is converted to 15 bits of precision before calculating the cosine.
     * @note The return fixed-point precision is always 29 bits.
     *
     * @sa sin()
     */
    [[gnu::const]]
    auto cos(Angle auto bam) noexcept {
        return fixed<int, 29>::from_data(__agbabi_sin(angle<int, 15>(bam).data() + 0x2000));
    }

    /**
     * @brief Calculates the arc tangent of the given coordinates.
     *
     * This function returns the arc tangent of the fixed-point coordinates (x, y).
     *
     * @param x The x-coordinate.
     * @param y The y-coordinate.
     * @return The arc tangent of the coordinates (x, y) as a binary angle.
     *
     * @note The x and y inputs are converted to 12 bits of precision before calculating the atan2.
     * @note The result is a 15-bit binary angle.
     * @note Notice that the order of the parameters are (x, y), unlike some atan2 implementations.
     *
     * @sa bios::ArcTan2()
     */
    [[gnu::const]]
    auto atan2(Fixed auto x, Fixed auto y) noexcept {
        return angle<int, 15>(__agbabi_atan2(fixed<int, 12>(x).data(), fixed<int, 12>(y).data()));
    }

    /**
     * @brief Computes the square root of the given fixed-point number.
     *
     * @tparam Exp How many extra bits of precision to add.
     * @param arg The fixed-point number for which to calculate the square root.
     * @return The square root of the input fixed-point number.
     *
     * @note The result will have half the precision in bits to the input. Precision can be increased with Exp.
     *
     * @warning Increasing the precision with Exp may result in losing the upper bits and producing an incorrect result.
     *
     * @sa bios::Sqrt()
     */
    template <std::size_t Exp = 0>
    [[gnu::const]]
    auto sqrt(Fixed auto x) noexcept {
        using fixed_type = decltype(x);
        using unsigned_type = std::make_unsigned_t<typename fixed_type::data_type>;

        static constexpr auto half_exp = (fixed_type::fractional_bits + 1) / 2;
        static constexpr auto odd_bit = fixed_type::fractional_bits % 2;

        return fixed<typename fixed_type::data_type, half_exp + Exp>::from_data(__agbabi_sqrt(unsigned_type(x.data() << ((Exp * 2) + odd_bit))));
    }

    /**
     * @brief Calculates the square root of a 32-bit unsigned integer.
     *
     * @param arg The 32-bit unsigned integer argument for which to calculate the square root.
     * @return The square root of the given argument.
     *
     * @sa bios::Sqrt()
     */
    [[gnu::const]]
    inline auto sqrt(u32 x) noexcept {
        return __agbabi_sqrt(x);
    }

}

} // namespace gba::agbabi

#endif // define GBAXX_EXT_AGBABI_MATH_HPP
