/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_BIOS_CHECKSUM_HPP
#define GBAXX_BIOS_CHECKSUM_HPP

#include <gba/inttype.hpp>

#include <gba/bios/swi_call.hpp>

namespace gba::bios::undocumented {

[[nodiscard, gnu::const, gnu::always_inline]]
inline auto GetBiosChecksum() noexcept {
    return swi<0x06, int32>();
}

} // namespace gba::bios::undocumented

#endif // define GBAXX_BIOS_CHECKSUM_HPP
