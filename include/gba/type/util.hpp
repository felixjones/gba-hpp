/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_UTIL_HPP
#define GBAXX_TYPE_UTIL_HPP

#include <concepts>
#include <cstdint>

#include <gba/type/vector.hpp>

namespace gba {

    template <typename T>
    concept Fundamental = std::is_fundamental_v<T> || Vector<T>;

    template <typename T>
    consteval auto round(std::floating_point auto x) {
        return T(x + decltype(x)(0.5));
    }

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

    template <std::int64_t From, std::int64_t To>
    constexpr auto shift_to(Fundamental auto x) noexcept {
        return shift_right<From - To>(x);
    }

    template <typename Sign, typename Target>
    using copy_sign = std::conditional_t<std::is_signed_v<Sign>,
                std::make_signed_t<Target>,
                std::make_unsigned_t<Target>
            >;

    template <typename T>
    struct make_bigger;

    template <std::integral T>
    struct make_bigger<T> {
        using type = copy_sign<T, std::conditional_t<std::is_same_v<std::make_signed_t<T>, char>,
                short,
                std::conditional_t<std::is_same_v<std::make_signed_t<T>, short>,
                        int,
                        long long // No larger integer type
                >>>;
    };

    template <Vector T>
    struct make_bigger<T> {
        using type = make_vector<typename make_bigger<typename vector_traits<T>::value_type>::type, vector_traits<T>::size>;
    };

} // namespace gba

#endif // define GBAXX_TYPE_UTIL_HPP
