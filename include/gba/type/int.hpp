/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_INT_HPP
#define GBAXX_TYPE_INT_HPP
/** @file */

#include <cstdint>
#include <type_traits>

namespace gba {

    /**
     * @brief Signed integer of a given minimum bit-length.
     *
     * @tparam N Minimum bit-length of the desired signed integer type.
     */
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

    /**
     * @brief Unsigned integer of a given minimum bit-length.
     *
     * @tparam N Minimum bit-length of the desired unsigned integer type.
     *
     * @sa inttype
     */
    template <std::size_t N>
    using uinttype = std::make_unsigned_t<inttype<N>>;

    /**
     * @struct u4x2
     * @brief 8-bit data type of two unsigned nibbles.
     *
     * @sa tile4bpp
     */
    struct alignas(uinttype<8>) u4x2 {
        uinttype<8> lo : 4{}; /**< The lower nibble bits (0x0F). */
        uinttype<8> hi : 4{}; /**< The upper nibble bits (0xF0). */
    };

} // namespace gba

#endif // define GBAXX_TYPE_INT_HPP
