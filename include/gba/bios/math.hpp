/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_BIOS_MATH_HPP
#define GBAXX_BIOS_MATH_HPP

#include <gba/type/fixed.hpp>

namespace gba::bios {

namespace {

    // Integer functions

    [[nodiscard, gnu::always_inline, gnu::const]]
    inline auto Div(int number, int denom) noexcept {
        register auto r0 asm("r0") = number;
        register auto r1 asm("r1") = denom;
        asm volatile inline ("swi 0x6 << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) :: "r3");
        return make_vector<int, 2>{r0, r1};
    }

    [[nodiscard, gnu::always_inline, gnu::const]]
    inline auto DivArm(int denom, int number) noexcept {
        register auto r0 asm("r0") = denom;
        register auto r1 asm("r1") = number;
        asm volatile inline ("swi 0x7 << ((1f - . == 4) * -16); 1:" : "+r"(r0) : "r"(r1) : "r3");
        return make_vector<int, 2>{r0, r1};
    }

    [[nodiscard, gnu::always_inline, gnu::const]]
    inline auto Sqrt(u32 arg) noexcept {
        register auto r0 asm("r0") = arg;
        asm volatile inline ("swi 0x8 << ((1f - . == 4) * -16); 1:" : "+r"(r0) :: "r1", "r3");
        return r0;
    }

    // Fixed point functions

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

    [[nodiscard, gnu::always_inline, gnu::const]]
    inline auto ArcTan(fixed<int, 14> arg) noexcept {
        register auto r0 asm("r0") = arg.data();
        asm volatile inline ("swi 0x9 << ((1f - . == 4) * -16); 1:" : "+r"(r0) :: "r1", "r3");
        return angle<int, 14>{r0};
    }

    [[nodiscard, gnu::always_inline, gnu::const]]
    inline auto ArcTan2(fixed<int, 14> x, fixed<int, 14> y) noexcept {
        register auto r0 asm("r0") = x.data();
        register auto r1 asm("r1") = y.data();
        asm volatile inline ("swi 0xA << ((1f - . == 4) * -16); 1:" : "+r"(r0) : "r"(r1) : "r3");
        return angle<int, 16>{r0};
    }

    struct bg_affine_src {
        fixed<int, 8> tex_x;
        fixed<int, 8> tex_y;
        short scr_x;
        short scr_y;
        fixed<short, 8> sx;
        fixed<short, 8> sy;
        angle<u16> alpha;
        short : 16;
    };

    struct bg_affine_dest {
        fixed<short, 8> pa;
        fixed<short, 8> pb;
        fixed<short, 8> pc;
        fixed<short, 8> pd;
        fixed<int, 8> dx;
        fixed<int, 8> dy;
    };

    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void BgAffineSet(const bg_affine_src* __restrict__ src, volatile bg_affine_dest* __restrict__ dest, std::size_t num) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        register auto r2 asm("r2") = num;
        asm volatile inline ("swi 0xE << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) : "r"(r2) : "r3", "memory");
    }

    struct obj_affine_src {
        fixed<short, 8> sx;
        fixed<short, 8> sy;
        angle<u16> alpha;
        short : 16;
    };

    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void ObjAffineSet(const obj_affine_src* __restrict__ src, volatile fixed<short, 8>* __restrict__ dest, std::size_t num, std::size_t stride) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        register auto r2 asm("r2") = num;
        register auto r3 asm("r3") = stride;
        asm volatile inline ("swi 0xF << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1), "+r"(r3) : "r"(r2) : "memory");
    }

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
