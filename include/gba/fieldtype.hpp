/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_FIELDTYPE_HPP
#define GBAXX_FIELDTYPE_HPP

#include <gba/inttype.hpp>

#if __cpp_lib_bit_cast
#   include <bit>
#elif !defined(__has_builtin)
#   define __has_builtin(x) 0
#endif

namespace gba {

template <typename Type>
class alignas(uintcontainer<Type>) bitfield {
private:
    using data_type = uintcontainer<Type>;
public:
    using element_type = Type;

    constexpr explicit bitfield(data_type data) noexcept : m_data{data} {}

    constexpr operator element_type() const noexcept {
#if __cpp_lib_bit_cast
        return std::bit_cast<const element_type>(m_data);
#elif __has_builtin(__builtin_bit_cast)
        return __builtin_bit_cast(const element_type, m_data);
#else
        return *reinterpret_cast<const element_type*>(this);
#endif
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

} // namespace gba

template <typename Type> requires (!gba::BitField<Type>)
auto operator|(const Type& lhs, const gba::BitField auto& rhs) noexcept {
#if __cpp_lib_bit_cast
    return std::bit_cast<Type>(rhs | std::bit_cast<const gba::bitfield<Type>>(lhs));
#elif __has_builtin(__builtin_bit_cast)
    return __builtin_bit_cast(Type, rhs | __builtin_bit_cast(const gba::bitfield<Type>, lhs));
#else
    return static_cast<Type>(rhs | *reinterpret_cast<const gba::bitfield<Type>*>(&lhs));
#endif
}

#endif // define GBAXX_FIELDTYPE_HPP
