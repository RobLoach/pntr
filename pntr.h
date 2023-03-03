/**
 * pntr: Image manipulation library for C99 and C++, with a focus on ease-of-use.
 *
 * Configuration:
 * - PNTR_IMPLEMENTATION: Define this in one of your .c files, before including pntr.h
 * - PNTR_SUPPORT_DEFAULT_FONT: Enables the default font
 * - PNTR_SUPPORT_TTF: Enables TTF font loading
 * - PNTR_NO_SUPPORT_PNG: Disables loading/saving PNG images
 * - PNTR_SUPPORT_FILTER_SMOOTH: When resizing images, use stb_image, which is slower, but can look better.
 * - PNTR_PIXELFORMAT_RGBA: Use the RGBA format
 * - PNTR_PIXELFORMAT_ARGB: Use the ARGB pixel format
 * - PNTR_NO_CUTE_PNG_IMPLEMENTATION: Skips defining CUTE_PNG_IMPLEMENTATION. Useful if you're using cute_png elsewhere.
 * - PNTR_NO_STB_TRUETYPE_IMPLEMENTATION: Skips defining STB_TRUETYPE_IMPLEMENTATION. Useful if you're using cute_png elsewhere.
 * - PNTR_LOAD_FILE: Callback to use when asked to load a file. Must match the pntr_load_file() definition.
 * - PNTR_SAVE_FILE: Callback to use when asked to save a file. Must match the pntr_save_file() definition.
 * - PNTR_NO_ALPHABLEND: Skips alpha blending when rendering images
 * - PNTR_NO_MATH: Disables dependency on C's math.h library. Will disable PNTR_SUPPORT_FILTER_SMOOTH, and PNTR_SUPPORT_TTF.
 *
 * @file pntr.h
 *
 * @copyright 2023 Rob Loach (@RobLoach, https://robloach.net)
 *
 * @license Zlib
 *
 * Copyright (c) 2023 Rob Loach (@RobLoach, https://robloach.net)
 *
 * This software is provided "as-is", without any express or implied warranty. In no event
 * will the authors be held liable for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose, including commercial
 * applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented; you must not claim that you
 *   wrote the original software. If you use this software in a product, an acknowledgment
 *   in the product documentation would be appreciated but is not required.
 *
 *   2. Altered source versions must be plainly marked as such, and must not be misrepresented
 *   as being the original software.
 *
 *   3. This notice may not be removed or altered from any source distribution.
 */
#ifndef PNTR_H__
#define PNTR_H__

#include <stdint.h> // uint32_t
#include <stdbool.h> // bool

/**
 * @defgroup pntr pntr
 * @{
 */

#ifdef _DOXYGEN_
    /**
     * @defgroup config Configuration
     * @{
     */

    /**
     * Define `PNTR_IMPLEMENTATION` in one of your `.c` files before including `pntr.h`.
     *
     * This will let pntr.h know where to implement its functions.
     */
    #define PNTR_IMPLEMENTATION

    /**
     * Enables support for pntr's default font. It's a small 8x8 font.
     *
     * @see pntr_load_default_font()
     */
    #define PNTR_SUPPORT_DEFAULT_FONT

    /**
     * Enables TTF font loading via stb_truetype.
     *
     * @see pntr_load_ttffont()
     */
    #define PNTR_SUPPORT_TTF

    /**
     * Disables loading/saving PNG images, and will avoid loading cute_png.h.
     *
     * @see pntr_load_image()
     */
    #define PNTR_NO_SUPPORT_PNG

    /**
     * When resizing images, use stb_image, which is slower, but can look better.
     *
     * @see PNTR_FILTER_SMOOTH
     */
    #define PNTR_SUPPORT_FILTER_SMOOTH

    /**
     * When defined, will use the RGBA format.
     *
     * @see PNTR_PIXELFORMAT_RGBA8888
     */
    #define PNTR_PIXELFORMAT_RGBA

    /**
     * When defined, will use the ARGB pixel format.
     *
     * @see PNTR_PIXELFORMAT_ARGB8888
     */
    #define PNTR_PIXELFORMAT_ARGB

    /**
     * Skips defining `CUTE_PNG_IMPLEMENTATION`. Useful if you're using cute_png elsewhere.
     */
    #define PNTR_NO_CUTE_PNG_IMPLEMENTATION

    /**
     * Skips defining `STB_IMAGE_RESIZE_IMPLEMENTATION`. Useful if you're using stb_image_resize elsewhere.
     */
    #define PTNR_NO_STB_IMAGE_RESIZE_IMPLEMENTATION

    /**
     * Skips defining `STB_TRUETYPE_IMPLEMENTATION`. Useful if you're using stb_truetype elsewhere.
     */
    #define PNTR_NO_STB_TRUETYPE_IMPLEMENTATION

    /**
     * Callback to use when asked to load a file. Must match the pntr_load_file() definition.
     *
     * @see pntr_load_file()
     */
    #define PNTR_LOAD_FILE

    /**
     * Callback to use when asked to save a file. Must match the pntr_save_file() definition.
     *
     * @see pntr_save_file()
     */
    #define PNTR_SAVE_FILE

    /**
     * Skips alpha blending when rendering images. Defining this will improve performance.
     *
     * @see pntr_draw_image_rec()
     */
    #define PNTR_NO_ALPHABLEND

    /**
     * Will disable pntr's dependency on C's math.h library.
     *
     * This will disable PNTR_SUPPORT_FILTER_SMOOTH and PNTR_SUPPORT_TTF.
     *
     * @see PNTR_SUPPORT_FILTER_SMOOTH
     * @see PNTR_SUPPORT_TTF
     */
    #define PNTR_NO_MATH

    /**
     * @}
     */
#endif

#ifndef PNTR_API
    /**
     * Prefix applied to all pntr methods.
     *
     * If you're using Emscripten, for instance, you could use the following to export all pntr methods:
     *
     *     #define PNTR_API EMSCRIPTEN_KEEPALIVE
     */
    #define PNTR_API
#endif

// Pixel Format. Default to PNTR_PIXELFORMAT_RGBA
#if !defined(PNTR_PIXELFORMAT_RGBA) && !defined(PNTR_PIXELFORMAT_ARGB)
    #define PNTR_PIXELFORMAT_RGBA
#endif

/**
 * Color, represented by an unsigned 32-bit integer.
 *
 * Has four components: Red, Green, Blue, and Alpha.
 *
 * @see pntr_new_color()
 * @see pntr_get_color()
 */
typedef union pntr_color {
    /**
     * The color data, represented by an unsigned 32-bit integer.
     */
    uint32_t data;

    struct {
        #if defined(PNTR_PIXELFORMAT_RGBA)
            /**
             * Red component.
             */
            unsigned char r;

            /**
             * Green component.
             */
            unsigned char g;

            /**
             * Blue component.
             */
            unsigned char b;

            /**
             * Alpha component.
             */
            unsigned char a;
        #elif defined(PNTR_PIXELFORMAT_ARGB)
            /**
             * Blue component.
             */
            unsigned char b;

            /**
             * Green component.
             */
            unsigned char g;

            /**
             * Red component.
             */
            unsigned char r;

            /**
             * Alpha component.
             */
            unsigned char a;
        #endif
    };
} pntr_color;

/**
 * An image, represented by pixel data.
 *
 * @see pntr_new_image()
 * @see pntr_gen_image_color()
 */
typedef struct pntr_image {
    /**
     * The pixel data for the image.
     */
    pntr_color* data;

    /**
     * The width of the image.
     */
    int width;

    /**
     * The height of the image.
     */
    int height;

    /**
     * The amount of bytes of one row of the image.
     */
    int pitch;
} pntr_image;

/**
 * A vector, represented by x and y coordinates.
 */
typedef struct pntr_vector {
    /**
     * The X coordinate.
     */
    int x;

    /**
     * The Y coordinate.
     */
    int y;
} pntr_vector;

/**
 * A rectangle.
 */
typedef struct pntr_rectangle {
    /**
     * The x position of the rectangle.
     */
    int x;

    /**
     * The y position of the rectangle.
     */
    int y;

    /**
     * The width of the rectangle.
     */
    int width;

    /**
     * The height of the rectangle.
     */
    int height;
} pntr_rectangle;

/**
 * Font.
 *
 * @see pntr_load_ttyfont()
 * @see pntr_load_ttffont()
 * @see pntr_load_bmfont()
 */
typedef struct pntr_font {
    /**
     * The image used for the character atlas for the font.
     */
    pntr_image* atlas;

    /**
     * The glyph source rectangles on the atlas.
     */
    pntr_rectangle* srcRects;

    /**
     * How the glyph appears when rendering.
     */
    pntr_rectangle* glyphRects;

    /**
     * An array of characters that are available in the font's atlas.
     */
    char* characters;

    /**
     * The number of characters that the font implements.
     */
    int charactersLen;
} pntr_font;

/**
 * Pixel format.
 */
typedef enum pntr_pixelformat {
    /**
     * RGBA, with 8 bytes for each component.
     */
    PNTR_PIXELFORMAT_RGBA8888 = 0,

    /**
     * ARGB, with 8 bytes for each component.
     */
    PNTR_PIXELFORMAT_ARGB8888,

    /**
     * Grayscale, with one byte for each pixel, 0 - 255. 0 being transparent, 255 being black.
     */
    PNTR_PIXELFORMAT_GRAYSCALE
} pntr_pixelformat;

/**
 * Possible image filters to apply.
 */
typedef enum pntr_filter {
    /**
     * The default filter will use PNTR_FILTER_SMOOTH when available, but fall back to PNTR_FILTER_BILINEAR when needed.
     */
    PNTR_FILTER_DEFAULT = 0,

    /**
     * Nearest-neighbor interpolation for fast processing.
     */
    PNTR_FILTER_NEARESTNEIGHBOR,

    /**
     * Bilinear interpolation will combine multiple pixels together when processing.
     *
     * @see pntr_color_bilinear_interpolate()
     */
    PNTR_FILTER_BILINEAR,

    /**
     * Smooth filter will use stb_image_resize, which combines a number of different filtering algorithms.
     */
    PNTR_FILTER_SMOOTH
} pntr_filter;

