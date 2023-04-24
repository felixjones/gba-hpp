/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_TYPE_FIXED_HPP
#define GBAXX_TYPE_FIXED_HPP

#include <concepts>
#include <type_traits>
#include <utility>

#include <gba/type/util.hpp>
#include <gba/type/vector.hpp>

namespace gba {

    template <Fundamental T, std::size_t F>
    struct fixed;

    template <typename T>
    concept Fixed = std::same_as<fixed<typename T::data_type, T::exp>, T>;

    template <Fundamental T, std::size_t F>
    struct fixed {
        using data_type = T;
        static constexpr auto exp = F;

        explicit consteval fixed(std::floating_point auto f) requires (!Vector<T>) : m_data{round<T>(f * (1 << F))} {}

        template <std::floating_point... Args>
        explicit consteval fixed(Args&&... args) requires Vector<T> : m_data{round<typename vector_traits<T>::value_type>(std::forward<Args>(args) * (1 << F))...} {}

        fixed() = default;

        explicit constexpr fixed(std::integral auto x) noexcept requires (!Vector<T>) : m_data(x << F) {}

        template <std::integral... Args>
        explicit constexpr fixed(Args&&... args) noexcept requires Vector<T> : m_data{typename vector_traits<T>::value_type(std::forward<Args>(args) << F)...} {}

        template <Fixed... Args>
        explicit constexpr fixed(Args&&... args) noexcept requires Vector<T> : m_data{(shift_to<Args::exp, F>(args.data()))...} {}

        template <Fundamental U, std::size_t F2>
        explicit constexpr fixed(fixed<U, F2> f) noexcept : m_data{shift_to<F2, F>(f.data())} {}

        template <Fundamental U, std::size_t F2>
        constexpr operator fixed<U, F2>() const noexcept {
            return fixed<U, F2>(*this);
        }

        template <Fundamental U, std::size_t F2>
        constexpr fixed& operator=(fixed<U, F2>&& f) noexcept {
            m_data = shift_to<F2, F>(f.data());
            return *this;
        }

        constexpr fixed operator-() const noexcept {
            return fixed::from_data(-m_data);
        }

        template <Fundamental U, std::size_t F2>
        constexpr fixed& operator+=(fixed<U, F2> rhs) noexcept {
            m_data += shift_to<F2, F>(rhs.data());
            return *this;
        }

        template <Fundamental U, std::size_t F2>
        constexpr fixed& operator-=(fixed<U, F2> rhs) noexcept {
            m_data -= shift_to<F2, F>(rhs.data());
            return *this;
        }

        template <std::integral U>
        explicit constexpr operator U() const noexcept {
            return U(m_data >> F);
        }

#ifdef _DEBUG
        template <std::floating_point U>
        explicit constexpr operator U() const noexcept {
            return m_data / U(1 << F);
        }
#endif

        constexpr T data() const noexcept {
            return m_data;
        }

        constexpr T& data() noexcept {
            return m_data;
        }

        constexpr auto operator[](std::integral auto i) const noexcept requires Vector<T> {
            return fixed<typename vector_traits<T>::value_type, F>::from_data(m_data[i]);
        }

        template <std::size_t Index>
        constexpr std::tuple_element_t<Index, fixed> get() const noexcept requires Vector<T> {
            return fixed<typename vector_traits<T>::value_type, F>::from_data(m_data[Index]);
        }

        T m_data{};

    protected:
        constexpr fixed(T x, std::nullptr_t) noexcept : m_data{x} {}

        template <std::integral... Args>
        explicit constexpr fixed(std::nullptr_t, Args... args) noexcept requires Vector<T> : m_data{args...} {}
    public:
        static constexpr auto from_data(Fundamental auto x) noexcept {
            if constexpr (Vector<decltype(x)>) {
                return fixed(__builtin_convertvector(x, data_type), nullptr);
            } else {
                return fixed(fixed::data_type(x), nullptr);
            }
        }

        template <std::integral... Args>
        static constexpr auto from_data(Args... args) noexcept requires Vector<T> {
            return fixed(nullptr, args...);
        }
    };

    template <Fixed Lhs, Fixed Rhs>
    constexpr auto operator+(Lhs lhs, Rhs rhs) noexcept {
        using data_type = decltype(typename Lhs::data_type() + typename Rhs::data_type());

        constexpr auto exp = (Lhs::exp + Rhs::exp) / 2;

        return fixed<data_type, exp>::from_data(shift_to<Lhs::exp, exp>(lhs.data()) + shift_to<Rhs::exp, exp>(rhs.data()));
    }

