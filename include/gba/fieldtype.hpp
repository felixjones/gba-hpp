/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_FIELDTYPE_HPP
#define GBAXX_FIELDTYPE_HPP

#include <bit>

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
