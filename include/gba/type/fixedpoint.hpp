/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_FIXEDPOINT_HPP
#define GBAXX_TYPE_FIXEDPOINT_HPP

#include <concepts>

namespace gba {

    template <std::size_t Int, std::size_t Frac, std::integral T>
    struct fixedpoint {
        T data;
    };

    template <std::size_t E>
    using make_fixed = fixedpoint<32 - E, E, int>;

} // namespace gba

#endif // define GBAXX_TYPE_FIXEDPOINT_HPP