    template <Fixed Lhs, Fixed Rhs>
    constexpr auto operator-(Lhs lhs, Rhs rhs) noexcept {
        using data_type = decltype(typename Lhs::data_type() - typename Rhs::data_type());

        constexpr auto exp = (Lhs::exp + Rhs::exp) / 2;

        return fixed<data_type, exp>::from_data(shift_to<Lhs::exp, exp>(lhs.data()) - shift_to<Rhs::exp, exp>(rhs.data()));
    }

    template <std::integral Lhs, Fixed Rhs>
    constexpr auto operator-(Lhs lhs, Rhs rhs) noexcept {
        return Rhs(lhs) - rhs;
    }

    template <std::integral Lhs, Fixed Rhs>
    constexpr auto operator+(Lhs lhs, Rhs rhs) noexcept {
        return Rhs(lhs) + rhs;
    }

    template <Fixed Lhs, std::integral Rhs>
    constexpr auto operator-(Lhs lhs, Rhs rhs) noexcept {
        return lhs - Lhs(rhs);
    }

    template <Fixed Lhs, std::integral Rhs>
    constexpr auto operator+(Lhs lhs, Rhs rhs) noexcept {
        return lhs + Lhs(rhs);
    }

    template <Fixed Lhs, Fixed Rhs>
    constexpr auto operator*(Lhs lhs, Rhs rhs) noexcept {
        using data_type = decltype(typename Lhs::data_type() * typename Rhs::data_type());
        using bigger_type = typename make_bigger<data_type>::type;

        constexpr auto exp = (Lhs::exp + Rhs::exp) / 2;
        constexpr auto rsh = (Lhs::exp + Rhs::exp) - exp;

        if constexpr (Vector<data_type>) {
            const auto biglhs = __builtin_convertvector(lhs.data(), bigger_type);
            const auto bigrhs = __builtin_convertvector(rhs.data(), bigger_type);
            return fixed<data_type, exp>::from_data((biglhs * bigrhs) >> rsh);
        } else {
            const auto data = bigger_type(lhs.data()) * rhs.data();
            return fixed<data_type, exp>::from_data(data >> rsh);
        }
    }

    template <Fixed Lhs, Fixed Rhs>
    constexpr auto operator/(Lhs lhs, Rhs rhs) noexcept {
        using data_type = decltype(typename Lhs::data_type() * typename Rhs::data_type());
        using bigger_type = typename make_bigger<data_type>::type;

        if constexpr (Vector<data_type>) {
            const auto biglhs = __builtin_convertvector(lhs.data(), bigger_type);
            const auto bigrhs = __builtin_convertvector(rhs.data(), bigger_type);
            return fixed<data_type, Lhs::exp>::from_data((biglhs << Rhs::exp) / bigrhs);
        } else {
            const auto data = (bigger_type(lhs.data()) << Rhs::exp) / rhs.data();
            return fixed<data_type, Lhs::exp>::from_data(data);
        }
    }

    template <Fixed Lhs, Fundamental Rhs>
    constexpr auto operator*(Lhs lhs, Rhs rhs) noexcept {
        return Lhs::from_data(lhs.data() * rhs);
    }

    template <Fundamental Lhs, Fixed Rhs>
    constexpr auto operator*(Lhs lhs, Rhs rhs) noexcept {
        return Rhs::from_data(lhs * rhs.data());
    }

    template <Fixed Lhs, Fundamental Rhs>
    constexpr auto operator/(Lhs lhs, Rhs rhs) noexcept {
        return Lhs::from_data(lhs.data() / rhs);
    }

    template <Fundamental Lhs, Fixed Rhs>
    constexpr auto operator/(Lhs lhs, Rhs rhs) noexcept {
        return fixed<Lhs, Rhs::exp>(lhs) / rhs;
    }

    template <Fixed Lhs, Fixed Rhs>
    constexpr auto operator<=>(Lhs lhs, Rhs rhs) noexcept {
        constexpr auto exp = (Lhs::exp + Rhs::exp) / 2;
        return shift_to<Lhs::exp, exp>(lhs.data()) <=> shift_to<Rhs::exp, exp>(rhs.data());
    }

    template <Fixed Lhs, std::integral Rhs>
    constexpr auto operator<=>(Lhs lhs, Rhs rhs) noexcept {
        return lhs <=> Lhs(rhs);
    }

} // namespace gba

namespace std {

    template <gba::Fixed F> requires gba::Vector<typename F::data_type>
    struct tuple_size<F> : integral_constant<size_t, gba::vector_traits<typename F::data_type>::size> {};

    template <size_t Index, gba::Fixed F> requires gba::Vector<typename F::data_type>
    struct tuple_element<Index, F> {
        static_assert(Index < gba::vector_traits<typename F::data_type>::size, "Index out of bounds for vector");
        using type = gba::fixed<typename gba::vector_traits<typename F::data_type>::value_type, F::exp>;
    };

} // namespace std

#endif // define GBAXX_TYPE_FIXED_HPP
