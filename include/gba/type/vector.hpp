/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_VECTOR_HPP
#define GBAXX_TYPE_VECTOR_HPP

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

} // namespace gba

#endif // define GBAXX_TYPE_VECTOR_HPP
