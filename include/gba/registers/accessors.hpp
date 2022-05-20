/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_REGISTERS_ACCESSORS_HPP
#define GBAXX_REGISTERS_ACCESSORS_HPP

#include <cstdint>
#include <type_traits>

#include <gba/ioregister.hpp>

#include <gba/util/array_traits.hpp>
#include <gba/util/bit_container.hpp>

namespace gba::reg {

template <class Type, std::uintptr_t Address>
struct read_only {
    static auto get() noexcept {
        return io::register_read<Type, Address>();
    }

    static auto get(int idx) noexcept requires util::Array<Type> {
        using type = util::array_value_type<Type>;

        const auto address = Address + idx * sizeof(type);

        if constexpr (std::is_fundamental_v<type> || std::is_pointer_v<type>) {
            return *reinterpret_cast<const volatile type*>(address);
        } else if constexpr (std::is_trivially_copyable_v<type>) {
            return static_cast<type>(*reinterpret_cast<const volatile util::bit_container<type>*>(address));
        } else {
            static_assert(util::always_false<type>);
        }
    }
};

template <class Type, std::uintptr_t Address>
struct write_only {
    static void set(Type value) noexcept {
        io::register_write<decltype(value), Address>(value);
    }

    static void set(int idx, auto value) noexcept requires util::Array<Type> {
        using type = util::array_value_type<Type>;

        const auto address = Address + idx * sizeof(type);

        if constexpr (std::is_fundamental_v<type> || std::is_pointer_v<type>) {
            *reinterpret_cast<volatile type*>(address) = value;
        } else if constexpr (std::is_trivially_copyable_v<type>) {
            reinterpret_cast<volatile util::bit_container<type>*>(address)->copy_from(value);
        } else {
            static_assert(util::always_false<type>);
        }
    }
};

template <class Type, std::uintptr_t Address>
struct read_write : public read_only<Type, Address>, public write_only<Type, Address> {};

} // namespace gba::reg

#endif // define GBAXX_REGISTERS_ACCESSORS_HPP
