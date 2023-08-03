/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_INPUT_KEYHELPER_HPP
#define GBAXX_INPUT_KEYHELPER_HPP
/** @file */

#include <bit>
#include <functional>

#include <gba/mmio.hpp>

#include <gba/input/key.hpp>

namespace gba {

    namespace key {

        /**
         * @struct constant
         * @brief Encodes keypad keys/buttons as an integer mask.
         */
        struct alignas(int) constant {

            /**
             * @brief Logical ORs two key constants to produce a combined constant.
             *
             * @param rhs Another key constant to combine with this one.
             * @return A new key constant with the combined masks.
             */
            [[nodiscard]]
            constexpr constant with(constant rhs) const noexcept {
                return constant{mask | rhs.mask};
            }

            int mask;
        };

        /**
         * @brief Operator alternative of the constant::with() member function.
         *
         * @param lhs First key constant.
         * @param rhs Second key constant.
         * @return A new key constant that combines the masks of `lhs` and `rhs`.
         *
         * @sa constant::with()
         */
        constexpr constant operator|(constant lhs, constant rhs) noexcept {
            return constant{lhs.mask | rhs.mask};
        }

        static constexpr auto a = constant{0x0001}; /**< @var a @brief Constant for button A. @sa constant */
        static constexpr auto b = constant{0x0002}; /**< @var b @brief Constant for button B. @sa constant */
        static constexpr auto select = constant{0x0004}; /**< @var select @brief Constant for button Select. @sa constant */
        static constexpr auto start = constant{0x0008}; /**< @var start @brief Constant for button Start. @sa constant */
        static constexpr auto right = constant{0x0010}; /**< @var right @brief Constant for button Right directional pad. @sa constant */
        static constexpr auto left = constant{0x0020}; /**< @var left @brief Constant for button Left directional pad. @sa constant */
        static constexpr auto up = constant{0x0040}; /**< @var up @brief Constant for button Up directional pad. @sa constant */
        static constexpr auto down = constant{0x0080}; /**< @var down @brief Constant for button Down directional pad. @sa constant */
        static constexpr auto r = constant{0x0100}; /**< @var r @brief Constant for button R shoulder @sa constant */
        static constexpr auto l = constant{0x0200}; /**< @var l @brief Constant for button L shoulder. @sa constant */


        /**
         * @var reset_combo
         * @brief Key constant of a, b, select, and start.
         *
         * This specific combination is often used as a soft-reset trigger in GBA software.
         *
         * @sa a
         * @sa b
         * @sa select
         * @sa start
         */
        static constexpr auto reset_combo = a | b | select | start;

    } // namespace key

    namespace detail {

        template <typename T>
        concept LogicalOp = std::same_as<T, std::logical_and<void>> ||
                std::same_as<T, std::logical_or<void>> ||
                std::same_as<T, std::logical_not<void>>;

        template <detail::LogicalOp Op>
        [[nodiscard]]
        constexpr bool apply_keyop(short value, int compare) noexcept {
            if constexpr (std::same_as<Op, std::logical_and<void>>) {
                return (compare & value) == 0;
            } else if constexpr (std::same_as<Op, std::logical_or<void>>) {
                return (compare & value) != compare;
            } else if constexpr (std::same_as<Op, std::logical_not<void>>) {
                return (compare & value) == compare;
            }
        }

    } // namespace detail

    /**
     * @struct keystate
     * @brief Helper struct that provides additional member functions for keyinput.
     *
     * This struct contains state for the previous keyinput, which is used to detect button presses and releases.
     *
     * @section Polling keys:
     * @code{cpp}
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     keystate keys{};
     *     while (true) {
     *         keys = *mmio::KEYINPUT;
     *         if (keys.held<std::logical_or>(key::a | key::b)) {
     *             // Either A or B key are held
     *         }
     *         if (keys.held<std::logical_not>(key::start)) {
     *             // Start key is not held
     *         }
     *         if (keys.held(key::l, key::r)) {
     *             // Both shoulder keys are held
     *         }
     *     }
     * }
     * @endcode
     *
     * @sa keyinput
     * @sa mmio::KEYINPUT
     */
    struct keystate : keyinput {
        /**
         * @brief Initializes the keyinput keys to "pulled high", and copies that to the previous keyinput state.
         */
        constexpr keystate() noexcept : keyinput(std::bit_cast<keyinput>(short(-1))), prev{*(keyinput*) this} {}

        /**
         * @brief Initializes the keyinput keys to another keyinput state, and sets the previous keyinput state to
         *        "pulled high".
         */
        explicit constexpr keystate(keyinput in) noexcept : keyinput(in), prev{std::bit_cast<keyinput>(short(-1))} {}

