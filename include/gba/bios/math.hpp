/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_BIOS_MATH_HPP
#define GBAXX_BIOS_MATH_HPP

#include <gba/fixedtype.hpp>
#include <gba/inttype.hpp>
#include <gba/vectype.hpp>

#include <gba/bios/swi_call.hpp>

namespace gba::bios {
namespace detail {

    template <typename T>
    concept Arithmetic = (IsFixed<T> || std::integral<T>) && sizeof(T) <= 4;

} // namespace detail

[[nodiscard, gnu::const, gnu::always_inline]]
inline auto Div(int32 x, int32 y) noexcept {
    return swi<0x06, ivec2>(x, y);
}

[[nodiscard, gnu::const, gnu::always_inline]]
inline auto DivArm(int32 y, int32 x) noexcept {
    return swi<0x07, ivec2>(y, x);
}

[[nodiscard, gnu::const, gnu::always_inline]]
inline auto Sqrt(std::integral auto arg) noexcept {
    return swi<0x08, int32>(arg);
}

[[nodiscard, gnu::const, gnu::always_inline]]
inline auto ArcTan(fixed<18, 14> arg) noexcept {
    return swi<0x09, int32>(arg);
}

[[nodiscard, gnu::const, gnu::always_inline]]
inline auto ArcTan2(detail::Arithmetic auto x, detail::Arithmetic auto y) noexcept {
    return swi<0x0A, int32>(x, y);
}

} // namespace gba::bios

#endif // define GBAXX_BIOS_MATH_HPP
