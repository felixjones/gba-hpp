/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_REGISTERS_SYSTEM_TYPES_HPP
#define GBAXX_REGISTERS_SYSTEM_TYPES_HPP

#include <gba/fieldtype.hpp>
#include <gba/inttype.hpp>

namespace gba {

enum class phi : uint8 {
    off,
    div_4,
    div_2,
    div_1
};

struct alignas(uint16) waitcnt_type {
    uint16 sram : 2;
    uint16 rom0_n : 2;
    uint16 rom0_s : 1;
    uint16 rom1_n : 2;
    uint16 rom1_s : 1;
    uint16 rom2_n : 2;
    uint16 rom2_s : 1;
    gba::phi phi : 2;
    uint16 : 3;
    bool prefetch : 1;
};

namespace waitcnt {

    static constexpr auto sram(std::integral auto i) noexcept {
        return field_of::integral<waitcnt_type, 0, 0x3>(i);
    }

    static constexpr auto rom0_n(std::integral auto i) noexcept {
        return field_of::integral<waitcnt_type, 2, 0x3>(i);
    }

    static constexpr auto rom0_s(std::integral auto i) noexcept {
        return field_of::integral<waitcnt_type, 4, 0x1>(i);
    }

    static constexpr auto rom1_n(std::integral auto i) noexcept {
        return field_of::integral<waitcnt_type, 5, 0x3>(i);
    }

    static constexpr auto rom1_s(std::integral auto i) noexcept {
        return field_of::integral<waitcnt_type, 7, 0x1>(i);
    }

    static constexpr auto rom2_n(std::integral auto i) noexcept {
        return field_of::integral<waitcnt_type, 8, 0x3>(i);
    }

    static constexpr auto rom2_s(std::integral auto i) noexcept {
        return field_of::integral<waitcnt_type, 10, 0x1>(i);
    }

    static constexpr auto prefetch = field_of::boolean<waitcnt_type, 14>();

} // namespace waitcnt

namespace undocumented {

    struct alignas(uint32) wramcnt_type {
        bool map_bios : 1;
        uint32 : 4;
        bool enable_wram : 1;
        uint32 : 18;
        uint32 wait_state : 4;
    };

    namespace wramcnt {

        static constexpr auto map_bios = field_of::boolean<wramcnt_type, 0>();
        static constexpr auto enable_wram = field_of::boolean<wramcnt_type, 5>();

        static constexpr auto wait_state(std::integral auto i) noexcept {
            return field_of::integral<wramcnt_type, 24, 0xf>(i);
        }

    } // namespace wramcnt

} // namespace undocumented
} // namespace gba

#endif // define GBAXX_REGISTERS_SYSTEM_TYPES_HPP
