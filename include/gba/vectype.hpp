/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_VECTYPE_HPP
#define GBAXX_VECTYPE_HPP

#include <bit>
#include <cstddef>
#include <cstdint>

namespace gba {

template <std::size_t X>
concept IsPowerOfTwo = (X == 0) || std::has_single_bit(X);

template <typename T, std::size_t N> requires IsPowerOfTwo<N>
using vec [[gnu::vector_size(sizeof(T) * N)]] = T;

template <typename T>
using vec2 = vec<T, 2>;

template <typename T>
using vec4 = vec<T, 4>;

using ivec2 = vec2<std::int32_t>;
using ivec4 = vec4<std::int32_t>;

using uvec2 = vec2<std::uint32_t>;
using uvec4 = vec4<std::uint32_t>;

} // namespace gba

#endif // define GBAXX_VECTYPE_HPP
