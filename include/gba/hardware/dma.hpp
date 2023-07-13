/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_HARDWARE_DMA_HPP
#define GBAXX_HARDWARE_DMA_HPP

#include <gba/type.hpp>

namespace gba {

    enum class dest_addr : u16 {
        increment = 0,
        decrement = 1,
        fixed = 2,
        inc_reload = 3
    };

    enum class src_addr : u16 {
        increment = 0,
        decrement = 1,
        fixed = 2
    };

    struct alignas(short) dma0cnt_h {
        short : 5;
        dest_addr dest_control : 2{};
        src_addr src_control : 2{};
    };

    using dma1cnt_h = dma0cnt_h;
    using dma2cnt_h = dma0cnt_h;
    using dma3cnt_h = dma0cnt_h;

} // namespace gba

#endif // define GBAXX_HARDWARE_DMA_HPP
