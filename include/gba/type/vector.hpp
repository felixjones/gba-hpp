/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_VECTOR_HPP
#define GBAXX_TYPE_VECTOR_HPP

#include <array>
#include <concepts>
#include <type_traits>

namespace gba {

    template <std::integral T, std::size_t N>
    using make_vector [[gnu::vector_size(sizeof(T) * N)]] = T;

    template <typename T>
    concept Vector = requires(T t, std::remove_cvref_t<decltype(t[0])> a) {
        requires std::same_as<T, make_vector<decltype(a), sizeof(t) / sizeof(a)>>;
    };

    template <Vector T>
    struct vector_traits {
        using value_type = typename std::remove_cvref_t<decltype(T{}[0])>;
        using size_type = typename std::size_t;

        static constexpr auto size = size_type(sizeof(T) / sizeof(value_type));
    };

    constexpr auto vector_to_array(Vector auto v) noexcept {
        using array_type = std::array<typename vector_traits<decltype(v)>::value_type, vector_traits<decltype(v)>::size>;
        return __builtin_bit_cast(array_type, v);
    }

    template <typename T, std::size_t N>
    constexpr auto array_to_vector(std::array<T, N> a) noexcept {
        using vector_type = make_vector<T, N>;
        return __builtin_bit_cast(vector_type, a);
    }

    template <typename As>
    constexpr auto vector_bind(Vector auto& v) noexcept {
        using vector_type = std::remove_cvref_t<decltype(v)>;

        constexpr auto vector_size = vector_traits<vector_type>::size;

        struct scoped_ref : std::array<As, vector_size> {
            constexpr explicit scoped_ref(vector_type& v) noexcept :
                std::array<As, vector_size>(__builtin_bit_cast(std::array<As, vector_size>, v)),
                m_owner{v} {}

            constexpr ~scoped_ref() noexcept {
                m_owner = __builtin_bit_cast(vector_type, *(std::array<As, vector_size>*) this);
            }

            consteval explicit operator bool() const noexcept {
                return !std::is_const_v<As>;
            }

            vector_type& m_owner;
        };

        return scoped_ref{v};
    }

} // namespace gba

#endif // define GBAXX_TYPE_VECTOR_HPP
