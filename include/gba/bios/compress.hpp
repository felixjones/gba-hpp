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

    /**
     * @brief Structure for unpacking bits.
     * @see <a href="https://mgba-emu.github.io/gbatek/#bitunpack---swi-10h-gbands7nds9dsi7dsi9">BitUnPack - SWI 10h (GBA/NDS7/NDS9/DSi7/DSi9)</a>
     *
     * This structure represents the information needed to unpack bits from a source into a destination buffer.
     *
     * @sa BitUnPack()
     */
    struct bit_un_pack {
        u16 src_len;
        u8 src_bpp;
        u8 dst_bpp;
        u32 dst_ofs : 31;
        bool offset_zero : 1;
    };

    /**
     * @brief Unpacks bit-packed data from source to destination buffer using provided unpack instructions.
     * @see <a href="https://mgba-emu.github.io/gbatek/#bitunpack---swi-10h-gbands7nds9dsi7dsi9">BitUnPack - SWI 10h (GBA/NDS7/NDS9/DSi7/DSi9)</a>
     *
     * @param src Pointer to the source buffer containing bit-packed data.
     * @param dest Pointer to the destination buffer where the unpacked data will be written.
     * @param unpack Pointer to the unpack instructions specifying how the data should be unpacked.
     *
     * @note The function assumes that the source buffer contains packed data in the format specified by the unpack instructions.
     *
     * @warning The function does not perform any bounds checking. It is the caller's responsibility to ensure that the
     *          source buffer is large enough to contain the packed data, and that the destination buffer has enough
     *          space to hold the unpacked data.
     *
     * @sa bit_un_pack
     */
    [[gnu::always_inline, gnu::nonnull(1, 2, 3)]]
    inline void BitUnPack(const void* __restrict__ src, volatile void* __restrict__ dest, const bit_un_pack* __restrict__ unpack) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        register auto* r2 asm("r2") = unpack;
        asm volatile inline ("swi 0x10 << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) : "r"(r2) : "r3", "memory");
    }

    /**
     * @brief Applies differential 8-bit unfiltering to the data stored in the source buffer and stores the result in
     *        the destination buffer.
     * @see <a href="https://mgba-emu.github.io/gbatek/#diff8bitunfilterwrite8bit-wram---swi-16h-gbands9dsi9">Diff8bitUnFilterWrite8bit (Wram) - SWI 16h (GBA/NDS9/DSi9)</a>
     *
     * @param src Pointer to the source buffer containing the data to be unfiltered.
     * @param dest Pointer to the destination buffer where the unfiltered data will be stored.
     *
     * @note This function is intended to be used for writing to WRAM and assumes that the provided source buffer is
     *       word aligned.
     *
     * @warning The function assumes that the src and dest buffers have sufficient space to store the unfiltered data,
     *          and that they do not overlap.
     *
     * @sa Diff8bitUnFilterVram()
     * @sa Diff16bitUnFilter()
     */
    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void Diff8bitUnFilterWram(const void* __restrict__ src, volatile void* __restrict__ dest) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        asm volatile inline ("swi 0x16 << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) :: "r3", "memory");
    }

    /**
     * @brief Applies differential 8-bit unfiltering to the data stored in the source buffer and stores the result in
     *        the destination buffer.
     * @see <a href="https://mgba-emu.github.io/gbatek/#diff8bitunfilterwrite16bit-vram---swi-17h-gba">Diff8bitUnFilterWrite16bit (Vram) - SWI 17h (GBA)</a>
     *
     * @param src Pointer to the source buffer containing the data to be unfiltered.
     * @param dest Pointer to the destination buffer where the unfiltered data will be stored.
     *
     * @note This function is intended to be used for writing to VRAM and assumes that the provided source buffer is
     *       word aligned.
     *
     * @warning The function assumes that the src and dest buffers have sufficient space to store the unfiltered data,
     *          and that they do not overlap.
     *
     * @sa Diff8bitUnFilterWram()
     * @sa Diff16bitUnFilter()
     */
    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void Diff8bitUnFilterVram(const void* __restrict__ src, volatile void* __restrict__ dest) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        asm volatile inline ("swi 0x17 << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) :: "r3", "memory");
    }

    /**
     * @brief Applies differential 16-bit unfiltering to the data stored in the source buffer and stores the result in
     *        the destination buffer.
     * @see <a href="https://mgba-emu.github.io/gbatek/#diff16bitunfilter---swi-18h-gbands9dsi9">Diff16bitUnFilter - SWI 18h (GBA/NDS9/DSi9)</a>
     *
     * @param src Pointer to the source buffer containing the data to be unfiltered.
     * @param dest Pointer to the destination buffer where the unfiltered data will be stored.
     *
     * @note This function assumes that the provided source buffer is word aligned.
     *
     * @warning The function assumes that the src and dest buffers have sufficient space to store the unfiltered data,
     *          and that they do not overlap.
     *
     * @sa Diff8bitUnFilterWram()
     * @sa Diff8bitUnFilterVram()
     */
    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void Diff16bitUnFilter(const void* __restrict__ src, volatile void* __restrict__ dest) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        asm volatile inline ("swi 0x18 << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) :: "r3", "memory");
    }

    /**
     * @brief Decompresses Huffman coded data.
     * @see <a href="https://mgba-emu.github.io/gbatek/#huffuncompreadnormal---swi-13h-gba">HuffUnCompReadNormal - SWI 13h (GBA)</a>
     *
     * @param src Pointer to the compressed source data.
     * @param dest Pointer to the destination buffer where the uncompressed data will be stored.
     *
     * @warning The function assumes that the dest buffer has sufficient space to store the decompressed data.
     */
    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void HuffUnComp(const void* __restrict__ src, volatile void* __restrict__ dest) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        asm volatile inline ("swi 0x13 << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) :: "r3", "memory");
    }

    /**
     * @brief Decompresses LZ77 compressed data.
     * @see <a href="https://mgba-emu.github.io/gbatek/#lz77uncompreadnormalwrite8bit-wram---swi-11h-gbands7nds9dsi7dsi9">LZ77UnCompReadNormalWrite8bit (Wram) - SWI 11h (GBA/NDS7/NDS9/DSi7/DSi9)</a>
     *
     * @param src Pointer to the compressed source data.
     * @param dest Pointer to the destination buffer where the uncompressed data will be written to.
     *
     * @note This function is intended to be used for writing to WRAM and assumes that the provided source buffer is
     *       word aligned.
     *
     * @warning The function assumes that the dest buffer has sufficient space to store the decompressed data.
     *
     * @sa LZ77UnCompVram()
     */
    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void LZ77UnCompWram(const void* __restrict__ src, volatile void* __restrict__ dest) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        asm volatile inline ("swi 0x11 << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) :: "r3", "memory");
    }

    /**
     * @brief Decompresses LZ77 compressed data.
     * @see <a href="https://mgba-emu.github.io/gbatek/#lz77uncompreadnormalwrite16bit-vram---swi-12h-gba">LZ77UnCompReadNormalWrite16bit (Vram) - SWI 12h (GBA)</a>
     *
     * @param src Pointer to the compressed source data.
     * @param dest Pointer to the destination buffer where the uncompressed data will be written to.
     *
     * @note This function is intended to be used for writing to VRAM and assumes that the provided source buffer is
     *       word aligned and that the provided dest buffer is half-word aligned.
     *
     * @warning The function assumes that the dest buffer has sufficient space to store the decompressed data.
     *
     * @sa LZ77UnCompWram()
     */
    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void LZ77UnCompVram(const void* __restrict__ src, volatile void* __restrict__ dest) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        asm volatile inline ("swi 0x12 << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) :: "r3", "memory");
    }

    /**
     * @brief Decompresses a RLE (Run-Length Encoding) compressed data.
     * @see <a href="https://mgba-emu.github.io/gbatek/#rluncompreadnormalwrite8bit-wram---swi-14h-gbands7nds9dsi7dsi9">RLUnCompReadNormalWrite8bit (Wram) - SWI 14h (GBA/NDS7/NDS9/DSi7/DSi9)</a>
     *
     * @param src Pointer to the compressed source data.
     * @param dest Pointer to the destination buffer where the uncompressed data will be written to.
     *
     * @note This function is intended to be used for writing to WRAM and assumes that the provided source buffer is
     *       word aligned.
     *
     * @warning The function assumes that the dest buffer has sufficient space to store the decompressed data.
     *
     * @sa RLUnCompVram()
     */
    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void RLUnCompWram(const void* __restrict__ src, volatile void* __restrict__ dest) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        asm volatile inline ("swi 0x14 << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) :: "r3", "memory");
    }

    /**
     * @brief Decompresses a RLE (Run-Length Encoding) compressed data.
     * @see <a href="https://mgba-emu.github.io/gbatek/#rluncompreadnormalwrite16bit-vram---swi-15h-gba">RLUnCompReadNormalWrite16bit (Vram) - SWI 15h (GBA)</a>
     *
     * @param src Pointer to the compressed source data.
     * @param dest Pointer to the destination buffer where the uncompressed data will be written to.
     *
     * @note This function is intended to be used for writing to VRAM and assumes that the provided source buffer is
     *       word aligned and that the provided dest buffer is half-word aligned.
     *
     * @warning The function assumes that the dest buffer has sufficient space to store the decompressed data.
     *
     * @sa RLUnCompWram()
     */
    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void RLUnCompVram(const void* __restrict__ src, volatile void* __restrict__ dest) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        asm volatile inline ("swi 0x15 << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) :: "r3", "memory");
    }

}

} // namespace gba::bios

#endif // define GBAXX_BIOS_COMPRESS_HPP
