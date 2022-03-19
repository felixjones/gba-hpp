/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_MATH_HPP
#define GBAXX_MATH_HPP

#include <type_traits>

#include <gba/fixedtype.hpp>

namespace gba {

template <class Lhs, class Rhs> requires is_fixed<Lhs> && is_fixed<Rhs>
constexpr auto nextafter(Lhs from, Rhs to) noexcept {
    using promote_type = fixed_promote_t<Lhs, Rhs>;

    auto f = promote_type(from);
    const auto t = promote_type(to);

    const auto dir = t.data() - f.data();
    if (dir > 0) {
        f.data() += 1;
    } else if (dir < 0) {
        f.data() -= 1;
    }
    return f;
}

template <class Lhs, class Rhs> requires std::is_integral_v<Lhs> && is_fixed<Rhs>
constexpr auto nextafter(Lhs from, Rhs to) noexcept {
    using promote_type = fixed_promote_t<Rhs>;

    auto f = promote_type(from);
    const auto t = promote_type(to);

    const auto dir = t.data() - f.data();
    if (dir > 0) {
        f.data() += 1;
    } else if (dir < 0) {
        f.data() -= 1;
    }
    return f;
}

} // namespace gba

#endif // define GBAXX_MATH_HPP