#ifdef __cplusplus
extern "C" {
#endif

PNTR_API pntr_image* pntr_new_image(int width, int height);
PNTR_API pntr_image* pntr_gen_image_color(int width, int height, pntr_color color);
PNTR_API pntr_image* pntr_image_copy(pntr_image* image);
PNTR_API pntr_image* pntr_image_from_image(pntr_image* image, int x, int y, int width, int height);
PNTR_API void pntr_unload_image(pntr_image* image);
PNTR_API void pntr_clear_background(pntr_image* image, pntr_color color);
PNTR_API void pntr_draw_pixel(pntr_image* dst, int x, int y, pntr_color color);
PNTR_API void pntr_draw_line(pntr_image* dst, int startPosX, int startPosY, int endPosX, int endPosY, pntr_color color);
PNTR_API void pntr_draw_rectangle(pntr_image* dst, int posX, int posY, int width, int height, pntr_color color);
PNTR_API void pntr_draw_rectangle_rec(pntr_image* dst, pntr_rectangle rect, pntr_color color);
PNTR_API void pntr_draw_circle(pntr_image* dst, int centerX, int centerY, int radius, pntr_color color);
PNTR_API void pntr_draw_image(pntr_image* dst, pntr_image* src, int posX, int posY);
PNTR_API void pntr_draw_image_rec(pntr_image* dst, pntr_image* src, pntr_rectangle srcRect, int posX, int posY);
PNTR_API void pntr_draw_text(pntr_image* dst, pntr_font* font, const char* text, int posX, int posY);
PNTR_API pntr_color pntr_new_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
PNTR_API pntr_color pntr_get_color(unsigned int hexValue);
PNTR_API void pntr_color_get_rgba(pntr_color color, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a);
PNTR_API unsigned char pntr_color_get_r(pntr_color color);
PNTR_API unsigned char pntr_color_get_g(pntr_color color);
PNTR_API unsigned char pntr_color_get_b(pntr_color color);
PNTR_API unsigned char pntr_color_get_a(pntr_color color);
PNTR_API void pntr_color_set_r(pntr_color* color, unsigned char r);
PNTR_API void pntr_color_set_g(pntr_color* color, unsigned char g);
PNTR_API void pntr_color_set_b(pntr_color* color, unsigned char b);
PNTR_API void pntr_color_set_a(pntr_color* color, unsigned char a);
PNTR_API pntr_color pntr_image_get_color(pntr_image* image, int x, int y);
PNTR_API pntr_color* pntr_image_get_color_pointer(pntr_image* image, int x, int y);
PNTR_API bool pntr_save_file(const char *fileName, const void *data, unsigned int bytesToWrite);
PNTR_API void* pntr_image_to_pixelformat(pntr_image* image, unsigned int* dataSize, pntr_pixelformat pixelFormat);
PNTR_API bool pntr_save_image(pntr_image* image, const char* fileName);
PNTR_API unsigned char* pntr_save_image_to_memory(pntr_image* image, unsigned int* dataSize);
PNTR_API int pntr_get_pixel_data_size(int width, int height, pntr_pixelformat pixelFormat);
PNTR_API pntr_image* pntr_load_image(const char* fileName);
PNTR_API pntr_image* pntr_load_image_from_memory(const unsigned char* fileData, unsigned int dataSize);
PNTR_API pntr_image* pntr_image_from_pixelformat(const void* data, int width, int height, pntr_pixelformat pixelFormat);
PNTR_API const char* pntr_get_error();
PNTR_API void* pntr_set_error(const char* error);
PNTR_API pntr_image* pntr_image_resize(pntr_image* image, int newWidth, int newHeight, pntr_filter filter);
PNTR_API void pntr_image_color_replace(pntr_image* image, pntr_color color, pntr_color replace);
PNTR_API pntr_color pntr_color_tint(pntr_color color, pntr_color tint);
PNTR_API void pntr_image_color_tint(pntr_image* image, pntr_color color);
PNTR_API pntr_color pntr_color_fade(pntr_color color, float alpha);
PNTR_API void pntr_image_color_fade(pntr_image* image, float alpha);
PNTR_API pntr_color pntr_color_brightness(pntr_color color, float factor);
PNTR_API pntr_color pntr_get_pixel_color(void* srcPtr, pntr_pixelformat srcPixelFormat);
PNTR_API void pntr_set_pixel_color(void* dstPtr, pntr_color color, pntr_pixelformat dstPixelFormat);
PNTR_API pntr_font* pntr_load_default_font();
PNTR_API void pntr_unload_font(pntr_font* font);
PNTR_API pntr_font* pntr_load_bmfont(const char* fileName, const char* characters);
PNTR_API pntr_font* pntr_load_bmfont_from_image(pntr_image* image, const char* characters);
PNTR_API pntr_font* pntr_load_bmfont_from_memory(const unsigned char* fileData, unsigned int dataSize, const char* characters);
PNTR_API int pntr_measure_text(pntr_font* font, const char* text);
PNTR_API pntr_vector pntr_measure_text_ex(pntr_font* font, const char* text);
PNTR_API pntr_image* pntr_gen_image_text(pntr_font* font, const char* text);
PNTR_API pntr_font* pntr_load_ttyfont(const char* fileName, int glyphWidth, int glyphHeight, const char* characters);
PNTR_API pntr_font* pntr_load_ttyfont_from_memory(const unsigned char* fileData, unsigned int dataSize, int glyphWidth, int glyphHeight, const char* characters);
PNTR_API pntr_font* pntr_load_ttyfont_from_image(pntr_image* image, int glyphWidth, int glyphHeight, const char* characters);
PNTR_API unsigned char* pntr_load_file(const char *fileName, unsigned int *bytesRead);
PNTR_API void pntr_unload_file(unsigned char* fileData);
PNTR_API pntr_font* pntr_load_ttffont(const char* fileName, int fontSize, pntr_color fontColor);
PNTR_API pntr_font* pntr_load_ttffont_from_memory(const unsigned char* fileData, unsigned int dataSize, int fontSize, pntr_color fontColor);
PNTR_API pntr_color pntr_color_invert(pntr_color color);
PNTR_API void pntr_image_color_invert(pntr_image* image);
PNTR_API pntr_color pntr_color_alpha_blend(pntr_color dst, pntr_color src);
PNTR_API pntr_rectangle pntr_image_alpha_border(pntr_image* image, float threshold);
PNTR_API void pntr_image_crop(pntr_image* image, int x, int y, int width, int height);
PNTR_API void pntr_image_alpha_crop(pntr_image* image, float threshold);
PNTR_API void pntr_image_color_brightness(pntr_image* image, float factor);
PNTR_API void pntr_image_flip_vertical(pntr_image* image);
PNTR_API void pntr_image_flip_horizontal(pntr_image* image);
PNTR_API pntr_color pntr_color_contrast(pntr_color color, float contrast);
PNTR_API void pntr_image_color_contrast(pntr_image* image, float contrast);
PNTR_API void pntr_image_alpha_mask(pntr_image* image, pntr_image* alphaMask, int posX, int posY);
PNTR_API void pntr_image_resize_canvas(pntr_image* image, int newWidth, int newHeight, int offsetX, int offsetY, pntr_color fill);
PNTR_API pntr_image* pntr_image_rotate(pntr_image* image, float rotation);
PNTR_API pntr_image* pntr_image_rotate_ex(pntr_image* image, float rotation, pntr_filter filter);
PNTR_API pntr_image* pntr_gen_image_gradient_vertical(int width, int height, pntr_color top, pntr_color bottom);
PNTR_API pntr_image* pntr_gen_image_gradient_horizontal(int width, int height, pntr_color left, pntr_color right);
PNTR_API pntr_image* pntr_gen_image_gradient(int width, int height, pntr_color topLeft, pntr_color topRight, pntr_color bottomLeft, pntr_color bottomRight);
PNTR_API pntr_color pntr_color_bilinear_interpolate(pntr_color color00, pntr_color color01, pntr_color color10, pntr_color color11, float coordinateX, float coordinateY);

#ifdef __cplusplus
}
#endif

#if defined(__cplusplus)
    #define CLITERAL(type)      type
#else
    #define CLITERAL(type)      (type)
#endif

#ifndef PNTR_LIGHTGRAY
/**
 * Light gray.
 */
#define PNTR_LIGHTGRAY  CLITERAL(pntr_color) { .r = 200, .g = 200, .b = 200, .a = 255 }
#endif
#ifndef PNTR_GRAY
/**
 * Gray.
 */
#define PNTR_GRAY       CLITERAL(pntr_color) { .r = 130, .g = 130, .b = 130, .a = 255 }
#endif
#ifndef PNTR_DARKGRAY
/**
 * Dark gray.
 */
#define PNTR_DARKGRAY   CLITERAL(pntr_color) { .r = 80,  .g = 80,  .b = 80,  .a = 255 }
#endif
#ifndef PNTR_YELLOW
/**
 * Yellow.
 */
#define PNTR_YELLOW     CLITERAL(pntr_color) { .r = 253, .g = 249, .b = 0,   .a =255  }
#endif
#ifndef PNTR_GOLD
/**
 * Gold.
 */
#define PNTR_GOLD       CLITERAL(pntr_color) { .r = 255, .g = 203, .b = 0,   .a =255  }
#endif
#ifndef PNTR_ORANGE
/**
 * Orange.
 */
#define PNTR_ORANGE     CLITERAL(pntr_color) { .r = 255, .g = 161, .b = 0,   .a =255  }
#endif
#ifndef PNTR_PINK
/**
 * Pink.
 */
#define PNTR_PINK       CLITERAL(pntr_color) { .r = 255, .g = 109, .b = 194, .a = 255 }
#endif
#ifndef PNTR_RED
/**
 * Red.
 */
#define PNTR_RED        CLITERAL(pntr_color) { .r = 230, .g = 41,  .b = 55,  .a = 255 }
#endif
#ifndef PNTR_MAROON
/**
 * Maroon.
 */
#define PNTR_MAROON     CLITERAL(pntr_color) { .r = 190, .g = 33,  .b = 55,  .a = 255 }
#endif
#ifndef PNTR_GREEN
/**
 * Green.
 */
#define PNTR_GREEN      CLITERAL(pntr_color) { .r = 0,   .g = 228, .b = 48,  .a = 255 }
#endif
#ifndef PNTR_LIME
/**
 * Lime.
 */
#define PNTR_LIME       CLITERAL(pntr_color) { .r = 0,   .g = 158, .b = 47,  .a = 255 }
#endif
#ifndef PNTR_DARKGREEN
/**
 * Dark green.
 */
#define PNTR_DARKGREEN  CLITERAL(pntr_color) { .r = 0,   .g = 117, .b = 44,  .a = 255 }
#endif
#ifndef PNTR_SKYBLUE
/**
 * Sky blue.
 */
#define PNTR_SKYBLUE    CLITERAL(pntr_color) { .r = 102, .g = 191, .b = 255, .a = 255 }
#endif
#ifndef PNTR_BLUE
/**
 * Blue.
 */
#define PNTR_BLUE       CLITERAL(pntr_color) { .r = 0,   .g = 121, .b = 241, .a = 255 }
#endif
#ifndef PNTR_DARKBLUE
/**
 * Dark blue.
 */
#define PNTR_DARKBLUE   CLITERAL(pntr_color) { .r = 0,   .g = 82,  .b = 172, .a = 255 }
#endif
#ifndef PNTR_PURPLE
/**
 * Purple.
 */
#define PNTR_PURPLE     CLITERAL(pntr_color) { .r = 200, .g = 122, .b = 255, .a = 255 }
#endif
#ifndef PNTR_VIOLET
/**
 * Violet.
 */
#define PNTR_VIOLET     CLITERAL(pntr_color) { .r = 135, .g = 60,  .b = 190, .a = 255 }
#endif
#ifndef PNTR_DARKPURPLE
/**
 * Dark purple.
 */
#define PNTR_DARKPURPLE CLITERAL(pntr_color) { .r = 112, .g = 31,  .b = 126, .a = 255 }
#endif
#ifndef PNTR_BEIGE
/**
 * Beige.
 */
#define PNTR_BEIGE      CLITERAL(pntr_color) { .r = 211, .g = 176, .b = 131, .a = 255 }
#endif
#ifndef PNTR_BROWN
/**
 * Brown.
 */
#define PNTR_BROWN      CLITERAL(pntr_color) { .r = 127, .g = 106, .b = 79,  .a = 255 }
#endif
#ifndef PNTR_DARKBROWN
/**
 * Dark brown.
 */
#define PNTR_DARKBROWN  CLITERAL(pntr_color) { .r = 76,  .g = 63,  .b = 47,  .a = 255 }
#endif
#ifndef PNTR_WHITE
/**
 * White.
 */
#define PNTR_WHITE      CLITERAL(pntr_color) { .r = 255, .g = 255, .b = 255, .a = 255 }
#endif
#ifndef PNTR_BLACK
/**
 * Black.
 */
#define PNTR_BLACK      CLITERAL(pntr_color) { .r = 0,   .g = 0,   .b = 0,   .a = 255 }
#endif
#ifndef PNTR_BLANK
/**
 * Blank, or transparent.
 */
#define PNTR_BLANK      CLITERAL(pntr_color) { .r = 0,   .g = 0,   .b = 0,   .a = 0   }
#endif
#ifndef PNTR_MAGENTA
/**
 * Magenta.
 */
#define PNTR_MAGENTA    CLITERAL(pntr_color) { .r = 255, .g = 0,   .b = 255, .a = 255 }
#endif
#ifndef PNTR_RAYWHITE
/**
 * The white used in raylib.
 */
#define PNTR_RAYWHITE   CLITERAL(pntr_color) { .r = 245, .g = 245, .b = 245, .a = 255 }
#endif

#endif  // PNTR_H__

#ifdef PNTR_IMPLEMENTATION
#ifndef PNTR_IMPLEMENTATION_ONCE
#define PNTR_IMPLEMENTATION_ONCE

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PNTR_MALLOC
    #include <stdlib.h>
    /**
     * Allocates the requested memory and returns a pointer to it.
     */
    #define PNTR_MALLOC(size) malloc((size_t)(size))
#endif  // PNTR_MALLOC

#ifndef PNTR_FREE
    #include <stdlib.h>
    /**
     * Deallocates the previously allocated memory.
     */
    #define PNTR_FREE(obj) free((void*)(obj))
#endif  // PNTR_FREE

#ifndef PNTR_REALLOC
    #include <stdlib.h>
    /**
     * Attempts to resize the memory block pointed to that was previously allocated.
     */
    #define PNTR_REALLOC realloc
#endif  // PNTR_REALLOC

#ifndef PNTR_MEMCPY
    #include <string.h>
    /**
     * Copies data from memory area src to the destination memory.
     */
    #define PNTR_MEMCPY(dest, src, n) memcpy((void*)(dest), (const void*)(src), (size_t)(n))
#endif  // PNTR_MEMCPY

#ifndef PNTR_MEMSET
    #include <string.h>
    /**
     * Copies the character c (an unsigned char) to the first n characters of the string pointed to, by the argument str.
     */
    #define PNTR_MEMSET memset
#endif  // PNTR_MEMSET

#ifdef PNTR_NO_MATH
    #ifdef PNTR_SUPPORT_TTF
        // TTF requires math.h
        #undef PNTR_SUPPORT_TTF
    #endif
    #ifdef PNTR_SUPPORT_FILTER_SMOOTH
        // stb_image_resize requires math.h
        #undef PNTR_SUPPORT_FILTER_SMOOTH
    #endif

    /**
     * Custom implementation of floorf().
     */
    float _pntr_floorf(float val) {
        if (val < 0.0f) {
            return (float)((int)val - 1);
        }
        else {
            return (float)((int)val);
        }
    }
    #define PNTR_FLOORF _pntr_floorf
#else
    #ifndef PNTR_COSF
        #include <math.h>
        /**
         * Computes the cosine of the argument in radians.
         */
        #define PNTR_COSF cosf
    #endif  // PNTR_COSF

    #ifndef PNTR_SINF
        #include <math.h>
        /**
         * Computes the sine of the argument in radians.
         */
        #define PNTR_SINF sinf
    #endif  // PNTR_SINF

    #ifndef PNTR_CEILF
        #include <math.h>
        /**
         * Computes the smallest integer value not less than arg.
         */
        #define PNTR_CEILF ceilf
    #endif  // PNTR_CEILF

    #ifndef PNTR_FABSF
        #include <math.h>
        /**
         * Computes the absolute value of a floating point value arg.
         */
        #define PNTR_FABSF fabsf
    #endif  // PNTR_FABSF

    #ifndef PNTR_FLOORF
        #include <math.h>
        /**
         * Computes the largest integer value not greater than arg.
         */
        #define PNTR_FLOORF floorf
    #endif  // PNTR_FLOORF
#endif  // PNTR_NO_MATH

#if !defined(PNTR_LOAD_FILE) || !defined(PNTR_SAVE_FILE)
    #include <stdio.h> // FILE, fopen, fread
#endif  // PNTR_LOAD_FILE, PNTR_SAVE_FILE

#ifndef PNTR_MAX
    /**
     * Return the largest value of the two given values.
     */
    #define PNTR_MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef PNTR_MIN
    /**
     * Return the smallest value of the two given values.
     */
    #define PNTR_MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef PNTR_PIXELFORMAT
    #if defined(PNTR_PIXELFORMAT_RGBA)
        /**
         * The set pixel format for the application.
         */
        #define PNTR_PIXELFORMAT PNTR_PIXELFORMAT_RGBA8888
    #elif defined(PNTR_PIXELFORMAT_ARGB)
        #define PNTR_PIXELFORMAT PNTR_PIXELFORMAT_ARGB8888
    #endif
#endif  // PNTR_PIXELFORMAT

/**
 * Draws a pixel on the canvas, ignoring sanity checks.
 *
 * TODO: Convert pntr_draw_pixel_unsafe to a method?
 */
