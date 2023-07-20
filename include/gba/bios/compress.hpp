/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_BIOS_COMPRESS_HPP
#define GBAXX_BIOS_COMPRESS_HPP

#include <gba/type.hpp>

namespace gba::bios {

namespace {

    struct bit_un_pack {
        u16 src_len;
        u8 src_bpp;
        u8 dst_bpp;
        u32 dst_ofs : 31;
        bool offset_zero : 1;
    };

    [[gnu::always_inline, gnu::nonnull(1, 2, 3)]]
    inline void BitUnPack(const void* __restrict__ src, volatile void* __restrict__ dest, const bit_un_pack* __restrict__ unpack) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        register auto* r2 asm("r2") = unpack;
        asm volatile inline ("swi 0x10 << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) : "r"(r2) : "r3", "memory");
    }

    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void Diff8bitUnFilterWram(const void* __restrict__ src, volatile void* __restrict__ dest) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        asm volatile inline ("swi 0x16 << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) :: "r3", "memory");
    }

    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void Diff8bitUnFilterVram(const void* __restrict__ src, volatile void* __restrict__ dest) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        asm volatile inline ("swi 0x17 << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) :: "r3", "memory");
    }

    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void Diff16bitUnFilter(const void* __restrict__ src, volatile void* __restrict__ dest) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        asm volatile inline ("swi 0x18 << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) :: "r3", "memory");
    }

    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void HuffUnComp(const void* __restrict__ src, volatile void* __restrict__ dest) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        asm volatile inline ("swi 0x13 << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) :: "r3", "memory");
    }

    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void LZ77UnCompWram(const void* __restrict__ src, volatile void* __restrict__ dest) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        asm volatile inline ("swi 0x11 << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) :: "r3", "memory");
    }

    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void LZ77UnCompVram(const void* __restrict__ src, volatile void* __restrict__ dest) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        asm volatile inline ("swi 0x12 << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) :: "r3", "memory");
    }

    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void RLUnCompWram(const void* __restrict__ src, volatile void* __restrict__ dest) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        asm volatile inline ("swi 0x14 << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) :: "r3", "memory");
    }

    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void RLUnCompVram(const void* __restrict__ src, volatile void* __restrict__ dest) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        asm volatile inline ("swi 0x15 << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) :: "r3", "memory");
    }

}

} // namespace gba::bios

#endif // define GBAXX_BIOS_COMPRESS_HPP
