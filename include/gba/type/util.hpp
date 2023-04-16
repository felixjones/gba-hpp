/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_UTIL_HPP
#define GBAXX_TYPE_UTIL_HPP

#include <concepts>

#include <gba/type/vector.hpp>

namespace gba {

    template <typename T>
    concept Fundamental = std::is_fundamental_v<T> || Vector<T>;

    template <typename T>
    consteval auto round(std::floating_point auto x) {
        return T(x + decltype(x)(0.5));
    }

    template <std::make_signed_t<std::size_t> Sh>
    constexpr auto shift_right(Fundamental auto x) noexcept {
        if constexpr (Sh < 0) {
            return x << -Sh;
        } else {
            return x >> Sh;
        }
    }

} // namespace gba

#endif // define GBAXX_TYPE_UTIL_HPP