#define pntr_draw_pixel_unsafe(dst, x, y, color) dst->data[((y) * (dst->pitch >> 2)) + (x)] = (color)

/**
 * Gets the color of the (x, y) coordinate of the image, ignoring sanity checks.
 *
 * TODO: Convert pntr_image_get_color_unsafe to a method?
 */
#define pntr_image_get_color_unsafe(image, x, y) image->data[((y) * (image->pitch >> 2)) + (x)]

// cute_png
#ifndef PNTR_NO_SUPPORT_PNG
    #ifndef PNTR_NO_CUTE_PNG_IMPLEMENTATION
        #define CUTE_PNG_IMPLEMENTATION
        #define CUTE_PNG_ALLOCA PNTR_MALLOC
        #define CUTE_PNG_ALLOC PNTR_MALLOC
        #define CUTE_PNG_FREE PNTR_FREE
        #define CUTE_PNG_CALLOC(num, size) PNTR_MALLOC((num) * (size))
        #define CUTE_PNG_REALLOC PNTR_REALLOC
        #define CUTE_PNG_MEMCPY PNTR_MEMCPY
        #define CUTE_PNG_MEMSET PNTR_MEMSET
        #define CUTE_PNG_FPRINTF (void)
        #define CUTE_PNG_ASSERT(condition) 0 // Skip assertions
        #define CUTE_PNG_SEEK_SET 0
        #define CUTE_PNG_SEEK_END 0
        #define CUTE_PNG_FILE void
        #define CUTE_PNG_FOPEN(filename, mode) (CUTE_PNG_FILE*)filename
        #define CUTE_PNG_FSEEK(stream, offset, origin) offset
        #define CUTE_PNG_FREAD(data, size, num, fp) (void)(data)
        #define CUTE_PNG_FTELL(fp) 0
        #define CUTE_PNG_FWRITE(data, size, num, fp) (void)(data)
        #define CUTE_PNG_FCLOSE (void)
        #define CUTE_PNG_FERROR(fp) 1
        #define CUTE_PNG_ATLAS_MUST_FIT 1
        #define CUTE_PNG_ATLAS_FLIP_Y_AXIS_FOR_UV 0
        #define CUTE_PNG_ATLAS_EMPTY_COLOR 0
    #endif  // PNTR_NO_CUTE_PNG_IMPLEMENTATION

    #if defined(__GNUC__) || defined(__clang__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpragmas"
        #pragma GCC diagnostic ignored "-Wunknown-pragmas"
        #pragma GCC diagnostic ignored "-Wsign-conversion"
        #pragma GCC diagnostic ignored "-Wconversion"
        #pragma GCC diagnostic ignored "-Wunused-function"
        #pragma GCC diagnostic ignored "-Wunused-variable"
        #pragma GCC diagnostic ignored "-Wsign-compare"
        #pragma GCC diagnostic ignored "-Wunused-value"
    #endif // defined(__GNUC__) || defined(__clang__)

    #include "external/cute_png.h"
    #define PNTR_NO_CUTE_PNG_IMPLEMENTATION
    #ifdef CUTE_PNG_IMPLEMENTATION
        #undef CUTE_PNG_IMPLEMENTATION
    #endif

    #if defined(__GNUC__) || defined(__clang__)
        #pragma GCC diagnostic pop
    #endif // defined(__GNUC__) || defined(__clang__)

#endif // PNTR_NO_SUPPORT_PNG

// STB TrueType
#ifdef PNTR_SUPPORT_TTF
    #ifdef PNTR_NO_STB_TRUETYPE_IMPLEMENTATION
        #ifdef STB_TRUETYPE_IMPLEMENTATION
            #undef STB_TRUETYPE_IMPLEMENTATION
        #endif  // STB_TRUETYPE_IMPLEMENTATION
    #else  // PNTR_NO_STB_TRUETYPE_IMPLEMENTATION
        #ifndef STBTT_malloc
            #define STBTT_malloc(x,u) ((void)(u), PNTR_MALLOC(x))
        #endif  // STBTT_malloc

        #ifndef STBTT_free
            #define STBTT_free(x,u)   ((void)(u), PNTR_FREE(x))
        #endif  // STBTT_free

        #ifndef STBTT_assert
            #define STBTT_assert(x) ((void)(0))
        #endif  // STBTT_assert

        #ifndef STBTT_strlen
            #include <string.h>
            #define STBTT_strlen(x) strlen(x)
        #endif  // STBTT_strlen

        #ifndef STBTT_memcpy
            #define STBTT_memcpy PNTR_MEMCPY
            #define STBTT_memset PNTR_MEMSET
        #endif  // STBTT_memcpy

        #define STB_TRUETYPE_IMPLEMENTATION
    #endif  // PNTR_NO_STB_TRUETYPE_IMPLEMENTATION

    #if defined(__GNUC__) || defined(__clang__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpragmas"
        #pragma GCC diagnostic ignored "-Wunknown-pragmas"
        #pragma GCC diagnostic ignored "-Wsign-conversion"
        #pragma GCC diagnostic ignored "-Wconversion"
    #endif  // defined(__GNUC__) || defined(__clang__)

    #include "external/stb_truetype.h"
    #define PNTR_NO_STB_TRUETYPE_IMPLEMENTATION

    #if defined(__GNUC__) || defined(__clang__)
        #pragma GCC diagnostic pop
    #endif  // defined(__GNUC__) || defined(__clang__)
#endif  // PNTR_SUPPORT_TTF

// STB Image Resize
#ifdef PNTR_SUPPORT_FILTER_SMOOTH
    #ifdef PTNR_NO_STB_IMAGE_RESIZE_IMPLEMENTATION
        #ifdef STB_IMAGE_RESIZE_IMPLEMENTATION
            #undef STB_IMAGE_RESIZE_IMPLEMENTATION
        #endif  // STB_IMAGE_RESIZE_IMPLEMENTATION
        // Just declare the function that we need from stb_image_resize.
        int stbir_resize_uint8_srgb(const unsigned char *input_pixels, int input_w, int input_h, int input_stride_in_bytes,
            unsigned char *output_pixels, int output_w, int output_h, int output_stride_in_bytes,
            int num_channels, int alpha_channel, int flags);
    #else  // PTNR_NO_STB_IMAGE_RESIZE_IMPLEMENTATION
        #define STB_IMAGE_RESIZE_IMPLEMENTATION
        #ifndef STBIR_MALLOC
            #define STBIR_MALLOC(size, context) ((void)(context), PNTR_MALLOC(size))
        #endif  // STBIR_MALLOC
        #ifndef STBIR_FREE
            #define STBIR_FREE(ptr, context) ((void)(context), PNTR_FREE(ptr))
        #endif  // STBIR_FREE
        #ifndef STBIR_ASSERT
            #define STBIR_ASSERT(val) ((void)(val))
        #endif  // STBIR_ASSERT

        #if defined(__GNUC__) || defined(__clang__)
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wpragmas"
            #pragma GCC diagnostic ignored "-Wunknown-pragmas"
            #pragma GCC diagnostic ignored "-Wsign-conversion"
            #pragma GCC diagnostic ignored "-Wconversion"
            #pragma GCC diagnostic ignored "-Wunused-parameter"
        #endif  // defined(__GNUC__) || defined(__clang__)

        #include "external/stb_image_resize.h"

        #if defined(__GNUC__) || defined(__clang__)
            #pragma GCC diagnostic pop
        #endif  // defined(__GNUC__) || defined(__clang__)

        #define PTNR_NO_STB_IMAGE_RESIZE_IMPLEMENTATION
    #endif  // PTNR_NO_STB_IMAGE_RESIZE_IMPLEMENTATION
#endif  // PNTR_SUPPORT_FILTER_SMOOTH

/**
 * The last error that was reported from pntr.
 *
 * This will not work across different threads.
 *
 * @see pntr_get_error
 * @see pntr_set_error
 * @internal
 * @private
 */
const char* _pntr_error;

/**
 * Gets the last error that was reported.
 *
 * @return The last error, or NULL if there wasn't an error.
 */
inline const char* pntr_get_error() {
    return _pntr_error;
}

/**
 * Sets an error.
 *
 * @param error A string representing the error that had just occured.
 *
 * @return Always returns NULL.
 */
inline void* pntr_set_error(const char* error) {
    _pntr_error = error;
    return NULL;
}

/**
 * Create a new image at the given width and height.
 *
 * This method will not clear the image upon instantiation.
 *
 * @param width The width of the new image.
 * @param height The height of the new image.
 *
 * @return The new image that is the size of the given width/height.
 */
pntr_image* pntr_new_image(int width, int height) {
    if (width <= 0 || height <= 0) {
        return pntr_set_error("pntr_new_image() requires a valid width and height");
    }

    pntr_image* image = (pntr_image*)PNTR_MALLOC(sizeof(pntr_image));
    if (image == NULL) {
        return pntr_set_error("pntr_new_image() failed to allocate memory for pntr_image");
    }

    image->pitch = width * (int)sizeof(pntr_color);
    image->width = width;
    image->height = height;
    image->data = (pntr_color*)PNTR_MALLOC(image->pitch * height);
    if (image->data == NULL) {
        PNTR_FREE(image);
        return pntr_set_error("pntr_new_image() failed to allocate memory for pntr_image data");
    }

    return image;
}

/**
 * Generate a new image with the given color.
 *
 * @param width The width of the new image.
 * @param height The height of the given image.
 * @param color The desired color that will fill the new image.
 *
 * @return A pointer to the new image in memory.
 */
pntr_image* pntr_gen_image_color(int width, int height, pntr_color color) {
    pntr_image* image = pntr_new_image(width, height);
    pntr_clear_background(image, color);

    return image;
}

/**
 * Create an image duplicate.
 *
 * @param image The image that you want to copy.
 *
 * @return A pointer to the new image that is a copy of the original image.
 */
pntr_image* pntr_image_copy(pntr_image* image) {
    if (image == NULL) {
        return pntr_set_error("pntr_image_copy() requires valid image data");
    }

    pntr_image* newImage = pntr_new_image(image->width, image->height);
    if (newImage == NULL) {
        return NULL;
    }

    PNTR_MEMCPY(newImage->data, image->data, newImage->pitch * newImage->height);

    return newImage;
}

/**
 * Get a new rectangle representing the intersection of the two given rectangles.
 *
 * @param rec1 A pointer to the first rectangle.
 * @param rec2 A pointer to the second rectangle.
 *
 * @return A new rectangle that represents the intersection of the original rectangles.
 */
pntr_rectangle pntr_rectangle_intersect(pntr_rectangle *rec1, pntr_rectangle *rec2) {
    int left   = PNTR_MAX(rec1->x, rec2->x);
    int right  = PNTR_MIN(rec1->x + rec1->width, rec2->x + rec2->width);
    int top    = PNTR_MAX(rec1->y, rec2->y);
    int bottom = PNTR_MIN(rec1->y + rec1->height, rec2->y + rec2->height);
    int width  = right - left;
    int height = bottom - top;
    return CLITERAL(pntr_rectangle) { left, top, PNTR_MAX(width, 0), PNTR_MAX(height, 0) };
}

/**
 * Creates a new image from a section of the original image.
 */
pntr_image* pntr_image_from_image(pntr_image* image, int x, int y, int width, int height) {
    if (image == NULL) {
        return pntr_set_error("pntr_image_from_image() requires valid source image");
    }

    pntr_rectangle srcRect = CLITERAL(pntr_rectangle){x, y, width, height};
    pntr_rectangle imgRect = CLITERAL(pntr_rectangle){0, 0, image->width, image->height};
    srcRect = pntr_rectangle_intersect(&imgRect, &srcRect);

    if (srcRect.width <= 0 || srcRect.height <= 0) {
        return NULL;
    }

    pntr_image* result = pntr_new_image(srcRect.width, srcRect.height);
    if (result == NULL) {
        return NULL;
    }

    for (int y = 0; y < srcRect.height; y++) {
        PNTR_MEMCPY(((unsigned char *)result->data) + y * srcRect.width * (int)sizeof(pntr_color),
            ((unsigned char *)image->data) + ((y + srcRect.y) * image->width + srcRect.x) * (int)sizeof(pntr_color),
            srcRect.width * (int)sizeof(pntr_color));
    }

    return result;
}

/**
 * Unloads the given image from memory.
 *
 * @param image The image to unload from memory.
 */
void pntr_unload_image(pntr_image* image) {
    if (image == NULL) {
        return;
    }

    if (image->data != NULL) {
        PNTR_FREE(image->data);
        image->data = NULL;
    }

    PNTR_FREE(image);
}

/**
 * Draws a line on the destination image.
 */
void pntr_draw_horizontal_line_unsafe(pntr_image* dst, int posX, int posY, int width, pntr_color color) {
    pntr_color *row = dst->data + posY * (dst->pitch >> 2);
    for (int x = posX; x < posX + width; ++x) {
        row[x] = color;
    }
}

/**
 * Clears an image with the given color.
 *
 * @param image The image to clear.
 * @param color The color to fill the image with.
 */
void pntr_clear_background(pntr_image* image, pntr_color color) {
    if (image == NULL) {
        return;
    }

    // Blank
    if (color.a == 0) {
        PNTR_MEMSET((void*)image->data, 0, (size_t)(image->height * image->pitch));
        return;
    }

    // Draw the first line
    pntr_draw_horizontal_line_unsafe(image, 0, 0, image->width, color);

    // Copy the line for the rest of the screen
    for (int y = 1; y < image->height; y++) {
        PNTR_MEMCPY(pntr_image_get_color_pointer(image, 0, y), image->data, image->pitch);
    }
}

/**
 * Creates a new color, provided the given red, green, blue, and alpha colors.
 *
 * @param red The red channel.
 * @param green The green channel.
 * @param blue The blue channel.
 * @param alpha The alpha channel.
 *
 * @return The color with the given red, green, blue, and alpha components.
 */
inline pntr_color pntr_new_color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) {
    return CLITERAL(pntr_color){
        .r = red,
        .g = green,
        .b = blue,
        .a = alpha
    };
}

