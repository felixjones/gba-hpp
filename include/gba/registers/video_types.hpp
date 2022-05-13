/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_REGISTERS_VIDEO_TYPES_HPP
#define GBAXX_REGISTERS_VIDEO_TYPES_HPP

#include <gba/fieldtype.hpp>
#include <gba/inttype.hpp>

namespace gba {

struct alignas(uint16) dispcnt_type {
    uint16 mode : 3;
    bool gb : 1;
    uint16 page : 1;
    bool oam_hbl : 1;
    bool obj_1d : 1;
    bool blank : 1;
    bool bg0 : 1;
    bool bg1 : 1;
    bool bg2 : 1;
    bool bg3 : 1;
    bool obj : 1;
    bool win0 : 1;
    bool win1 : 1;
    bool winobj : 1;
};

namespace dispcnt {

    static constexpr auto mode(std::integral auto i) noexcept {
        return field_of::integral<dispcnt_type, 0, 0x7>(i);
    }

    static constexpr auto page(std::integral auto i) noexcept {
        return field_of::integral<dispcnt_type, 4, 0x1>(i);
    }

    static constexpr auto oam_hbl = field_of::boolean<dispcnt_type, 5>();
    static constexpr auto obj_1d = field_of::boolean<dispcnt_type, 6>();
    static constexpr auto blank = field_of::boolean<dispcnt_type, 7>();
    static constexpr auto bg0 = field_of::boolean<dispcnt_type, 8>();
    static constexpr auto bg1 = field_of::boolean<dispcnt_type, 9>();
    static constexpr auto bg2 = field_of::boolean<dispcnt_type, 10>();
    static constexpr auto bg3 = field_of::boolean<dispcnt_type, 11>();
    static constexpr auto obj = field_of::boolean<dispcnt_type, 12>();
    static constexpr auto win0 = field_of::boolean<dispcnt_type, 13>();
    static constexpr auto win1 = field_of::boolean<dispcnt_type, 14>();
    static constexpr auto winobj = field_of::boolean<dispcnt_type, 15>();

} // namespace dispcnt

struct alignas(uint16) dispstat_type {
    bool in_vbl : 1;
    bool in_hbl : 1;
    bool in_vct : 1;
    bool vbl_irq : 1;
    bool hbl_irq : 1;
    bool vct_irq : 1;
    uint8 : 2;
    uint8 vct : 8;
};

namespace dispstat {

    static constexpr auto vbl_irq = field_of::boolean<dispstat_type, 3>();
    static constexpr auto hbl_irq = field_of::boolean<dispstat_type, 4>();
    static constexpr auto vct_irq = field_of::boolean<dispstat_type, 5>();

    static constexpr auto vct(std::integral auto i) noexcept {
        return field_of::integral<dispstat_type, 8, 0xff>(i);
    }

} // namespace dispstat
} // namespace gba

#endif // define GBAXX_REGISTERS_VIDEO_TYPES_HPP
