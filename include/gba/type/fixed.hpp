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
    template <Fixed Rhs>
    explicit constexpr fixed(Rhs rhs) noexcept : m_data{shift_to<Rhs::fractional_bits, fractional_bits>(rhs.data())} {}

    template <Fixed Rhs>
    constexpr fixed& operator=(Rhs rhs) noexcept {
        m_data = shift_to<Rhs::fractional_bits, fractional_bits>(rhs.data());
        return *this;
    }

    // Floating point conversion
    template <std::floating_point Rhs>
    explicit consteval fixed(Rhs rhs) : m_data{round_float<data_type>(rhs * data_unit)} {}

    template <std::floating_point Rhs>
    consteval fixed& operator=(Rhs rhs) {
        m_data = round_float<data_type>(rhs * data_unit);
        return *this;
    }

    template <std::floating_point Lhs>
    explicit consteval operator Lhs() const {
        return m_data / Lhs{data_unit};
    }

    // Integral conversion
    template <std::integral Rhs>
    explicit consteval fixed(Rhs rhs) : m_data{rhs << fractional_bits} {}

    template <std::integral Rhs>
    constexpr fixed& operator=(Rhs rhs) noexcept {
        m_data = rhs << fractional_bits;
        return *this;
    }

    template <std::integral Lhs>
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

    data_type m_data{};
};

template <typename T>
concept FixedScopedRef = std::same_as<T, typename T::owner_type::scoped_ref> && Fixed<typename T::owner_type>;

} // namespace gba

namespace std {
    template <gba::FixedScopedRef T>
    struct tuple_size<T> : std::integral_constant<typename T::owner_type::size_type, T::owner_type::size> {};

    template <std::size_t Index, gba::FixedScopedRef T>
    struct tuple_element<Index, T>
            : tuple_element<Index, gba::n_tuple<typename T::owner_type::value_type, T::owner_type::size>> {};

}

#endif // define GBAXX_TYPE_FIXED_HPP
