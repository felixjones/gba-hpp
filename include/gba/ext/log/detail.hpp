/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_LOG_DETAIL_HPP
#define GBAXX_EXT_LOG_DETAIL_HPP

#include <algorithm>
#include <string>
#include <string_view>
#include <type_traits>

#if defined GBAXX_HAS_AGBABI
#include <aeabi.h>
#endif

#include <gba/inttype.hpp>
#include <gba/ioregister.hpp>

#include <gba/bios/swi_call.hpp>

#include <gba/registers/system.hpp>

#include <gba/util/byte_array.hpp>

namespace gba::log {

enum class level : int {
    fatal = 0,
    error = 1,
    warn = 2,
    info = 3,
    debug = 4,
    trace = 5
};

namespace detail {

struct interface {
    bool (*init)();
    std::size_t (*puts)(std::size_t begin, std::string_view text);
    void (*flush)(log::level level);
};

struct interface_none {
    static bool init() noexcept {
        return true;
    }

    static std::size_t puts([[maybe_unused]] std::size_t begin, [[maybe_unused]] std::string_view text) noexcept {
        return 0u;
    }

    static void flush([[maybe_unused]] level level) noexcept {}
};

struct interface_mgba {
    static constexpr auto code = uint16{0xC0DE};
    static constexpr auto idea = uint16{0x1DEA};

    [[nodiscard]]
    static bool init() noexcept {
        io::register_emplace<uint16, 0x04FFF780>(code);
        return io::register_read<uint16, 0x04FFF780>() == idea;
    }

    static std::size_t puts(std::size_t begin, std::string_view text) noexcept {
        const auto len = std::min(256u - begin, text.size());
#if defined GBAXX_HAS_AGBABI
        __aeabi_memcpy(reinterpret_cast<char*>(0x04FFF600) + begin, text.cbegin(), len);
#else
        std::memcpy(reinterpret_cast<char*>(0x04FFF600) + begin, text.cbegin(), len);
#endif
        return len;
    }

    static void flush(level level) noexcept {
        using underlying_type = std::underlying_type_t<log::level>;

        io::register_emplace<uint16, 0x04FFF700>(static_cast<uint16>(static_cast<underlying_type>(level) | 0x100));
    }
};

struct interface_nocash {
    static constexpr auto nocashgba = util::to_char_array("no$gba");

    [[nodiscard]]
    static bool init() noexcept {
        const auto signature = *reinterpret_cast<std::array<char, 6>*>(0x04FFFA00);
        return signature == nocashgba;
    }

    static std::size_t puts([[maybe_unused]] std::size_t begin, std::string_view text) noexcept {
        for (auto c: text) {
            io::register_emplace<char, 0x04FFFA1C>(c);
        }
        return text.size();
    }

    static void flush([[maybe_unused]] level level) noexcept {
        io::register_emplace<char, 0x04FFFA1C>('\n');
    }
};

} // namespace detail
} // namespace gba::log

#endif // define GBAXX_EXT_LOG_DETAIL_HPP
