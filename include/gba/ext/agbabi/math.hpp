/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_MATH_HPP
#define GBAXX_EXT_AGBABI_MATH_HPP

#include <agbabi.h>

#include <gba/fixedtype.hpp>
#include <gba/inttype.hpp>

namespace gba::agbabi {
namespace detail {

template <typename T>
concept UnsignedIntegral64 = sizeof(T) == 8 && std::is_unsigned_v<T>;

template <typename T>
concept UnsignedIntegral32 = sizeof(T) == 4 && std::is_unsigned_v<T>;

} // namespace detail

[[nodiscard, gnu::const]]
inline auto divides(detail::UnsignedIntegral64 auto lhs, detail::UnsignedIntegral32 auto rhs) noexcept {
    return __agbabi_uluidiv(lhs, rhs);
}

[[nodiscard, gnu::const]]
inline auto modulus(detail::UnsignedIntegral64 auto lhs, detail::UnsignedIntegral32 auto rhs) noexcept {
    return __agbabi_uluidivmod(lhs, rhs)[1];
}

[[nodiscard, gnu::const]]
inline auto sin(std::integral auto arg) noexcept {
    return fixed<2, 29>::from_data(__agbabi_sin(arg));
}

[[nodiscard, gnu::const]]
inline auto cos(std::integral auto arg) noexcept {
    return fixed<2, 29>::from_data(__agbabi_sin(arg + 0x2000));
}

[[nodiscard, gnu::const, gnu::always_inline]]
inline auto sqrt(std::integral auto arg) noexcept {
    return __agbabi_sqrt(arg);
}

[[nodiscard, gnu::const, gnu::always_inline]]
inline auto atan2(std::integral auto x, std::integral auto y) noexcept {
    return __agbabi_atan2(x, y);
}

[[nodiscard, gnu::const, gnu::always_inline]]
inline auto atan2(IsFixed auto x, IsFixed auto y) noexcept {
    return __agbabi_atan2(x.data(), y.data());
}

} // namespace gba::agbabi

#endif // define GBAXX_EXT_AGBABI_MATH_HPP
