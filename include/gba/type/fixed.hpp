/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_FIXED_HPP
#define GBAXX_TYPE_FIXED_HPP
/** @file */

#include <array>
#include <cstddef>
#include <cstdlib>
#include <type_traits>

#include <gba/type/memory.hpp>
#include <gba/type/util.hpp>
#include <gba/type/vector.hpp>

namespace gba {

/**
 * @brief Provides type traits for either std::integral or Vector types.
 *
 * Either implements for an integer type, or forwards vector_traits.
 *
 * @tparam T The type of the value
 */
template <typename T> requires std::integral<T> || Vector<T>
struct value_traits;

/**
 * @brief Provides type traits for either std::integral or Vector types.
 *
 * This has the same signature and members as vector_traits.
 *
 * @tparam T The type of the value
 */
template <typename T> requires std::integral<T>
struct value_traits<T> {
    using size_type = std::size_t;
    using value_type = T;
    static constexpr size_type size = 1; /**< Pseudo vector size. A single integer is one element. */
};

/**
 * @brief Provides type traits for either std::integral or Vector types.
 *
 * Forwards vector_traits.
 *
 * @tparam T The type of the value
 *
 * @sa vector_traits
 */
template <typename T> requires Vector<T>
struct value_traits<T> : vector_traits<T> {};

template <Fundamental DataType, std::size_t FractionalBits>
struct fixed;

/**
 * @brief Fixed point type concept.
 *
 * @tparam T Expected to be fixed.
 *
 * @sa fixed
 */
template <typename T>
concept Fixed = std::same_as<T, fixed<typename T::data_type, T::fractional_bits>>;

/**
 * @struct fixed
 * @brief A fixed-point number class template.
 * @see <a href="https://en.wikipedia.org/wiki/Fixed-point_arithmetic">Fixed-point arithmetic - Wikipedia</a>
 *
 * @tparam DataType The underlying data type used to store the fixed-point number.
 * @tparam FractionalBits The number of fractional bits.
 */
template <Fundamental DataType, std::size_t FractionalBits>
struct fixed {
    using data_type = DataType;
    static constexpr auto fractional_bits = FractionalBits;

    constexpr data_type& data() noexcept {
        return m_data;
    }

    constexpr data_type data() const noexcept {
        return m_data;
    }

    // Vector related detail
    using size_type = typename value_traits<data_type>::size_type;
    using value_type = fixed<typename value_traits<data_type>::value_type, fractional_bits>;
    static constexpr auto size = value_traits<data_type>::size;

    // Helper details
    static constexpr typename value_traits<data_type>::value_type data_unit = 1 << fractional_bits;

    // default functions
    constexpr fixed() noexcept = default;
    constexpr fixed(const fixed&) noexcept = default;
    constexpr fixed(fixed&&) noexcept = default;
    constexpr fixed& operator=(const fixed&) noexcept = default;
    constexpr fixed& operator=(fixed&&) noexcept = default;

    // raw data

    /**
     * @brief Helper constructor for directly setting the underlying data for a fixed-point value.
     *
     * @note The second parameter, nullptr, is used to differentiate from the normal integer conversion constructor.
     *
     * @param data The data to be stored in the fixed object.
     */
    constexpr fixed(data_type data, std::nullptr_t) noexcept : m_data{data} {}

    static constexpr fixed from_data(data_type data) noexcept {
        return fixed(data, nullptr);
    }

    // Fixed point conversion

    /**
     * @brief Convert from a fixed point number of another format.
     *
     * @note The source value must match the vector size of the destination value.
     *
     * @param rhs Source fixed point number.
     */
    constexpr fixed(Fixed auto rhs) noexcept requires (Vector<data_type> == Vector<typename decltype(rhs)::data_type>) :
            m_data{vector_cast<data_type>(shift_to<decltype(rhs)::fractional_bits, fractional_bits>(rhs.m_data))} {}

    constexpr fixed& operator=(Fixed auto rhs) noexcept {
        m_data = vector_cast<data_type>(shift_to<decltype(rhs)::fractional_bits, fractional_bits>(rhs.m_data));
        return *this;
    }

    // Floating point conversion

    /**
     * @brief Convert from a floating point value.
     *
     * @note This is only available in constexpr context.
     *
     * @param rhs Floating point number.
     */
    consteval fixed(std::floating_point auto rhs) requires (!Vector<data_type>) : m_data{round_float<data_type>(rhs * data_unit)} {}

