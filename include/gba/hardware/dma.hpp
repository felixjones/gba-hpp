/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_HARDWARE_DMA_HPP
#define GBAXX_HARDWARE_DMA_HPP
/** @file */

#include <gba/type.hpp>

namespace gba {

    /**
     * @enum dest_addr
     * @brief Enumeration for the operation to be applied to the destination address.
     *
     * `inc_reload` will behave like `increment`, only when the transfer is completed the destination address will be
     * set back to the original value from prior to the beginning of the transfer.
     *
     * @sa dmacnt_h
     * @sa src_addr
     */
    enum class dest_addr : u16 {
        increment = 0,
        decrement = 1,
        fixed = 2,
        inc_reload = 3 /**< Behaves like dest_addr::increment, only when the transfer is completed the destination
                            address will be set back to the original value from prior to the beginning of the transfer. */
    };

    /**
     * @enum src_addr
     * @brief Enumeration for the operation to be applied to the source address.
     *
     * @sa dmacnt_h
     * @sa dest_addr
     */
    enum class src_addr : u16 {
        increment = 0,
        decrement = 1,
        fixed = 2
    };

    /**
     * @enum start
     * @brief Enumeration for setting when to begin a DMA transfer.
     *
     * The `special` mode depends on which DMA register is doing the operation.
     *
     * @sa dmacnt_h
     */
    enum class start : u16 {
        immediate = 0, /**< The DMA will begin immediately. */
        vblank = 1, /**< The DMA will begin at Vertical Blank. */
        hblank = 2, /**< The DMA will begin at Horizontal Blank. */
        special = 3 /**< DMA1/DMA2 = Sound FIFO. DMA3 = Video Capture. */
    };

    /**
     * @struct dmacnt_h
     * @brief Parameters for a DMA transfer.
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000bah---dma0cnt_h---dma-0-control-rw">40000BAh - DMA0CNT_H - DMA 0 Control (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000c6h---dma0cnt_h---dma-1-control-rw">40000C6h - DMA1CNT_H - DMA 1 Control (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000d2h---dma0cnt_h---dma-2-control-rw">40000D2h - DMA2CNT_H - DMA 2 Control (R/W)</a>
     * @see <a href="https://mgba-emu.github.io/gbatek/#40000deh---dma0cnt_h---dma-3-control-rw">40000DEh - DMA3CNT_H - DMA 3 Control (R/W)</a>
     *
     * @sa mmio::DMA0_CONTROL
     * @sa mmio::DMA1_CONTROL
     * @sa mmio::DMA2_CONTROL
     * @sa mmio::DMA3_CONTROL
     * @sa mmio::DMA_CONTROL
     */
    struct alignas(short) dmacnt_h {
        short : 5;
        dest_addr dest_control : 2{}; /**< @sa dest_addr */
        src_addr src_control : 2{}; /**< @sa src_addr */
        bool repeat : 1{}; /**< Repeat the DMA transfer for HBlank and VBlank DMA. @sa dmacnt_h::start_time @sa start::vblank @sa start::hblank */
        bool transfer_32bit : 1{}; /**< Transfer unit size. true = 32-bit, false = 16-bit. */
        bool dma3_pak_req : 1{}; /**< For DMA channel 3: Allow the GamePak IRQ pin to start DMA3 transfer. @sa irq::gamepak */
        start start_time : 2{}; /**< @sa start */
        bool irq_after : 1{}; /**< Raise an interrupt when the DMA transfer is finished. @sa irq::dma0 @sa irq::dma1 @sa irq::dma2 @sa irq::dma3 */
        bool enabled : 1{}; /**< The DMA transfer will begin when dmacnt_h::enabled = true. */
    };

} // namespace gba

#endif // define GBAXX_HARDWARE_DMA_HPP
