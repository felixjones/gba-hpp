/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_LUT_HPP
#define GBAXX_TYPE_LUT_HPP

#include <array>

#include <gba/type/angle.hpp>
#include <gba/type/util.hpp>

namespace gba::lut {

    template <std::size_t N, class Generator>
    consteval auto make(Generator g) -> std::array<decltype(g(std::size_t{})), N> {
        auto result = std::array<decltype(g(std::size_t{})), N>{};
        for (std::size_t ii = 0; ii < N; ++ii) {
            result[ii] = g(ii);
        }
        return result;
    }

    template <typename T, Angle U>
    struct angle_array : public std::array<T, 1 << U::bits> {
        template <Angle V>
        constexpr T operator[](V i) const noexcept {
            constexpr auto mask = (1 << U::bits) - 1;

            return this->data()[shift_to<V::bits, U::bits>(i.data()) & mask];
        }
    };

    template <Angle T, class Generator>
    consteval auto make(Generator g) {
        constexpr auto size = 1 << T::bits;
        using result_type = decltype(g(T(), size));

        auto result = std::array<result_type, size>{};
        for (std::size_t ii = 0; ii < size; ++ii) {
            result[ii] = g(T(ii), size);
        }
        return angle_array<result_type, T>{result};
    }

} // namespace gba::lut

#endif // define GBAXX_TYPE_LUT_HPP
