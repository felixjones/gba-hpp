/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_UTIL_BIT_CONTAINER_HPP
#define GBAXX_UTIL_BIT_CONTAINER_HPP

#include <array>
#include <bit>
#include <cstddef>
#include <cstring>
#include <memory>
#include <type_traits>

#include <gba/inttype.hpp>

namespace gba::util {
namespace detail {

    template <std::size_t Start, std::size_t N, typename D, typename S>
    [[gnu::always_inline]]
    constexpr void constexpr_array_copy(D dst[N], S src[N]) noexcept {
        if constexpr (Start < N) {
            constexpr auto index = std::integral_constant<decltype(Start), Start>();
            dst[index] = src[index];
            constexpr_array_copy<Start + 1, N, D, S>(dst, src);
        }
    }

    struct empty_long_container {
        constexpr empty_long_container() noexcept = default;

        explicit empty_long_container(const volatile empty_long_container*) noexcept {}

        void copy_longs(volatile empty_long_container*) const noexcept {}
    };

    template <std::size_t Sz>
    struct long_container {
        constexpr long_container() noexcept = default;

        explicit long_container(const volatile long_container* o) noexcept {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
            constexpr_array_copy<0, Sz>(data32, o->data32);
#pragma GCC diagnostic pop
        }

        void copy_longs(volatile long_container* to) const noexcept {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
            constexpr_array_copy<0, Sz>(to->data32, data32);
#pragma GCC diagnostic pop
        }

        uint32 data32[Sz];
    };

    struct empty_short_container {
        constexpr empty_short_container() noexcept = default;

        explicit empty_short_container(const volatile empty_short_container*) noexcept {}

        void copy_shorts(volatile empty_short_container*) const noexcept {}
    };

    template <std::size_t Sz>
    struct alignas(short) short_container {
        constexpr short_container() noexcept = default;

        explicit short_container(const volatile short_container* o) noexcept {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
            constexpr_array_copy<0, Sz>(data16, o->data16);
#pragma GCC diagnostic pop
        }

        void copy_shorts(volatile short_container* to) const noexcept {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
            constexpr_array_copy<0, Sz>(to->data16, data16);
#pragma GCC diagnostic pop
        }

        uint16 data16[Sz];
    };

    struct empty_byte_container {
        constexpr empty_byte_container() noexcept = default;

        explicit empty_byte_container(const volatile empty_byte_container*) noexcept {}

        void copy_bytes(volatile empty_byte_container*) const noexcept {}
    };

    template <std::size_t Sz>
    struct alignas(char) byte_container {
        constexpr byte_container() noexcept = default;

        explicit byte_container(const volatile byte_container* o) noexcept {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
            constexpr_array_copy<0, Sz>(data8, o->data8);
#pragma GCC diagnostic pop
        }

        void copy_bytes(volatile byte_container* to) const noexcept {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
            constexpr_array_copy<0, Sz>(to->data8, data8);
#pragma GCC diagnostic pop
        }

        uint8 data8[Sz];
    };

} // namespace detail

template <typename Type, std::size_t Longs = sizeof(Type) / 4, std::size_t Shorts = (sizeof(Type) - (Longs * 4)) / 2, std::size_t Bytes = sizeof(Type) - (Longs * 4) - (Shorts * 2)>
struct alignas(std::conditional_t<sizeof(Type) < sizeof(int), Type, int>) bit_container :
        std::conditional_t<Longs == 0, detail::empty_long_container, detail::long_container<Longs>>,
        std::conditional_t<Shorts == 0, detail::empty_short_container, detail::short_container<Shorts>>,
        std::conditional_t<Bytes == 0, detail::empty_byte_container, detail::byte_container<Bytes>>
{
    using longs_type = std::conditional_t<Longs == 0, detail::empty_long_container, detail::long_container<Longs>>;
    using shorts_type = std::conditional_t<Shorts == 0, detail::empty_short_container, detail::short_container<Shorts>>;
    using bytes_type = std::conditional_t<Bytes == 0, detail::empty_byte_container, detail::byte_container<Bytes>>;

    using value_type = Type;

    constexpr bit_container() noexcept = default;
    constexpr bit_container(const bit_container& o) noexcept = default;

    explicit bit_container(const volatile bit_container* o) noexcept : longs_type(o), shorts_type(o), bytes_type(o) {}

    void copy_from(const value_type& from) volatile noexcept {
        std::bit_cast<const bit_container>(from).copy_longs(this);
        std::bit_cast<const bit_container>(from).copy_shorts(this);
        std::bit_cast<const bit_container>(from).copy_bytes(this);
    }

    void copy_from(const bit_container& from) volatile noexcept {
        from.copy_longs(this);
        from.copy_shorts(this);
        from.copy_bytes(this);
    }

    [[nodiscard]]
    explicit operator value_type() const volatile noexcept {
        return std::bit_cast<const value_type>(*this);
    }

    template <typename... Args>
    [[nodiscard]]
    static bit_container construct(Args&&... args) noexcept {
        return bit_container(std::bit_cast<const bit_container>(Type{std::forward<Args>(args)...}));
    }
};

template <class...>
constexpr std::false_type always_false{};

} // namespace gba::util

#endif // define GBAXX_UTIL_BIT_CONTAINER_HPP
