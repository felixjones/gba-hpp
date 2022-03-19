/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_GBA_HPP
#define GBAXX_GBA_HPP

/**
 * Major version
 */
#define GBAXX_VERSION_MAJOR 4
/**
 * Minor version
 */
#define GBAXX_VERSION_MINOR 0
/**
 * Patch version
 */
#define GBAXX_VERSION_PATCH 0

/**
 * Combined version number
 *
 * Calculated as:
 * \code
 * ( GBAXX_VERSION_MAJOR * 10000 ) + ( GBAXX_VERSION_MINOR * 100 ) + ( GBAXX_VERSION_PATCH * 1 )
 * \endcode
 */
#define GBAXX_VERSION ((GBAXX_VERSION_MAJOR * 10000) + (GBAXX_VERSION_MINOR * 100) + GBAXX_VERSION_PATCH)

#include <gba/fixedtype.hpp>
#include <gba/inttype.hpp>
#include <gba/math.hpp>
#include <gba/vectype.hpp>

#include <gba/bios/math.hpp>
#include <gba/bios/swi_call.hpp>

#endif // define GBAXX_GBA_HPP
