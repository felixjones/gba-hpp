/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_MGBA_LOG_HPP
#define GBAXX_EXT_MGBA_LOG_HPP
/** @file */

#include <cstdarg>
#include <cstring>
#include <type_traits>

#if defined __has_include
#if __has_include(<posprintf.h>)
extern "C" {
#include <posprintf.h>
}
#endif
#endif

#include <gba/type.hpp>

namespace gba::mgba {

    /**
     * @enum log
     * @brief Enumeration class for log levels.
     *
     * This enumeration class represents different levels of logging that can be used in mGBA.
     * The levels are ordered from most severe to least severe: fatal, error, warn, info, debug.
     */
    enum class log : int {
        fatal = 0, /**< mGBA will halt and display the message in an error dialog. */
        error = 1, /**< General program errors. */
        warn = 2, /**< General program warnings. */
        info = 3, /**< Information. */
        debug = 4, /**< Output specific to debugging. */
    };

    namespace mmio {

        /**
         * @var DEBUG_STRING
         * @brief A 256 character string register for logging.
         *
         * Pointer to a volatile character array of 256 bytes at memory address 0x04FFF600.
         *
         * This register is used for printing debug messages in mGBA.
         *
         * @see DEBUG_FLAGS
         * @see DEBUG_ENABLE
         */
        inline constexpr auto DEBUG_STRING = const_ptr<volatile char[256]>{0x04FFF600};

        /**
         * @var DEBUG_FLAGS
         * @brief Sends DEBUG_STRING to the mGBA logger.
         *
         * Mapped to an unsigned 16-bit integer at address 0x04FFF700.
         *
         * Used for triggering mGBA debug printing at a given log level.
         *
         * @note Writing 0x100 to this register triggers mGBA debug printing.
         * @note The lower 8 bits can be OR'd with a log level.
         *
         * @see DEBUG_STRING
         * @see DEBUG_ENABLE
         * @see log
         */
        inline constexpr auto DEBUG_FLAGS = registral<const_ptr<volatile u16>(0x04FFF700)>{};

        /**
         * @var DEBUG_ENABLE
         * @brief Enables and detects mGBA logging features.
         *
         * Mapped to an unsigned 16-bit integer at address 0x04FFF780.
         *
         * Used for enabling mGBA debug printing.
         *
         * @note Writing 0xC0DE to this register enables mGBA debug printing.
         * @note If mGBA is in use, reading from this register will return 0x1DEA.
         * @note This can be used to detect mGBA.
         *
         * @see DEBUG_STRING
         * @see DEBUG_FLAGS
         * @see open()
         * @see close()
         */
        inline constexpr auto DEBUG_ENABLE = registral<const_ptr<volatile u16>(0x04FFF780)>{};

    }

    /**
     * @brief Enables mGBA debug features.
     *
     * @return true if mGBA is present and the debug features were enabled.
     * @note This can be used to detect mGBA.
     *
     * @code{cpp}
     * // Testing for the presence of mGBA
     *
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     if (mgba::open()) {
     *         // mGBA is present
     *     }
     * }
     * @endcode
     *
     * @sa mmio::DEBUG_ENABLE
     * @sa close()
     */
    inline bool open() noexcept {
        mmio::DEBUG_ENABLE = 0xC0DE;
        return *mmio::DEBUG_ENABLE == 0x1DEA;
    }

    /**
     * @brief Disables mGBA debug features.
     *
     * Counterpart to open().
     *
     * @sa mmio::DEBUG_ENABLE
     * @sa open()
     */
    inline void close() noexcept {
        mmio::DEBUG_ENABLE = 0;
    }

    /**
     * @brief Outputs a string to the mGBA logger with the specified log level.
     *
     * @param level The log level to determine the type of message to output.
     * @param str The string to be printed.
     *
     * @note A maximum of 256 characters will be printed from the string.
     * @note mGBA must be present for this function to work.
     *
     * @sa open()
     */
    inline void puts(log level, const char* str) noexcept {
        std::strncpy(&mmio::DEBUG_STRING, str, 256);
        mmio::DEBUG_FLAGS.emplace(static_cast<std::underlying_type_t<log>>(level) | 0x100);
    }

    /**
     * @brief Outputs a formatted string to the mGBA logger with the specified log level.
     *
     * std::vsnprintf() is used to format the string.
     *
     * @param level The log level to determine the type of message to output.
     * @param str The format string for the output.
     * @param ... Additional arguments to be formatted and printed.
     *
     * @note A maximum of 256 characters will be printed from the string.
     * @note mGBA must be present for this function to work.
     *
     * @sa open()
     * @sa <a href="https://en.cppreference.com/w/cpp/io/c/vsnprintf">std::vsnprintf()</a>
     */
    [[gnu::format(printf, 2, 3)]]
    inline void printf(log level, const char* str, ...) noexcept {
        va_list args;
        va_start(args, str);
        std::vsnprintf(&mmio::DEBUG_STRING, 256, str, args);
        va_end(args);
        mmio::DEBUG_FLAGS.emplace(static_cast<std::underlying_type_t<log>>(level) | 0x100);
    }

#ifdef _PSPRINTF_HEADER_
    /**
     * @brief Outputs a formatted string to the mGBA logger with the specified log level.
     *
     * posprintf() is used to format the string.
     *
     * @param level The log level to determine the type of message to output.
     * @param str The format string for the output.
     * @param ... Additional arguments to be formatted and printed.
     *
     * @note A maximum of 256 characters will be printed from the string.
     * @note mGBA must be present for this function to work.
     *
     * @warning No safety or bounds check is made, despite there being a 256 character limited. The caller must make
     *          sure the resulting formatted string will be fewer than 256 characters in length.
     *
     * @sa printf()
     * @sa open()
     */
    [[gnu::always_inline]]
    inline void posprintf(log level, const char* str, ...) noexcept {
        ::posprintf(&mmio::DEBUG_STRING, str, __builtin_va_arg_pack());
        mmio::DEBUG_FLAGS.emplace(static_cast<std::underlying_type_t<log>>(level) | 0x100);
    }
#endif

} // namespace gba::mgba

#endif // define GBAXX_EXT_MGBA_LOG_HPP
