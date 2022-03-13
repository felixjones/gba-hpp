/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_BIOS_MATH_HPP
#define GBAXX_BIOS_MATH_HPP

#include <gba/bios/swi_call.hpp>
#include <gba/inttype.hpp>
#include <gba/vectype.hpp>

namespace gba::bios {

[[nodiscard, gnu::const, gnu::always_inline]]
inline auto Div(int32 number, int32 denom) noexcept {
    return swi<0x06, ivec2>(number, denom);
}

[[nodiscard, gnu::const, gnu::always_inline]]
inline auto DivArm(int32 denom, int32 number) noexcept {
    return swi<0x07, ivec2>(denom, number);
}

[[nodiscard, gnu::const, gnu::always_inline]]
inline auto Sqrt(uint32 x) noexcept {
    return swi<0x08, uint32>(x);
}

} // namespace gba::bios

#endif // define GBAXX_BIOS_MATH_HPP
