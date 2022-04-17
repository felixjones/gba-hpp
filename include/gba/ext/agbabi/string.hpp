/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_STRING_HPP
#define GBAXX_EXT_AGBABI_STRING_HPP

#include <string>

#include <agbabi.h>

#include <gba/fixedtype.hpp>

namespace gba::agbabi {

inline auto to_string(IsFixed auto val) noexcept -> std::string {
    char buffer[40];
    if constexpr (std::is_signed_v<typename decltype(val)::sign>) {
        __agbabi_fixed_tostr(val.data(), buffer, decltype(val)::fractional_bits);
    } else {
        __agbabi_ufixed_tostr(val.data(), buffer, decltype(val)::fractional_bits);
    }
    return buffer;
}

} // namespace gba::agbabi

#endif // define GBAXX_EXT_AGBABI_STRING_HPP
