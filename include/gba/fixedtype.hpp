/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_FIXEDTYPE_HPP
#define GBAXX_FIXEDTYPE_HPP

#include <cmath>
#include <concepts>
#include <cstddef>
#include <type_traits>

#include <gba/inttype.hpp>

namespace gba {

template <int shift>
constexpr auto signed_shift(std::integral auto v) noexcept {
    return shift < 0 ? v << std::abs(shift) : v >> shift;
}

template <std::size_t IntBits, std::size_t FracBits, typename DataType = int32> requires (std::is_unsigned_v<DataType> || IntBits > 0) && (IntBits + FracBits <= binary_digits<DataType>)
class fixed;

template <std::size_t IntBits, std::size_t FracBits, typename DataType = uint32>
using ufixed = fixed<IntBits, FracBits, DataType>;

template <std::size_t FracBits, typename DataType = int32> requires (FracBits < std::numeric_limits<DataType>::digits)
using make_fixed = fixed<binary_digits<DataType> - FracBits, FracBits, DataType>;

template <std::size_t FracBits, typename DataType = uint32>
using make_ufixed = make_fixed<FracBits, DataType>;

template <class T>
concept IsFixed = std::is_same_v<T, fixed<T::integer_bits, T::fractional_bits, typename T::data_type>>;

template <class Lhs, class Rhs = Lhs> requires IsFixed<Lhs> && IsFixed<Rhs>
struct fixed_promote {
private:
    using lhs_packed_type = inttype<Lhs::integer_bits + Lhs::fractional_bits, typename Lhs::sign>;
    using rhs_packed_type = inttype<Rhs::integer_bits + Rhs::fractional_bits, typename Rhs::sign>;
    using promoted_type = decltype(lhs_packed_type{} + rhs_packed_type{});

    // Try to promote to 32-bit word
    using data_type = std::conditional_t<(binary_digits<promoted_type> > 32),
            promoted_type,
            copysign<int32, promoted_type>
        >;

    static constexpr auto integer_bits = std::min(
            std::max(Lhs::integer_bits, Rhs::integer_bits),
            binary_digits<data_type>
        );

    static constexpr auto desired_fractional_bits = std::max(Lhs::fractional_bits, Rhs::fractional_bits);
    static constexpr auto fractional_bits = integer_bits + desired_fractional_bits > binary_digits<data_type> ?
        binary_digits<data_type> - integer_bits : desired_fractional_bits;
public:
    using type = fixed<integer_bits, fractional_bits, data_type>;
};

template <class Lhs, class Rhs = Lhs> requires IsFixed<Lhs> && IsFixed<Rhs>
using fixed_promote_t = typename fixed_promote<Lhs, Rhs>::type;

template <class Lhs, class Rhs = Lhs> requires IsFixed<Lhs> && IsFixed<Rhs>
struct fixed_promote_wider {
private:
    static constexpr auto fractional_bits = Lhs::fractional_bits + Rhs::fractional_bits;

    using lhs_data_type = inttype<Lhs::integer_bits + fractional_bits, typename Lhs::data_type>;
    using rhs_data_type = inttype<Rhs::integer_bits + fractional_bits, typename Rhs::data_type>;
public:
    using type = fixed_promote_t<
            fixed<Lhs::integer_bits, fractional_bits, lhs_data_type>,
            fixed<Rhs::integer_bits, fractional_bits, rhs_data_type>
        >;
};

template <class Lhs, class Rhs = Lhs>
using fixed_promote_wider_t = typename fixed_promote_wider<Lhs, Rhs>::type;

template <std::size_t IntBits, std::size_t FracBits, typename DataType> requires (std::is_unsigned_v<DataType> || IntBits > 0) && (IntBits + FracBits <= binary_digits<DataType>)
class alignas(DataType) fixed {
public:
    using data_type = std::conditional_t<binary_digits<DataType> >= IntBits + FracBits,
            DataType,
            inttype<IntBits + FracBits, DataType>
        >;

    using sign = std::conditional_t<std::is_signed_v<DataType>, signed, unsigned>;
    static constexpr auto integer_bits = IntBits;
    static constexpr auto fractional_bits = FracBits;
    static constexpr auto is_signed = std::is_signed_v<sign>;
    static constexpr auto is_unsigned = std::is_unsigned_v<sign>;

    static constexpr auto from_data(data_type v) noexcept {
        return fixed(v, 0);
    }

    constexpr fixed() noexcept : m_data {} {}

    template <std::size_t RI, std::size_t RF, typename RS> requires (IntBits >= RI && FracBits >= RF)
    constexpr fixed(fixed<RI, RF, RS> rhs) noexcept : m_data {convert_from(rhs).data()} {}

    template <std::size_t RI, std::size_t RF, typename RS>
    constexpr explicit fixed(fixed<RI, RF, RS> rhs) noexcept : m_data {convert_from(rhs).data()} {}

    consteval fixed(std::floating_point auto v) noexcept : m_data {static_cast<data_type>(v * (1LL << FracBits))} {}

