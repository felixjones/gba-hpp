/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_FIELDTYPE_HPP
#define GBAXX_FIELDTYPE_HPP

#include <bit>
#include <type_traits>

#include <gba/fixedtype.hpp>
#include <gba/inttype.hpp>

namespace gba {

template <typename Type>
class alignas(uintcontainer<Type>) bitfield {
private:
    using data_type = uintcontainer<Type>;
public:
    using element_type = Type;

    constexpr explicit bitfield(data_type data) noexcept : m_data{data} {}

    constexpr operator element_type() const noexcept {
        return std::bit_cast<const element_type>(m_data);
    }

    constexpr auto operator|(const bitfield& rhs) const noexcept {
        return bitfield(m_data | rhs.m_data);
    }

    constexpr auto operator|(const element_type& rhs) const noexcept {
        return bitfield(m_data | bitfield{rhs}.m_data);
    }

protected:
    data_type m_data;
};

namespace field_of {

    template <typename Type, std::size_t Shift>
    struct boolean : bitfield<Type> {
        consteval boolean() noexcept : bitfield<Type>{1 << Shift} {}

        explicit operator bool() const noexcept {
            return (this->m_data >> Shift) & 1;
        }
    };

    template <typename Type, std::size_t Shift, std::size_t Mask>
    struct integral : bitfield<Type> {
        explicit constexpr integral(std::integral auto i) noexcept : bitfield<Type>((i & Mask) << Shift) {}

        explicit operator uintcontainer<Type>() const noexcept {
            return (this->m_data >> Shift) & Mask;
        }
    };

    template <typename Type, std::size_t Shift, typename Enum, Enum Max> requires std::is_enum_v<Enum> && std::is_unsigned_v<std::underlying_type_t<Enum>>
    struct enum_class : bitfield<Type> {
        using underlying_type = std::underlying_type_t<Enum>;
        static constexpr auto mask = uint_bit_mask(static_cast<underlying_type>(Max));

        explicit constexpr enum_class(Enum i) noexcept : bitfield<Type>((static_cast<underlying_type>(i) & mask) << Shift) {}

        explicit operator Enum() const noexcept {
            return static_cast<Enum>((this->m_data >> Shift) & mask);
        }
    };

    template <typename Type, std::size_t Shift, typename Fixed> requires IsFixed<Fixed> && std::is_unsigned_v<typename Fixed::sign>
    struct fixed : bitfield<Type> {
        using fixed_type = Fixed;
        static constexpr auto mask = (1u << (Fixed::integer_bits + Fixed::fractional_bits)) - 1u;

        explicit constexpr fixed(Fixed i) noexcept : bitfield<Type>((i.data() & mask) << Shift) {}

        explicit operator Fixed() const noexcept {
            return Fixed::from_data((this->m_data >> Shift) & mask);
        }
    };

} // namespace field_of

template <class T>
concept BitField = requires(bitfield<typename T::element_type>& a) {
    {std::is_base_of_v<decltype(a), T>};
};

template <typename Type> requires (!gba::BitField<Type>)
constexpr auto operator|(const Type& lhs, const gba::BitField auto& rhs) noexcept {
    return std::bit_cast<Type>(rhs | std::bit_cast<const gba::bitfield<Type>>(lhs));
}

} // namespace gba

#endif // define GBAXX_FIELDTYPE_HPP