    /**
     * @brief Convert from a floating point value to a vector.
     *
     * This copies the `rhs` value into all elements of the m_data vector.
     *
     * @note This is only available in constexpr context.
     */
    consteval fixed(std::floating_point auto rhs) requires Vector<data_type> {
        const auto val = round_float<data_type>(rhs * data_unit);
        for (size_type ii = 0; ii < size; ++ii) {
            m_data[ii] = val;
        }
    }

    consteval fixed& operator=(std::floating_point auto rhs) {
        m_data = round_float<data_type>(rhs * data_unit);
        return *this;
    }

    /**
     * @brief Convert from fixed point to floating point.
     *
     * @note This is only available in constexpr context.
     *
     * @return Floating point representation of `m_data`.
     */
    template <std::floating_point Lhs>
    explicit consteval operator Lhs() const {
        return m_data / Lhs{data_unit};
    }

    // Integral conversion

    /**
     * @brief Convert from integer to fixed point.
     */
    constexpr fixed(std::integral auto rhs) requires (!Vector<data_type>) : m_data{typename value_type::data_type(rhs << fractional_bits)} {}

    /**
     * @brief Convert from integer to fixed point vector.
     *
     * This copies the `rhs` value into all elements of the m_data vector.
     */
    constexpr fixed(std::integral auto rhs) requires Vector<data_type> {
        const auto val = rhs << fractional_bits;
        for (size_type ii = 0; ii < size; ++ii) {
            m_data[ii] = typename value_type::data_type(val);
        }
    }

    constexpr fixed& operator=(std::integral auto rhs) noexcept {
        m_data = rhs << fractional_bits;
        return *this;
    }

    /**
     * @brief Convert from fixed point to integer.
     *
     * @note This behaves similar to floating point to integer conversion.
     *
     * @tparam Lhs Destination integer type.
     * @return Integer conversion of the fixed point value.
     */
    template <std::integral Lhs> requires (!std::same_as<Lhs, bool>)
    explicit constexpr operator Lhs() const noexcept {
        return static_cast<Lhs>(m_data >> fractional_bits);
    }

    // Vector conversion

    /**
     * @brief Initializes fixed point value with other fixed-point values.
     *
     * The number of arguments must match the size of the vector.
     *
     * @note This will call a conversion constructor for each argument passed.
     *
     * @tparam Args The type of the fixed-point arguments.
     */
    template <Fixed... Args>
    constexpr fixed(Args... args) requires Vector<data_type> : m_data{value_type(std::forward<Args>(args)).m_data...} {
        static_assert(sizeof...(Args) == size);
    }

    /**
     * @brief Initializes fixed point value with integer values.
     *
     * The number of arguments must match the size of the vector.
     *
     * @note This will call a conversion constructor for each argument passed.
     *
     * @tparam Args The type of the integer arguments.
     */
    template <std::integral... Args>
    constexpr fixed(Args... args) requires Vector<data_type> : m_data{value_type(std::forward<Args>(args)).m_data...} {
        static_assert(sizeof...(Args) == size);
    }

    /**
     * @brief Initializes fixed point value with floating-point values.
     *
     * The number of arguments must match the size of the vector.
     *
     * @note This will call a conversion constructor for each argument passed.
     *
     * @tparam Args The type of the floating-point arguments.
     */
    template <std::floating_point... Args>
    consteval fixed(Args... args) requires Vector<data_type> : m_data{value_type(std::forward<Args>(args)).m_data...} {
        static_assert(sizeof...(Args) == size);
    }

    /**
     * @brief Converts to an array of the same type and size as the vector.
     *
     * @note The returned array is a copy, not a reference, to the underlying vector.
     *
     * @return The array representation of the vector.
     */
    constexpr std::array<value_type, size> to_array() const noexcept requires Vector<data_type> {
        return __builtin_bit_cast(std::array<value_type, size>, m_data);
    }

    /**
     * @brief Calls to_array().
     *
     * @return The array representation of the vector.
     *
     * @sa to_array()
     */
    explicit constexpr operator std::array<value_type, size>() const noexcept requires Vector<data_type> {
        return to_array();
    }

    /**
     * @brief Converts from an array into a vector.
     *
     * @note The array must be the same type and size as the vector.
     */
    constexpr fixed(const std::array<value_type, size>& data) requires Vector<data_type> :
            m_data{__builtin_bit_cast(data_type, data)} {}

