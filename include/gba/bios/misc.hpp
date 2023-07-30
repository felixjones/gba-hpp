/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_BIOS_MISC_HPP
#define GBAXX_BIOS_MISC_HPP

#include <gba/type.hpp>
#include <gba/hardware/serial.hpp>

namespace gba::bios {

namespace {

    /**
     * @brief Retrieves the BIOS checksum.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-0dh-gba---getbioschecksum-undocumented">SWI 0Dh (GBA) - GetBiosChecksum (Undocumented)</a>
     *
     * The GBA is expected to return `0xBAAE187F`, whereas the DS (in GBA mode) is expected to return `0xBAAE1880`.
     *
     * @return The checksum value of the BIOS.
     *
     * @note This is an undocumented feature.
     */
    [[nodiscard, gnu::always_inline, gnu::const]]
    inline int GetBiosChecksum() noexcept {
        register int i asm("r0");
        asm volatile inline ("swi 0xD << ((1f - . == 4) * -16); 1:" : "=r"(i) :: "r1", "r3");
        return i;
    }

    /**
     * @brief Structure for CpuFastSet().
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-0ch-gbands7nds9dsi7dsi9---cpufastset">SWI 0Ch (GBA/NDS7/NDS9/DSi7/DSi9) - CpuFastSet</a>
     *
     * The fill flag is used to change CpuFastSet() from memcpy() behavior, to memset() behavior.
     *
     * @note The count is the number of words to copy (AKA: number of bytes divided by 4).
     * @note For CpuFastSet() the `count` must be a multiple of 8 words.
     *
     * @sa CpuFastSet()
     */
    struct alignas(int) cpu_fast_set {
        u32 count : 21{};
        int : 3;
        bool fill : 1{};
    };

    /**
     * @brief CpuFastSet function copies data from source to destination using CPU-specific optimizations.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-0ch-gbands7nds9dsi7dsi9---cpufastset">SWI 0Ch (GBA/NDS7/NDS9/DSi7/DSi9) - CpuFastSet</a>
     *
     * @param src Pointer to the source memory region.
     * @param dest Pointer to the destination memory region.
     * @param lenmode The length (multiple of 8) and mode (copy/fill) of the data to be copied.
     *
     * @note The lenmode::count must be a multiple of 8 words.
     * @note When lenmode::fill is true this function performs a faster memset() than using DMA (Direct Memory Access)
     *       registers.
     * @note This function assumes that the provided source buffer and destination buffers are word aligned.
     *
     * @warning The function assumes that the src and dest buffers have sufficient space to store the data, and that
     *          they do not overlap.
     *
     * @sa cpu_fast_set
     * @sa CpuSet()
     */
    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void CpuFastSet(const void* __restrict__ src, volatile void* __restrict__ dest, cpu_fast_set lenmode) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        register auto r2 asm("r2") = lenmode;
        asm volatile inline ("swi 0xC << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) : "r"(r2) : "r3", "memory");
    }

    /**
     * @brief Structure for CpuSet().
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-0bh-gbands7nds9dsi7dsi9---cpuset">SWI 0Bh (GBA/NDS7/NDS9/DSi7/DSi9) - CpuSet</a>
     *
     * The fill flag is used to change CpuSet() from memcpy() behavior, to memset() behavior.
     * The set_32bit flag changes from 16-bit element size to 32-bit element size.
     *
     * @note The count is the number of elements to copy (AKA: number of bytes divided by 2 or 4, if set_32bit is set).
     * @note For CpuSet() the `count` must be a multiple of 2 for 16-bit elements, or a multiple of 4 for 32-bit
     *       elements (if set_32bit is set).
     *
     * @sa CpuSet()
     */
    struct alignas(int) cpu_set {
        u32 count : 21{};
        int : 3;
        bool fill : 1{};
        int : 1;
        bool set_32bit : 1{};
    };

    /**
     * @brief CpuSet function copies data from source to destination using CPU-specific optimizations.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-0bh-gbands7nds9dsi7dsi9---cpuset">SWI 0Bh (GBA/NDS7/NDS9/DSi7/DSi9) - CpuSet</a>
     *
     * @param src Pointer to the source memory region.
     * @param dest Pointer to the destination memory region.
     * @param lenmode The length (number of elements), mode (copy/fill), and size of elements for the data to be copied.
     *
     * @note The lenmode::count must be a multiple of the element size (when cpu_set::set_32bit is set).
     * @note This function assumes that the provided source buffer and destination buffers are half-word aligned when
     *       cpu_set::set_32bit is false, or word aligned when cpu_set::set_32bit is true.
     *
     * @warning The function assumes that the src and dest buffers have sufficient space to store the data, and that
     *          they do not overlap.
     *
     * @sa cpu_set
     * @sa CpuFastSet()
     */
    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void CpuSet(const void* __restrict__ src, volatile void* __restrict__ dest, cpu_set lenmode) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        register auto r2 asm("r2") = lenmode;
        asm volatile inline ("swi 0xB << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) : "r"(r2) : "r3", "memory");
    }

