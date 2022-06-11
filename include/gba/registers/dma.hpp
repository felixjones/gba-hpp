/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_REGISTERS_DMA_HPP
#define GBAXX_REGISTERS_DMA_HPP

#include <gba/inttype.hpp>

#include <gba/registers/accessors.hpp>
#include <gba/registers/dma_types.hpp>

namespace gba::reg {

using dma0sad = write_only<const void*, 0x40000B0>;
using dma1sad = write_only<const void*, 0x40000BC>;
using dma2sad = write_only<const void*, 0x40000C8>;
using dma3sad = write_only<const void*, 0x40000D4>;

using dma0dad = write_only<void*, 0x40000B4>;
using dma1dad = write_only<void*, 0x40000C0>;
using dma2dad = write_only<void*, 0x40000CC>;
using dma3dad = write_only<void*, 0x40000D8>;

using dma0cnt_l = write_only<uint16, 0x40000B8>;
using dma1cnt_l = write_only<uint16, 0x40000C4>;
using dma2cnt_l = write_only<uint16, 0x40000D0>;
using dma3cnt_l = write_only<uint16, 0x40000DC>;

using dma0cnt_h = read_write<dmacnt_h_type, 0x40000BA>;
using dma1cnt_h = read_write<dmacnt_h_type, 0x40000C6>;
using dma2cnt_h = read_write<dmacnt_h_type, 0x40000D2>;
using dma3cnt_h = read_write<dmacnt_h_type, 0x40000DE>;

using dmarec = write_only<std::array<dmarec_type, 4>, 0x40000B0>;

using dma0rec = write_only<dmarec_type, 0x40000B0>;
using dma1rec = write_only<dmarec_type, 0x40000BC>;
using dma2rec = write_only<dmarec_type, 0x40000C8>;
using dma3rec = write_only<dmarec_type, 0x40000D4>;

} // namespace gba::reg

#endif // define GBAXX_REGISTERS_DMA_HPP
