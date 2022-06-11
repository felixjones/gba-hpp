/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_REGISTERS_IRQ_HPP
#define GBAXX_REGISTERS_IRQ_HPP

#include <gba/registers/accessors.hpp>
#include <gba/registers/irq_types.hpp>

namespace gba::reg {

using ime = read_write<bool, 0x4000208>;

using ie = read_write<irq_type, 0x4000200>;
using _if = read_write<irq_type, 0x4000202>;

} // namespace gba::reg

#endif // define GBAXX_REGISTERS_IRQ_HPP
