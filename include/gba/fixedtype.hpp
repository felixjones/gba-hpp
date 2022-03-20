/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_FIXEDTYPE_HPP
#define GBAXX_FIXEDTYPE_HPP

#include <cmath>
#include <cstddef>
#include <type_traits>

#include <gba/inttype.hpp>

namespace gba {

template <int shift, typename T> requires std::is_integral_v<T>
constexpr auto signed_shift(T v) noexcept {
    return shift < 0 ? v << std::abs(shift) : v >> shift;
}

template <std::size_t IntBits, std::size_t FracBits, typename Sign = signed>
class fixed;

template <std::size_t Integer, std::size_t Fractional, typename Sign = unsigned>
using ufixed = fixed<Integer, Fractional, Sign>;

template <class T>
concept IsFixed = std::is_same_v<T, fixed<T::integer_bits, T::fractional_bits, typename T::sign>>;

template <class Lhs, class Rhs = Lhs> requires IsFixed<Lhs> && IsFixed<Rhs>
struct fixed_promote {
private:
    using data_type = decltype(typename Lhs::data_type{} + typename Rhs::data_type{});

    static constexpr auto is_signed = std::is_signed_v<typename Lhs::data_type> || std::is_signed_v<typename Rhs::data_type>;
    static constexpr auto integer_bits = std::max(Lhs::integer_bits, Rhs::integer_bits) - is_signed;
    static constexpr auto fractional_bits = std::max(Lhs::fractional_bits, Rhs::fractional_bits);
    static constexpr auto data_type_bits = std::numeric_limits<data_type>::digits;

    static constexpr auto real_integer_bits = ((integer_bits + fractional_bits) > data_type_bits ? integer_bits : (data_type_bits - fractional_bits));
    static constexpr auto real_fractional_bits = data_type_bits - real_integer_bits;
public:
    using type = fixed<real_integer_bits + is_signed, real_fractional_bits, data_type>;
};

template <class Lhs, class Rhs = Lhs> requires IsFixed<Lhs> && IsFixed<Rhs>
using fixed_promote_t = typename fixed_promote<Lhs, Rhs>::type;

template <class Lhs, class Rhs = Lhs> requires IsFixed<Lhs> && IsFixed<Rhs>
using fixed_promote_wider = fixed_promote_t<
        fixed<Lhs::integer_bits, Lhs::fractional_bits + Rhs::fractional_bits, typename Lhs::sign>,
        fixed<Rhs::integer_bits, Lhs::fractional_bits + Rhs::fractional_bits, typename Rhs::sign>
    >;

template <std::size_t IntBits, std::size_t FracBits, typename Sign>
class fixed {
public:
    using data_type = inttype<IntBits + FracBits, Sign>;

    using sign = Sign;
    static constexpr auto integer_bits = IntBits;
    static constexpr auto fractional_bits = FracBits;

    static constexpr auto from_data(data_type v) noexcept {
        return fixed(v, 0);
    }

    constexpr fixed() noexcept : m_data {} {}

    template <std::size_t RI, std::size_t RF, typename RS> requires (IntBits >= RI && FracBits >= RF)
    constexpr fixed(fixed<RI, RF, RS> rhs) noexcept : m_data {convert_from(rhs).data()} {}

    template <std::size_t RI, std::size_t RF, typename RS>
    constexpr explicit fixed(fixed<RI, RF, RS> rhs) noexcept : m_data {convert_from(rhs).data()} {}

    template <typename T> requires std::is_floating_point_v<T>
    consteval fixed(T v) noexcept : m_data {static_cast<data_type>(v * (1 << FracBits) + 0.5)} {}

    template <typename T> requires std::is_integral_v<T>
    constexpr fixed(T v) noexcept : m_data {static_cast<data_type>(v) << FracBits} {}

    template <typename T> requires std::is_integral_v<T> && (!std::is_same_v<bool, T>)
    constexpr explicit operator T() const noexcept {
        constexpr auto spare_bits = std::numeric_limits<std::make_unsigned_t<data_type>>::digits - (IntBits + FracBits);

        return static_cast<T>((m_data << spare_bits) >> (FracBits + spare_bits));
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
        using wider_type = fixed_promote_wider<fixed, Rhs>;

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
        using wider_type = fixed_promote_wider<fixed, Rhs>;

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

        return from_data(signed_shift<shift>(static_cast<data_type>(rhs.data())));
    }

    data_type m_data;
};

} // namespace gba

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
    using wider_type = gba::fixed_promote_wider<Lhs, Rhs>;

    const auto result = static_cast<typename wider_type::data_type>(lhs.data()) * rhs.data();
    return promote_type::from_data(result >> (wider_type::fractional_bits - promote_type::fractional_bits));
}

template <class Lhs, class Rhs> requires gba::IsFixed<Lhs> && gba::IsFixed<Rhs>
constexpr auto operator/(Lhs lhs, Rhs rhs) noexcept {
    using promote_type = gba::fixed_promote_t<Lhs, Rhs>;
    using wider_type = gba::fixed_promote_wider<promote_type, Rhs>;

    return promote_type::from_data(wider_type(lhs).data() / rhs.data());
}

template <class Lhs, class Rhs> requires std::is_integral_v<Lhs> && gba::IsFixed<Rhs>
constexpr auto operator/(Lhs lhs, Rhs rhs) noexcept {
    using wider_type = gba::fixed_promote_wider<Rhs>;

    return Rhs::from_data(wider_type(lhs).data() / rhs.data());
}

#endif // define GBAXX_FIXEDTYPE_HPP
