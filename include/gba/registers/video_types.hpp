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
    uint16 : 2;
    uint16 vct : 8;
};

namespace dispstat {

    static constexpr auto vbl_irq = field_of::boolean<dispstat_type, 3>();
    static constexpr auto hbl_irq = field_of::boolean<dispstat_type, 4>();
    static constexpr auto vct_irq = field_of::boolean<dispstat_type, 5>();

    static constexpr auto vct(std::integral auto i) noexcept {
        return field_of::integral<dispstat_type, 8, 0xff>(i);
    }

} // namespace dispstat

struct alignas(uint16) bgcnt_type {
    uint16 prio : 2;
    uint16 cbb : 2;
    uint16 : 2;
    bool mosaic : 1;
    bool _8bpp : 1;
    uint16 sbb : 5;
    bool wrap : 1;
    uint16 size : 2;
};

namespace bgcnt {

    static constexpr auto mosaic = field_of::boolean<bgcnt_type, 6>();
    static constexpr auto _8bpp = field_of::boolean<bgcnt_type, 7>();
    static constexpr auto wrap = field_of::boolean<bgcnt_type, 13>();

    static constexpr auto prio(std::integral auto i) noexcept {
        return field_of::integral<bgcnt_type, 0, 0x3>(i);
    }

    static constexpr auto cbb(std::integral auto i) noexcept {
        return field_of::integral<bgcnt_type, 2, 0x3>(i);
    }

    static constexpr auto sbb(std::integral auto i) noexcept {
        return field_of::integral<bgcnt_type, 8, 0x1f>(i);
    }

    static constexpr auto size(std::integral auto i) noexcept {
        return field_of::integral<bgcnt_type, 14, 0x3>(i);
    }

} // namespace bgcnt

} // namespace gba

#endif // define GBAXX_REGISTERS_VIDEO_TYPES_HPP
