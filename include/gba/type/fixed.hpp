/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_FIXED_HPP
#define GBAXX_TYPE_FIXED_HPP

#include <array>
#include <cstddef>
#include <type_traits>

#include <gba/type/util.hpp>
#include <gba/type/vector.hpp>

namespace gba {

template <typename T> requires std::integral<T> || Vector<T>
struct value_traits;

template <typename T> requires std::integral<T>
struct value_traits<T> {
    using size_type = std::size_t;
    using value_type = T;
    static constexpr size_type size = 1;
};

template <typename T> requires Vector<T>
struct value_traits<T> : vector_traits<T> {};

template <Fundamental DataType, std::size_t FractionalBits>
struct fixed;

template <typename T>
concept Fixed = std::same_as<T, fixed<typename T::data_type, T::fractional_bits>>;

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
    constexpr fixed(data_type data, std::nullptr_t) noexcept : m_data{data} {}

    static constexpr fixed from_data(data_type data) noexcept {
        return fixed(data, nullptr);
    }

    // Fixed point conversion
    explicit constexpr fixed(Fixed auto rhs) noexcept requires (Vector<data_type> == Vector<typename decltype(rhs)::data_type>) :
            m_data{shift_to<decltype(rhs)::fractional_bits, fractional_bits>(rhs.m_data)} {}

    constexpr fixed& operator=(Fixed auto rhs) noexcept {
        m_data = shift_to<decltype(rhs)::fractional_bits, fractional_bits>(rhs.m_data);
        return *this;
    }

    // Floating point conversion
    explicit consteval fixed(std::floating_point auto rhs) requires (!Vector<data_type>) : m_data{round_float<data_type>(rhs * data_unit)} {}

    explicit constexpr fixed(std::floating_point auto rhs) requires Vector<data_type> {
        const auto val = round_float<data_type>(rhs * data_unit);
        for (size_type ii = 0; ii < size; ++ii) {
            m_data[ii] = val;
        }
    }

    consteval fixed& operator=(std::floating_point auto rhs) {
        m_data = round_float<data_type>(rhs * data_unit);
        return *this;
    }

    template <std::floating_point Lhs>
    explicit consteval operator Lhs() const {
        return m_data / Lhs{data_unit};
    }

    // Integral conversion
    explicit constexpr fixed(std::integral auto rhs) requires (!Vector<data_type>) : m_data{rhs << fractional_bits} {}

    explicit constexpr fixed(std::integral auto rhs) requires Vector<data_type> {
        const auto val = rhs << fractional_bits;
        for (size_type ii = 0; ii < size; ++ii) {
            m_data[ii] = val;
        }
    }

    constexpr fixed& operator=(std::integral auto rhs) noexcept {
        m_data = rhs << fractional_bits;
        return *this;
    }

    template <std::integral Lhs> requires (!std::same_as<Lhs, bool>)
    explicit constexpr operator Lhs() const noexcept {
        return m_data >> fractional_bits;
    }

    // Vector conversion
    template <Fundamental... Args>
    explicit consteval fixed(Args... args) requires Vector<data_type> : m_data{value_type(std::forward<Args>(args)).m_data...} {
        static_assert(sizeof...(Args) == size);
    }

    constexpr std::array<value_type, size> to_array() const noexcept requires Vector<data_type> {
        std::array<value_type, size> r;
        for (size_type ii = 0; ii < size; ++ii) {
            r[ii].m_data = m_data[ii];
        }
        return r;
    }

    explicit constexpr operator std::array<value_type, size>() const noexcept requires Vector<data_type> {
        return to_array();
    }

    // Vector access
    struct scoped_ref : std::array<value_type, size> {
        using owner_type = fixed;

        scoped_ref() noexcept = default;

        constexpr ~scoped_ref() noexcept {
            if (!m_owner) {
                return;
            }

            for (size_type ii = 0; ii < size; ++ii) {
                m_owner->m_data[ii] = this->at(ii).m_data;
            }
        }

        constexpr scoped_ref& operator=(scoped_ref&& other) noexcept {
            m_owner = other.m_owner;
            for (size_type ii = 0; ii < size; ++ii) {
                m_data[ii] = other.m_data[ii];
            }
            other.m_owner = nullptr;
            return *this;
        }

        constexpr explicit operator bool() const noexcept {
            return m_owner != nullptr;
        }

    private:
        friend fixed;
        constexpr explicit scoped_ref(fixed* owner) noexcept :
            std::array<value_type, size>{owner->to_array()}, m_owner{owner} {}

        fixed* m_owner;
    };

    constexpr auto tie() noexcept requires Vector<data_type> {
        return scoped_ref(this);
    }

    constexpr auto tie() volatile noexcept requires Vector<data_type> {
        // Cast away volatile to allow registers to call ->tie()
        return scoped_ref(const_cast<std::remove_volatile_t<fixed>*>(this));
    }

    // Returns const value to prevent []= pattern
    constexpr const value_type operator[](size_type idx) const noexcept requires Vector<data_type> {
        return value_type::from_data(m_data[idx]);
    }

    constexpr fixed& set(size_type idx, Fixed auto value) noexcept requires Vector<data_type> && (value.size == 1) {
        m_data[idx] = fixed(value).m_data;
        return *this;
    }

    data_type m_data{};

    // Unary operators
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
        m_data += shift_to<decltype(rhs)::fractional_bits, fractional_bits>(rhs.m_data);
        return *this;
    }

    constexpr fixed& operator-=(Fixed auto rhs) noexcept {
        m_data -= shift_to<decltype(rhs)::fractional_bits, fractional_bits>(rhs.m_data);
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
        const auto data = __builtin_convertvector(lhs.data(), bigger_type) * __builtin_convertvector(rhs.data(), bigger_type);
        return fixed<data_type, frac_bits>::from_data(__builtin_convertvector(shift_to<bits_combined, frac_bits>(data), data_type));
    } else {
        const auto data = bigger_type(lhs.data()) * rhs.data();
        return fixed<data_type, frac_bits>::from_data(shift_to<bits_combined, frac_bits>(data));
    }
}

constexpr auto operator*(Fixed auto lhs, Fundamental auto rhs) noexcept {
    return decltype(lhs)::from_data(lhs.data() * rhs);
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

    if constexpr (Vector<data_type>) {
        const auto data = shift_to<Lhs::fractional_bits, frac_bits + Rhs::fractional_bits>(__builtin_convertvector(lhs.data(), bigger_type));
        return fixed<data_type, frac_bits>::from_data(__builtin_convertvector(data / __builtin_convertvector(rhs.data(), bigger_type), data_type));
    } else {
        const auto data = shift_to<Lhs::fractional_bits, frac_bits + Rhs::fractional_bits>(bigger_type(lhs.data()));
        return fixed<data_type, frac_bits>::from_data(data / rhs.data());
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

} // namespace gba

namespace std {
    template <gba::FixedScopedRef T>
    struct tuple_size<T> : std::integral_constant<typename T::owner_type::size_type, T::owner_type::size> {};

    template <std::size_t Index, gba::FixedScopedRef T>
    struct tuple_element<Index, T>
            : tuple_element<Index, gba::n_tuple<typename T::owner_type::value_type, T::owner_type::size>> {};

}

#endif // define GBAXX_TYPE_FIXED_HPP
