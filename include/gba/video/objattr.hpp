/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_VIDEO_OBJATTR_HPP
#define GBAXX_VIDEO_OBJATTR_HPP

#include <gba/type.hpp>

namespace gba {
    enum class obj_display : u16 {
        normal = 0x0,
        affine = 0x1,
        hidden = 0x2,
        affine_double = 0x3
    };

    enum class obj_effect : u16 {
        normal = 0x0,
        semi_transparent = 0x1,
        window = 0x2
    };

    enum class obj_shape : u16 {
        square = 0x0,
        horizontal = 0x1,
        vertical = 0x2
    };

    struct alignas(short) objattr0 {
        u16 y : 8;
        obj_display style : 2;
        obj_effect mode : 2;
        bool mosaic : 1;
        bool bpp8 : 1;
        obj_shape shape : 2;
    };

    struct alignas(short) objattr1 {
        u16 x : 9;
        u16 : 3;
        bool hflip : 1;
        bool vflip : 1;
        u16 size : 2;
    };

    struct alignas(short) objattr2 {
        u16 tile_id : 10;
        u16 priority : 2;
        u16 palbank : 4;
    };

    struct objattr : objattr0, objattr1, objattr2 {};
    static_assert(sizeof(objattr) == 6);

    struct alignas(short) objattr1_affine {
        u16 x : 9;
        u16 affine_index : 5;
        u16 size : 2;
    };

    struct objattr_affine : objattr0, objattr1_affine, objattr2 {};
    static_assert(sizeof(objattr_affine) == 6);

} // namespace gba

#endif // define GBAXX_VIDEO_OBJATTR_HPP
