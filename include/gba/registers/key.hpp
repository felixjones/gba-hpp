/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_REGISTERS_KEY_HPP
#define GBAXX_REGISTERS_KEY_HPP

#include <gba/ioregister.hpp>

#include <gba/registers/accessors.hpp>
#include <gba/registers/key_types.hpp>

namespace gba::reg {

    using keyinput = read_only<key_type, 0x4000130>;
    using keycnt = read_only<key_type, 0x4000130>;

} // namespace gba::reg

#endif // define GBAXX_REGISTERS_KEY_HPP
