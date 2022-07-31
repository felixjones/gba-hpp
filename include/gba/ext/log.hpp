/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_LOG_HPP
#define GBAXX_EXT_LOG_HPP

#include <initializer_list>
#include <string_view>
#include <utility>

#include <gba/ext/log/detail.hpp>

namespace gba::log {
namespace {

    [[maybe_unused]]
    constexpr auto mgba = detail::interface{
        detail::interface_mgba::init,
        detail::interface_mgba::puts,
        detail::interface_mgba::flush
    };

    [[maybe_unused]]
    constexpr auto nocash = detail::interface{
        detail::interface_nocash::init,
        detail::interface_nocash::puts,
        detail::interface_nocash::flush
    };

    [[maybe_unused]]
    constexpr auto none = detail::interface{
        detail::interface_none::init,
        detail::interface_none::puts,
        detail::interface_none::flush
    };

}

namespace detail {

    [[gnu::section(".ewram.data.gbaxx_log_detail_current_interface")]]
    static constinit const interface* current_interface = &none;

} // namespace detail

template <typename... T>
bool init(auto&& _, T&&... interfaces) noexcept {
    if (_.init()) {
        detail::current_interface = &_;
        return true;
    }

    if constexpr (sizeof...(T) > 0) {
        return init(std::forward<T>(interfaces)...);
    } else {
        return false;
    }
}

namespace {

void print(level level, std::string_view text) noexcept {
    detail::current_interface->puts(0, text);
    detail::current_interface->flush(level);
}

} // namespace
} // namespace gba::log

#endif // define GBAXX_EXT_LOG_HPP