/**
 * Get a pntr_color from a RGBA hexadecimal value
 *
 * @param hexValue The hexadecimal value representing the color. For example: 0x052c46ff.
 *
 * @return The color representing the given hex value.
 */
inline pntr_color pntr_get_color(unsigned int hexValue) {
    return CLITERAL(pntr_color){
        .r = (unsigned char)(hexValue >> 24) & 0xFF,
        .g = (unsigned char)(hexValue >> 16) & 0xFF,
        .b = (unsigned char)(hexValue >> 8) & 0xFF,
        .a = (unsigned char)hexValue & 0xFF
    };
}

inline unsigned char pntr_color_get_r(pntr_color color) {
    return color.r;
}

inline unsigned char pntr_color_get_g(pntr_color color) {
    return color.g;
}

inline unsigned char pntr_color_get_b(pntr_color color) {
    return color.b;
}

inline unsigned char pntr_color_get_a(pntr_color color) {
    return color.a;
}

inline void pntr_color_set_r(pntr_color* color, unsigned char r) {
    color->r = r;
}

inline void pntr_color_set_g(pntr_color* color, unsigned char g) {
    color->g = g;
}

inline void pntr_color_set_b(pntr_color* color, unsigned char b) {
    color->b = b;
}

inline void pntr_color_set_a(pntr_color* color, unsigned char a) {
    color->a = a;
}

inline void pntr_color_get_rgba(pntr_color color, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a) {
    *r = color.r;
    *g = color.g;
    *b = color.b;
    *a = color.a;
}

/**
 * Draws a pixel on the given image.
 */
inline void pntr_draw_pixel(pntr_image* dst, int x, int y, pntr_color color) {
    // TODO: Allow drawing Alpha-Transparency pixels
    if ((color.a == 0) || (dst == NULL) || (x < 0) || (x >= dst->width) || (y < 0) || (y >= dst->height)) {
        return;
    }

    pntr_draw_pixel_unsafe(dst, x, y, color);
}

/**
 * Draws a line on the given image.
 */
void pntr_draw_line(pntr_image *dst, int startPosX, int startPosY, int endPosX, int endPosY, pntr_color color) {
    if (dst == NULL) {
        return;
    }

    int changeInX = (endPosX - startPosX);
    int absChangeInX = (changeInX < 0) ? -changeInX : changeInX;
    int changeInY = (endPosY - startPosY);
    int absChangeInY = (changeInY < 0) ? -changeInY : changeInY;

    int startU, startV, endU, stepV;
    int A, B, P;
    int reversedXY = (absChangeInY < absChangeInX);

    if (reversedXY) {
        A = 2 * absChangeInY;
        B = A - 2 * absChangeInX;
        P = A - absChangeInX;

        if (changeInX > 0) {
            startU = startPosX;
            startV = startPosY;
            endU = endPosX;
            //endV = endPosY;
        }
        else {
            startU = endPosX;
            startV = endPosY;
            endU = startPosX;
            //endV = startPosY;

            // Since start and end are reversed
            changeInX = -changeInX;
            changeInY = -changeInY;
        }

        stepV = (changeInY < 0) ? -1 : 1;

        pntr_draw_pixel(dst, startU, startV, color);
    }
    else {
        A = 2 * absChangeInX;
        B = A - 2 * absChangeInY;
        P = A - absChangeInY;

        if (changeInY > 0) {
            startU = startPosY;
            startV = startPosX;
            endU = endPosY;
        }
        else {
            startU = endPosY;
            startV = endPosX;
            endU = startPosY;

            changeInX = -changeInX;
            changeInY = -changeInY;
        }

        stepV = (changeInX < 0) ? -1 : 1;

        pntr_draw_pixel(dst, startV, startU, color);
    }

    for (int u = startU + 1, v = startV; u <= endU; u++) {
        if (P >= 0) {
            v += stepV;
            P += B;
        }
        else {
            P += A;
        }

        if (reversedXY) {
            pntr_draw_pixel(dst, u, v, color);
        }
        else {
            pntr_draw_pixel(dst, v, u, color);
        }
    }
}

/**
 * Draws a rectangle on the given image.
 */
inline void pntr_draw_rectangle(pntr_image* dst, int posX, int posY, int width, int height, pntr_color color) {
    pntr_draw_rectangle_rec(dst, CLITERAL(pntr_rectangle){posX, posY, width, height}, color);
}

/**
 * Draws a rectangle on the given image, using a rectangle as input data.
 */
void pntr_draw_rectangle_rec(pntr_image* dst, pntr_rectangle rect, pntr_color color) {
    if (dst == NULL) {
        return;
    }

    pntr_rectangle dstRect = CLITERAL(pntr_rectangle){0, 0, dst->width, dst->height};
    rect = pntr_rectangle_intersect(&rect, &dstRect);
    if (rect.width <= 0 || rect.height <= 0) {
        return;
    }

    pntr_draw_horizontal_line_unsafe(dst, rect.x, rect.y, rect.width, color);

    pntr_color* srcPixel = pntr_image_get_color_pointer(dst, rect.x, rect.y);
    for (int y = rect.y + 1; y < rect.y + rect.height; y++) {
        PNTR_MEMCPY(pntr_image_get_color_pointer(dst, rect.x, y), srcPixel, (size_t)rect.width * sizeof(pntr_color));
    }
}

/**
 * Draws a circle on the given image.
 */
void pntr_draw_circle(pntr_image* dst, int centerX, int centerY, int radius, pntr_color color) {
    if (dst == NULL) {
        return;
    }

    int largestX = radius;
    int r2 = radius * radius;
    for (int y = 0; y <= radius; ++y) {
        int y2 = y * y;
        for (int x = largestX; x >= 0; --x) {
            if (x * x + y2 <= r2) {
                pntr_draw_horizontal_line_unsafe(dst, centerX - x, centerY + y, x, color);
                pntr_draw_horizontal_line_unsafe(dst, centerX - x, centerY - y, x, color);
                pntr_draw_horizontal_line_unsafe(dst, centerX, centerY + y, x, color);
                pntr_draw_horizontal_line_unsafe(dst, centerX, centerY - y, x, color);
                largestX = x;
                break;
            }
        }
    }
}

/**
 * Get image pixel color at (x, y) position.
 */
pntr_color pntr_image_get_color(pntr_image* image, int x, int y) {
    if (image == NULL) {
        return PNTR_BLANK;
    }
    if (x < 0 || y < 0 || x >= image->width || y >= image->height) {
        return PNTR_BLANK;
    }
    return image->data[y * (image->pitch >> 2) + x];
}

/**
 * Get a pointer to the color at (x, y) position.
 *
 * @param image The image to get the pointer of.
 * @param x The x position of the pixel on the image.
 * @param y The y position of the pixel on the image.
 *
 * @return A pointer to the pixel on the image.
 */
inline pntr_color* pntr_image_get_color_pointer(pntr_image* image, int x, int y) {
    return image->data + y * (image->pitch >> 2) + x;
}

/**
 * Load an image from memory buffer.
 *
 * Not supported if PNTR_NO_SUPPORT_PNG is defined.
 *
 * @see PNTR_NO_SUPPORT_PNG
 */
pntr_image* pntr_load_image_from_memory(const unsigned char *fileData, unsigned int dataSize) {
    if (fileData == NULL || dataSize <= 0) {
        return pntr_set_error("pntr_load_image_from_memory() requires valid file data");
    }

    #ifdef PNTR_NO_SUPPORT_PNG
        return pntr_set_error("pntr_load_image_from_memory() requires PNG support. PNTR_NO_SUPPORT_PNG was defined.");
    #else
        cp_image_t image = cp_load_png_mem(fileData, (int)dataSize);
        if (image.pix == NULL) {
            return pntr_set_error(cp_error_reason);
        }

        pntr_image* output = pntr_image_from_pixelformat((const void*)image.pix, image.w, image.h, PNTR_PIXELFORMAT_RGBA8888);
        cp_free_png(&image);

        return output;
    #endif
}

/**
 * Load an image from file.
 *
 * @param fileName The name of the file to load from the file system.
 *
 * @return The newly loaded file.
 */
pntr_image* pntr_load_image(const char* fileName) {
    if (fileName == NULL) {
        return pntr_set_error("pntr_load_image() requires a valid fileName");
    }

    unsigned int bytesRead;
    const unsigned char* fileData = pntr_load_file(fileName, &bytesRead);
    if (fileData == NULL) {
        return pntr_set_error("Failed to load file");
    }

    pntr_image* output = pntr_load_image_from_memory(fileData, bytesRead);
    pntr_unload_file((unsigned char*)fileData);

    return output;
}

/**
 * Draw an image onto the destination image.
 */
inline void pntr_draw_image(pntr_image* dst, pntr_image* src, int posX, int posY) {
    pntr_draw_image_rec(dst, src, CLITERAL(pntr_rectangle){0, 0, src->width, src->height}, posX, posY);
}

/**
 * Get source alpha-blended into destination color.
 *
 * @param dst The destination color.
 * @param src The source color.
 *
 * @return The new alpha-blended color.
 */
pntr_color pntr_color_alpha_blend(pntr_color dst, pntr_color src) {
    if (src.a == 0) {
        return dst;
    }
    else if (src.a == 255) {
        return src;
    }

    pntr_color out;
    unsigned int alpha = (unsigned int)src.a + 1;     // We are shifting by 8 (dividing by 256), so we need to take that excess into account
    out.a = (unsigned char)(((unsigned int)alpha * 256 + (unsigned int)dst.a * (256 - alpha)) >> 8);

    if (out.a > 0) {
        out.r = (unsigned char)((((unsigned int)src.r * alpha * 256 + (unsigned int)dst.r * (unsigned int)dst.a * (256 - alpha)) / out.a) >> 8);
        out.g = (unsigned char)((((unsigned int)src.g * alpha * 256 + (unsigned int)dst.g * (unsigned int)dst.a * (256 - alpha)) / out.a) >> 8);
        out.b = (unsigned char)((((unsigned int)src.b * alpha * 256 + (unsigned int)dst.b * (unsigned int)dst.a * (256 - alpha)) / out.a) >> 8);
    }

    return out;
}

/**
 * Draw a source image within a destination image.
 *
 * You can improve performance of this method by defining PNTR_NO_ALPHABLEND.
 *
 * @param dst The destination image.
 * @param src The source image.
 * @param srcRect The source rectangle of what to draw from the source image.
 * @param posX Where to draw the image on the x coordinate.
 * @param posY Where to draw the image on the y coordinate.
 *
 * @see PNTR_NO_ALPHABLEND
 */
void pntr_draw_image_rec(pntr_image* dst, pntr_image* src, pntr_rectangle srcRect, int posX, int posY) {
    if (dst == NULL || src == NULL || posX >= dst->width || posY >= dst->height) {
        return;
    }

    // Scaling is not supported
    pntr_rectangle dstRect = CLITERAL(pntr_rectangle){posX, posY, srcRect.width, srcRect.height};
    pntr_rectangle dstCanvas = CLITERAL(pntr_rectangle){0, 0, dst->width, dst->height};

    // Update the source coordinates based on the destination
    if (dstRect.x < 0) {
        srcRect.x -= dstRect.x;
        srcRect.width += dstRect.x;
    }
    if (dstRect.y < 0) {
        srcRect.y -= dstRect.y;
        srcRect.height += dstRect.y;
    }

    // Figure out the final desintation
    dstRect = pntr_rectangle_intersect(&dstRect, &dstCanvas);
    dstRect.width = PNTR_MIN(dstRect.width, srcRect.width);
    dstRect.height = PNTR_MIN(dstRect.height, srcRect.height);

    // Final sanity checks
    if (srcRect.width <= 0 || srcRect.height <= 0 || dstRect.width <= 0 || dstRect.height <= 0 || dstRect.x >= dst->width || dstRect.y >= dst->height) {
        return;
    }

    // Determine how many bits to skip for each line.
    int dst_skip = dst->pitch >> 2;
    int src_skip = src->pitch >> 2;

    // Find the first pixel to render.
    pntr_color *dstPixel = dst->data + dst_skip * dstRect.y + dstRect.x;
    pntr_color *srcPixel = src->data + src_skip * srcRect.y + srcRect.x;

    int rows_left = dstRect.height;
    int cols = dstRect.width;

    while (rows_left-- > 0) {
        for (int x = 0; x < cols; ++x) {
            #ifndef PNTR_NO_ALPHABLEND
                dstPixel[x] = pntr_color_alpha_blend(dstPixel[x], srcPixel[x]);
            #else
                // Alpha transparency threshold
                if (srcPixel[x].a >= 128) {
                    dstPixel[x] = srcPixel[x];
                }
            #endif  // PNTR_NO_ALPHABLEND
        }

        dstPixel += dst_skip;
        srcPixel += src_skip;
    }
}

/**
 * Creates a new image based on the given image data, from the given original pixel format.
 *
 * This will not clear the original imageData.
 *
 * @param imageData The data of the image in memory.
 * @param width The width of the image.
 * @param height The height of the image.
 * @param pixelFormat The pixel format of the image in memory.
 *
 * @return A new image built from the given image data.
 */
pntr_image* pntr_image_from_pixelformat(const void* imageData, int width, int height, pntr_pixelformat pixelFormat) {
    if (imageData == NULL || width <= 0 || height <= 0 || pixelFormat < 0) {
        return pntr_set_error("pntr_image_from_data() requires valid imageData");
    }

    // Check how we are to convert the pixel format.
    switch (pixelFormat) {
        case PNTR_PIXELFORMAT_GRAYSCALE: {
            pntr_image* output = pntr_new_image(width, height);
            if (output == NULL) {
                return NULL;
            }

            unsigned char* source = (unsigned char*)imageData;
            for (int i = 0; i < width * height; i++) {
                output->data[i] = pntr_get_pixel_color((void*)(source + i), pixelFormat);
            }

            return output;
        }

        case PNTR_PIXELFORMAT_ARGB8888:
        case PNTR_PIXELFORMAT_RGBA8888: {
            pntr_image* output = pntr_new_image(width, height);

            pntr_color* source = (pntr_color*)imageData;
            for (int i = 0; i < width * height; i++) {
                output->data[i] = pntr_get_pixel_color((void*)(source + i), pixelFormat);
            }

            return output;
        }

        default: {
            return pntr_set_error("Unknown pixel format");
        }
    }
}

