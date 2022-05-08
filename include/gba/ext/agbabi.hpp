/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_EXT_AGBABI_HPP
#define GBAXX_EXT_AGBABI_HPP
#if __has_include(<agbabi.h>)

#include <gba/ext/agbabi/coroutine.hpp>
#include <gba/ext/agbabi/fiber.hpp>
#include <gba/ext/agbabi/irq.hpp>
#include <gba/ext/agbabi/math.hpp>
#include <gba/ext/agbabi/pull_coroutine.hpp>
#include <gba/ext/agbabi/push_coroutine.hpp>
#include <gba/ext/agbabi/string.hpp>

#define GBAXX_HAS_AGBABI

#endif // __has_include agbabi.h
#endif // define GBAXX_EXT_AGBABI_HPP
