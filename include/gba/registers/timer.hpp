/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_REGISTERS_TIMER_HPP
#define GBAXX_REGISTERS_TIMER_HPP

#include <gba/registers/accessors.hpp>
#include <gba/registers/timer_types.hpp>

namespace gba::reg {

using tm0cnt_l = read_write<uint16, 0x4000100>;
using tm1cnt_l = read_write<uint16, 0x4000104>;
using tm2cnt_l = read_write<uint16, 0x4000108>;
using tm3cnt_l = read_write<uint16, 0x400010C>;

using tm0cnt_h = read_write<timer_type, 0x4000102>;
using tm1cnt_h = read_write<timer_type, 0x4000106>;
using tm2cnt_h = read_write<timer_type, 0x400010A>;
using tm3cnt_h = read_write<timer_type, 0x400010E>;

using tm0cnt = read_write<tmrec_type, 0x4000100>;
using tm1cnt = read_write<tmrec_type, 0x4000104>;
using tm2cnt = read_write<tmrec_type, 0x4000108>;
using tm3cnt = read_write<tmrec_type, 0x400010C>;

using tmcnt = read_write<std::array<tmrec_type, 4>, 0x4000100>;

} // namespace gba::reg

#endif // define GBAXX_REGISTERS_TIMER_HPP
