/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_REGISTERS_KEY_HPP
#define GBAXX_REGISTERS_KEY_HPP

#include <bit>

#include <gba/ioregister.hpp>
#include <gba/inttype.hpp>

#include <gba/registers/accessors.hpp>
#include <gba/registers/key_types.hpp>

namespace gba {
namespace reg {

    using key = read_only<uint16, 0x4000130>;
    using keycnt = read_only<keycnt_type, 0x4000132>;

} // namespace reg

class key_state {
public:
    auto& poll() noexcept {
        m_previous = m_current;
        m_current = reg::key::get();
        return *this;
    }

    [[nodiscard]]
    auto down(key_mask mask) const noexcept {
        const auto bits = std::bit_cast<uint32>(mask);
        return (m_current & bits) == 0;
    }

    [[nodiscard]]
    auto up(key_mask mask) const noexcept {
        const auto bits = std::bit_cast<uint32>(mask);
        return (m_current & bits) == bits;
    }

    [[nodiscard]]
    auto hit(key_mask mask) const noexcept {
        const auto bits = std::bit_cast<uint32>(mask);
        if ((m_previous & bits) == bits) {
            return (m_current & bits) == 0;
        }
    }

    [[nodiscard]]
    auto released(key_mask mask) const noexcept {
        const auto bits = std::bit_cast<uint32>(mask);
        if ((m_previous & bits) == 0) {
            return (m_current & bits) == bits;
        }
    }

    [[nodiscard]]
    auto horizontal() const noexcept {
        const auto sign = (static_cast<int>(~m_current) << 26) >> 30;
        return sign | (sign >> 1); // Fixup low bit
    }

    [[nodiscard]]
    auto vertical() const noexcept {
        const auto sign = (static_cast<int>(~m_current) << 24) >> 30;
        return sign | (sign >> 1); // Fixup low bit
    }
private:
    uint16 m_current{0xffff};
    uint16 m_previous{0xffff};
};

} // namespace gba

#endif // define GBAXX_REGISTERS_KEY_HPP
