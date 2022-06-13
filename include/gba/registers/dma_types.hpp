/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_REGISTERS_DMA_TYPES_HPP
#define GBAXX_REGISTERS_DMA_TYPES_HPP

#include <gba/fieldtype.hpp>
#include <gba/inttype.hpp>

namespace gba {

enum class dma_dst : uint8 {
    inc,
    dec,
    fixed,
    reload
};

enum class dma_src : uint8 {
    inc,
    dec,
    fixed
};

enum class dma_at : uint8 {
    vblank,
    hblank,
    fifo,
    refresh = fifo
};

struct alignas(uint16) dmacnt_h_type {
    uint16 : 4;
    dma_dst dst : 2;
    dma_src src : 2;
    bool repeat : 1;
    bool word : 1;
    bool gamepak : 1;
    dma_at at : 2;
    bool irq : 1;
    bool enable : 1;
};

namespace dma {

    static constexpr auto dst(dma_dst i) noexcept {
        return field_of::enum_class<dmacnt_h_type, 5, dma_dst, dma_dst::reload>(i);
    }

    static constexpr auto src(dma_src i) noexcept {
        return field_of::enum_class<dmacnt_h_type, 7, dma_src, dma_src::fixed>(i);
    }

    static constexpr auto repeat = field_of::boolean<dmacnt_h_type, 9>();
    static constexpr auto word = field_of::boolean<dmacnt_h_type, 10>();
    static constexpr auto gamepak = field_of::boolean<dmacnt_h_type, 11>();

    static constexpr auto at(dma_at i) noexcept {
        return field_of::enum_class<dmacnt_h_type, 12, dma_at, dma_at::refresh>(i);
    }

    static constexpr auto irq = field_of::boolean<dmacnt_h_type, 14>();
    static constexpr auto enable = field_of::boolean<dmacnt_h_type, 15>();

} // namespace dma

struct dmarec_type {
    const void* src;
    void* dst;
    uint16 count;
    dmacnt_h_type mode;
};

} // namespace gba

#endif // define GBAXX_REGISTERS_DMA_TYPES_HPP