    /**
     * @brief Convert from fixed point to integer.
     *
     * @note This behaves similar to floating point to integer conversion.
     *
     * @tparam Lhs Destination integer type.
     * @return Integer conversion of the fixed point value.
     */
    template <Vector Lhs> requires std::integral<typename vector_traits<Lhs>::value_type> && (!std::same_as<typename vector_traits<Lhs>::value_type, bool>)
    explicit constexpr operator Lhs() const noexcept requires Vector<data_type> {
        return vector_cast<Lhs>(m_data >> fractional_bits);
    }

    // Vector access

    /**
     * @struct scoped_ref
     * @brief Scoped accessor to the underlying fixed-point vector. RAII is used to write-back any changes.
     *
     * Due to limitations with GNU vector extensions, it is not possible to return a reference to a vector element with
     * a different pointer type. To avoid this limitation, is used to maintain a temporary array that represents the
     * value of the vector at construction, and writes back any changes at destruction.
     *
     * @sa tie()
     */
    struct scoped_ref : std::array<value_type, size> {
        using owner_type = fixed;

        scoped_ref() noexcept = default;

        constexpr ~scoped_ref() noexcept {
            if (!m_owner) {
                return;
            }

            volatile_store(m_owner, fixed(*this));
        }

        constexpr scoped_ref& operator=(scoped_ref&& other) noexcept {
            m_owner = other.m_owner;
            for (size_type ii = 0; ii < size; ++ii) {
                this->at(ii) = other[ii];
            }
            other.m_owner = nullptr;
            return *this;
        }

        constexpr explicit operator bool() const noexcept {
            return m_owner != nullptr;
        }

    private:
        friend fixed;
        constexpr explicit scoped_ref(volatile fixed* owner) noexcept :
            std::array<value_type, size>{volatile_load(owner).to_array()}, m_owner{owner} {}

        volatile fixed* m_owner{};
    };

    /**
     * @brief Creates a temporary scoped_ref of the vector data.
     *
     * This is intended for similar usage to that of std::tie(), but it is not identical.
     *
     * @return scoped_ref of this fixed point vector.
     *
     * @code{cpp}
     * // Mutating a fixed-point vector with tie()
     *
     * #include <gba/gba.hpp>
     *
     * int main() {
     *     using namespace gba;
     *
     *     using fixed16_vec = fixed<make_vector<int, 2>, 16>;
     *     using fixed16 = fixed16_vec::value_type;
     *
     *     auto my_vector = fixed16_vec{fixed16{1.23}, fixed16{4.56}};
     *     if (auto tied = my_vector.tie()) {
     *         auto& [x, y] = tied;
     *         std::swap(x, y);
     *         // tied is written back to my_vector on scope exit
     *     }
     * }
     * @endcode
     *
     * @sa scoped_ref
     */
    constexpr auto tie() noexcept requires Vector<data_type> {
        return scoped_ref(this);
    }

    /**
     * @brief volatile qualified tie().
     *
     * @return scoped_ref of this fixed point vector.
     *
     * @sa scoped_ref
     */
    constexpr auto tie() volatile noexcept requires Vector<data_type> {
        return scoped_ref(this);
    }

    /**
     * @brief Array-access operator.
     *
     * @warning This returns a const value to prevent []= pattern, which is incompatible with GNU vector extensions.
     *
     * @param idx Vector element index.
     * @return const-qualified copy of the element at the index.
     *
     * @sa tie()
     * @sa set()
     */
    constexpr const value_type operator[](size_type idx) const noexcept requires Vector<data_type> {
        return value_type::from_data(m_data[idx]);
    }

    /**
     * @brief Direct array element setting.
     *
     * @note The []= pattern is not available, so this member function is provided for convenience.
     *
     * @param idx Vector element index.
     * @param value Value to set the vector at the given index.
     * @return Reference to this fixed-point vector.
     *
     * @sa tie()
     */
    constexpr fixed& set(size_type idx, Fixed auto value) noexcept requires Vector<data_type> && (value.size == 1) {
        m_data[idx] = fixed(value).m_data;
        return *this;
    }

    data_type m_data{};

    // Unary operators

    /**
     * @brief Boolean conversion.
     *
     * @return True if any element is not zero.
     */
    [[nodiscard]]
    explicit constexpr operator bool() const noexcept {
        if constexpr (Vector<data_type>) {
            bool r = false;
            for (size_type ii = 0; ii < size; ++ii) {
                r |= bool(m_data[ii]);
            }
            return r;
        } else {
            return m_data;
        }
    }

