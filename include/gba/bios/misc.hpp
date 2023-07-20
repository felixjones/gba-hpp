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

    [[nodiscard, gnu::always_inline, gnu::const]]
    inline int GetBiosChecksum() noexcept {
        register int i asm("r0");
        asm volatile inline ("swi 0xD << ((1f - . == 4) * -16); 1:" : "=r"(i) :: "r1", "r3");
        return i;
    }

    struct alignas(int) cpu_fast_set {
        u32 count : 21{};
        int : 3;
        bool fill : 1{};
    };

    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void CpuFastSet(const void* __restrict__ src, volatile void* __restrict__ dest, cpu_fast_set lenmode) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        register auto r2 asm("r2") = lenmode;
        asm volatile inline ("swi 0xC << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) : "r"(r2) : "r3", "memory");
    }

    struct alignas(int) cpu_set {
        u32 count : 21{};
        int : 3;
        bool fill : 1{};
        int : 1;
        bool set_32bit : 1{};
    };

    [[gnu::always_inline, gnu::nonnull(1, 2)]]
    inline void CpuSet(const void* __restrict__ src, volatile void* __restrict__ dest, cpu_set lenmode) noexcept {
        register auto* r0 asm("r0") = src;
        register auto* r1 asm("r1") = dest;
        register auto r2 asm("r2") = lenmode;
        asm volatile inline ("swi 0xB << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) : "r"(r2) : "r3", "memory");
    }

    [[noreturn, gnu::always_inline]]
    inline void SoftReset() noexcept {
        asm volatile inline ("swi 0x0 << ((1f - . == 4) * -16); 1:");
        __builtin_unreachable();
    }

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

    [[gnu::always_inline]]
    inline void RegisterRamReset(ram_reset flags) noexcept {
        register auto r0 asm("r0") = flags;
        asm volatile inline ("swi 0x1 << ((1f - . == 4) * -16); 1:" : "+r"(r0) :: "r1", "r3", "memory");
    }

    [[noreturn, gnu::always_inline]]
    inline void HardReset() noexcept {
        asm volatile inline ("swi 0x26 << ((1f - . == 4) * -16); 1:");
        __builtin_unreachable();
    }

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