    template <typename T> requires std::is_integral_v<T>
    constexpr fixed(T v) noexcept : m_data {static_cast<data_type>(v << FracBits)} {}

    template <typename T> requires std::is_integral_v<T> && (!std::is_same_v<bool, T>)
    constexpr explicit operator T() const noexcept {
        constexpr auto spare_bits = std::numeric_limits<std::make_unsigned_t<data_type>>::digits - (IntBits + FracBits);

        if constexpr (is_signed && std::is_signed_v<T>) {
            const auto banged = (m_data << spare_bits) >> spare_bits;

            return static_cast<T>(banged / (1LL << FracBits));
        } else {
            return static_cast<T>((m_data << spare_bits) >> (FracBits + spare_bits));
        }
    }

    constexpr explicit operator bool() const noexcept {
        return static_cast<bool>(m_data);
    }

    constexpr auto data() const noexcept {
        return m_data;
    }

    constexpr auto& data() noexcept {
        return m_data;
    }

    constexpr auto operator+() const noexcept {
        return fixed_promote_t<fixed>(*this);
    }

    constexpr auto operator-() const noexcept {
        return from_data(-m_data);
    }

    template <class Rhs> requires IsFixed<Rhs>
    constexpr auto& operator+=(Rhs rhs) noexcept {
        m_data += convert_from(rhs).data();
        return *this;
    }

    template <class Rhs> requires std::is_integral_v<Rhs>
    constexpr auto& operator+=(Rhs rhs) noexcept {
        m_data += fixed(rhs).data();
        return *this;
    }

    template <class Rhs> requires IsFixed<Rhs>
    constexpr auto& operator-=(Rhs rhs) noexcept {
        m_data -= convert_from(rhs).data();
        return *this;
    }

    template <class Rhs> requires std::is_integral_v<Rhs>
    constexpr auto& operator-=(Rhs rhs) noexcept {
        m_data -= fixed(rhs).data();
        return *this;
    }

    template <class Rhs> requires IsFixed<Rhs>
    constexpr auto& operator*=(Rhs rhs) noexcept {
        using wider_type = fixed_promote_wider_t<fixed, Rhs>;

        const auto result = static_cast<typename wider_type::data_type>(m_data) * rhs.data();
        m_data = result >> Rhs::fractional_bits;
        return *this;
    }

    template <class Rhs> requires std::is_integral_v<Rhs>
    constexpr auto& operator*=(Rhs rhs) noexcept {
        m_data *= rhs;
        return *this;
    }

    template <class Rhs> requires IsFixed<Rhs>
    constexpr auto& operator/=(Rhs rhs) noexcept {
        using wider_type = fixed_promote_wider_t<fixed, Rhs>;

        m_data = wider_type(*this).data() / rhs.data();
        return *this;
    }

    template <class Rhs> requires std::is_integral_v<Rhs>
    constexpr auto& operator/=(Rhs rhs) noexcept {
        m_data /= rhs;
        return *this;
    }
public:
    constexpr fixed(data_type v, int) noexcept : m_data {v} {}

    template <class Rhs> requires IsFixed<Rhs>
    constexpr static auto convert_from(Rhs rhs) noexcept {
        constexpr auto shift = static_cast<int>(Rhs::fractional_bits) - static_cast<int>(fractional_bits);

        using shift_type = typename fixed_promote_t<fixed, Rhs>::data_type;

        return from_data(signed_shift<shift>(static_cast<shift_type>(rhs.data())));
    }

