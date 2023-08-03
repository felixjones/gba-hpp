/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_BIOS_MISC_HPP
#define GBAXX_BIOS_MISC_HPP
/** @file */

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
     * @struct cpu_fast_set
     * @brief Parameters for CpuFastSet().
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-0ch-gbands7nds9dsi7dsi9---cpufastset">SWI 0Ch (GBA/NDS7/NDS9/DSi7/DSi9) - CpuFastSet</a>
     *
     * @sa CpuFastSet()
     */
    struct alignas(int) cpu_fast_set {
        u32 count : 21{}; /**< Number of 32-bit words to copy. @note Must be a multiple of 8 words. */
        int : 3;
        bool fill : 1{}; /**< Switch between memcpy() and memset() behavior. */
    };

    /**
     * @brief Copies or fills data from source to destination.
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
     * @struct cpu_set
     * @brief Parameters for CpuSet().
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-0bh-gbands7nds9dsi7dsi9---cpuset">SWI 0Bh (GBA/NDS7/NDS9/DSi7/DSi9) - CpuSet</a>
     *
     * @sa CpuSet()
     */
    struct alignas(int) cpu_set {
        u32 count : 21{}; /**< Number of elements to copy. cpu_set::set_32bit controls if this is 32-bit or 16-bit elements. */
        int : 3;
        bool fill : 1{}; /**< Switch between memcpy() and memset() behavior. */
        int : 1;
        bool set_32bit : 1{}; /**< Switches between 32-bit or 16-bit element size. */
    };

    /**
     * @brief Copies or fills data from source to destination.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-0bh-gbands7nds9dsi7dsi9---cpuset">SWI 0Bh (GBA/NDS7/NDS9/DSi7/DSi9) - CpuSet</a>
     *
     * @param src Pointer to the source memory region.
     * @param dest Pointer to the destination memory region.
     * @param lenmode The length (number of elements), mode (copy/fill), and size of elements for the data to be copied.
     *
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
     * @struct ram_reset
     * @brief Parameters for RegisterRamReset().
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-01h-gba---registerramreset">SWI 01h (GBA) - RegisterRamReset</a>
     *
     * This struct provides a convenient way to configure the reset behavior for different sections of RAM. Each member
     * variable corresponds to a specific section of RAM and determines whether that section should be reset or not.
     *
     * @note The ram_reset::reg flag does not include ram_reset::reg_sio or ram_reset::reg_sound. All three reg flags
     *       must be set if you want to clear all registers.
     *
     * @sa RegisterRamReset()
     */
    struct alignas(int) ram_reset {
        bool ewram : 1{}; /**< Clear External Work RAM. @warning Do not set if you are executing from EWRAM. */
        bool iwram : 1{}; /**< Clear Internal Work RAM. @note Excludes the last 512 bytes to preserve stack. @warning Do not set if you are executing from IWRAM. */
        bool palette : 1{}; /**< Clear Palette RAM. */
        bool vram : 1{}; /**< Clear Video RAM. */
        bool oam : 1{}; /**< Clear Object Attribute Memory. */
        bool reg_sio : 1{}; /**< Clear Serial IO registers. */
        bool reg_sound : 1{}; /**< Clear Sound registers. */
        bool reg : 1{}; /**< Clear all registers, except for Serial IO or Sound. */
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