/**
 * Resize an image.
 *
 * @param image The image to resize.
 * @param newWidth The desired width of the new image.
 * @param newHeight THe desired height of the new image.
 * @param filter Which filter to apply when resizing. If you're unsure, use PNTR_FILTER_DEFAULT.
 */
pntr_image* pntr_image_resize(pntr_image* image, int newWidth, int newHeight, pntr_filter filter) {
    if (image == NULL || newWidth <= 0 || newHeight <= 0 || filter < 0) {
        return pntr_set_error("pntr_image_resize() requires a valid image and width/height");
    }

    pntr_image* output = pntr_new_image(newWidth, newHeight);
    if (output == NULL) {
        return NULL;
    }

    // The default uses the smooth filter if it's available, but falls back to bilinear.
    if (filter == PNTR_FILTER_DEFAULT) {
        #ifdef PNTR_SUPPORT_FILTER_SMOOTH
            filter = PNTR_FILTER_SMOOTH;
        #else
            filter = PNTR_FILTER_BILINEAR;
        #endif
    }

    switch (filter) {
        case PNTR_FILTER_SMOOTH: {
            #ifndef PNTR_SUPPORT_FILTER_SMOOTH
                pntr_unload_image(output);
                return pntr_set_error("To use the Smooth filter, define PNTR_SUPPORT_FILTER_SMOOTH, or use PNTR_FILTER_DEFAULT to have a fallback to PNTR_FILTER_B");
            #else
                int result = stbir_resize_uint8_srgb(
                    (const unsigned char*)image->data,
                    image->width, image->height,
                    0, // Input stride
                    (unsigned char*)output->data, output->width, output->height,
                    0, // Output stride
                    4, // Number of channels
                    // TODO: pntr_image_resize() - Is the alpha channel always 3?
                    #if defined(PNTR_PIXELFORMAT_RGBA)
                        3, // Alpha channel
                    #elif defined(PNTR_PIXELFORMAT_ARGB)
                        3, // Alpha channel
                    #endif
                    0);

                if (result == 0) {
                    pntr_unload_image(output);
                    return pntr_set_error("Failed to reszize imagess");
                }
            #endif
        }
        break;
        case PNTR_FILTER_BILINEAR: {
            float xRatio = (float)image->width / (float)newWidth;
            float yRatio = (float)image->height / (float)newHeight;

            for (int y = 0; y < newHeight; y++) {
                float srcY = (float)y * yRatio;
                int srcYPixel = (int)srcY;
                int srcYPixelPlusOne = y == newHeight - 1 ? (int)srcY : (int)srcY + 1;
                for (int x = 0; x < newWidth; x++) {
                    float srcX = (float)x * xRatio;
                    int srcXPixel = (int)srcX;
                    int srcXPixelPlusOne = x == newWidth - 1 ? (int)srcX : (int)srcX + 1;
                    output->data[(y * (output->pitch >> 2)) + x] = pntr_color_bilinear_interpolate(
                        image->data[srcYPixel * (image->pitch >> 2) + srcXPixel],
                        image->data[srcYPixelPlusOne * (image->pitch >> 2) + srcXPixel],
                        image->data[srcYPixel * (image->pitch >> 2) + srcXPixelPlusOne],
                        image->data[srcYPixelPlusOne * (image->pitch >> 2) + srcXPixelPlusOne],
                        srcX - PNTR_FLOORF(srcX),
                        srcY - PNTR_FLOORF(srcY));
                }
            }
        }
        break;
        case PNTR_FILTER_NEARESTNEIGHBOR:
        default: {
            int xRatio = (image->width << 16) / newWidth + 1;
            int yRatio = (image->height << 16) / newHeight + 1;

            for (int y = 0; y < newHeight; y++) {
                for (int x = 0; x < newWidth; x++) {
                    int x2 = (x * xRatio) >> 16;
                    int y2 = (y * yRatio) >> 16;
                    output->data[(y * (output->pitch >> 2)) + x] = image->data[(y2 * image->width) + x2];
                }
            }
        }
        break;
    }

    return output;
}

/**
 * Flip an image vertically.
 *
 * @param image The image to flip.
 */
void pntr_image_flip_vertical(pntr_image* image) {
    if (image == NULL) {
        return;
    }

    unsigned char *flippedData = (unsigned char*)PNTR_MALLOC(image->height * image->pitch);
    for (int y = image->height - 1, offsetSize = 0; y >= 0; y--) {
        memcpy(flippedData + offsetSize, ((unsigned char*)image->data) + y * image->pitch, (size_t)image->pitch);
        offsetSize += image->pitch;
    }

    PNTR_FREE(image->data);
    image->data = (pntr_color*)flippedData;
}

/**
 * Flip an image horizontally.
 *
 * @param image The image to flip.
 */
void pntr_image_flip_horizontal(pntr_image* image) {
    if (image == NULL) {
        return;
    }

    pntr_color* data = (pntr_color*)image->data;
    pntr_color swap;
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width / 2; x++) {
            swap = data[y * image->width + x];
            data[y * image->width + x] = data[y * image->width + (image->width - 1 - x)];
            data[y * image->width + (image->width - 1 - x)] = swap;
        }
    }
}

/**
 * Replace the given color with another color on an image.
 *
 * @param image The image to process.
 * @param color The color to search for.
 * @param replace The color that will replace the original color.
 */
void pntr_image_color_replace(pntr_image* image, pntr_color color, pntr_color replace) {
    if (image == NULL) {
        return;
    }

    for (int i = 0; i < image->width * image->height; i++) {
        if (image->data[i].data == color.data) {
            image->data[i].data = replace.data;
        }
    }
}

/**
 * Tints the given color by another color.
 *
 * @param color The source color to apply the tint to.
 * @param tint The tint color.
 *
 * @return The newly tinted color.
 *
 * @see pntr_image_color_tint()
 */
inline pntr_color pntr_color_tint(pntr_color color, pntr_color tint) {
    return CLITERAL(pntr_color) {
        .r = (unsigned char)(((float)color.r / 255.0f * (float)tint.r / 255.0f) * 255.0f),
        .g = (unsigned char)(((float)color.g / 255.0f * (float)tint.g / 255.0f) * 255.0f),
        .b = (unsigned char)(((float)color.b / 255.0f * (float)tint.b / 255.0f) * 255.0f),
        .a = (unsigned char)(((float)color.a / 255.0f * (float)tint.a / 255.0f) * 255.0f)
    };
}

/**
 * Change brightness of the given color by a factor from -1.0f to 1.0f.
 *
 * @param color The color to change the brightness of.
 * @param factor The factor in which to change the brightness from -1.0f to 1.0f.
 *
 * @see pntr_image_color_brightness()
 */
pntr_color pntr_color_brightness(pntr_color color, float factor) {
    if (factor < -1.0f) {
        factor = -1.0f;
    }
    else if (factor > 1.0f) {
        factor = 1.0f;
    }

    if (factor < 0.0f) {
        factor = 1.0f + factor;
        color.r = (unsigned char)((float)color.r * factor);
        color.g = (unsigned char)((float)color.g * factor);
        color.b = (unsigned char)((float)color.b * factor);
    }
    else {
        color.r = (unsigned char)(((float)(255 - color.r) * factor) + color.r);
        color.g = (unsigned char)(((float)(255 - color.r) * factor) + color.r);
        color.b = (unsigned char)(((float)(255 - color.r) * factor) + color.r);
    }

    return color;
}

/**
 * Fades the given color by a factor from -1.0f to 1.0f.
 *
 * @param color The color that you would like to fade.
 * @param factor The factor of which to fade the color, from -1.0f to 1.0f.
 *
 * @return The faded new color.
 *
 * @see pntr_image_color_fade()
 */
pntr_color pntr_color_fade(pntr_color color, float factor) {
    if (factor < -1.0f) {
        factor = -1.0f;
    }
    else if (factor > 1.0f) {
        factor = 1.0f;
    }

    if (factor < 0.0f) {
        color.a = (unsigned char)((float)color.a * (1.0f + factor));
    }
    else {
        color.a = (unsigned char)(((float)(255 - color.a) * factor) + color.a);
    }

    return color;
}

/**
 * Fade an image by the given factor.
 *
 * @param image The image to fade.
 * @param factor The factor of which to fade the image, from -1.0f to 1.0f.
 *
 * @see pntr_color_fade()
 */
void pntr_image_color_fade(pntr_image* image, float factor) {
    if (image == NULL) {
        return;
    }

    if (factor < -1.0f) {
        factor = -1.0f;
    }
    else if (factor > 1.0f) {
        factor = 1.0f;
    }

    for (int i = 0; i < image->width * image->height; i++) {
        if (image->data[i].a > 0) {
            image->data[i] = pntr_color_fade(image->data[i], factor);
        }
    }
}

/**
 * Set the pixel color of the given pixel color pointer.
 *
 * @param dstPtr A pointer to the pixel in memory.
 * @param color The color to apply.
 * @param dstPixelFormat The desired destination pixel format.
 */
void pntr_set_pixel_color(void* dstPtr, pntr_color color, pntr_pixelformat dstPixelFormat) {
    if (PNTR_PIXELFORMAT == dstPixelFormat) {
        *((pntr_color*)dstPtr) = color;
        return;
    }

    switch (dstPixelFormat) {
        case PNTR_PIXELFORMAT_RGBA8888:
            *((uint32_t*)(dstPtr)) = ((uint32_t)color.a << 24) | ((uint32_t)color.b << 16) | ((uint32_t)color.g << 8) | (uint32_t)color.r;
        break;
        case PNTR_PIXELFORMAT_ARGB8888:
            // TODO: pntr_set_pixel_color() Verify the ARGB conversion.
            *((uint32_t*)(dstPtr)) = ((int)color.b << 24) | ((int)color.g << 16) | ((int)color.r << 8) | (int)color.a;
        break;
        case PNTR_PIXELFORMAT_GRAYSCALE: {
            float r = (float)color.r / 255.0f;
            float g = (float)color.g / 255.0f;
            float b = (float)color.b / 255.0f;
            ((unsigned char *)dstPtr)[0] = (unsigned char)((r * 0.299f + g * 0.587f + b * 0.114f) * 255.0f);
        }
        break;
    }
}

/**
 * Gets the pixel color of the given pointer.
 *
 * @param srcPtr A pointer to the given source data.
 * @param srcPixelFormat The pixel format of the given source color.
 *
 * @return The color of the pixel in memory.
 */
pntr_color pntr_get_pixel_color(void* srcPtr, pntr_pixelformat srcPixelFormat) {
    switch (srcPixelFormat) {
        case PNTR_PIXELFORMAT_RGBA8888:
            return CLITERAL(pntr_color) {
                .r = ((unsigned char *)srcPtr)[0],
                .g = ((unsigned char *)srcPtr)[1],
                .b = ((unsigned char *)srcPtr)[2],
                .a = ((unsigned char *)srcPtr)[3]
            };
        case PNTR_PIXELFORMAT_ARGB8888:
            return CLITERAL(pntr_color) {
                .a = ((unsigned char *)srcPtr)[0],
                .r = ((unsigned char *)srcPtr)[1],
                .g = ((unsigned char *)srcPtr)[2],
                .b = ((unsigned char *)srcPtr)[3]
            };
        case PNTR_PIXELFORMAT_GRAYSCALE:
            // White, with alpha determining grayscale value. Use tint to change color afterwards.
            return CLITERAL(pntr_color) {
                .r = 255,
                .g = 255,
                .b = 255,
                .a = ((unsigned char*)srcPtr)[0]
            };
    }

    return PNTR_BLANK;
}

/**
 * Tints the given image by the provided color.
 *
 * @param image The image to tint.
 * @param tint The color of which to tint the image by.
 *
 * @see pntr_color_tint()
 */
void pntr_image_color_tint(pntr_image* image, pntr_color tint) {
    if (image == NULL) {
        return;
    }

    for (int i = 0; i < image->width * image->height; i++) {
        image->data[i] = pntr_color_tint(image->data[i], tint);
    }
}

/**
 * Load a BMFont from the file system.
 *
 * @param fileName The name of the image file to load as the bmfont.
 * @param characters A string representing the characters to load from the atlas.
 *
 * @return The newly loaded font.
 *
 * @example examples/resources/bmfont.png
 */
pntr_font* pntr_load_bmfont(const char* fileName, const char* characters) {
    pntr_image* image = pntr_load_image(fileName);
    if (image == NULL) {
        return NULL;
    }

    return pntr_load_bmfont_from_image(image, characters);
}

pntr_font* pntr_load_bmfont_from_memory(const unsigned char* fileData, unsigned int dataSize, const char* characters) {
    if (fileData == NULL || dataSize == 0 || characters == NULL) {
        return pntr_set_error("pntr_load_bmfont_from_memory() requires valid file data, size and characters");
    }

    pntr_image* image = pntr_load_image_from_memory(fileData, dataSize);
    if (image == NULL) {
        return NULL;
    }

    return pntr_load_bmfont_from_image(image, characters);
}

pntr_font* _pntr_new_font(int numCharacters, pntr_image* atlas) {
    if (numCharacters <= 0) {
        return pntr_set_error("requires at least one character for fonts, none found");
    }

    pntr_font* font = PNTR_MALLOC(sizeof(pntr_font));
    if (font == NULL) {
        return pntr_set_error("pntr_new_font() failed to allocate pntr_font memory");
    }

    // Source Rectangles
    font->srcRects = PNTR_MALLOC(sizeof(pntr_rectangle) * (size_t)numCharacters);
    if (font->srcRects == NULL) {
        PNTR_FREE(font);
        return pntr_set_error("Failed to allocate memory for source rects");
    }

    // Glyph Rectangles
    font->glyphRects = PNTR_MALLOC(sizeof(pntr_rectangle) * (size_t)numCharacters);
    if (font->glyphRects == NULL) {
        PNTR_FREE(font->srcRects);
        PNTR_FREE(font);
        return pntr_set_error("Failed to allocate memory for glyphRects");
    }

    // Characters
    font->characters = PNTR_MALLOC(sizeof(char) * (size_t)numCharacters);
    if (font->characters == NULL) {
        PNTR_FREE(font->srcRects);
        PNTR_FREE(font->glyphRects);
        PNTR_FREE(font);
        return pntr_set_error("Failed to allocate memory for characters");
    }

    font->charactersLen = numCharacters;
    font->atlas = atlas;

    return font;
}

