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

    [[gnu::const]]
    auto sin(Angle auto bam) noexcept {
        return fixed<int, 29>::from_data(__agbabi_sin(angle<int, 15>(bam).data()));
    }

    [[gnu::const]]
    auto cos(Angle auto bam) noexcept {
        return fixed<int, 29>::from_data(__agbabi_sin(angle<int, 15>(bam).data() + 0x2000));
    }

    [[gnu::const]]
    auto atan2(Fixed auto x, Fixed auto y) noexcept {
        return angle<int, 15>(__agbabi_atan2(fixed<int, 12>(x).data(), fixed<int, 12>(y).data()));
    }

    [[gnu::const]]
    auto sqrt(Fixed auto x) noexcept {
        using fixed_type = decltype(x);
        using unsigned_type = std::make_unsigned_t<typename fixed_type::data_type>;

        static constexpr auto odd_bit = fixed_type::exp % 2;

        return fixed<typename fixed_type::data_type, (fixed_type::exp + odd_bit) / 2>::from_data(__agbabi_sqrt(unsigned_type(x.data() << odd_bit)));
    }

}

} // namespace gba::agbabi

#endif // define GBAXX_EXT_AGBABI_MATH_HPP
