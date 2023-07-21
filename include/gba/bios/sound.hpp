/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_BIOS_SOUND_HPP
#define GBAXX_BIOS_SOUND_HPP

#include <gba/type.hpp>

namespace gba::bios {

namespace {

    [[nodiscard, gnu::always_inline, gnu::const]]
    inline auto MidiKey2Freq(const void* wave, int key, int fineadj) noexcept {
        register union {
            const void* ptr;
            u32 res;
        } r0 asm("r0") = {wave};
        register auto r1 asm("r1") = key;
        register auto r2 asm("r2") = fineadj;
        asm volatile inline ("swi 0x1F << ((1f - . == 4) * -16); 1:" : "+r"(r0), "+r"(r1) : "r"(r2) : "r3");
        return r0.res;
    }

    [[gnu::always_inline]]
    inline void SoundBias(u16 bias) noexcept {
        register auto r0 asm("r0") = bias;
        asm volatile inline ("swi 0x19 << ((1f - . == 4) * -16); 1:" : "+r"(r0) :: "r1", "r3");
    }

    [[gnu::always_inline]]
    inline void SoundChannelClear() noexcept {
        asm volatile inline ("swi 0x1E << ((1f - . == 4) * -16); 1:" ::: "r0", "r1", "r3", "memory");
    }

    [[gnu::always_inline]]
    inline void SoundDriverInit(void* sa) noexcept {
        register auto* r0 asm("r0") = sa;
        asm volatile inline ("swi 0x1A << ((1f - . == 4) * -16); 1:" : "+r"(r0) :: "r1", "r3", "memory");
    }

    [[gnu::always_inline]]
    inline void SoundDriverMain() noexcept {
        asm volatile inline ("swi 0x1C << ((1f - . == 4) * -16); 1:" ::: "r0", "r1", "r3", "memory");
    }

    struct alignas(int) sound_mode {
        u32 reverb : 7{};
        bool use_reverb : 1{};
        u32 virtual_channels : 4{8};
        u32 volume : 4{15};
        u32 frequency : 4{4};
        u32 num_bits : 4{9};
    };

    [[gnu::always_inline]]
    inline void SoundDriverMode(const sound_mode* mode) noexcept {
        register auto* r0 asm("r0") = mode;
        asm volatile inline ("swi 0x1B << ((1f - . == 4) * -16); 1:" : "+r"(r0) :: "r1", "r3", "memory");
    }

    [[gnu::always_inline]]
    inline void SoundDriverVSync() noexcept {
        asm volatile inline ("swi 0x1D << ((1f - . == 4) * -16); 1:" ::: "r0", "r1", "r3");
    }

    [[gnu::always_inline]]
    inline void SoundDriverVSyncOff() noexcept {
        asm volatile inline ("swi 0x28 << ((1f - . == 4) * -16); 1:" ::: "r0", "r1", "r3");
    }

    [[gnu::always_inline]]
    inline void SoundDriverVSyncOn() noexcept {
        asm volatile inline ("swi 0x29 << ((1f - . == 4) * -16); 1:" ::: "r0", "r1", "r3");
    }

}

} // namespace gba::bios

#endif // define GBAXX_BIOS_SOUND_HPP
