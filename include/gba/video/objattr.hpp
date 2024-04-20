/*
===============================================================================

 Copyright (C) 2022-2023 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_VIDEO_OBJATTR_HPP
#define GBAXX_VIDEO_OBJATTR_HPP
/** @file */

#include <gba/type.hpp>

namespace gba {

    /**
     * @enum obj_display
     * @brief Specifies the display modes for an object.
     */
    enum class obj_display : u16 {
        normal = 0, /**< Render the sprite normally. */
        affine = 1, /**< Render the sprite using affine transformation parameters. @sa mmio::AFFINE_PARAM_A @sa mmio::AFFINE_PARAM_B @sa mmio::AFFINE_PARAM_C @sa mmio::AFFINE_PARAM_D */
        hidden = 2, /**< Hide the sprite. */
        affine_double = 3, /**< Same as affine, however the area that the sprite renders to is double its normal size. This allows sprites to grow in size somewhat without immediately clipping to its edges. @sa affine */
    };

    /**
     * @enum obj_effect
     * @brief Specifies the rendering effect to apply to the object.
     */
    enum class obj_effect : u16 {
        normal = 0, /**< Render the sprite normally. */
        semi_transparent = 1, /**< Render the sprite with blend effects applied. @sa mmio::BLDCNT */
        window = 2, /**< Clip the sprite to a window. @sa mmio::WININ @sa mmio::WINOUT */
    };

    /**
     * @enum obj_shape
     * @brief Specifies the shape of the object.
     *
     * @sa dispcnt::obj_vram_1d
     * @sa obj_display::affine_double
     */
    enum class obj_shape : u16 {
        square = 0, /**< Width and height of the sprite graphics are equal. */
        horizontal = 1, /**< Width of the sprite graphics is greater than height. */
        vertical = 2, /**< Height of the sprite graphics is greater than width. */
    };

    /**
     * @struct objattr0
     * @brief The first object attribute parameters.
     * @see <a href="https://mgba-emu.github.io/gbatek/#obj-attribute-0-rw">OBJ Attribute 0 (R/W)</a>
     *
     * @sa mmio::OBJ_ATTR0
     */
    struct alignas(short) objattr0 {
        u16 y : 8{}; /**< Y position of the object on screen. */
        obj_display style : 2{}; /**< Display mode of the object. @sa obj_display */
        obj_effect mode : 2{}; /**< Rendering special effect of the object. @sa obj_effect */
        bool mosaic : 1{}; /**< Does the mosaic effect apply to this object. @sa mmio::MOSAIC */
        bool bpp8 : 1{}; /**< Bit-depth of the sprite character graphics. True = 8-bpp, false = 4-bpp. */
        obj_shape shape : 2{}; /**< Is the sprite square/wide/tall. @sa obj_shape @sa dispcnt::obj_vram_1d */
    };

    /**
     * @struct objattr1
     * @brief The second object attribute parameters.
     * @see <a href="https://mgba-emu.github.io/gbatek/#obj-attribute-1-rw">OBJ Attribute 1 (R/W)</a>
     *
     * @sa mmio::OBJ_ATT1
     */
    struct alignas(short) objattr1 {
        u16 x : 9{}; /**< X position of the object on screen. */
        short : 3;
        bool hflip : 1{}; /**< Should the sprite graphics be visually flipped in the x-axis. */
        bool vflip : 1{}; /**< Should the sprite graphics be visually flipped in the y-axis. */
        u16 size : 2{}; /**< This is combined with objattr0::shape for the overall canvas size of the sprite. @sa dispcnt::obj_vram_1d */
    };

    /**
     * @struct objattr2
     * @brief The third object attribute parameters.
     * @see <a href="https://mgba-emu.github.io/gbatek/#obj-attribute-2-rw">OBJ Attribute 2 (R/W)</a>
     *
     * @sa mmio::OBJ_ATTR2
     */
    struct alignas(short) objattr2 {
        u16 tile_id : 10{}; /**< First character tile index. @sa dispcnt::obj_vram_1d @sa mmio::OBJ_TILES */
        u16 priority : 2{}; /**< Priority relative to background layers. 0 is highest priority. @warning This may not behave as one immediately expects. */
        u16 palbank : 4{}; /**< Which 16-color palette bank to use for this object's character graphics. Only applies to 4-bpp sprites. @sa objattr0::bpp8 @sa mmio::OBJ_PALETTE */
    };

    /**
     * @struct objattr
     * @brief Combined object attribute structure for convenience.
     *
     * @sa mmio::OBJ_ATTR
     */
    struct objattr : objattr0, objattr1, objattr2 {};
    static_assert(sizeof(objattr) == 6);

    /**
     * @struct objattr8
     * @brief 8-byte padded objattr.
     *
     * @sa objattr
     */
    struct alignas(4) objattr8 : objattr {
        char : 8;
    };
    static_assert(sizeof(objattr8) == 8);

    /**
     * @struct objattr1_affine
     * @brief The second object attribute parameters, but specialized for affine objects.
     * @see <a href="https://mgba-emu.github.io/gbatek/#obj-attribute-1-rw">OBJ Attribute 1 (R/W)</a>
     *
     * @sa mmio::OBJ_ATTR1_AFFINE
     */
    struct alignas(short) objattr1_affine {
        u16 x : 9{}; /**< X position of the object on screen. */
        u16 affine_index : 5{}; /**< Which affine matrix to use for this object. @sa mmio::AFFINE_PARAM_A @sa mmio::AFFINE_PARAM_B @sa mmio::AFFINE_PARAM_C @sa mmio::AFFINE_PARAM_D */
        u16 size : 2{}; /**< This is combined with objattr0::shape for the overall canvas size of the sprite. @sa dispcnt::obj_vram_1d */
    };

    /**
     * @struct objattr
     * @brief Combined affine object attribute structure for convenience.
     *
     * @sa mmio::OBJ_ATTR_AFFINE
     */
    struct objattr_affine : objattr0, objattr1_affine, objattr2 {};
    static_assert(sizeof(objattr_affine) == 6);

    /**
     * @struct objattr_affine8
     * @brief 8-byte aligned objattr_affine.
     *
     * @sa objattr_affine
     */
    struct objattr_affine8 : objattr_affine {
        char : 8;
    };
    static_assert(sizeof(objattr_affine8) == 8);

} // namespace gba

#endif // define GBAXX_VIDEO_OBJATTR_HPP
