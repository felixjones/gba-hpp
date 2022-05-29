/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_REGISTERS_VIDEO_TYPES_HPP
#define GBAXX_REGISTERS_VIDEO_TYPES_HPP

#include <concepts>

#include <gba/fieldtype.hpp>
#include <gba/fixedtype.hpp>
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

struct alignas(uint8) win_type {
    bool bg0 : 1;
    bool bg1 : 1;
    bool bg2 : 1;
    bool bg3 : 1;
    bool obj : 1;
    bool bld : 1;
};

namespace win {

    static constexpr auto bg0 = field_of::boolean<win_type, 0>();
    static constexpr auto bg1 = field_of::boolean<win_type, 1>();
    static constexpr auto bg2 = field_of::boolean<win_type, 2>();
    static constexpr auto bg3 = field_of::boolean<win_type, 3>();
    static constexpr auto obj = field_of::boolean<win_type, 4>();
    static constexpr auto bld = field_of::boolean<win_type, 5>();

} // namespace win

struct alignas(uint32) mosaic_type {
    uint32 bh : 4;
    uint32 bv : 4;
    uint32 oh : 4;
    uint32 ov : 4;
};

namespace mosaic {

    static constexpr auto bh(std::integral auto i) noexcept {
        return field_of::integral<mosaic_type, 0, 0xf>(i);
    }

    static constexpr auto bv(std::integral auto i) noexcept {
        return field_of::integral<mosaic_type, 4, 0xf>(i);
    }

    static constexpr auto oh(std::integral auto i) noexcept {
        return field_of::integral<mosaic_type, 8, 0xf>(i);
    }

    static constexpr auto ov(std::integral auto i) noexcept {
        return field_of::integral<mosaic_type, 12, 0xf>(i);
    }

} // namespace mosaic

enum class blend_mode : uint8 {
    none,
    alpha,
    white,
    black
};

struct alignas(uint16) bld_type {
    bool top_bg0 : 1;
    bool top_bg1 : 1;
    bool top_bg2 : 1;
    bool top_bg3 : 1;
    bool top_obj : 1;
    bool top_backdrop : 1;
    blend_mode mode : 2;
    bool bot_bg0 : 1;
    bool bot_bg1 : 1;
    bool bot_bg2 : 1;
    bool bot_bg3 : 1;
    bool bot_obj : 1;
    bool bot_backdrop : 1;
};

namespace bld {

    static constexpr auto top_bg0 = field_of::boolean<bld_type, 0>();
    static constexpr auto top_bg1 = field_of::boolean<bld_type, 1>();
    static constexpr auto top_bg2 = field_of::boolean<bld_type, 2>();
    static constexpr auto top_bg3 = field_of::boolean<bld_type, 3>();
    static constexpr auto top_obj = field_of::boolean<bld_type, 4>();
    static constexpr auto top_backdrop = field_of::boolean<bld_type, 5>();

    static constexpr auto mode(blend_mode i) noexcept {
        return field_of::enum_class<bld_type, 6, blend_mode, blend_mode::black>(i);
    }

    static constexpr auto bot_bg0 = field_of::boolean<bld_type, 8>();
    static constexpr auto bot_bg1 = field_of::boolean<bld_type, 9>();
    static constexpr auto bot_bg2 = field_of::boolean<bld_type, 10>();
    static constexpr auto bot_bg3 = field_of::boolean<bld_type, 11>();
    static constexpr auto bot_obj = field_of::boolean<bld_type, 12>();
    static constexpr auto bot_backdrop = field_of::boolean<bld_type, 13>();

} // namespace bld

struct alignas(uint16) bldalpha_type {
    uint16 eva : 5;
    uint16 : 3;
    uint16 evb : 5;
};

namespace bldalpha {

    static constexpr auto eva(ufixed<1, 4, uint8> i) noexcept {
        return field_of::fixed<bldalpha_type, 0, decltype(i)>(i);
    }

    static constexpr auto evb(ufixed<1, 4, uint8> i) noexcept {
        return field_of::fixed<bldalpha_type, 8, decltype(i)>(i);
    }

    static consteval auto make(std::floating_point auto v) noexcept {
        return eva(v) | evb(1.0 - v);
    }

    static constexpr auto make(IsFixed auto v) noexcept {
        return eva(v) | evb(1 - v);
    }

} // namespace bldalpha
} // namespace gba

#endif // define GBAXX_REGISTERS_VIDEO_TYPES_HPP
