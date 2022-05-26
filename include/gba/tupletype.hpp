/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TUPLETYPE_HPP
#define GBAXX_TUPLETYPE_HPP

#include <cstddef>
#include <utility>
#include <type_traits>

namespace gba {
namespace detail {

    template <std::size_t Idx, typename Type, bool IsClass>
    struct tuple_base {
        Type value;
    };

    template <std::size_t Idx, typename Type>
    struct tuple_base<Idx, Type, true> : Type {};

    template <std::size_t Idx, typename Type>
    [[maybe_unused]]
    constexpr auto& tuple_get(tuple_base<Idx, Type, false>& t) noexcept {
        return t.value;
    }

    template <std::size_t Idx, typename Type>
    [[maybe_unused]]
    constexpr const auto& tuple_get(const tuple_base<Idx, Type, false>& t) noexcept {
        return t.value;
    }

    template <std::size_t Idx, typename Type>
    [[maybe_unused]]
    constexpr auto& tuple_get(tuple_base<Idx, Type, true>& t) noexcept {
        return t;
    }

    template <std::size_t Idx, typename Type>
    [[maybe_unused]]
    constexpr const auto& tuple_get(const tuple_base<Idx, Type, true>& t) noexcept {
        return t;
    }

    template <class...>
    struct tuple_impl;

    template <std::size_t... Indices, typename... Ts>
    struct tuple_impl<std::index_sequence<Indices...>, Ts...> : tuple_base<Indices, Ts, std::is_class_v<Ts>>... {
        constexpr tuple_impl() noexcept = default;
        constexpr ~tuple_impl() noexcept = default;
        constexpr tuple_impl(tuple_impl&&) noexcept = default;
        constexpr tuple_impl(const tuple_impl&) noexcept = default;

        template <typename... Args>
        explicit constexpr tuple_impl(Args&&... arg) noexcept : tuple_base<Indices, Ts, std::is_class_v<Ts>>{std::forward<Args>(arg)}... {}
    };

    template <std::size_t Idx, typename Type, bool Get>
    struct nth_base {};

    template <std::size_t Idx, typename Type>
    struct nth_base<Idx, Type, true> {
        using type = Type;
    };

    template <std::size_t, typename...>
    struct nth_impl;

    template <std::size_t Idx, std::size_t... Indices, typename... Ts>
    struct nth_impl<Idx, std::index_sequence<Indices...>, Ts...> : nth_base<Indices, Ts, Idx == Indices>... {};

    template <std::size_t Idx, typename... Ts>
    using nth = nth_impl<Idx, std::index_sequence_for<Ts...>, Ts...>;

} // namespace detail

template <typename... Ts>
struct tuple : detail::tuple_impl<std::index_sequence_for<Ts...>, Ts...> {
    constexpr tuple() noexcept = default;
    constexpr ~tuple() noexcept = default;
    constexpr tuple(tuple&&) noexcept = default;
    constexpr tuple(const tuple&) noexcept = default;

    template <typename... Args>
    explicit constexpr tuple(Args&&... arg) noexcept : tuple::tuple_impl{std::forward<Args>(arg)...} {}
};

} // namespace gba

namespace std { // NOLINT(cert-dcl58-cpp)

    template <typename... Ts>
    class [[maybe_unused]] tuple_size<gba::tuple<Ts...>> : public integral_constant<size_t, sizeof...(Ts)> {};

    template <size_t Idx, typename... Ts>
    class [[maybe_unused]] tuple_element<Idx, gba::tuple<Ts...>> : public gba::detail::nth<Idx, Ts...> {};

    template <size_t Idx, typename... Ts>
    [[maybe_unused]]
    constexpr auto& get(gba::tuple<Ts...>& t) noexcept {
        return gba::detail::tuple_get<Idx>(t);
    }

    template <size_t Idx, typename... Ts>
    [[maybe_unused]]
    constexpr const auto& get(const gba::tuple<Ts...>& t) noexcept {
        return gba::detail::tuple_get<Idx>(t);
    }

} // namespace std

#endif // define GBAXX_TUPLETYPE_HPP
