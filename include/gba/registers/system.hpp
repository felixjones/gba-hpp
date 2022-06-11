/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_REGISTERS_SYSTEM_HPP
#define GBAXX_REGISTERS_SYSTEM_HPP

#include <gba/registers/accessors.hpp>
#include <gba/registers/system_types.hpp>

namespace gba::reg {

using waitcnt = read_write<waitcnt_type, 0x4000204>;

namespace undocumented {

    using postflg = read_write<bool, 0x4000300>;
    using wramcnt = read_write<gba::undocumented::wramcnt_type, 0x4000800>;

} // namespace undocuments

} // namespace gba::reg

#endif // define GBAXX_REGISTERS_SYSTEM_HPP
