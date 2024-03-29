/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_GBA_HPP
#define GBAXX_GBA_HPP
/** @file */

#include <gba/mmio.hpp>
#include <gba/type.hpp>

#include <gba/bios/compress.hpp>
#include <gba/bios/halt.hpp>
#include <gba/bios/math.hpp>
#include <gba/bios/misc.hpp>
#include <gba/bios/sound.hpp>

#include <gba/ext/agbabi/agbabi.hpp>
#include <gba/ext/mgba/log.hpp>

#include <gba/input/keyhelper.hpp>

namespace gba {

    /**
     * @brief Constant 240 representing the horizontal resolution of the GBA.
     *
     * @tparam T Desired value type.
     *
     * @sa screen_height_v
     */
    template <typename T>
    inline constexpr auto screen_width_v = T(240);

    /**
     * @brief Constant 160 representing the vertical resolution of the GBA.
     *
     * @tparam T Desired value type.
     *
     * @sa screen_width_v
     */
    template <typename T>
    inline constexpr auto screen_height_v = T(160);

    /**
     * @brief Constant 240 representing the horizontal resolution of the GBA.
     *
     * @sa screen_height
     */
    inline constexpr auto screen_width = screen_width_v<int>;

    /**
     * @brief Constant 160 representing the vertical resolution of the GBA.
     *
     * @sa screen_width
     */
    inline constexpr auto screen_height = screen_height_v<int>;

} // namespace gba

#endif // define GBAXX_GBA_HPP