    data_type m_data;
};

template <class Lhs, class Rhs> requires gba::IsFixed<Lhs> && gba::IsFixed<Rhs>
constexpr auto operator+(Lhs lhs, Rhs rhs) noexcept {
    using promote_type = gba::fixed_promote_t<Lhs, Rhs>;

    return promote_type::from_data(promote_type(lhs).data() + promote_type(rhs).data());
}

template <class Lhs, class Rhs> requires gba::IsFixed<Lhs> && std::is_integral_v<Rhs>
constexpr auto operator+(Lhs lhs, Rhs rhs) noexcept {
    return Lhs::from_data(lhs.data() + Lhs(rhs).data());
}

template <class Lhs, class Rhs> requires std::is_integral_v<Lhs> && gba::IsFixed<Rhs>
constexpr auto operator+(Lhs lhs, Rhs rhs) noexcept {
    return Rhs::from_data(Rhs(lhs).data() + rhs.data());
}

template <class Lhs, class Rhs> requires gba::IsFixed<Lhs> && gba::IsFixed<Rhs>
constexpr auto operator-(Lhs lhs, Rhs rhs) noexcept {
    using promote_type = gba::fixed_promote_t<Lhs, Rhs>;

    return promote_type::from_data(promote_type(lhs).data() - promote_type(rhs).data());
}

template <class Lhs, class Rhs> requires gba::IsFixed<Lhs> && std::is_integral_v<Rhs>
constexpr auto operator-(Lhs lhs, Rhs rhs) noexcept {
    return Lhs::from_data(lhs.data() - Lhs(rhs).data());
}

template <class Lhs, class Rhs> requires std::is_integral_v<Lhs> && gba::IsFixed<Rhs>
constexpr auto operator-(Lhs lhs, Rhs rhs) noexcept {
    return Rhs::from_data(Rhs(lhs).data() - rhs.data());
}

template <class Lhs, class Rhs> requires gba::IsFixed<Lhs> && gba::IsFixed<Rhs>
constexpr auto operator<=>(Lhs lhs, Rhs rhs) noexcept {
    using promote_type = gba::fixed_promote_t<Lhs, Rhs>;

    return promote_type(lhs).data() <=> promote_type(rhs).data();
}

template <class Lhs, class Rhs> requires gba::IsFixed<Lhs> && std::is_integral_v<Rhs>
constexpr auto operator<=>(Lhs lhs, Rhs rhs) noexcept {
    return lhs.data() <=> Lhs(rhs).data();
}

template <class Lhs, class Rhs> requires std::is_integral_v<Lhs> && gba::IsFixed<Rhs>
constexpr auto operator<=>(Lhs lhs, Rhs rhs) noexcept {
    return Rhs(lhs).data() <=> rhs.data();
}

template <class Lhs, class Rhs> requires gba::IsFixed<Lhs> && gba::IsFixed<Rhs>
constexpr auto operator==(Lhs lhs, Rhs rhs) noexcept {
    using promote_type = gba::fixed_promote_t<Lhs, Rhs>;

    return promote_type(lhs).data() == promote_type(rhs).data();
}

template <class Lhs, class Rhs> requires gba::IsFixed<Lhs> && std::is_integral_v<Rhs>
constexpr auto operator==(Lhs lhs, Rhs rhs) noexcept {
    return lhs.data() == Lhs(rhs).data();
}

template <class Lhs, class Rhs> requires std::is_integral_v<Lhs> && gba::IsFixed<Rhs>
constexpr auto operator==(Lhs lhs, Rhs rhs) noexcept {
    return Rhs(lhs).data() == rhs.data();
}

template <class Lhs, class Rhs> requires gba::IsFixed<Lhs> && gba::IsFixed<Rhs>
constexpr auto operator!=(Lhs lhs, Rhs rhs) noexcept {
    using promote_type = gba::fixed_promote_t<Lhs, Rhs>;

    return promote_type(lhs).data() != promote_type(rhs).data();
}

template <class Lhs, class Rhs> requires gba::IsFixed<Lhs> && std::is_integral_v<Rhs>
constexpr auto operator!=(Lhs lhs, Rhs rhs) noexcept {
    return lhs.data() != Lhs(rhs).data();
}

template <class Lhs, class Rhs> requires std::is_integral_v<Lhs> && gba::IsFixed<Rhs>
constexpr auto operator!=(Lhs lhs, Rhs rhs) noexcept {
    return Rhs(lhs).data() != rhs.data();
}

template <class Lhs, class Rhs> requires gba::IsFixed<Lhs> && std::is_integral_v<Rhs>
constexpr auto operator*(Lhs lhs, Rhs rhs) noexcept {
    return Lhs::from_data(lhs.data() * rhs);
}

template <class Lhs, class Rhs> requires std::is_integral_v<Lhs> && gba::IsFixed<Rhs>
constexpr auto operator*(Lhs lhs, Rhs rhs) noexcept {
    return Rhs::from_data(lhs * rhs.data());
}

template <class Lhs, class Rhs> requires gba::IsFixed<Lhs> && std::is_integral_v<Rhs>
constexpr auto operator/(Lhs lhs, Rhs rhs) noexcept {
    return Lhs::from_data(lhs.data() / rhs);
}

template <class Lhs, class Rhs> requires gba::IsFixed<Lhs> && gba::IsFixed<Rhs>
constexpr auto operator*(Lhs lhs, Rhs rhs) noexcept {
    using promote_type = gba::fixed_promote_t<Lhs, Rhs>;
    using wider_type = gba::fixed_promote_wider_t<Lhs, Rhs>;

    const auto result = static_cast<typename wider_type::data_type>(lhs.data()) * rhs.data();
    return promote_type::from_data(result >> (wider_type::fractional_bits - promote_type::fractional_bits));
}

template <class Lhs, class Rhs> requires gba::IsFixed<Lhs> && gba::IsFixed<Rhs>
constexpr auto operator/(Lhs lhs, Rhs rhs) noexcept {
    using promote_type = gba::fixed_promote_t<Lhs, Rhs>;
    using wider_type = gba::fixed_promote_wider_t<promote_type, Rhs>;

    return promote_type::from_data(wider_type(lhs).data() / rhs.data());
}

template <class Lhs, class Rhs> requires std::is_integral_v<Lhs> && gba::IsFixed<Rhs>
constexpr auto operator/(Lhs lhs, Rhs rhs) noexcept {
    using wider_type = gba::fixed_promote_wider_t<Rhs>;

    return Rhs::from_data(wider_type(lhs).data() / rhs.data());
}

} // namespace gba

#endif // define GBAXX_FIXEDTYPE_HPP
