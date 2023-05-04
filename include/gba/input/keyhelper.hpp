/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_INPUT_KEYHELPER_HPP
#define GBAXX_INPUT_KEYHELPER_HPP

#include <bit>
#include <functional>

#include <gba/mmio.hpp>

#include <gba/input/key.hpp>

namespace gba {

    namespace key {

        struct alignas(int) constant {
            constexpr constant with(constant rhs) const noexcept {
                return constant{mask | rhs.mask};
            }

            int mask;
        };

        constexpr constant operator|(constant lhs, constant rhs) noexcept {
            return constant{lhs.mask | rhs.mask};
        }

        namespace {

            constexpr auto a = constant{0x0001};
            constexpr auto b = constant{0x0002};
            constexpr auto select = constant{0x0004};
            constexpr auto start = constant{0x0008};
            constexpr auto right = constant{0x0010};
            constexpr auto left = constant{0x0020};
            constexpr auto up = constant{0x0040};
            constexpr auto down = constant{0x0080};
            constexpr auto r = constant{0x0100};
            constexpr auto l = constant{0x0200};

        }

    } // namespace key

    namespace detail {

        template <typename T>
        concept LogicalOp = std::same_as<T, std::logical_and<void>> ||
                std::same_as<T, std::logical_or<void>> ||
                std::same_as<T, std::logical_not<void>>;

    } // namespace detail

    struct keystate : keyinput {
        constexpr keystate() noexcept : keyinput(std::bit_cast<keyinput>(short(-1))), prev{*(keyinput*) this} {}

        explicit constexpr keystate(keyinput in) noexcept : keyinput(in), prev{std::bit_cast<keyinput>(short(-1))} {}

        constexpr keystate& operator=(keyinput in) noexcept {
            prev = *(keyinput*) this;
            *(keyinput*) this = in;
            return *this;
        }

        template <detail::LogicalOp Op, std::same_as<key::constant>... Keys>
        constexpr bool held(Op, Keys... keys) const noexcept {
            const auto curr = std::bit_cast<short>(*(keyinput*) this);
            const auto mask = (... | keys);

            if constexpr (std::same_as<Op, std::logical_and<void>>) {
                return (mask.mask & curr) == 0;
            } else if constexpr (std::same_as<Op, std::logical_or<void>>) {
                return (mask.mask & curr) != mask.mask;
            } else if constexpr (std::same_as<Op, std::logical_not<void>>) {
                return (mask.mask & curr) == mask.mask;
            } else {
                return false; //TODO
            }
        }

        keyinput prev;
    };

} // namespace gba

#endif // define GBAXX_INPUT_KEYHELPER_HPP