    [[nodiscard]]
    constexpr bool operator!() const noexcept {
        return !this->operator bool();
    }

    constexpr auto operator-() const noexcept {
        return fixed<decltype(-m_data), fractional_bits>::from_data(-m_data);
    }

    constexpr fixed operator+() const noexcept {
        return *this;
    }

    // Comparison operators
    constexpr auto operator<=>(Fixed auto rhs) const noexcept requires (!Vector<data_type>) {
        return m_data <=> fixed(rhs).m_data;
    }

    constexpr auto operator==(Fixed auto rhs) const noexcept {
        const auto other = fixed(rhs);

        if constexpr (Vector<data_type>) {
            bool r = true;
            for (size_type ii = 0; ii < size; ++ii) {
                r &= m_data[ii] == other.m_data[ii];
            }
            return r;
        } else {
            return m_data == other.m_data;
        }
    }

    constexpr auto operator!=(Fixed auto rhs) const noexcept {
        return !this->operator ==(rhs);
    }

    constexpr auto operator<=>(Fundamental auto rhs) const noexcept requires (!Vector<data_type>) {
        return this->operator <=>(fixed(rhs));
    }

    constexpr auto operator==(Fundamental auto rhs) const noexcept {
        return this->operator ==(fixed(rhs));
    }

    constexpr auto operator!=(Fundamental auto rhs) const noexcept {
        return this->operator !=(fixed(rhs));
    }

    // Simple arithmetic
    constexpr fixed& operator+=(Fixed auto rhs) noexcept {
        m_data += shift_to<decltype(rhs)::fractional_bits, fractional_bits>(vector_cast<data_type>(rhs.m_data));
        return *this;
    }

    constexpr fixed& operator-=(Fixed auto rhs) noexcept {
        m_data -= shift_to<decltype(rhs)::fractional_bits, fractional_bits>(vector_cast<data_type>(rhs.m_data));
        return *this;
    }

    constexpr fixed& operator+=(Fundamental auto rhs) noexcept {
        m_data += (rhs << fractional_bits);
        return *this;
    }

    constexpr fixed& operator-=(Fundamental auto rhs) noexcept {
        m_data -= (rhs << fractional_bits);
        return *this;
    }

    // Integer arithmetic
    constexpr fixed& operator*=(Fundamental auto rhs) noexcept {
        m_data *= rhs;
        return *this;
    }

    constexpr fixed& operator/=(Fundamental auto rhs) noexcept {
        m_data /= rhs;
        return *this;
    }

    constexpr fixed& operator<<=(size_type rhs) noexcept {
        m_data <<= rhs;
        return *this;
    }

    constexpr fixed& operator>>=(size_type rhs) noexcept {
        m_data >>= rhs;
        return *this;
    }

    // Fixed multiply
    constexpr fixed& operator*=(Fixed auto rhs) noexcept {
        using bigger_type = typename make_bigger<decltype(data_type() * typename decltype(rhs)::data_type())>::type;

        if constexpr (Vector<data_type>) {
            const auto data = __builtin_convertvector(m_data, bigger_type) * rhs.m_data;
            m_data = __builtin_convertvector(data >> decltype(rhs)::fractional_bits, data_type);
        } else {
            const auto data = bigger_type(m_data) * rhs.m_data;
            m_data = data_type(data >> decltype(rhs)::fractional_bits);
        }

        return *this;
    }

