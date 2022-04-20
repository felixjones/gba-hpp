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

template <class Lhs, class Rhs> requires IsFixed<Lhs> && IsFixed<Rhs>
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

template <class Lhs, class Rhs> requires std::is_integral_v<Lhs> && IsFixed<Rhs>
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

template <class T> requires IsFixed<T>
constexpr auto floor(T x) noexcept {
    constexpr auto spare_bits = std::numeric_limits<std::make_unsigned_t<typename T::data_type>>::digits - (T::integer_bits + T::fractional_bits);

    return static_cast<typename T::data_type>((x.data() << spare_bits) >> (T::fractional_bits + spare_bits));
}

template <class T> requires IsFixed<T>
constexpr auto ceil(T x) noexcept {
    constexpr auto spare_bits = std::numeric_limits<std::make_unsigned_t<typename T::data_type>>::digits - (T::integer_bits + T::fractional_bits);

    const auto floor = static_cast<typename T::data_type>((x.data() << spare_bits) >> (T::fractional_bits + spare_bits));

    constexpr auto fractional_mask = (1 << T::fractional_bits) - 1;
    if ((x.data() & fractional_mask) != 0) {
        return floor + 1;
    }

    return floor;
}

template <class T> requires IsFixed<T>
constexpr auto round(T x) noexcept {
    constexpr auto half = T{0.5};

    return floor(x + half);
}

template <class T> requires IsFixed<T>
constexpr auto abs(T x) noexcept {
    if constexpr (T::is_signed) {
        if (x < 0) {
            return 0 - x;
        }
    }
    return x;
}

template <class T> requires IsFixed<T>
constexpr auto mod(T x, std::integral auto* iptr) noexcept {
    // Attempt to return fraction within a 32-bit word
    using result_data_type = std::conditional_t<binary_digits<typename T::data_type> >= 32, typename T::data_type, copysign<int32, typename T::sign>>;
    using result_type = fixed<T::is_signed ? 1 : 0, T::fractional_bits, result_data_type>;

    constexpr auto fractional_mask = (1LL << T::fractional_bits) - 1;

    *iptr = static_cast<std::remove_pointer_t<decltype(iptr)>>(x);

    if constexpr (T::is_signed) {
        if (x < 0) {
            constexpr auto sign_bits = std::numeric_limits<std::make_unsigned_t<typename T::data_type>>::digits - T::fractional_bits;
            constexpr auto integer_mask = ((1LL << sign_bits) - 1) << T::fractional_bits;

            return result_type::from_data(integer_mask | ((x.data() << sign_bits) >> sign_bits));
        }
    }

    return result_type::from_data(x.data() & fractional_mask);
}

} // namespace gba

#endif // define GBAXX_MATH_HPP