pntr_font* pntr_load_bmfont_from_image(pntr_image* image, const char* characters) {
    if (image == NULL || characters == NULL) {
        return pntr_set_error("pntr_load_bmfont_from_image() requires a valid image and characters");
    }

    pntr_color seperator = pntr_image_get_color(image, 0, 0);
    pntr_rectangle currentRectangle = CLITERAL(pntr_rectangle){1, 0, 0, image->height};

    // Find out how many characters there are.
    int numCharacters = 0;
    for (int i = 0; i < image->width; i++) {
        if (pntr_image_get_color(image, i, 0).data == seperator.data) {
            numCharacters++;
        }
    }

    pntr_font* font = _pntr_new_font(numCharacters, image);
    if (font == NULL) {
        return NULL;
    }

    // Set up the data structures.
    int currentCharacter = 0;
    for (int i = 1; i < image->width; i++) {
        if (pntr_image_get_color(image, i, 0).data == seperator.data) {
            font->characters[currentCharacter] = characters[currentCharacter];
            font->srcRects[currentCharacter] = currentRectangle;
            font->glyphRects[currentCharacter] = CLITERAL(pntr_rectangle) {
                .x = 0,
                .y = 0,
                .width = currentRectangle.width,
                .height = currentRectangle.height,
            };
            currentRectangle.width = 0;
            currentRectangle.x = i + 1;
            currentCharacter++;
        }
        else {
            // Increase the width of the active glyph rectangle.
            currentRectangle.width++;
        }
    }

    return font;
}

/**
 * Loads a TTY Font from the file system.
 *
 * @param fileName The name of the image file to load as the font.
 * @param glyphWidth The width of each character on the font atlas.
 * @param glyphHeight The height of each character on the font atlas.
 * @param characters A string of characters that appear in the image.
 *
 * @return The newly loaded TTY font.
 *
 * @example examples/resources/ttyfont-16x16.png
 */
pntr_font* pntr_load_ttyfont(const char* fileName, int glyphWidth, int glyphHeight, const char* characters) {
    pntr_image* image = pntr_load_image(fileName);
    if (image == NULL) {
        return NULL;
    }

    pntr_font* output = pntr_load_ttyfont_from_image(image, glyphWidth, glyphHeight, characters);
    if (output == NULL) {
        pntr_unload_image(image);
    }

    return output;
}

pntr_font* pntr_load_ttyfont_from_memory(const unsigned char* fileData, unsigned int dataSize, int glyphWidth, int glyphHeight, const char* characters) {
    if (fileData == NULL || dataSize <= 0 || characters == NULL || glyphWidth <= 0 || glyphHeight <= 0) {
        return pntr_set_error("pntr_load_ttyfont_from_memory() requires valid file data, size, glyph size, and characters");
    }

    pntr_image* image = pntr_load_image_from_memory(fileData, dataSize);
    if (image == NULL) {
        return NULL;
    }

    pntr_font* output = pntr_load_ttyfont_from_image(image, glyphWidth, glyphHeight, characters);
    if (output == NULL) {
        pntr_unload_image(image);
    }

    return output;
}

pntr_font* pntr_load_ttyfont_from_image(pntr_image* image, int glyphWidth, int glyphHeight, const char* characters) {
    if (image == NULL || characters == NULL || glyphWidth <= 0 || glyphHeight <= 0) {
        return pntr_set_error("pntr_load_ttyfont_from_image() requires a valid image and characters");
    }

    // Find out how many characters there are.
    int numCharacters = 0;
	int i = 0;
	while (characters[i++] != '\0') {
		numCharacters++;
	}

    // Create the font
    pntr_font* font = _pntr_new_font(numCharacters, image);
    if (font == NULL) {
        return NULL;
    }

    // Set up the font data.
    for (int currentCharIndex = 0; currentCharIndex < font->charactersLen; currentCharIndex++) {
        // Source rectangle
        font->srcRects[currentCharIndex] = CLITERAL(pntr_rectangle) {
            .x = (currentCharIndex % (image->width / glyphWidth)) * glyphWidth,
            .y = (currentCharIndex / (image->width / glyphWidth)) * glyphHeight,
            .width = glyphWidth,
            .height = glyphHeight
        };

        // Where the glyph will be rendered.
        font->glyphRects[currentCharIndex] = CLITERAL(pntr_rectangle) {
            .x = 0,
            .y = 0,
            .width = glyphWidth,
            .height = glyphHeight,
        };

        // Set the character.
        font->characters[currentCharIndex] = characters[currentCharIndex];
    }

    return font;
}

/**
 * Unloads the given font from memory.
 *
 * @param font The font to unload from memory.
 */
void pntr_unload_font(pntr_font* font) {
    if (font == NULL) {
        return;
    }

    if (font->atlas != NULL) {
        pntr_unload_image(font->atlas);
        font->atlas = NULL;
    }

    if (font->srcRects != NULL) {
        PNTR_FREE(font->srcRects);
        font->srcRects = NULL;
    }

    if (font->glyphRects != NULL) {
        PNTR_FREE(font->glyphRects);
        font->glyphRects = NULL;
    }

    if (font->characters != NULL) {
        PNTR_FREE(font->characters);
        font->characters = NULL;
    }

    PNTR_FREE(font);
}

void pntr_draw_text(pntr_image* dst, pntr_font* font, const char* text, int posX, int posY) {
    if (dst == NULL || font == NULL || text == NULL) {
        return;
    }

    int x = posX;
    int y = posY;
    int tallestCharacter;

    const char * currentChar = text;
    while (currentChar != NULL && *currentChar != '\0') {
        if (*currentChar == '\n') {
            // TODO: pntr_draw_text(): Allow for center/right alignment
            x = posX;
            y += tallestCharacter;
        }
        else {
            for (int i = 0; i < font->charactersLen; i++) {
                if (font->characters[i] == *currentChar) {
                    pntr_draw_image_rec(dst, font->atlas, font->srcRects[i], x + font->glyphRects[i].x, y + font->glyphRects[i].y);
                    x += font->glyphRects[i].x + font->glyphRects[i].width;
                    if (tallestCharacter < font->glyphRects[i].y + font->glyphRects[i].height) {
                        tallestCharacter = font->glyphRects[i].y + font->glyphRects[i].height;
                    }
                    break;
                }
            }
        }

        currentChar++;
    }
}

/**
 * Measures the horizontal length of the text when rendered with the given font.
 *
 * @param font The font to use when rendering the text.
 * @param text The text to measure the length of.
 *
 * @return The amount of pixels the text is when rendered with the font.
 */
inline int pntr_measure_text(pntr_font* font, const char* text) {
    return pntr_measure_text_ex(font, text).x;
}

pntr_vector pntr_measure_text_ex(pntr_font* font, const char* text) {
    if (font == NULL || text == NULL) {
        return CLITERAL(pntr_vector){0, 0};
    }

    pntr_vector output = CLITERAL(pntr_vector) {.x = 0, .y = 0};
    int currentX = 0;
    int currentY = 0;
    const char * currentChar = text;

    while (currentChar != NULL && *currentChar != '\0') {
        if (*currentChar == '\n') {
            output.y += currentY;
            currentX = 0;
        }
        else {
            for (int i = 0; i < font->charactersLen; i++) {
                if (font->characters[i] == *currentChar) {
                    currentX += font->glyphRects[i].x + font->glyphRects[i].width;
                    if (currentX > output.x) {
                        output.x = currentX;
                    }

                    // Find the tallest character
                    if (currentY < font->glyphRects[i].y + font->glyphRects[i].height) {
                        currentY = font->glyphRects[i].y + font->glyphRects[i].height;
                    }
                    break;
                }
            }
        }
        currentChar++;
    }

    // Has at least one line.
    output.y += currentY;

    return output;
}

/**
 * Generate an image with rendered text on it.
 *
 * @param font The font to use when rendering the text.
 * @param text The text to render.
 *
 * @return A new image with text on it, using the given font.
 */
pntr_image* pntr_gen_image_text(pntr_font* font, const char* text) {
    pntr_vector size = pntr_measure_text_ex(font, text);
    if (size.x <= 0 || size.y <= 0) {
        return NULL;
    }

    pntr_image* output = pntr_gen_image_color(size.x, size.y, PNTR_BLANK);
    if (output == NULL) {
        return NULL;
    }

    pntr_draw_text(output, font, text, 0, 0);
    return output;
}

/**
 * Load the default font.
 *
 * This must be unloaded manually afterwards with pntr_unload_font().
 *
 * Define PNTR_SUPPORT_DEFAULT_FONT to allow using the default 8x8 font.
 *
 * You can change this by defining your own PNTR_DEFAULT_FONT. It must match the definition of pntr_load_default_font()
 *
 * #define PNTR_DEFAULT_FONT load_my_font
 *
 * @return The default font, which must be unloaded when finished using.
 *
 * @see pntr_unload_font()
 * @see PNTR_SUPPORT_DEFAULT_FONT
 * @see PNTR_DEFAULT_FONT
 */
pntr_font* pntr_load_default_font() {
    #ifdef PNTR_DEFAULT_FONT
        return PNTR_DEFAULT_FONT();
    #elif defined(PNTR_SUPPORT_DEFAULT_FONT)
        // https://github.com/Grumbel/SDL_tty/blob/master/src/font8x8.h
        #include "external/font8x8.h"

        // Port the font8x8 data to a pntr_image
        pntr_image* atlas = pntr_image_from_pixelformat((const void*)font8x8_data, font8x8_width, font8x8_height, PNTR_PIXELFORMAT_RGBA8888);
        if (atlas == NULL) {
            return pntr_set_error("pntr_load_default_font() failed to convert default image");
        }

        // Load the font from the new image.
        pntr_font* font = pntr_load_ttyfont_from_image(atlas, font8x8_glyph_width, font8x8_glyph_height, font8x8_glyphs);
        if (font == NULL) {
            pntr_unload_image(atlas);
            return pntr_set_error("Failed to load default font from image");
        }

        return font;
    #else
        return pntr_set_error("pntr_load_default_font() requires PNTR_SUPPORT_DEFAULT_FONT");
    #endif
}

/**
 * Loads a truetype font from the file system.
 *
 * @param fileName The name of the .ttf file.
 * @param fontSize The size of the font, in pixels.
 * @param fontColor The color of the font.
 *
 * @return The newly loaded truetype font.
 *
 * @example examples/resources/tuffy.ttf
 */
pntr_font* pntr_load_ttffont(const char* fileName, int fontSize, pntr_color fontColor) {
    if (fileName == NULL || fontSize <= 0) {
        return pntr_set_error("pntr_load_ttffont() requires a valid fileName and font size.");
    }

    #ifndef PNTR_SUPPORT_TTF
        (void)fontColor;
        return pntr_set_error("pntr_load_ttffont requires PNTR_SUPPORT_TTF");
    #else
        unsigned int bytesRead;
        unsigned char* fileData = pntr_load_file(fileName, &bytesRead);
        if (fileData == NULL) {
            return NULL;
        }

        pntr_font* output = pntr_load_ttffont_from_memory(fileData, bytesRead, fontSize, fontColor);
        pntr_unload_file(fileData);

        return output;
    #endif
}

pntr_font* pntr_load_ttffont_from_memory(const unsigned char* fileData, unsigned int dataSize, int fontSize, pntr_color fontColor) {
    if (fileData == NULL || dataSize == 0 || fontSize <= 0) {
        return pntr_set_error("TTF Fonts requires valid file data, data size, and fontSize.");
    }

    #ifndef PNTR_SUPPORT_TTF
        (void)fontColor;
        return pntr_set_error("pntr_load_ttffont requires PNTR_SUPPORT_TTF");
    #else
        // Create the bitmap data with ample space based on the font size
        int width = fontSize * 10;
        int height = fontSize * 10;
        unsigned char* bitmap = (unsigned char*)PNTR_MALLOC((size_t)(width * height));
        if (bitmap == NULL) {
            return pntr_set_error("Failed to allocate memory for bitmap");
        }

        #define PNTR_NUM_GLYPHS 95
        stbtt_bakedchar characterData[PNTR_NUM_GLYPHS];
        int result = stbtt_BakeFontBitmap(fileData, 0, (float)fontSize, bitmap, width, height, 32, PNTR_NUM_GLYPHS, characterData);

        // Check to make sure the font was baked correctly
        if (result == 0) {
            PNTR_FREE(bitmap);
            return pntr_set_error("When baking font, no rows were created");
        }

        // Port the bitmap to a pntr_image as the atlas.
        pntr_image* atlas = pntr_image_from_pixelformat((const void*)bitmap, width, height, PNTR_PIXELFORMAT_GRAYSCALE);
        PNTR_FREE(bitmap);
        if (atlas == NULL) {
            return pntr_set_error("Failed to convert pixel format for font");
        }

        // Clear up the unused atlas space from memory from top left
        pntr_rectangle crop = pntr_image_alpha_border(atlas, 0.0f);
        pntr_image_crop(atlas, 0, 0, crop.x + crop.width, crop.y + crop.height);

        // Apply the font color
        if (fontColor.data != PNTR_WHITE.data) {
            pntr_image_color_tint(atlas, fontColor);
        }

        // Create the font
        pntr_font* font = _pntr_new_font(PNTR_NUM_GLYPHS, atlas);
        if (font == NULL) {
            pntr_unload_image(atlas);
            return NULL;
        }

        // Capture each glyph data
        for (int i = 0; i < PNTR_NUM_GLYPHS; i++) {
            // Calculate the source rectangles.
            font->srcRects[i] = CLITERAL(pntr_rectangle) {
                .x = characterData[i].x0,
                .y = characterData[i].y0,
                .width = characterData[i].x1 - characterData[i].x0,
                .height = characterData[i].y1 - characterData[i].y0
            };

            // Find where the glyphs will be rendered.
            font->glyphRects[i] = CLITERAL(pntr_rectangle) {
                .x = (int)characterData[i].xoff,
                .y = (int)characterData[i].yoff + (int)((float)fontSize / 1.5f), // TODO: Determine correct y glyph value
                .width = (int)characterData[i].xadvance,
                .height = (int)((float)fontSize / 3.0f) // TODO: Determine the correct glyph height
            };

            // Set up the active character.
            font->characters[i] = (char)(32 + i);
        }

        return font;
    #endif
}

