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

#include <gba/fieldtype.hpp>
#include <gba/fixedtype.hpp>
#include <gba/inttype.hpp>
#include <gba/ioregister.hpp>
#include <gba/math.hpp>
#include <gba/mattype.hpp>
#include <gba/tupletype.hpp>
#include <gba/vectype.hpp>

#include <gba/bios/checksum.hpp>
#include <gba/bios/cpuset.hpp>
#include <gba/bios/decompress.hpp>
#include <gba/bios/halt.hpp>
#include <gba/bios/math.hpp>
#include <gba/bios/reset.hpp>
#include <gba/bios/swi_call.hpp>

#include <gba/ext/agbabi.hpp>

#include <gba/interrupt/handler.hpp>

#include <gba/registers/accessors.hpp>
#include <gba/registers/dma.hpp>
#include <gba/registers/dma_types.hpp>
#include <gba/registers/irq.hpp>
#include <gba/registers/irq_types.hpp>
#include <gba/registers/key.hpp>
#include <gba/registers/key_types.hpp>
#include <gba/registers/system.hpp>
#include <gba/registers/system_types.hpp>
#include <gba/registers/timer.hpp>
#include <gba/registers/timer_types.hpp>
#include <gba/registers/video.hpp>
#include <gba/registers/video_types.hpp>

#include <gba/util/array_traits.hpp>
#include <gba/util/bit_container.hpp>
#include <gba/util/byte_array.hpp>
#include <gba/util/constexpr.hpp>
#include <gba/util/diff_filter.hpp>
#include <gba/util/huff_compress.hpp>
#include <gba/util/lz77_compress.hpp>
#include <gba/util/rle_compress.hpp>
#include <gba/util/tiles.hpp>
#include <gba/util/tuple_traits.hpp>

#include <gba/video/modes.hpp>
#include <gba/video/palettes.hpp>

#endif // define GBAXX_GBA_HPP
