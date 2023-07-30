/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_GBA_HPP
#define GBAXX_GBA_HPP

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

namespace {

    constexpr auto screen_width = 240;
    constexpr auto screen_height = 160;

}

} // namespace gba

#endif // define GBAXX_GBA_HPP