/**
 * Inverts the given color.
 *
 * @param color The color to invert.
 *
 * @return The new inverted color.
 *
 * @see pntr_image_color_invert()
 */
inline pntr_color pntr_color_invert(pntr_color color) {
    return CLITERAL(pntr_color) {
        .r = 255 - color.r,
        .g = 255 - color.g,
        .b = 255 - color.b,
        .a = color.a
    };
}

/**
 * Inverts the given image.
 *
 * @param image The image to invert.
 *
 * @see pntr_color_invert()
 */
void pntr_image_color_invert(pntr_image* image) {
    if (image == NULL) {
        return;
    }

    for (int i = 0; i < image->width * image->height; i++) {
        image->data[i] = pntr_color_invert(image->data[i]);
    }
}

/**
 * Changes the brightness of the given image by a factor.
 *
 * @param image The image to change brightness of.
 * @param factor The factor in which to affect the brightness from -1.0f to 1.0f.
 *
 * @see pntr_color_brightness()
 */
void pntr_image_color_brightness(pntr_image* image, float factor) {
    if (image == NULL) {
        return;
    }

    if (factor < -1.0f) {
        factor = -1.0f;
    }
    else if (factor > 1.0f) {
        factor = 1.0f;
    }

    for (int i = 0; i < image->width * image->height; i++) {
        image->data[i] = pntr_color_brightness(image->data[i], factor);
    }
}

/**
 * Loads a file from the file system.
 *
 * This data must be cleared with pntr_unload_file() when finished.
 *
 * You can define your own callback for this by defining PNTR_LOAD_FILE.
 *
 * @param fileName The name of the file to load.
 * @param bytesRead Where to stick the amount of bytes that were read. Use NULL if you don't need the file size.
 *
 * @return A pointer to the file data in memory.
 *
 * @see pntr_unload_file()
 * @see PNTR_LOAD_FILE
 */
unsigned char* pntr_load_file(const char* fileName, unsigned int* bytesRead) {
    if (fileName == NULL) {
        return pntr_set_error("pntr_load_file() requires a valid fileName");
    }

    #ifdef PNTR_LOAD_FILE
        return PNTR_LOAD_FILE(fileName, bytesRead);
    #else
        FILE* file = fopen(fileName, "rb");
        if (file == NULL) {
            if (bytesRead != NULL) {
                *bytesRead = 0;
            }
            return pntr_set_error("Failed to open file");
        }

        fseek(file, 0, SEEK_END);
        size_t size = (size_t)ftell(file);
        fseek(file, 0, SEEK_SET);

        if (size <= 0) {
            fclose(file);
            if (bytesRead != NULL) {
                *bytesRead = 0;
            }
            return pntr_set_error("Failed to read file");
        }

        unsigned char* data = (unsigned char*)PNTR_MALLOC(size * sizeof(unsigned char));
        if (data == NULL) {
            fclose(file);
            if (bytesRead != NULL) {
                *bytesRead = 0;
            }
            return pntr_set_error("Failed to allocate data for file");
        }

        // Read the file
        unsigned int bytes = (unsigned int)fread(data, sizeof(unsigned char), size, file);
        fclose(file);
        if (bytesRead != NULL) {
            *bytesRead = bytes;
        }

        return data;
    #endif
}

/**
 * Saves a file to the file system.
 *
 * You can define your own callback for this by defining PNTR_SAVE_FILE.
 *
 * @param fileName The name of the file to save.
 * @param data A pointer to the memory data in memory.
 * @param bytesToWrite The size of the data in memory.
 *
 * @return True if the file saved properly, false otherwise.
 */
bool pntr_save_file(const char *fileName, const void *data, unsigned int bytesToWrite) {
    if (fileName == NULL || data == NULL) {
        return pntr_set_error("pntr_save_file() requires a valid fileName");
    }

    #ifdef PNTR_SAVE_FILE
        return PNTR_SAVE_FILE(fileName, data, bytesToWrite);
    #else
        FILE *file = fopen(fileName, "wb");
        if (file == NULL) {
            return pntr_set_error("Failed to open file for writing");
        }

        size_t count = fwrite(data, sizeof(unsigned char), bytesToWrite, file);

        if (count <= 0) {
            fclose(file);
            return pntr_set_error("Failed to write data to file");
        }

        if (count != (size_t)bytesToWrite) {
            fclose(file);
            return pntr_set_error("Failed to write the correct amount of data");
        }

        return fclose(file) == 0;
    #endif
}

/**
 * Retrieve the size of the image data in memory, depending on the given pixel format.
 *
 * @param width The width of the image data.
 * @param height The height of the image data.
 * @param pixelFormat The pixel format of the image data.
 *
 * @return The size of the image data, in bytes.
 */
int pntr_get_pixel_data_size(int width, int height, pntr_pixelformat pixelFormat) {
    if (width <= 0 || height <= 0 || pixelFormat < 0) {
        return 0;
    }

    int bitsPerPixel = 0;
    int bitsPerByte = 8;
    switch (pixelFormat) {
        case PNTR_PIXELFORMAT_RGBA8888:
        case PNTR_PIXELFORMAT_ARGB8888:
            bitsPerPixel = (int)sizeof(pntr_color) * bitsPerByte;
            break;
        case PNTR_PIXELFORMAT_GRAYSCALE:
            bitsPerPixel = (int)sizeof(unsigned char) * bitsPerByte;
            break;
        default:
            bitsPerPixel = (int)sizeof(pntr_color) * bitsPerByte;
            pntr_set_error("Unknown pixel format for pntr_get_pixel_data_size");
            break;
    }

    return bitsPerPixel * width * height / bitsPerByte; // Bytes
}

/**
 * Convert the given image to a new image using the provied pixel format.
 *
 * @param image The image to convert.
 * @param dataSize Where to put the resulting size of the image data. Use NULL if you have no need for the resulting data size.
 * @param pixelFormat The desired pixel format of the resulting image.
 *
 * @return A pointer to the resulting image data in memory.
 */
void* pntr_image_to_pixelformat(pntr_image* image, unsigned int* dataSize, pntr_pixelformat pixelFormat) {
    if (image == NULL) {
        return pntr_set_error("requires a valid image");
    }

    int imageSize = pntr_get_pixel_data_size(image->width, image->height, pixelFormat);
    if (imageSize <= 0) {
        return pntr_set_error("Resulted in no image");
    }

    void* data = PNTR_MALLOC(imageSize);
    if (data == NULL) {
        return pntr_set_error("Failed to allocate memory for new image with different pixel format");
    }

    int pixelSize = pntr_get_pixel_data_size(1, 1, pixelFormat);

    for (int i = 0; i < image->width * image->height; i++) {
        void* dstPtr = ((unsigned char*)data) + (i * pixelSize) ;
        pntr_set_pixel_color(dstPtr, image->data[i], pixelFormat);
    }

    // Output the data size
    if (dataSize != NULL) {
        *dataSize = (unsigned int)imageSize;
    }

    return data;
}

/**
 * Gets a PNG representation of the given image in memory.
 *
 * If PNTR_NO_SUPPORT_PNG is defined, this function will not be supported.
 *
 * @param image The image to save to memory.
 * @param dataSize Where to put the resulting size of the image. Use NULL if you do not care about getting the file size.
 *
 * @return The image data in memory.
 *
 * @see PNTR_NO_SUPPORT_PNG
 */
unsigned char* pntr_save_image_to_memory(pntr_image* image, unsigned int* dataSize) {
    if (image == NULL) {
        return pntr_set_error("Requires an actual image");
    }

    #ifdef PNTR_NO_SUPPORT_PNG
        if (dataSize != NULL) {
            *dataSize = 0;
        }
        return pntr_set_error("Saving images requires to not define PNTR_NO_SUPPORT_PNG");
    #else
        cp_image_t cpImage = CLITERAL(cp_image_t) {
            .w = image->width,
            .h = image->height
        };

        cpImage.pix = (cp_pixel_t*)pntr_image_to_pixelformat(image, NULL, PNTR_PIXELFORMAT_RGBA8888);
        if (cpImage.pix == NULL) {
            return pntr_set_error("Failed to port image to RGBA8888");
        }

        cp_saved_png_t png = cp_save_png_to_memory(&cpImage);
        if (png.data == NULL) {
            cp_free_png(&cpImage);
            return pntr_set_error("Failed to save image to memory");
        }

        // Export the datasize
        if (dataSize != NULL) {
            *dataSize = (unsigned int)png.size;
        }

        // Free up the temporary copy
        cp_free_png(&cpImage);

        return (unsigned char*)png.data;
    #endif  // PNTR_NO_SUPPORT_PNG
}

/**
 * Saves an image to the file system.
 *
 * @param image The image to save to the file system.
 * @param fileName The name of the file to save.
 *
 * @return True when the file was saved successfully, false otherwise.
 */
bool pntr_save_image(pntr_image* image, const char* fileName) {
    unsigned int dataSize;
    unsigned char* data = pntr_save_image_to_memory(image, &dataSize);
    if (data == NULL) {
        return pntr_set_error("Failed to save image");
    }

    bool result = pntr_save_file(fileName, data, dataSize);
    PNTR_FREE(data);

    return result;
}

/**
 * Unloads the given file data.
 *
 * @param fileData The data of the file to unload from memory.
 */
inline void pntr_unload_file(unsigned char* fileData) {
    if (fileData != NULL) {
        PNTR_FREE(fileData);
    }
}

/**
 * Calculates a rectangle representing the available alpha border.
 */
pntr_rectangle pntr_image_alpha_border(pntr_image* image, float threshold) {
    if (image == NULL) {
        return CLITERAL(pntr_rectangle) {0, 0, 0, 0};
    }

    unsigned char alphaThreshold = (unsigned char)(threshold * 255.0f);
    int xMin = 9999999;
    int xMax = 0;
    int yMin = 9999999;
    int yMax = 0;

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            if (image->data[y * (image->pitch >> 2) + x].a > alphaThreshold) {
                if (x < xMin) {
                    xMin = x;
                }
                if (x > xMax) {
                    xMax = x;
                }
                if (y < yMin) {
                    yMin = y;
                }
                if (y > yMax) {
                    yMax = y;
                }
            }
        }
    }

    // Check for empty blank image
    if ((xMin != 9999999) && (xMax != 9999999)) {
        return CLITERAL(pntr_rectangle) {
            .x = xMin,
            .y = yMin,
            .width = xMax + 1 - xMin,
            .height = yMax + 1 - yMin
        };
    }

    return CLITERAL(pntr_rectangle) {0, 0, 0, 0};
}

/**
 * Crops an image by the given coordinates.
 *
 * @param image The image to crop.
 * @param x The x coordinate to crop from.
 * @param y The y coordinate to crop from.
 * @param width The width of the desired crop.
 * @param height The height of the desired crop.
 */
void pntr_image_crop(pntr_image* image, int x, int y, int width, int height) {
    if (image == NULL) {
        return;
    }

    pntr_rectangle destination = CLITERAL(pntr_rectangle) { 0, 0, image->width, image->height };
    pntr_rectangle source = CLITERAL(pntr_rectangle) { x, y, width, height };
    source = pntr_rectangle_intersect(&source, &destination);

    if (source.width <= 0 || source.height <= 0 || source.width > image->width || source.height > image->height) {
        return;
    }

    pntr_image* newImage = pntr_image_from_image(image, source.x, source.y, source.width, source.height);
    if (newImage == NULL) {
        return;
    }

    PNTR_FREE(image->data);
    image->data = newImage->data;
    image->width = newImage->width;
    image->height = newImage->height;
    image->pitch = newImage->pitch;
    PNTR_FREE(newImage);
}

/**
 * Crop an image based on the alpha border.
 *
 * @param image The image to crop.
 * @param threshold The alpha threshold to crop by, from 0.0f to 1.0f.
 *
 * @see pntr_image_alpha_border()
 * @see pntr_image_crop()
 */
void pntr_image_alpha_crop(pntr_image* image, float threshold) {
    if (image == NULL) {
        return;
    }

    pntr_rectangle crop = pntr_image_alpha_border(image, threshold);

    if (crop.width > 0 && crop.height > 0) {
        pntr_image_crop(image, crop.x, crop.y, crop.width, crop.height);
    }
}

/**
 * Apply contrast to the given color.
 *
 * @param color The color to apply constrast to.
 * @param contrast The amount of constrast to apply, from -1.0f to 1.0f.
 *
 * @return The new color with the contrast applied.
 */
pntr_color pntr_color_contrast(pntr_color color, float contrast) {
    if (contrast < -1.0f) {
        contrast = -1.0f;
    }
    else if (contrast > 1.0f) {
        contrast = 1.0f;
    }

    contrast = (1.0f + contrast) * contrast;

    float pR = (float)color.r / 255.0f - 0.5f;
    pR *= contrast;
    pR += 0.5f;
    pR *= 255;
    if (pR < 0) {
        pR = 0;
    }
    else if (pR > 255) {
        pR = 255;
    }

    float pG = (float)color.g / 255.0f - 0.5f;
    pG *= contrast;
    pG += 0.5f;
    pG *= 255;
    if (pG < 0) {
        pG = 0;
    }
    else if (pG > 255) {
        pG = 255;
    }

    float pB = (float)color.b / 255.0f - 0.5f;
    pB *= contrast;
    pB += 0.5f;
    pB *= 255;
    if (pB < 0) {
        pB = 0;
    }
    else if (pB > 255) {
        pB = 255;
    }

    return CLITERAL(pntr_color) {
        .r = (unsigned char)pR,
        .g = (unsigned char)pG,
        .b = (unsigned char)pB,
        .a = color.a,
    };
}

