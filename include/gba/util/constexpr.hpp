/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_UTIL_CONSTEXPR_HPP
#define GBAXX_UTIL_CONSTEXPR_HPP

#include <array>
#include <cstddef>
#include <type_traits>

namespace gba::util {

template <auto Data>
inline constexpr const auto& make_static = Data;

} // namespace gba::util

#endif // define GBAXX_UTIL_CONSTEXPR_HPP
