/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_INPUT_KEY_HPP
#define GBAXX_INPUT_KEY_HPP

#include <gba/type/memory.hpp>

namespace gba {

    struct alignas(short) keyinput {
        bool a : 1 {true};
        bool b : 1 {true};
        bool select : 1 {true};
        bool start : 1 {true};
        bool right : 1 {true};
        bool left : 1 {true};
        bool up : 1 {true};
        bool down : 1 {true};
        bool r : 1 {true};
        bool l : 1 {true};

        [[nodiscard]]
        constexpr int xaxis() const noexcept {
            int keys = __builtin_bit_cast(short, *this);
            keys = (keys << 26) >> 30; // Sign extend
#if defined(__thumb__)
            return (0 - keys) >> 1; // Optimize for register pressure
#else
            return (keys >> 1) - keys; // Optimize for code density
#endif
        }

        [[nodiscard]]
        constexpr int yaxis() const noexcept {
            int keys = __builtin_bit_cast(short, *this);
            keys = (keys << 24) >> 30; // Sign extend
#if defined(__thumb__)
            return (0 - keys) >> 1; // Optimize for register pressure
#else
            return (keys >> 1) - keys; // Optimize for code density
#endif
        }

        [[nodiscard]]
        constexpr int r_xaxis() const noexcept {
            int keys = __builtin_bit_cast(short, *this);
            keys = (keys << 26) >> 30; // Sign extend
#if defined(__thumb__)
            return (keys + 1) >> 1; // Optimize for register pressure
#else
            return keys - (keys >> 1); // Optimize for code density
#endif
        }

        [[nodiscard]]
        constexpr int r_yaxis() const noexcept {
            int keys = __builtin_bit_cast(short, *this);
            keys = (keys << 24) >> 30; // Sign extend
#if defined(__thumb__)
            return (keys + 1) >> 1; // Optimize for register pressure
#else
            return keys - (keys >> 1); // Optimize for code density
#endif
        }
    };

} // namespace gba

#endif // define GBAXX_INPUT_KEY_HPP
