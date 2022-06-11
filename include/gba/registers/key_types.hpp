/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_REGISTERS_KEY_TYPES_HPP
#define GBAXX_REGISTERS_KEY_TYPES_HPP

#include <bit>

#include <gba/fieldtype.hpp>
#include <gba/inttype.hpp>

namespace gba {

enum class keycnt_condition : uint8 {
    _or,
    _and
};

struct alignas(uint16) keycnt_type {
    bool a : 1;
    bool b : 1;
    bool select : 1;
    bool start : 1;
    bool right : 1;
    bool left : 1;
    bool up : 1;
    bool down : 1;
    bool r : 1;
    bool l : 1;
    uint16 : 4;
    bool irq : 1;
    keycnt_condition condition : 1;
};

namespace keycnt {

    static constexpr auto a = field_of::boolean<keycnt_type, 0>();
    static constexpr auto b = field_of::boolean<keycnt_type, 1>();
    static constexpr auto select = field_of::boolean<keycnt_type, 2>();
    static constexpr auto start = field_of::boolean<keycnt_type, 3>();
    static constexpr auto right = field_of::boolean<keycnt_type, 4>();
    static constexpr auto left = field_of::boolean<keycnt_type, 5>();
    static constexpr auto up = field_of::boolean<keycnt_type, 6>();
    static constexpr auto down = field_of::boolean<keycnt_type, 7>();
    static constexpr auto r = field_of::boolean<keycnt_type, 8>();
    static constexpr auto l = field_of::boolean<keycnt_type, 9>();
    static constexpr auto irq = field_of::boolean<keycnt_type, 14>();

    static constexpr auto condition(keycnt_condition i) noexcept {
        return field_of::enum_class<keycnt_type, 15, keycnt_condition, keycnt_condition::_and>(i);
    }

} // namespace keycnt

struct alignas(uint16) key_mask {
    bool a : 1;
    bool b : 1;
    bool select : 1;
    bool start : 1;
    bool right : 1;
    bool left : 1;
    bool up : 1;
    bool down : 1;
    bool r : 1;
    bool l : 1;
};

namespace key {

    static constexpr auto a = field_of::boolean<key_mask, 0>();
    static constexpr auto b = field_of::boolean<key_mask, 1>();
    static constexpr auto select = field_of::boolean<key_mask, 2>();
    static constexpr auto start = field_of::boolean<key_mask, 3>();
    static constexpr auto right = field_of::boolean<key_mask, 4>();
    static constexpr auto left = field_of::boolean<key_mask, 5>();
    static constexpr auto up = field_of::boolean<key_mask, 6>();
    static constexpr auto down = field_of::boolean<key_mask, 7>();
    static constexpr auto r = field_of::boolean<key_mask, 8>();
    static constexpr auto l = field_of::boolean<key_mask, 9>();

} // namespace key
} // namespace gba

#endif // define GBAXX_REGISTERS_KEY_TYPES_HPP
