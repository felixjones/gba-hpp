/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_BIOS_MATH_HPP
#define GBAXX_BIOS_MATH_HPP
/** @file */

#include <gba/type/fixed.hpp>

namespace gba::bios {

namespace {

    // Integer functions

    /**
     * @brief A function to divide two integers.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-06h-gba-or-swi-09h-nds7nds9dsi7dsi9---div">SWI 06h (GBA) or SWI 09h (NDS7/NDS9/DSi7/DSi9) - Div</a>
     *
     * @param number The numerator to be divided.
     * @param denom The denominator to divide by (must be nonzero).
     *
     * @return Vector containing the result of the division operation and the remainder.
     *
     * @warning If the denominator is zero then this function is likely to enter an infinite loop.
     */
    [[nodiscard, gnu::always_inline, gnu::const]]
    inline auto Div(int number, int denom) noexcept {
        register auto r0 asm("r0") = number;
        register auto r1 asm("r1") = denom;
        asm volatile inline ("swi 0x6 << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) :: "r3");
        return make_vector<int, 2>{r0, r1};
    }

    /**
     * @brief A function to divide two integers.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-07h-gba---divarm">SWI 07h (GBA) - DivArm</a>
     *
     * @param denom The denominator to divide by (must be nonzero).
     * @param number The numerator to be divided.
     *
     * @return Vector containing the result of the division operation and the remainder.
     *
     * @note This is the same as Div(), however the parameters are swapped. It is recommended to use Div().
     *
     * @warning If the denominator is zero then this function is likely to enter an infinite loop.
     *
     * @sa Div()
     */
    [[nodiscard, gnu::always_inline, gnu::const]]
    inline auto DivArm(int denom, int number) noexcept {
        register auto r0 asm("r0") = denom;
        register auto r1 asm("r1") = number;
        asm volatile inline ("swi 0x7 << ((1f - . == 4) * -16); 1:" : "+r"(r0) : "r"(r1) : "r3");
        return make_vector<int, 2>{r0, r1};
    }

    /**
     * @brief Calculates the square root of a 32-bit unsigned integer.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-08h-gba-or-swi-0dh-nds7nds9dsi7dsi9---sqrt">SWI 08h (GBA) or SWI 0Dh (NDS7/NDS9/DSi7/DSi9) - Sqrt</a>
     *
     * @param arg The 32-bit unsigned integer argument for which to calculate the square root.
     * @return The square root of the given argument.
     */
    [[nodiscard, gnu::always_inline, gnu::const]]
    inline auto Sqrt(u32 arg) noexcept {
        register auto r0 asm("r0") = arg;
        asm volatile inline ("swi 0x8 << ((1f - . == 4) * -16); 1:" : "+r"(r0) :: "r1", "r3");
        return r0;
    }

    // Fixed point functions

    /**
     * @brief Computes the square root of the given fixed-point number.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-08h-gba-or-swi-0dh-nds7nds9dsi7dsi9---sqrt">SWI 08h (GBA) or SWI 0Dh (NDS7/NDS9/DSi7/DSi9) - Sqrt</a>
     *
     * @tparam Exp How many extra bits of precision to add.
     * @param arg The fixed-point number for which to calculate the square root.
     * @return The square root of the input fixed-point number.
     *
     * @note The result will have half the precision in bits to the input. Precision can be increased with Exp.
     *
     * @warning Increasing the precision with Exp may result in losing the upper bits and producing an incorrect result.
     */
    template <std::size_t Exp = 0>
    [[nodiscard, gnu::always_inline, gnu::const]]
    inline auto Sqrt(Fixed auto arg) noexcept {
        using fixed_type = decltype(arg);
        using unsigned_type = std::make_unsigned_t<typename fixed_type::data_type>;

        static constexpr auto half_exp = (fixed_type::fractional_bits + 1) / 2;
        static constexpr auto odd_bit = fixed_type::fractional_bits % 2;

        register auto r0 asm("r0") = unsigned_type(arg.data() << ((Exp * 2) + odd_bit));
        asm volatile inline ("swi 0x8 << ((1f - . == 4) * -16); 1:" : "+r"(r0) :: "r1", "r3");
        return fixed<unsigned_type, half_exp + Exp>::from_data(r0);
    }

    /**
     * @brief Calculates the arc tangent of the given fixed-point number.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-09h-gba---arctan">SWI 09h (GBA) - ArcTan</a>
     *
     * This function returns the arc tangent of the specified fixed-point value using a precision of 14 fractional bits.
     *
     * @param arg The fixed-point number for which to calculate the arc tangent.
     * @return The arc tangent of the input fixed-point number as a binary angle.
     */
    [[nodiscard, gnu::always_inline, gnu::const]]
    inline auto ArcTan(fixed<int, 14> arg) noexcept {
        register auto r0 asm("r0") = arg.data();
        asm volatile inline ("swi 0x9 << ((1f - . == 4) * -16); 1:" : "+r"(r0) :: "r1", "r3");
        return angle<int, 14>{r0};
    }

    /**
     * @brief Calculates the arc tangent of the given coordinates.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-0ah-gba---arctan2">SWI 0Ah (GBA) - ArcTan2</a>
     *
     * This function returns the arc tangent of the fixed-point coordinates (x, y) using a precision of 14 fractional
     * bits.
     *
     * @param x The x-coordinate.
     * @param y The y-coordinate.
     * @return The arc tangent of the coordinates (x, y) as a binary angle.
     *
     * @note Unlike ArcTan(), the result is a 16-bit binary angle.
     * @note Notice that the order of the parameters are (x, y), unlike some atan2 implementations.
     */
    [[nodiscard, gnu::always_inline, gnu::const]]
    inline auto ArcTan2(fixed<int, 14> x, fixed<int, 14> y) noexcept {
        register auto r0 asm("r0") = x.data();
        register auto r1 asm("r1") = y.data();
        asm volatile inline ("swi 0xA << ((1f - . == 4) * -16); 1:" : "+r"(r0) : "r"(r1) : "r3");
        return angle<int, 16>{r0};
    }

    /**
     * @struct bg_affine_src
     * @brief Input parameters for BgAffineSet().
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-0eh-gba---bgaffineset">SWI 0Eh (GBA) - BgAffineSet</a>
     *
     * @sa bg_affine_dest
     * @sa BgAffineSet()
     */
    struct bg_affine_src {
        fixed<int, 8> tex_x; /**< Texture source center X. */
        fixed<int, 8> tex_y; /**< Texture source center Y. */
        short scr_x; /**< Screen destination center X. */
        short scr_y; /**< Screen destination center Y. */
        fixed<short, 8> sx; /**< Texture scale X. */
        fixed<short, 8> sy; /**< Texture scale Y. */
        angle<u16> alpha; /**< Texture rotation around source center. @sa bg_affine_src::tex_x @sa bg_affine_src::tex_y */
        short : 16;
    };

    /**
     * @struct bg_affine_dest
     * @brief Affine matrix result of BgAffineSet().
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-0eh-gba---bgaffineset">SWI 0Eh (GBA) - BgAffineSet</a>
     *
     * This is a 2x3 affine transformation matrix.
     *
     * @sa bg_affine_src
     * @sa BgAffineSet()
     */
    struct bg_affine_dest {
        fixed<short, 8> pa;
        fixed<short, 8> pb;
        fixed<short, 8> pc;
        fixed<short, 8> pd;
        fixed<int, 8> dx;
        fixed<int, 8> dy;
    };

    /**
     * @brief Fills the destination buffer with affine matrices built from the source buffer.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-0eh-gba---bgaffineset">SWI 0Eh (GBA) - BgAffineSet</a>
     *
     * @param src Pointer to the source buffer in bg_affine_src format.
     * @param dest Pointer to the destination buffer in bg_affine_dest format.
     * @param num The number of matrices to calculate.
     *
     * @note This function assumes that both `src` and `dest` buffers have sufficient memory allocated to hold `num`
     *       elements. Also, it is the responsibility of the caller to ensure that the memory regions pointed by `src`
     *       and `dest` do not overlap.
     *
     * @sa bg_affine_dest
     * @sa bg_affine_src
     */
    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void BgAffineSet(const bg_affine_src* __restrict__ src, volatile bg_affine_dest* __restrict__ dest, std::size_t num) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        register auto r2 asm("r2") = num;
        asm volatile inline ("swi 0xE << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) : "r"(r2) : "r3", "memory");
    }

    /**
     * @struct obj_affine_src
     * @brief Input parameters for ObjAffineSet().
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-0fh-gba---objaffineset">SWI 0Fh (GBA) - ObjAffineSet</a>
     *
     * @sa ObjAffineSet()
     */
    struct obj_affine_src {
        fixed<short, 8> sx; /**< Object scale X. */
        fixed<short, 8> sy; /**< Object scale Y. */
        angle<u16> alpha; /**< Object rotation around center. */
        short : 16;
    };

    /**
     * @brief Fills the destination with affine matrices built from the source buffer.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-0fh-gba---objaffineset">SWI 0Fh (GBA) - ObjAffineSet</a>
     *
     * @param src Pointer to the source buffer in obj_affine_src format.
     * @param dest Pointer to the first element of the destination matrices.
     * @param num The number of matrices to calculate.
     * @param stride The stride between each element of the destination matrices.
     *
     * @note The stride must be greater than or equal 2.
     * @note This function assumes that both `src` and `dest` arrays have sufficient memory allocated to hold `num`
     *       elements. Also, it is the responsibility of the caller to ensure that the memory regions pointed by `src`
     *       and `dest` do not overlap.
     *
     * @sa obj_affine_src
     */
    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void ObjAffineSet(const obj_affine_src* __restrict__ src, volatile fixed<short, 8>* __restrict__ dest, std::size_t num, std::size_t stride) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        register auto r2 asm("r2") = num;
        register auto r3 asm("r3") = stride;
        asm volatile inline ("swi 0xF << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1), "+r"(r3) : "r"(r2) : "memory");
    }

    /**
     * @brief Calculates the sine and cosine of the given angle.
     *
     * This function utilizes ObjAffineSet() to simultaneously perform the calculation.
     *
     * @param alpha The binary angle.
     * @return A vector of two fixed-point numbers containing the sine and cosine values.
     *
     * @note The return fixed-point values have a precision of 8 bits and a storage of 16 bits each, meaning they fit
     *       within a single 32-bit register.
     *
     * @sa ObjAffineSet()
     */
    [[nodiscard, gnu::always_inline, gnu::const]]
    inline auto SinCos(Angle auto alpha) noexcept {
        const obj_affine_src src = {fixed<short, 8>{1}, fixed<short, 8>{1}, alpha};
        fixed<short, 8> dest[4];
        ObjAffineSet(&src, dest, 1, 2);
        return fixed<make_vector<short, 2>, 8>{dest[2], dest[0]};
    }

}

} // namespace gba::bios

#endif // define GBAXX_BIOS_MATH_HPP
