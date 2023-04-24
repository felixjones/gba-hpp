/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_HPP
#define GBAXX_TYPE_HPP

#include <gba/type/angle.hpp>
#include <gba/type/fixed.hpp>
#include <gba/type/int.hpp>
#include <gba/type/lut.hpp>
#include <gba/type/memory.hpp>
#include <gba/type/vector.hpp>

namespace gba {

    using u8 = uinttype<8>;
    using u8x2 = make_vector<u8, 2>;

    using u16 = uinttype<16>;

    using u32 = uinttype<32>;

} // namespace gba

#endif // define GBAXX_TYPE_HPP
