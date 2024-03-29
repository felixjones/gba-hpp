/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_TILE_HPP
#define GBAXX_TYPE_TILE_HPP
/** @file */

#include <gba/type/int.hpp>

namespace gba {

    /**
     * @struct tile4bpp
     * @brief 4bpp 8x8 tile.
     */
    struct tile4bpp {
        u4x2 data[32];
    };

    /**
     * @struct tile8bpp
     * @brief 8bpp 8x8 tile.
     */
    struct tile8bpp {
        uinttype<8> data[64];
    };

} // namespace gba

#endif // define GBAXX_TYPE_TILE_HPP
