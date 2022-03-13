/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_BIOS_SWI_CALL_HPP
#define GBAXX_BIOS_SWI_CALL_HPP

#include <cstddef>

namespace gba::bios {

template <std::size_t Swi, class Ret, class... Args>
#ifndef __clang__
[[gnu::optimize(1)]]
#endif
[[nodiscard, gnu::const, gnu::naked]]
inline auto swi(Args...) noexcept -> Ret {
    asm volatile (
#ifdef __thumb__
        "swi\t%[Swi]\n\t"
#else
        "swi\t%[Swi] << 16\n\t"
#endif
        "bx\tlr"
        :: [Swi]"i"(Swi)
    );
}

} // namespace gba::bios

#endif // define GBAXX_BIOS_SWI_CALL_HPP
