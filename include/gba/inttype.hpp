/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_INTTYPE_HPP
#define GBAXX_INTTYPE_HPP

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace gba {

template <typename T, typename Sign>
using copysign = std::conditional_t<std::is_signed_v<Sign>, std::make_signed_t<T>, std::make_unsigned_t<T>>;

template <typename T> requires std::integral<T>
constexpr auto binary_digits = static_cast<std::size_t>(std::numeric_limits<T>::digits + std::is_signed_v<T>);

template <std::size_t MinBits, typename Sign = signed>
using inttype = copysign<
    typename std::conditional_t<MinBits == 0, void,
        typename std::conditional_t<MinBits <= 8, std::int8_t,
            typename std::conditional_t<MinBits <= 16, std::int16_t,
                typename std::conditional_t<MinBits <= 32, std::int32_t,
                    typename std::conditional_t<MinBits <= 64, std::int64_t,
                        void
                    >
                >
            >
        >
    >, Sign>;

template <std::size_t Bits, typename Sign = unsigned>
using uinttype = inttype<Bits, Sign>;

using int8 = inttype<8>;
using int16 = inttype<16>;
using int32 = inttype<32>;
using int64 = inttype<64>;

using uint8 = uinttype<8>;
using uint16 = uinttype<16>;
using uint32 = uinttype<32>;
using uint64 = uinttype<64>;

template <typename Type, typename Sign = signed>
using intcontainer = inttype<sizeof(Type) * 8, Sign>;

template <typename Type, typename Sign = unsigned>
using uintcontainer = uinttype<sizeof(Type) * 8, Sign>;

template <class T, std::size_t Bits>
concept BinaryDigits = std::is_same_v<T, inttype<Bits, T>>;

constexpr auto uint_bit_mask(std::unsigned_integral auto x) noexcept {
    static_assert(sizeof(x) <= 8);
    constexpr auto bits = binary_digits<decltype(x)>;

    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);

    if constexpr (bits >= 16) {
        x |= (x >> 8);
    }

    if constexpr (bits >= 32) {
        x |= (x >> 16);
    }

    if constexpr (bits >= 64) {
        x |= (x >> 32);
    }

    return x;
}

} // namespace gba

#endif // define GBAXX_INTTYPE_HPP
