/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_MGBA_LOG_HPP
#define GBAXX_EXT_MGBA_LOG_HPP

#include <cstdarg>
#include <cstring>

#if defined __has_include
#if __has_include(<posprintf.h>)
extern "C" {
#include <posprintf.h>
}
#endif
#endif

#include <gba/type.hpp>

namespace gba::mgba {

namespace {

    enum class log : int {
        fatal = 0,
        error = 1,
        warn = 2,
        info = 3,
        debug = 4,
    };

    namespace mmio {

        static constexpr auto DEBUG_STRING = const_ptr<volatile char[256]>{0x04FFF600};
        static constexpr auto DEBUG_FLAGS = registral<const_ptr<volatile u16>(0x04FFF700)>{};
        static constexpr auto DEBUG_ENABLE = registral<const_ptr<volatile u16>(0x04FFF780)>{};

    }

    inline bool open() noexcept {
        mmio::DEBUG_ENABLE = 0xC0DE;
        return *mmio::DEBUG_ENABLE == 0x1DEA;
    }

    inline void close() noexcept {
        mmio::DEBUG_ENABLE = 0;
    }

    inline void puts(log level, const char* str) noexcept {
        std::strncpy(&mmio::DEBUG_STRING, str, 256);
        mmio::DEBUG_FLAGS = u16(level) | 0x100;
    }

    [[gnu::format(printf, 2, 3)]]
    inline void printf(log level, const char* str, ...) noexcept {
        va_list args;
        va_start(args, str);
        std::vsnprintf(&mmio::DEBUG_STRING, 256, str, args);
        va_end(args);
        mmio::DEBUG_FLAGS = u16(level) | 0x100;
    }

#ifdef _PSPRINTF_HEADER_
    [[gnu::always_inline]]
    inline void posprintf(log level, const char* str, ...) noexcept {
        ::posprintf(&mmio::DEBUG_STRING, str, __builtin_va_arg_pack());
        mmio::DEBUG_FLAGS = u16(level) | 0x100;
    }
#endif

}

} // namespace gba::mgba

#endif // define GBAXX_EXT_MGBA_LOG_HPP