        /**
         * @brief Updates keystate with new keyinput.
         *
         * The current keyinput is copied into the previous keyinput state, and is then set to the new keyinput state.
         * This essentially implements key polling.
         *
         * @param in The new `keyinput` state.
         * @return A reference to the current `keystate` after assignment.
         *
         * @section Polling keys:
         * @code{cpp}
         * #include <gba/gba.hpp>
         *
         * int main() {
         *     using namespace gba;
         *
         *     keystate keys{};
         *     while (true) {
         *         keys = *mmio::KEYINPUT;
         *         if (keys.pressed(key::b)) {
         *             // B key was pressed
         *         } else if (keys.held(key::b)) {
         *             // B key is held
         *         } else if (keys.released(key::b)) {
         *             // B key is released
         *         }
         *     }
         * }
         * @endcode
         *
         * @sa keyinput
         * @sa mmio::KEYINPUT
         */
        constexpr keystate& operator=(keyinput in) noexcept {
            prev = *(keyinput*) this;
            *(keyinput*) this = in;
            return *this;
        }

        /**
         * @brief Returns whether the specified keys are held.
         *
         * @tparam T The logical operation to be applied to the keys.
         * @tparam Args key::constant.
         * @param keys The keys to check if they are held.
         * @return True if all of the specified keys, with the logical operation, are held.
         *
         * @sa constant
         * @sa std::logical_and
         * @sa std::logical_or
         * @sa std::logical_not
         */
        template <template<typename> typename T = std::logical_and, typename... Args> requires (std::same_as<key::constant, Args> && ...)
        [[nodiscard]]
        constexpr bool held(Args... keys) const noexcept {
            return detail::apply_keyop<T<void>>(std::bit_cast<short>(*(keyinput*) this), (... | keys.mask));
        }

        /**
         * @brief Returns whether the specified keys were pressed.
         *
         * @tparam T The logical operation to be applied to the keys.
         * @tparam Args key::constant.
         * @param keys The keys to check if they were pressed.
         * @return True if all of the specified keys, with the logical operation, were pressed.
         *
         * @sa key::constant
         * @sa std::logical_and
         * @sa std::logical_or
         * @sa std::logical_not
         * @sa released()
         */
        template <template<typename> typename T = std::logical_and, typename... Args> requires (std::same_as<key::constant, Args> && ...)
        [[nodiscard]]
        constexpr bool pressed(Args... keys) const noexcept {
            const auto compare = (... | keys.mask);
            const auto previous = detail::apply_keyop<T<void>>(std::bit_cast<short>(this->prev), compare);
            const auto current = detail::apply_keyop<T<void>>(std::bit_cast<short>(*(keyinput*) this), compare);

            return !previous && current;
        }

        /**
         * @brief Returns whether the specified keys were released.
         *
         * @tparam T The logical operation to be applied to the keys.
         * @tparam Args key::constant.
         * @param keys The keys to check if they were released.
         * @return True if all of the specified keys, with the logical operation, were released.
         *
         * @sa key::constant
         * @sa std::logical_and
         * @sa std::logical_or
         * @sa std::logical_not
         * @sa pressed()
         */
        template <template<typename> typename T = std::logical_and, typename... Args> requires (std::same_as<key::constant, Args> && ...)
        [[nodiscard]]
        constexpr bool released(Args... keys) const noexcept {
            const auto compare = (... | keys.mask);
            const auto previous = detail::apply_keyop<T<void>>(std::bit_cast<short>(this->prev), compare);
            const auto current = detail::apply_keyop<T<void>>(std::bit_cast<short>(*(keyinput*) this), compare);

            return previous && !current;
        }

        /**
         * @brief Converts the shoulder buttons to -1, 0, +1.
         *
         * @return Value from -1 to +1.
         */
        [[nodiscard]]
        constexpr int lraxis() const noexcept {
            int keys = std::bit_cast<short>(*(keyinput*) this);
            keys = (keys << 22) >> 30; // Sign extend
#if defined(__thumb__)
            return (0 - keys) >> 1; // Optimize for register pressure
#else
            return (keys >> 1) - keys; // Optimize for code density
#endif
        }

        /**
         * @brief Converts the shoulder buttons to -1, 0, +1, but inverted (R = -1, L = +1).
         *
         * @return Value from -1 to +1.
         */
        [[nodiscard]]
        constexpr int i_lraxis() const noexcept {
            int keys = std::bit_cast<short>(*(keyinput*) this);
            keys = (keys << 22) >> 30; // Sign extend
#if defined(__thumb__)
            return (keys + 1) >> 1; // Optimize for register pressure
#else
            return keys - (keys >> 1); // Optimize for code density
#endif
        }

        keyinput prev; /**< @var prev @brief The previous keyinput state. @sa operator=() */
    };

} // namespace gba

#endif // define GBAXX_INPUT_KEYHELPER_HPP
