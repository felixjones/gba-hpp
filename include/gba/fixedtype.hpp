/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_FIXEDTYPE_HPP
#define GBAXX_FIXEDTYPE_HPP

#include <cstddef>

#include <gba/inttype.hpp>

namespace gba {

template <std::size_t IntBits, std::size_t FracBits, typename Sign = signed>
class fixed {
private:
    using data_type = inttype<IntBits + FracBits, Sign>;

    data_type m_data = {};
public:
    template <typename T>
    consteval fixed(T v) requires std::is_floating_point_v<T> : m_data { static_cast<data_type>(v * (1 << FracBits) + 0.5) } {}
};

template <std::size_t Integer, std::size_t Fractional, typename Sign = unsigned>
using ufixed = fixed<Integer, Fractional, Sign>;

} // namespace gba

#endif // define GBAXX_FIXEDTYPE_HPP