    /**
     * @brief Performs a soft reset.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-00h-gbands7nds9---softreset">SWI 00h (GBA/NDS7/NDS9) - SoftReset</a>
     *
     * This function triggers a software reset, also known as a soft reset, which restarts the program execution from
     * the beginning.
     *
     * @note A flag stored in 0x3007FFA determines if the reset returns to ROM code, or EWRAM code.
     * @note Clears 200h bytes of RAM (containing stacks, and BIOS IRQ vector/flags).
     * @note Initializes system, supervisor, and irq stack pointers.
     * @note Sets R0-R12, LR_svc, SPSR_svc, LR_irq, and SPSR_irq to zero.
     * @note Enters system mode.
     */
    [[noreturn, gnu::always_inline]]
    inline void SoftReset() noexcept {
        asm volatile inline ("swi 0x0 << ((1f - . == 4) * -16); 1:");
        __builtin_unreachable();
    }

    /**
     * @brief Represents the reset configuration for various sections of RAM.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-01h-gba---registerramreset">SWI 01h (GBA) - RegisterRamReset</a>
     *
     * This struct provides a convenient way to configure the reset behavior for different sections of RAM. Each member
     * variable corresponds to a specific section of RAM and determines whether that section should be reset or not.
     *
     * @note The reg flag does not include reg_sio or reg_sound. All three reg flags must be set if you want to clear
     *       all registers.
     *
     * @sa RegisterRamReset()
     */
    struct alignas(int) ram_reset {
        bool ewram : 1{};
        bool iwram : 1{};
        bool palette : 1{};
        bool vram : 1{};
        bool oam : 1{};
        bool reg_sio : 1{};
        bool reg_sound : 1{};
        bool reg : 1{};
    };

    /**
     * \brief Resets the RAM based on the specified flags.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-01h-gba---registerramreset">SWI 01h (GBA) - RegisterRamReset</a>
     *
     * This function resets the RAM based on the flags passed as an argument.
     * The RAM can be reset partially or fully depending on the flags.
     *
     * \param flags A bitmask specifying the RAM reset flags.
     *
     * \sa ram_reset
     */
    [[gnu::always_inline]]
    inline void RegisterRamReset(ram_reset flags) noexcept {
        register auto r0 asm("r0") = flags;
        asm volatile inline ("swi 0x1 << ((1f - . == 4) * -16); 1:" : "+r"(r0) :: "r1", "r3", "memory");
    }

    /**
     * @brief Performs a hard reset of the system.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-26h-gba---hardreset-undocumented">SWI 26h (GBA) - HardReset (Undocumented)</a>
     *
     * The hard reset will completely shutdown and restart the system, restarting from the initial BIOS boot animation.
     *
     * @note This is an undocumented feature.
     *
     * @sa SoftReset()
     */
    [[noreturn, gnu::always_inline]]
    inline void HardReset() noexcept {
        asm volatile inline ("swi 0x26 << ((1f - . == 4) * -16); 1:");
        __builtin_unreachable();
    }

    /**
     * @brief Begins the MultiBoot protocol to transfer and execute program code to connected GBA serial clients.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-25h-gba---multiboot">SWI 25h (GBA) - MultiBoot</a>
     *
     * @param param A pointer to the multi_boot_param structure.
     * @param mbmode The multi_boot_mode specifying the desired transfer speed and behavior.
     * @return 0 if MultiBoot succeeded, 1 if MultiBoot failed.
     *
     * @remarks The multi_boot_param structure should be properly initialized before passing it to this function.
     *
     * @see multi_boot_param
     * @see multi_boot_mode
     */
    [[gnu::always_inline, gnu::nonnull(1)]]
    inline auto MultiBoot(const multi_boot_param* param, multi_boot_mode mbmode) noexcept {
        register union {
            const multi_boot_param* ptr;
            int res;
        } r0 asm("r0") = {param};
        register auto r1 asm("r1") = mbmode;
        asm volatile inline ("swi 0x25 << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) :: "r3");
        return r0.res;
    }

}

} // namespace gba::bios

#endif // define GBAXX_BIOS_MISC_HPP
