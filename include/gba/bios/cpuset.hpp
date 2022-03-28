/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_BIOS_CPUSET_HPP
#define GBAXX_BIOS_CPUSET_HPP

#include <gba/inttype.hpp>

#include <gba/bios/swi_call.hpp>

namespace gba::bios {

struct cpuset {
    enum class type {
        half = 0,
        word = 1
    };

    uint32 transfers : 21, : 3;
    bool fill : 1, : 1;
    cpuset::type type : 1;
};

[[gnu::always_inline]]
inline void CpuSet(const void* __restrict__ source, void* __restrict__ destination, cpuset lengthMode) noexcept {
    swi<0x0B, void>(__builtin_assume_aligned(source, 4), __builtin_assume_aligned(destination, 4), lengthMode);
}

struct cpufastset {
    uint32 transfers : 21, : 3;
    bool fill : 1;
};

[[gnu::always_inline]]
inline void CpuFastSet(const void* __restrict__ source, void* __restrict__ destination, cpufastset lengthMode) noexcept {
    swi<0x0C, void>(__builtin_assume_aligned(source, 4), __builtin_assume_aligned(destination, 4), lengthMode);
}

} // namespace gba::bios

#endif // define GBAXX_BIOS_CPUSET_HPP
