/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_STRING_HPP
#define GBAXX_EXT_AGBABI_STRING_HPP

#include <agbabi.h>

#include <cstdint>

namespace gba::agbabi {

    namespace {

        [[gnu::nonnull(1, 2)]]
        inline void memcpy2(void* dest, const void* src, std::size_t n) noexcept {
            __agbabi_memcpy2(dest, src, n);
        }

        [[gnu::nonnull(1, 2)]]
        inline void memcpy1(void* dest, const void* src, std::size_t n) noexcept {
            __agbabi_memcpy1(dest, src, n);
        }

        [[gnu::nonnull(1, 2)]]
        inline void rmemcpy1(void* dest, const void* src, std::size_t n) noexcept {
            __agbabi_rmemcpy1(dest, src, n);
        }

        [[gnu::nonnull(1, 2)]]
        inline void rmemcpy(void* dest, const void* src, std::size_t n) noexcept {
            __agbabi_rmemcpy(dest, src, n);
        }

        [[gnu::nonnull(1)]]
        inline void lwordset4(void* dest, std::size_t n, std::int64_t c) noexcept {
            __agbabi_lwordset4(dest, n, c);
        }

        [[gnu::nonnull(1)]]
        inline void wordset4(void* dest, std::size_t n, std::int32_t c) noexcept {
            __agbabi_wordset4(dest, n, c);
        }

    }

    namespace fiq {

        namespace {

            [[gnu::nonnull(1, 2)]]
            inline void memcpy4x4(void* dest, const void* src, std::size_t n) noexcept {
                __agbabi_fiq_memcpy4x4(dest, src, n);
            }

            [[gnu::nonnull(1, 2)]]
            inline void memcpy4(void* dest, const void* src, std::size_t n) noexcept {
                __agbabi_fiq_memcpy4(dest, src, n);
            }

        }

    } // namespace fiq

} // namespace gba::agbabi

#endif // define GBAXX_EXT_AGBABI_STRING_HPP
