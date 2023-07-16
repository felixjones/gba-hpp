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

    [[nodiscard, gnu::always_inline, gnu::const]]
    inline auto Div(int number, int denom) noexcept {
        register int r0 asm("r0") = number;
        register int r1 asm("r1") = denom;
        asm volatile inline ("swi 0x6 << ((1f - . == 4) * -16); 1:" : "=r"(r0), "=r"(r1) :: "r3");
        return make_vector<int, 2>{r0, r1};
    }

    [[nodiscard, gnu::always_inline, gnu::const]]
    inline auto DivArm(int denom, int number) noexcept {
        register int r0 asm("r0") = denom;
        register int r1 asm("r1") = number;
        asm volatile inline ("swi 0x7 << ((1f - . == 4) * -16); 1:" : "=r"(r0), "=r"(r1) :: "r3");
        return make_vector<int, 2>{r0, r1};
    }

    [[nodiscard, gnu::always_inline, gnu::const]]
    inline auto Sqrt(u32 arg) noexcept {
        register u32 r0 asm("r0") = arg;
        asm volatile inline ("swi 0x8 << ((1f - . == 4) * -16); 1:" : "=r"(r0) :: "r1", "r3");
        return r0;
    }

    [[nodiscard, gnu::always_inline, gnu::const]]
    inline auto ArcTan(fixed<int, 14> arg) noexcept {
        register auto r0 asm("r0") = arg.data();
        asm volatile inline ("swi 0x9 << ((1f - . == 4) * -16); 1:" : "=r"(r0) :: "r1", "r3");
        return angle<int, 14>{r0};
    }

    [[nodiscard, gnu::always_inline, gnu::const]]
    inline auto ArcTan2(fixed<int, 14> x, fixed<int, 14> y) noexcept {
        register auto r0 asm("r0") = x.data();
        register auto r1 asm("r1") = y.data();
        asm volatile inline ("swi 0xA << ((1f - . == 4) * -16); 1:" : "=r"(r0), "=r"(r1) :: "r3");
        return angle<int, 16>{x.data()};
    }

}

} // namespace gba::bios

#endif // define GBAXX_BIOS_MATH_HPP
