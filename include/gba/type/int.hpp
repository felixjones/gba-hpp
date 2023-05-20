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
    using inttype = std::conditional_t<N <= 64 && N % 8 == 0,
                std::conditional_t<N == 8,
                    std::int8_t,
                    std::conditional_t<N == 16,
                        std::int16_t,
                        std::conditional_t<N == 32,
                            std::int32_t,
                            std::conditional_t<N == 64,
                                std::int64_t,
                                void
                            >
                        >
                    >
                >, void>;

    template <std::size_t N>
    using uinttype = std::make_unsigned_t<inttype<N>>;

    struct alignas(uinttype<8>) u4x2 {
        uinttype<8> lo : 4{};
        uinttype<8> hi : 4{};
    };

} // namespace gba

#endif // define GBAXX_TYPE_INT_HPP
