/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_HPP
#define GBAXX_TYPE_HPP
/** @file */

#include <gba/type/angle.hpp>
#include <gba/type/fixed.hpp>
#include <gba/type/int.hpp>
#include <gba/type/lut.hpp>
#include <gba/type/memory.hpp>
#include <gba/type/tile.hpp>
#include <gba/type/vector.hpp>

namespace gba {

    /**
     * @brief 8-bit unsigned type.
     */
    using u8 = uinttype<8>;

    /**
     * @brief 16-bit unsigned type.
     */
    using u16 = uinttype<16>;

    /**
     * @brief 32-bit unsigned type.
     */
    using u32 = uinttype<32>;

    // Vectors

    /**
     * @brief GNU vector of two 8-bit unsigned types.
     */
    using u8x2 = make_vector<u8, 2>;

    /**
     * @brief GNU vector of four 8-bit unsigned types.
     *
     * @note This fits perfectly within a single 32-bit register.
     */
    using u8x4 = make_vector<u8, 4>;

    /**
     * @brief GNU vector of two 16-bit unsigned types.
     *
     * @note This fits perfectly within a single 32-bit register.
     */
    using u16x2 = make_vector<u16, 2>;

    /**
     * @brief GNU vector of two 32-bit unsigned types.
     *
     * @note This fits across two 32-bit registers.
     */
    using u32x2 = make_vector<u32, 2>;

} // namespace gba

#endif // define GBAXX_TYPE_HPP