/**
 * Apply contrast to the given image.
 *
 * @param image The image you would like to apply contrast to.
 * @param contrast The factor of contrast to apply, from -1.0f to 1.0f.
 *
 * @see pntr_color_contrast()
 */
void pntr_image_color_contrast(pntr_image* image, float contrast) {
    if (image == NULL) {
        return;
    }

    if (contrast < -1.0f) {
        contrast = -1.0f;
    }
    else if (contrast > 1.0f) {
        contrast = 1.0f;
    }

    for (int i = 0; i < image->width * image->height; i++) {
        image->data[i] = pntr_color_contrast(image->data[i], contrast);
    }
}

/**
 * Apply an alpha mask to an image.
 *
 * @param image The image to apply the alpha mask to.
 * @param alphaMask An image that has the alphaMask data.
 * @param posX Where to position the alpha mask on the image.
 * @param posY Where to position the alpha mask on the image.
 */
void pntr_image_alpha_mask(pntr_image* image, pntr_image* alphaMask, int posX, int posY) {
    if (image == NULL || alphaMask == NULL) {
        return;
    }

    pntr_rectangle srcRect = CLITERAL(pntr_rectangle){0, 0, alphaMask->width, alphaMask->height};
    pntr_rectangle dstRect = CLITERAL(pntr_rectangle){posX, posY, alphaMask->width, alphaMask->height};
    pntr_rectangle dstCanvas = CLITERAL(pntr_rectangle){0, 0, image->width, image->height};

    // Update the source coordinates based on the destination
    if (dstRect.x < 0) {
        srcRect.x -= dstRect.x;
        srcRect.width += dstRect.x;
    }
    if (dstRect.y < 0) {
        srcRect.y -= dstRect.y;
        srcRect.height += dstRect.y;
    }

    // Figure out the final desintation
    dstRect = pntr_rectangle_intersect(&dstRect, &dstCanvas);
    dstRect.width = PNTR_MIN(dstRect.width, srcRect.width);
    dstRect.height = PNTR_MIN(dstRect.height, srcRect.height);

    // Final sanity checks
    if (srcRect.width <= 0 || srcRect.height <= 0 || dstRect.width <= 0 || dstRect.height <= 0 || dstRect.x >= image->width || dstRect.y >= image->height) {
        return;
    }

    for (int y = 0; y < dstRect.height; y++) {
        for (int x = 0; x < dstRect.width; x++) {
            pntr_color* pixel = pntr_image_get_color_pointer(image, posX + x, posY + y);
            if (pixel->a > 0) {
                pntr_color* alphaPixel = pntr_image_get_color_pointer(alphaMask, x, y);
                pixel->a = alphaPixel->a;
            }
        }
    }
}

/**
 * Resize the canvas of the given image.
 *
 * @param image The image you would like to resize.
 * @param newWidth The desired width of the new canvas.
 * @param newHeight The desired height of the new canvas.
 * @param offsetX How to offset the image once its canvas is resized. Use 0 if you like to keep the original image on the left.
 * @param offsetY How to offset the image once its canvas is resized. Use 0 if you like to keep the original image at the top.
 * @param fill The color to use for the background of the new image.
 */
void pntr_image_resize_canvas(pntr_image* image, int newWidth, int newHeight, int offsetX, int offsetY, pntr_color fill) {
    if (image == NULL) {
        return;
    }

    pntr_image* newImage = pntr_gen_image_color(newWidth, newHeight, fill);
    if (newImage == NULL) {
        pntr_set_error("pntr_image_resize_canvas: Failed to build new image");
        return;
    }

    pntr_draw_image(newImage, image, offsetX, offsetY);

    pntr_color* oldData = image->data;
    image->data = newImage->data;
    image->width = newImage->width;
    image->height = newImage->height;
    image->pitch = newImage->pitch;

    PNTR_FREE(oldData);
    PNTR_FREE(newImage);
}

/**
 * Creates a new image based off the given image, that's rotated by the given rotation.
 *
 * When the rotation is not 90 degrees, it will rotate using bilinear filtering.
 *
 * @param image The image to rotate.
 * @param rotation The desired rotation from 0.0f to 1.0f. 0.25f == 90 degrees. 0.5f == 180 degress.
 *
 * @return The new rotated image.
 *
 * @see pntr_image_rotate_ex()
 */
pntr_image* pntr_image_rotate(pntr_image* image, float rotation) {
    if (image == NULL) {
        return NULL;
    }

    while (rotation >= 1.0f) {
        rotation -= 1.0f;
    }
    while (rotation < 0.0f) {
        rotation += 1.0f;
    }

    if (rotation == 0.0f) {
        return pntr_image_copy(image);
    }

    if (rotation == 0.25f) {
        pntr_image* result = pntr_new_image(image->height, image->width);
        if (result == NULL) {
            pntr_set_error("Failed to create memory result for rotation");
            return NULL;
        }

        for (int y = 0; y < image->height; y++) {
            for (int x = 0; x < image->width; x++) {
                result->data[x * image->height + y] = image->data[y*image->width + image->width - x - 1];
            }
        }

        return result;
    }
    else if (rotation == 0.5f) {
        pntr_image* result = pntr_image_copy(image);
        if (result == NULL) {
            return NULL;
        }
        pntr_image_flip_vertical(result);
        pntr_image_flip_horizontal(result);

        return result;
    }
    else if (rotation == 0.75f) {
        pntr_image* result = pntr_new_image(image->height, image->width);
        if (result == NULL) {
            pntr_set_error("Failed to create memory result for rotation");
            return NULL;
        }

        for (int y = 0; y < image->height; y++) {
            for (int x = 0; x < image->width; x++) {
                result->data[x * image->height + (image->height - y - 1)] = pntr_image_get_color(image, x, y);
            }
        }

        return result;
    }

    return pntr_image_rotate_ex(image, rotation, PNTR_FILTER_BILINEAR);
}

/**
 * Bilinear interpolate the given colors, in the sequence below, based on their given 0-1 coordinates.
 *
 * The colors appear in the following order:
 *
 *     00 10
 *     01 11
 *
 * @param color00 The top left color.
 * @param color01 The bottom left color.
 * @param color10 The top right color.
 * @param color11 The bottom right color.
 * @param coordinateX A 0.0f to 1.0f fraction between color00 and color 10.
 * @param coordinateY A 0.0f to 1.0f fraction between color00 and color 01.
 */
inline pntr_color pntr_color_bilinear_interpolate(pntr_color color00, pntr_color color01, pntr_color color10, pntr_color color11, float coordinateX, float coordinateY) {
    return CLITERAL(pntr_color) {
        .r = (uint8_t)(color00.r * (1 - coordinateX) * (1 - coordinateY) + color01.r * (1 - coordinateX) * coordinateY + color10.r * coordinateX * (1 - coordinateY) + color11.r * coordinateX * coordinateY),
        .g = (uint8_t)(color00.g * (1 - coordinateX) * (1 - coordinateY) + color01.g * (1 - coordinateX) * coordinateY + color10.g * coordinateX * (1 - coordinateY) + color11.g * coordinateX * coordinateY),
        .b = (uint8_t)(color00.b * (1 - coordinateX) * (1 - coordinateY) + color01.b * (1 - coordinateX) * coordinateY + color10.b * coordinateX * (1 - coordinateY) + color11.b * coordinateX * coordinateY),
        .a = (uint8_t)(color00.a * (1 - coordinateX) * (1 - coordinateY) + color01.a * (1 - coordinateX) * coordinateY + color10.a * coordinateX * (1 - coordinateY) + color11.a * coordinateX * coordinateY)
    };
}

/**
 * Rotates an image, allowing to change the desired filter.
 *
 * @param image The image you would like to rotate.
 * @param rotation The desired rotation from 0.0f to 1.0f. 0.25f == 90 degrees. 0.5f == 180 degress.
 * @param filter The filter you would like to apply when required. Supported filters are PNTR_FILTER_NEARESTNEIGHBOR or PNTR_FILTER_BILINEAR.
 *
 * @return A new image that has been rotated around the center of the original image.
 */
pntr_image* pntr_image_rotate_ex(pntr_image* image, float rotation, pntr_filter filter) {
    if (image == NULL) {
        return pntr_set_error("image_rotate requires a valid image");
    }

    #ifdef PNTR_NO_MATH
        (void)rotation;
        (void)filter;
        return pntr_set_error("pntr_image_rotate_ex requires the math library, without PNTR_NO_MATH");
    #else
        float radians = rotation * 6.283185307f; // 360.0f * M_PI / 180.0f;
        float cosTheta = PNTR_COSF(radians);
        float sinTheta = PNTR_SINF(radians);

        int newWidth = (int)PNTR_CEILF(PNTR_FABSF((float)image->width * cosTheta) + PNTR_FABSF((float)image->height * sinTheta));
        int newHeight = (int)PNTR_CEILF(PNTR_FABSF((float)image->width * sinTheta) + PNTR_FABSF((float)image->height * cosTheta));

        pntr_image* rotatedImage = pntr_gen_image_color(newWidth, newHeight, PNTR_BLANK);
        if (rotatedImage == NULL) {
            return NULL;
        }

        float centerX = (float)image->width / 2.0f;
        float centerY = (float)image->height / 2.0f;

        for (int y = 0; y < newHeight; y++) {
            for (int x = 0; x < newWidth; x++) {
                float srcX = (float)(x - newWidth / 2) * cosTheta - (float)(y - newHeight / 2) * sinTheta + centerX;
                float srcY = (float)(x - newWidth / 2) * sinTheta + (float)(y - newHeight / 2) * cosTheta + centerY;

                if (srcX >= 0 && srcX < image->width - 1 && srcY >= 0 && srcY < image->height - 1) {
                    if (filter == PNTR_FILTER_NEARESTNEIGHBOR) {
                        rotatedImage->data[y * (rotatedImage->pitch >> 2) + x] = image->data[(int)srcY * (image->pitch >> 2) + (int)srcX];
                    }
                    else {
                        rotatedImage->data[y * (rotatedImage->pitch >> 2) + x] = pntr_color_bilinear_interpolate(
                            image->data[(int)srcY * (image->pitch >> 2) + (int)srcX],
                            image->data[((int)srcY + 1) * (image->pitch >> 2) + (int)srcX],
                            image->data[(int)srcY * (image->pitch >> 2) + (int)srcX + 1],
                            image->data[((int)srcY + 1) * (image->pitch >> 2) + (int)srcX + 1],
                            srcX - PNTR_FLOORF(srcX),
                            srcY - PNTR_FLOORF(srcY)
                        );
                    }
                }
            }
        }

        return rotatedImage;
    #endif  // PNTR_NO_MATH
}

/**
 * Generate image: vertical gradient.
 *
 * @param width The width of the new image.
 * @param height The height of the new image.
 * @param top The color at the top of the image.
 * @param bottom The color at the bottom of the image.
 *
 * @return A pointer to the new image.
 */
pntr_image* pntr_gen_image_gradient_vertical(int width, int height, pntr_color top, pntr_color bottom) {
    pntr_image* image = pntr_new_image(width, height);
    if (image == NULL) {
        return NULL;
    }

    for (int y = 0; y < height; y++) {
        float factor = (float)y / (float)height;
        for (int x = 0; x < width; x++) {
            image->data[y * width + x].r = (unsigned char)((float)bottom.r * factor + (float)top.r * (1.0f - factor));
            image->data[y * width + x].g = (unsigned char)((float)bottom.g * factor + (float)top.g * (1.0f - factor));
            image->data[y * width + x].b = (unsigned char)((float)bottom.b * factor + (float)top.b * (1.0f - factor));
            image->data[y * width + x].a = (unsigned char)((float)bottom.a * factor + (float)top.a * (1.0f - factor));
        }
    }

    return image;
}

/**
 * Generate image: horizontal gradient.
 *
 * @param width The width of the new image.
 * @param height The height of the new image.
 * @param left The color at the left of the image.
 * @param right The color at the right of the image.
 *
 * @return A pointer to the new image.
 */
pntr_image* pntr_gen_image_gradient_horizontal(int width, int height, pntr_color left, pntr_color right) {
    pntr_image* image = pntr_new_image(width, height);
    if (image == NULL) {
        return NULL;
    }

    for (int x = 0; x < width; x++) {
        float factor = (float)x / (float)width;
        for (int y = 0; y < height; y++) {
            image->data[y * width + x].r = (unsigned char)((float)right.r * factor + (float)left.r * (1.0f - factor));
            image->data[y * width + x].g = (unsigned char)((float)right.g * factor + (float)left.g * (1.0f - factor));
            image->data[y * width + x].b = (unsigned char)((float)right.b * factor + (float)left.b * (1.0f - factor));
            image->data[y * width + x].a = (unsigned char)((float)right.a * factor + (float)left.a * (1.0f - factor));
        }
    }

    return image;
}

pntr_image* pntr_gen_image_gradient(int width, int height, pntr_color topLeft, pntr_color topRight, pntr_color bottomLeft, pntr_color bottomRight) {
    pntr_image* image = pntr_new_image(width, height);
    if (image == NULL) {
        return NULL;
    }

    for (int x = 0; x < width; x++) {
        float factorX = (float)x / (float)width;
        for (int y = 0; y < height; y++) {
            float factorY = (float)y / (float)height;
            image->data[y * width + x] = pntr_color_bilinear_interpolate(topLeft, bottomLeft, topRight, bottomRight, factorX, factorY);
        }
    }

    return image;
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif  // PNTR_IMPLEMENTATION_ONCE
#endif  // PNTR_IMPLEMENTATION
