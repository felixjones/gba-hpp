/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_BIOS_SOUND_HPP
#define GBAXX_BIOS_SOUND_HPP
/** @file */

#include <gba/type.hpp>

namespace gba::bios {

    /**
     * @brief Calculates the frequency of a MIDI key with fine adjustment.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-1fh-gba---midikey2freq">SWI 1Fh (GBA) - MidiKey2Freq</a>
     *
     * This function takes a pointer to a wave, a MIDI key number, and a fine adjustment value as inputs and calculates the corresponding frequency.
     *
     * @param wave A pointer to the wave.
     * @param key The MIDI key number.
     * @param fineadj The fine adjustment value.
     * @return The frequency of the MIDI key with fine adjustment.
     *
     * @note This function assumes that the wave pointer is pointing to a valid WaveData object.
     * @note This function is made famous by its use to read BIOS memory without triggering the BIOS protection
     *
     * @section Reading BIOS memory:
     * @code{cpp}
     * auto read_bios_byte(std::size_t idx) noexcept {
     *     const auto a = gba::bios::MidiKey2Freq(reinterpret_cast&lt;const void*>(idx - 4), 180 - 12, 0) * 2;
     *     return std::byte(a >> 24);
     * }
     * @endcode
     */
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

    /**
     * @brief Transitions the bias level for sound output.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-19h-gba-or-swi-08h-nds7dsi7---soundbias">SWI 19h (GBA) or SWI 08h (NDS7/DSi7) - SoundBias</a>
     *
     * Increments or decrements the current level of the SOUNDBIAS register (with short delays) until reaching the
     * desired new level. The upper bits of the register are kept unchanged.
     *
     * @param bias The bias value to be set.
     */
    [[gnu::always_inline]]
    inline void SoundBias(u16 bias) noexcept {
        register auto r0 asm("r0") = bias;
        asm volatile inline ("swi 0x19 << ((1f - . == 4) * -16); 1:" : "+r"(r0) :: "r1", "r3");
    }

    /**
     * @brief Clears all direct sound channels and stops the sound.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-1eh-gba---soundchannelclear">SWI 1Eh (GBA) - SoundChannelClear</a>
     */
    [[gnu::always_inline]]
    inline void SoundChannelClear() noexcept {
        asm volatile inline ("swi 0x1E << ((1f - . == 4) * -16); 1:" ::: "r0", "r1", "r3", "memory");
    }

    /**
     * @brief Initializes the sound driver. Call this only once when the game starts up.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-1ah-gba---sounddriverinit">SWI 1Ah (GBA) - SoundDriverInit</a>
     *
     * @param sa Pointer to SoundArea structure for sound driver work area.
     *
     * @warning You cannot initialize this driver multiple times, even if separate work areas have been prepared.
     */
    [[gnu::always_inline]]
    inline void SoundDriverInit(void* sa) noexcept {
        register auto* r0 asm("r0") = sa;
        asm volatile inline ("swi 0x1A << ((1f - . == 4) * -16); 1:" : "+r"(r0) :: "r1", "r3", "memory");
    }

    /**
     * @brief Main routine of the sound driver.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-1ch-gba---sounddrivermain">SWI 1Ch (GBA) - SoundDriverMain</a>
     *
     * Call every 1/60 of a second.
     */
    [[gnu::always_inline]]
    inline void SoundDriverMain() noexcept {
        asm volatile inline ("swi 0x1C << ((1f - . == 4) * -16); 1:" ::: "r0", "r1", "r3", "memory");
    }

    /**
     * @struct Parameters for SoundDriverMode().
     * @brief The sound driver mode configuration.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-1bh-gba---sounddrivermode">SWI 1Bh (GBA) - SoundDriverMode</a>
     *
     * The sound_mode struct stores the settings for sound mode including reverb, number of virtual channels, volume,
     * frequency, and number digital-to-analog bits (between 6 and 9 bits, default is 8).
     *
     * @sa SoundDriverMode()
     */
    struct alignas(int) sound_mode {
        u32 reverb : 7{}; /**< Strength of reverb. @note sound_mode::use_reverb must be true. */
        bool use_reverb : 1{}; /**< Enable the reverb effect. @sa sound_mode::reverb. */
        u32 virtual_channels : 4{8}; /**< Number of sound channels to mix. @note Default is 8. */
        u32 volume : 4{15}; /**< Mix volume. @note Default is 15. */
        u32 frequency : 4{4}; /**< Playback frequency. @note Default is 4. */
        u32 num_bits : 4{9}; /**< Final output bit-depth. Higher number = less bits. @note Default is 9. */
    };

    /**
     * @brief Sets the sound driver mode based on the provided sound mode.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-1bh-gba---sounddrivermode">SWI 1Bh (GBA) - SoundDriverMode</a>
     *
     * \param mode Pointer to the sound mode structure.
     *
     * @sa sound_mode
     */
    [[gnu::always_inline]]
    inline void SoundDriverMode(const sound_mode* mode) noexcept {
        register auto* r0 asm("r0") = mode;
        asm volatile inline ("swi 0x1B << ((1f - . == 4) * -16); 1:" : "+r"(r0) :: "r1", "r3", "memory");
    }

    /**
     * @brief Intended to be called in a VBlank interrupt handler.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-1dh-gba---sounddrivervsync">SWI 1Dh (GBA) - SoundDriverVSync</a>
     *
     * @warning The timing is critical, so make sure to call this first (highest priority) in your interrupt handler.
     *
     * @sa SoundDriverVSyncOff()
     * @sa SoundDriverVSyncOn()
     */
    [[gnu::always_inline]]
    inline void SoundDriverVSync() noexcept {
        asm volatile inline ("swi 0x1D << ((1f - . == 4) * -16); 1:" ::: "r0", "r1", "r3");
    }

    /**
     * @brief Disables the SoundDriverVSync() handler.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-28h-gba---sounddrivervsyncoff">SWI 28h (GBA) - SoundDriverVSyncOff</a>
     *
     * @warning If VBlank interrupts are stopped for whatever reason then this function must be called to protect the
     *          sound driver until VBlank interrupts are re-enabled.
     *
     * @sa SoundDriverVSync()
     * @sa SoundDriverVSyncOn()
     */
    [[gnu::always_inline]]
    inline void SoundDriverVSyncOff() noexcept {
        asm volatile inline ("swi 0x28 << ((1f - . == 4) * -16); 1:" ::: "r0", "r1", "r3");
    }

    /**
     * @brief Enables the SoundDriverVSync() handler.
     * @see <a href="https://mgba-emu.github.io/gbatek/#swi-29h-gba---sounddrivervsyncon">SWI 29h (GBA) - SoundDriverVSyncOn</a>
     *
     * Corresponding function to SoundDriverVSyncOff() for when VSync interrupts are re-enabled.
     *
     * @warning After re-enabling VBlank interrupt and calling this function the SoundDriverVSync() must trigger via
     *          VBlank interrupt ASAP.
     *
     * @sa SoundDriverVSync()
     * @sa SoundDriverVSyncOff()
     */
    [[gnu::always_inline]]
    inline void SoundDriverVSyncOn() noexcept {
        asm volatile inline ("swi 0x29 << ((1f - . == 4) * -16); 1:" ::: "r0", "r1", "r3");
    }

} // namespace gba::bios

#endif // define GBAXX_BIOS_SOUND_HPP