    // Fixed divide
    constexpr fixed& operator/=(Fixed auto rhs) noexcept {
        using bigger_type = typename make_bigger<decltype(data_type() / typename decltype(rhs)::data_type())>::type;

        if constexpr (Vector<data_type>) {
            const auto data = __builtin_convertvector(m_data, bigger_type) << decltype(rhs)::fractional_bits;
            m_data = __builtin_convertvector(data / rhs.m_data, data_type);
        } else {
            const auto data = bigger_type(m_data) << decltype(rhs)::fractional_bits;
            m_data = data_type(data / rhs.m_data);
        }

        return *this;
    }
};

template <typename T>
concept FixedScopedRef = std::same_as<T, typename T::owner_type::scoped_ref> && Fixed<typename T::owner_type>;

// operator+
template <Fixed Lhs, Fixed Rhs>
constexpr auto operator+(Lhs lhs, Rhs rhs) noexcept {
    constexpr auto frac_bits = (Lhs::fractional_bits + Rhs::fractional_bits) / 2;
    using data_type = decltype(typename Lhs::data_type() + typename Rhs::data_type());

    const auto lhsData = shift_to<Lhs::fractional_bits, frac_bits>(lhs.data());
    const auto rhsData = shift_to<Rhs::fractional_bits, frac_bits>(rhs.data());

    return fixed<data_type, frac_bits>::from_data(lhsData + rhsData);
}

template <Fixed Lhs, Fundamental Rhs>
constexpr auto operator+(Lhs lhs, Rhs rhs) noexcept {
    constexpr auto frac_bits = Lhs::fractional_bits;
    using data_type = decltype(typename Lhs::data_type() + Rhs());

    const auto lhsData = shift_to<Lhs::fractional_bits, frac_bits>(lhs.data());
    const auto rhsData = shift_to<0, frac_bits>(rhs);

    return fixed<data_type, frac_bits>::from_data(lhsData + rhsData);
}

template <Fundamental Lhs, Fixed Rhs>
constexpr auto operator+(Lhs lhs, Rhs rhs) noexcept {
    constexpr auto frac_bits = Rhs::fractional_bits;
    using data_type = decltype(Lhs() + typename Rhs::data_type());

    const auto lhsData = shift_to<0, frac_bits>(lhs);
    const auto rhsData = shift_to<Rhs::fractional_bits, frac_bits>(rhs.data());

    return fixed<data_type, frac_bits>::from_data(lhsData + rhsData);
}

// operator-
template <Fixed Lhs, Fixed Rhs>
constexpr auto operator-(Lhs lhs, Rhs rhs) noexcept {
    constexpr auto frac_bits = (Lhs::fractional_bits + Rhs::fractional_bits) / 2;
    using data_type = decltype(typename Lhs::data_type() - typename Rhs::data_type());

    const auto lhsData = shift_to<Lhs::fractional_bits, frac_bits>(lhs.data());
    const auto rhsData = shift_to<Rhs::fractional_bits, frac_bits>(rhs.data());

    return fixed<data_type, frac_bits>::from_data(lhsData - rhsData);
}

template <Fixed Lhs, Fundamental Rhs>
constexpr auto operator-(Lhs lhs, Rhs rhs) noexcept {
    constexpr auto frac_bits = Lhs::fractional_bits;
    using data_type = decltype(typename Lhs::data_type() - Rhs());

    const auto lhsData = shift_to<Lhs::fractional_bits, frac_bits>(lhs.data());
    const auto rhsData = shift_to<0, frac_bits>(rhs);

    return fixed<data_type, frac_bits>::from_data(lhsData - rhsData);
}

template <Fundamental Lhs, Fixed Rhs>
constexpr auto operator-(Lhs lhs, Rhs rhs) noexcept {
    constexpr auto frac_bits = Rhs::fractional_bits;
    using data_type = decltype(Lhs() - typename Rhs::data_type());

    const auto lhsData = shift_to<0, frac_bits>(lhs);
    const auto rhsData = shift_to<Rhs::fractional_bits, frac_bits>(rhs.data());

    return fixed<data_type, frac_bits>::from_data(lhsData - rhsData);
}

// operator*
template <Fixed Lhs, Fixed Rhs>
constexpr auto operator*(Lhs lhs, Rhs rhs) noexcept {
    using data_type = decltype(typename Lhs::data_type() * typename Rhs::data_type());
    using bigger_type = typename make_bigger<data_type>::type;

    constexpr auto bits_combined = Lhs::fractional_bits + Rhs::fractional_bits;
    constexpr auto frac_bits = bits_combined / 2;

    if constexpr (Vector<data_type>) {
        const auto data = __builtin_convertvector(lhs.data(), bigger_type) * vector_cast<bigger_type>(rhs.data());
        return fixed<data_type, frac_bits>::from_data(__builtin_convertvector(shift_to<bits_combined, frac_bits>(data), data_type));
    } else {
        const auto data = bigger_type(lhs.data()) * rhs.data();
        return fixed<data_type, frac_bits>::from_data(static_cast<data_type>(shift_to<bits_combined, frac_bits>(data)));
    }
}

constexpr auto operator*(Fixed auto lhs, Fundamental auto rhs) noexcept {
    return decltype(lhs)::from_data(lhs.data() * vector_cast<typename decltype(lhs)::data_type>(rhs));
}

constexpr auto operator*(Fundamental auto lhs, Fixed auto rhs) noexcept {
    return decltype(rhs)::from_data(lhs * rhs.data());
}

// operator/
template <Fixed Lhs, Fixed Rhs>
constexpr auto operator/(Lhs lhs, Rhs rhs) noexcept {
    using data_type = decltype(typename Lhs::data_type() / typename Rhs::data_type());
    using bigger_type = typename make_bigger<data_type>::type;

    constexpr auto bits_combined = Lhs::fractional_bits + Rhs::fractional_bits;
    constexpr auto frac_bits = bits_combined / 2;

    const auto data = shift_to<Lhs::fractional_bits, frac_bits + Rhs::fractional_bits>(vector_cast<bigger_type>(lhs.data()));
    if constexpr (Vector<data_type>) {
        return fixed<data_type, frac_bits>::from_data(__builtin_convertvector(data / __builtin_convertvector(rhs.data(), bigger_type), data_type));
    } else {
        return fixed<data_type, frac_bits>::from_data(static_cast<data_type>(data / rhs.data()));
    }
}

constexpr auto operator/(Fixed auto lhs, Fundamental auto rhs) noexcept {
    return decltype(lhs)::from_data(lhs.data() / rhs);
}

template <Fixed Rhs>
constexpr auto operator/(Fundamental auto lhs, Fixed auto rhs) noexcept {
    return Rhs(lhs) / rhs;
}

// Lhs equality operators
template <Fixed Rhs>
constexpr auto operator<=>(Fundamental auto lhs, Rhs rhs) noexcept {
    return Rhs(lhs) <=> rhs;
}

template <Fixed Rhs>
constexpr auto operator==(Fundamental auto lhs, Rhs rhs) noexcept {
    return Rhs(lhs) == rhs;
}

template <Fixed Rhs>
constexpr auto operator!=(Fundamental auto lhs, Rhs rhs) noexcept {
    return Rhs(lhs) != rhs;
}

// Shifting
template <Fixed Lhs>
constexpr auto operator<<(Lhs lhs, std::integral auto rhs) noexcept {
    return Lhs::from_data(lhs.data() << rhs);
}

template <Fixed Lhs>
constexpr auto operator>>(Lhs lhs, std::integral auto rhs) noexcept {
    return Lhs::from_data(lhs.data() >> rhs);
}

// Arithmetic functions

/**
 * @brief Performs std::abs() on a fixed point value.
 *
 * @note If x is unsigned, this will simply return x.
 *
 * @param x Fixed-point value.
 * @return The absolute value of x.
 */
constexpr auto abs(Fixed auto x) noexcept {
    if constexpr (std::is_signed_v<typename decltype(x)::data_type>) {
        return decltype(x)::from_data(std::abs(x.data()));
    } else {
        return x;
    }
}

/**
 * @brief Returns the fractional value of x.
 *
 * @param x Fixed-point value.
 * @return The fractional portion of x.
 */
constexpr auto frac(Fixed auto x) noexcept {
    const auto mask = decltype(x)::data_unit - 1;
    return decltype(x)::from_data(x.data() & mask);
}

/**
 * @brief Returns the smaller value between lhs and rhs.
 *
 * @param lhs Fixed-point first value.
 * @param rhs Fixed-point second value.
 * @return The smaller value between lhs and rhs.
 *
 * @sa max()
 */
constexpr auto min(Fixed auto lhs, Fixed auto rhs) noexcept {
    return lhs < rhs ? lhs : rhs;
}

/**
 * @brief Returns the larger value between lhs and rhs.
 *
 * @param lhs Fixed-point first value.
 * @param rhs Fixed-point second value.
 * @return The larger value between lhs and rhs.
 *
 * @sa min()
 */
constexpr auto max(Fixed auto lhs, Fixed auto rhs) noexcept {
    return lhs > rhs ? lhs : rhs;
}

} // namespace gba

namespace std {
    template <gba::FixedScopedRef T>
    struct tuple_size<T> : std::integral_constant<typename T::owner_type::size_type, T::owner_type::size> {};

    template <std::size_t Index, gba::FixedScopedRef T>
    struct tuple_element<Index, T>
            : tuple_element<Index, gba::n_tuple<typename T::owner_type::value_type, T::owner_type::size>> {};

}

#endif // define GBAXX_TYPE_FIXED_HPP
