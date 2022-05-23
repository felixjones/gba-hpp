/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_REGISTERS_VIDEO_HPP
#define GBAXX_REGISTERS_VIDEO_HPP

#include <array>

#include <gba/fixedtype.hpp>
#include <gba/inttype.hpp>
#include <gba/ioregister.hpp>
#include <gba/vectype.hpp>

#include <gba/registers/accessors.hpp>
#include <gba/registers/video_types.hpp>

namespace gba::reg {

using dispcnt = read_write<dispcnt_type, 0x4000000>;

namespace undocumented {

    using greenswap = read_write<bool, 0x4000002>;

} // namespace undocumented

using dispstat = read_write<dispstat_type, 0x4000004>;

using vcount = read_only<uint16, 0x4000006>;

using bgcnt = read_write<std::array<bgcnt_type, 4>, 0x4000008>;
using bg0cnt = read_write<bgcnt_type, 0x4000008>;
using bg1cnt = read_write<bgcnt_type, 0x400000A>;
using bg2cnt = read_write<bgcnt_type, 0x400000C>;
using bg3cnt = read_write<bgcnt_type, 0x400000E>;

using bgofs = write_only<std::array<vec2<uint16>, 4>, 0x4000010>;
using bg0ofs = write_only<vec2<uint16>, 0x4000010>;
using bg1ofs = write_only<vec2<uint16>, 0x4000014>;
using bg2ofs = write_only<vec2<uint16>, 0x4000018>;
using bg3ofs = write_only<vec2<uint16>, 0x400001C>;

using bg0hofs = write_only<uint16, 0x4000010>;
using bg0vofs = write_only<uint16, 0x4000012>;
using bg1hofs = write_only<uint16, 0x4000014>;
using bg1vofs = write_only<uint16, 0x4000016>;
using bg2hofs = write_only<uint16, 0x4000018>;
using bg2vofs = write_only<uint16, 0x400001A>;
using bg3hofs = write_only<uint16, 0x400001C>;
using bg3vofs = write_only<uint16, 0x400001E>;

using bg2pa = write_only<make_fixed<7, int16>, 0x4000020>;
using bg2pb = write_only<make_fixed<7, int16>, 0x4000022>;
using bg2pc = write_only<make_fixed<7, int16>, 0x4000024>;
using bg2pd = write_only<make_fixed<7, int16>, 0x4000026>;

using bg2x = write_only<fixed<20, 8, int32>, 0x4000028>;
using bg2y = write_only<fixed<20, 8, int32>, 0x400002C>;

using bg3pa = write_only<make_fixed<7, int16>, 0x4000030>;
using bg3pb = write_only<make_fixed<7, int16>, 0x4000032>;
using bg3pc = write_only<make_fixed<7, int16>, 0x4000034>;
using bg3pd = write_only<make_fixed<7, int16>, 0x4000036>;

using bg3x = write_only<fixed<20, 8, int32>, 0x4000038>;
using bg3y = write_only<fixed<20, 8, int32>, 0x400003C>;

} // namespace gba::reg

#endif // define GBAXX_REGISTERS_VIDEO_HPP
