/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_HPP
#define GBAXX_TYPE_HPP

#include <gba/type/fixedpoint.hpp>
#include <gba/type/memory.hpp>

namespace gba {

    using u8x2 [[gnu::vector_size(sizeof(unsigned char) * 2)]] = unsigned char;

} // namespace gba

#endif // define GBAXX_TYPE_HPP
