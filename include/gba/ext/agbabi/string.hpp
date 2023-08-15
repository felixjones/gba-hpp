/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_STRING_HPP
#define GBAXX_EXT_AGBABI_STRING_HPP
/** @file */

#include <agbabi.h>

#include <cstdint>

namespace gba::agbabi {

    /**
     * @brief Copy memory block from source to destination (half-word aligned).
     *
     * This function copies `n` bytes of memory from the `src` to the `dest` location.
     * It does not perform any overlapping checks, as such behavior is undefined.
     *
     * @param dest Pointer to the memory block where the copied data will be stored.
     * @param src Pointer to the memory block to be copied.
     * @param n Number of bytes to be copied.
     *
     * @note The behavior is undefined if the source and destination memory blocks overlap.
     *
     * @warning The function should only be used with valid pointers and sizes.
     *
     * @sa memcpy1()
     */
    [[gnu::nonnull(1, 2)]]
    inline void memcpy2(void* dest, const void* src, std::size_t n) noexcept {
        __agbabi_memcpy2(dest, src, n);
    }

    /**
     * @brief Copy memory block from source to destination (byte aligned).
     *
     * This function slowly copies `n` bytes of memory from the `src` to the `dest` location byte-by-byte.
     * It does not perform any overlapping checks, as such behavior is undefined.
     *
     * @param dest Pointer to the memory block where the copied data will be stored.
     * @param src Pointer to the memory block to be copied.
     * @param n Number of bytes to be copied.
     *
     * @note The behavior is undefined if the source and destination memory blocks overlap.
     *
     * @warning The function should only be used with valid pointers and sizes.
     *
     * @sa memcpy2()
     */
    [[gnu::nonnull(1, 2)]]
    inline void memcpy1(void* dest, const void* src, std::size_t n) noexcept {
        __agbabi_memcpy1(dest, src, n);
    }

    /**
     * @brief Reverse-copy memory block from source to destination (byte aligned).
     *
     * This function slowly copies `n` bytes of memory from the `src` to the `dest` location byte-by-byte, in
     * reverse (from the end to the beginning).
     * It does not perform any overlapping checks, as such behavior is undefined.
     *
     * @param dest Pointer to the memory block where the copied data will be stored.
     * @param src Pointer to the memory block to be copied.
     * @param n Number of bytes to be copied.
     *
     * @note The behavior is undefined if the source and destination memory blocks overlap.
     *
     * @warning The function should only be used with valid pointers and sizes.
     *
     * @sa memcpy1()
     * @sa rmemcpy()
     */
    [[gnu::nonnull(1, 2)]]
    inline void rmemcpy1(void* dest, const void* src, std::size_t n) noexcept {
        __agbabi_rmemcpy1(dest, src, n);
    }

    /**
     * @brief Copy memory block from source to destination.
     *
     * This function copies `n` bytes of memory from the `src` to the `dest` location.
     * It does not perform any overlapping checks, as such behavior is undefined.
     *
     * @param dest Pointer to the memory block where the copied data will be stored.
     * @param src Pointer to the memory block to be copied.
     * @param n Number of bytes to be copied.
     *
     * @note The behavior is undefined if the source and destination memory blocks overlap.
     *
     * @warning The function should only be used with valid pointers and sizes.
     *
     * @sa rmemcpy1()
     */
    [[gnu::nonnull(1, 2)]]
    inline void rmemcpy(void* dest, const void* src, std::size_t n) noexcept {
        __agbabi_rmemcpy(dest, src, n);
    }

    /**
     * @brief lwordset4 sets consecutive 8-byte spans of memory to a given value.
     *
     * This function sets each 8-byte block of memory starting from the address pointed
     * by `dest` for `n` blocks to the value specified by `c`. The blocks are set
     * in a word-wise manner, meaning each block is set using 64-bit (8-byte) operations.
     *
     * @param dest Pointer to the starting address of the memory area to be set.
     * @param n Number of bytes to be set.
     * @param c Value to set each 8-byte block to.
     *
     * @note The destination pointer is assumed to be word-aligned.
     * @note The memory area must be allocated and have enough space to hold `n` bytes.
     *
     * @see wordset4()
     */
    [[gnu::nonnull(1)]]
    inline void lwordset4(void* dest, std::size_t n, std::int64_t c) noexcept {
        __agbabi_lwordset4(dest, n, c);
    }

    /**
     * @brief wordset4 sets consecutive 4-byte spans of memory to a given value.
     *
     * This function sets each 4-byte block of memory starting from the address pointed
     * by `dest` for `n` blocks to the value specified by `c`. The blocks are set
     * in a word-wise manner, meaning each block is set using 32-bit (4-byte) operations.
     *
     * @param dest Pointer to the starting address of the memory area to be set.
     * @param n Number of bytes to be set.
     * @param c Value to set each 4-byte block to.
     *
     * @note The destination pointer is assumed to be word-aligned.
     * @note The memory area must be allocated and have enough space to hold `n` bytes.
     *
     * @see lwordset4()
     */
    [[gnu::nonnull(1)]]
    inline void wordset4(void* dest, std::size_t n, std::int32_t c) noexcept {
        __agbabi_wordset4(dest, n, c);
    }

    namespace fiq {

        /**
         * @brief Copy 16-byte memory blocks from source to destination (word aligned).
         *
         * This is an extremely fast implementation of a memcpy() routine that takes advantage of fiq CPU mode.
         *
         * @param dest Pointer to the memory where the copied data will be stored.
         * @param src Pointer to the memory to be copied.
         * @param n Number of bytes to be copied (multiple of 16).
         *
         * @note The behavior is undefined if the source and destination memory blocks overlap.
         *
         * @warning `n` must be a multiple of 16.
         * @warning The function should only be used with valid pointers and sizes.
         *
         * @sa memcpy4x4()
         */
        [[gnu::nonnull(1, 2)]]
        inline void memcpy4x4(void* dest, const void* src, std::size_t n) noexcept {
            __agbabi_fiq_memcpy4x4(dest, src, n);
        }

        /**
         * @brief Copy memory block from source to destination (word aligned).
         *
         * This is an extremely fast implementation of a memcpy() routine that takes advantage of fiq CPU mode.
         *
         * @param dest Pointer to the memory where the copied data will be stored.
         * @param src Pointer to the memory to be copied.
         * @param n Number of bytes to be copied.
         *
         * @note The behavior is undefined if the source and destination memory blocks overlap.
         *
         * @warning The function should only be used with valid pointers and sizes.
         *
         * @sa memcpy4x4()
         */
        [[gnu::nonnull(1, 2)]]
        inline void memcpy4(void* dest, const void* src, std::size_t n) noexcept {
            __agbabi_fiq_memcpy4(dest, src, n);
        }

    } // namespace fiq

} // namespace gba::agbabi

#endif // define GBAXX_EXT_AGBABI_STRING_HPP
