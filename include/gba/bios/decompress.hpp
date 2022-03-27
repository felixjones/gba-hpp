/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_BIOS_DECOMPRESS_HPP
#define GBAXX_BIOS_DECOMPRESS_HPP

#include <gba/inttype.hpp>

#include <gba/bios/swi_call.hpp>

namespace gba::bios {

struct bit_unpack {
    uint16 length;
    uint8 in_digits;
    uint8 out_digits;
    uint32 data_incr : 31;
    bool incr_zero : 1;
};

[[gnu::always_inline]]
inline void BitUnPack(const void* __restrict__ source, void* __restrict__ destination, const bit_unpack& unpack) noexcept {
    swi<0x10, void>(source, __builtin_assume_aligned(destination, 4), &unpack);
}

struct uncomp_header {
    enum class type : uint32 {
        lz77 = 0x10,
        huffman_4 = 0x24,
        huffman_8 = 0x28,
        rle = 0x30,
        filter_8 = 0x81,
        filter_16 = 0x82,
    };

    type type : 8;
    uint32 length : 24;
};

[[gnu::always_inline]]
inline void LZ77UnCompWRAM(const uncomp_header* __restrict__ source, void* __restrict__ destination) noexcept {
    swi<0x11, void>(__builtin_assume_aligned(source, 4), destination);
}

[[gnu::always_inline]]
inline void LZ77UnCompVRAM(const uncomp_header* __restrict__ source, void* __restrict__ destination) noexcept {
    swi<0x12, void>(__builtin_assume_aligned(source, 4), __builtin_assume_aligned(destination, 2));
}

[[gnu::always_inline]]
inline void HuffUnComp(const uncomp_header* __restrict__ source, void* __restrict__ destination) noexcept {
    swi<0x13, void>(__builtin_assume_aligned(source, 4), destination);
}

[[gnu::always_inline]]
inline void RLUnCompWRAM(const uncomp_header* __restrict__ source, void* __restrict__ destination) noexcept {
    swi<0x14, void>(__builtin_assume_aligned(source, 4), destination);
}

[[gnu::always_inline]]
inline void RLUnCompVRAM(const uncomp_header* __restrict__ source, void* __restrict__ destination) noexcept {
    swi<0x15, void>(__builtin_assume_aligned(source, 4), __builtin_assume_aligned(destination, 2));
}

[[gnu::always_inline]]
inline void Diff8bitUnFilterWRAM(const uncomp_header* __restrict__ source, void* __restrict__ destination) noexcept {
    swi<0x16, void>(__builtin_assume_aligned(source, 4), destination);
}

[[gnu::always_inline]]
inline void Diff8bitUnFilterVRAM(const uncomp_header* __restrict__ source, void* __restrict__ destination) noexcept {
    swi<0x17, void>(__builtin_assume_aligned(source, 4), __builtin_assume_aligned(destination, 2));
}

[[gnu::always_inline]]
inline void Diff16bitUnFilter(const uncomp_header* __restrict__ source, void* __restrict__ destination) noexcept {
    swi<0x18, void>(__builtin_assume_aligned(source, 4), __builtin_assume_aligned(destination, 2));
}

} // namespace gba::bios

#endif // define GBAXX_BIOS_DECOMPRESS_HPP
