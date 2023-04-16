/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_INT_HPP
#define GBAXX_TYPE_INT_HPP

#include <cstdint>
#include <type_traits>

namespace gba {

    template <std::size_t N>
    using inttype = typename std::conditional<N <= 64 && N % 8 == 0,
            typename std::conditional<N == 8, std::int8_t,
                    typename std::conditional<N == 16, std::int16_t,
                            typename std::conditional<N == 32, std::int32_t,
                                    typename std::conditional<N == 64, std::int64_t,
                                            void>::type>::type>::type>::type,
            void>::type;

    template <std::size_t N>
    using uinttype = std::make_unsigned_t<inttype<N>>;

} // namespace gba

#endif // define GBAXX_TYPE_INT_HPP
