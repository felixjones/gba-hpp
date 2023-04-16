/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_MATH_HPP
#define GBAXX_EXT_AGBABI_MATH_HPP

#include <agbabi.h>

#include <gba/type.hpp>

namespace gba::agbabi {

namespace {

    auto sin(Angle auto bam) noexcept {
        return fixed<int, 29>::from_data(__agbabi_sin(angle<int, 15>(bam).data()));
    }

}

} // namespace gba::agbabi

#endif // define GBAXX_EXT_AGBABI_MATH_HPP
