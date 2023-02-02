/**
 * pntr.
 *
 * Configuration:
 *
 * PNTR_SUPPORT_DEFAULT_FONT: Enables the default font
 * PNTR_PIXELFORMAT_RGBA: Use the RGBA format
 * PNTR_PIXELFORMAT_ARGB: Use the ARGB pixel format
 * PNTR_NO_STB_IMAGE_IMPLEMENTATION: Skips implementing STB_IMAGE
 * PNTR_SUPPORT_BDFFONT
 * PNTR_LOAD_FILE Macro to load a file. Matching unsigned char *pntr_load_file(const char *fileName, unsigned int *bytesRead)
 */
#ifndef PNTR_H__
#define PNTR_H__

#include <stdint.h> // uint32_t

#ifndef PNTR_API
#define PNTR_API
#endif

// Pixel Format. Default to PNTR_PIXELFORMAT_RGBA
#if !defined(PNTR_PIXELFORMAT_RGBA) && !defined(PNTR_PIXELFORMAT_ARGB)
#define PNTR_PIXELFORMAT_RGBA
#endif

typedef union {
    uint32_t data;
    struct {
        #if defined(PNTR_PIXELFORMAT_RGBA)
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
        #elif defined(PNTR_PIXELFORMAT_ARGB)
        unsigned char b;
        unsigned char g;
        unsigned char r;
        unsigned char a;
        #endif
    };
} pntr_color;

typedef struct pntr_image {
    pntr_color* data;
    int width;
    int height;
    int pitch;
} pntr_image;

typedef struct pntr_vector {
    int x;
    int y;
} pntr_vector;

typedef struct pntr_rectangle {
    int x;
    int y;
    int width;
    int height;
} pntr_rectangle;

typedef enum {
    PNTR_FONTTYPE_BM = 0,
    PNTR_FONTTYPE_TTY,
    PNTR_FONTTYPE_BDF
} pntr_fontType;

#ifndef PNTR_MAX_FONTS
#define PNTR_MAX_FONTS 256
#endif

typedef struct pntr_font {
    pntr_image* atlas;
    pntr_rectangle rectangles[PNTR_MAX_FONTS];
    char characters[PNTR_MAX_FONTS];
    int charactersFound;
    pntr_color color;
    void* userdata;
    pntr_fontType type;
} pntr_font;

typedef enum {
    PNTR_PIXELFORMAT_RGBA8888 = 0,
    PNTR_PIXELFORMAT_ARGB8888
} pntr_pixelformat;

typedef enum {
    PNTR_FILTER_NEARESTNEIGHBOR = 0
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
PNTR_API pntr_image* pntr_load_image(const char* fileName);
PNTR_API pntr_image* pntr_load_image_from_memory(const unsigned char* fileData, unsigned int dataSize);
PNTR_API pntr_image* pntr_image_from_pixelformat(void* data, int width, int height, pntr_pixelformat pixelFormat);
PNTR_API const char* pntr_get_error();
PNTR_API void* pntr_set_error(const char* error);
PNTR_API pntr_image* pntr_image_resize(pntr_image* image, int newWidth, int newHeight, pntr_filter filter);
PNTR_API void pntr_image_color_replace(pntr_image* image, pntr_color color, pntr_color replace);
PNTR_API pntr_color pntr_color_tint(pntr_color color, pntr_color tint);
PNTR_API void pntr_image_color_tint(pntr_image* image, pntr_color color);
PNTR_API pntr_color pntr_color_fade(pntr_color, float alpha);
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
PNTR_API unsigned char *pntr_load_file(const char *fileName, unsigned int *bytesRead);
PNTR_API pntr_font* pntr_load_bdffont(const char* fileName, pntr_color defaultColor);
PNTR_API pntr_font* pntr_load_bdffont_from_memory(const unsigned char* fileData, unsigned int dataSize, pntr_color defaultColor);


#ifdef __cplusplus
}
#endif

#if defined(__cplusplus)
    #define CLITERAL(type)      type
#else
    #define CLITERAL(type)      (type)
#endif

#ifndef PNTR_LIGHTGRAY
#define PNTR_LIGHTGRAY  CLITERAL(pntr_color){ .r = 200, .g = 200, .b = 200, .a = 255 }
#endif
#ifndef PNTR_GRAY
#define PNTR_GRAY       CLITERAL(pntr_color){ .r = 130, .g = 130, .b = 130, .a = 255 }
#endif
#ifndef PNTR_DARKGRAY
#define PNTR_DARKGRAY   CLITERAL(pntr_color){ .r = 80,  .g = 80,  .b = 80,  .a = 255 }
#endif
#ifndef PNTR_YELLOW
#define PNTR_YELLOW     CLITERAL(pntr_color){ .r = 253, .g = 249, .b = 0,   .a =255  }
#endif
#ifndef PNTR_GOLD
#define PNTR_GOLD       CLITERAL(pntr_color){ .r = 255, .g = 203, .b = 0,   .a =255  }
#endif
#ifndef PNTR_ORANGE
#define PNTR_ORANGE     CLITERAL(pntr_color){ .r = 255, .g = 161, .b = 0,   .a =255  }
#endif
#ifndef PNTR_PINK
#define PNTR_PINK       CLITERAL(pntr_color){ .r = 255, .g = 109, .b = 194, .a = 255 }
#endif
#ifndef PNTR_RED
#define PNTR_RED        CLITERAL(pntr_color){ .r = 230, .g = 41,  .b = 55,  .a = 255 }
#endif
#ifndef PNTR_MAROON
#define PNTR_MAROON     CLITERAL(pntr_color){ .r = 190, .g = 33,  .b = 55,  .a = 255 }
#endif
#ifndef PNTR_GREEN
#define PNTR_GREEN      CLITERAL(pntr_color){ .r = 0,   .g = 228, .b = 48,  .a = 255 }
#endif
#ifndef PNTR_LIME
#define PNTR_LIME       CLITERAL(pntr_color){ .r = 0,   .g = 158, .b = 47,  .a = 255 }
#endif
#ifndef PNTR_DARKGREEN
#define PNTR_DARKGREEN  CLITERAL(pntr_color){ .r = 0,   .g = 117, .b = 44,  .a = 255 }
#endif
#ifndef PNTR_SKYBLUE
#define PNTR_SKYBLUE    CLITERAL(pntr_color){ .r = 102, .g = 191, .b = 255, .a = 255 }
#endif
#ifndef PNTR_BLUE
#define PNTR_BLUE       CLITERAL(pntr_color){ .r = 0,   .g = 121, .b = 241, .a = 255 }
#endif
#ifndef PNTR_DARKBLUE
#define PNTR_DARKBLUE   CLITERAL(pntr_color){ .r = 0,   .g = 82,  .b = 172, .a = 255 }
#endif
#ifndef PNTR_PURPLE
#define PNTR_PURPLE     CLITERAL(pntr_color){ .r = 200, .g = 122, .b = 255, .a = 255 }
#endif
#ifndef PNTR_VIOLET
#define PNTR_VIOLET     CLITERAL(pntr_color){ .r = 135, .g = 60,  .b = 190, .a = 255 }
#endif
#ifndef PNTR_DARKPURPLE
#define PNTR_DARKPURPLE CLITERAL(pntr_color){ .r = 112, .g = 31,  .b = 126, .a = 255 }
#endif
#ifndef PNTR_BEIGE
#define PNTR_BEIGE      CLITERAL(pntr_color){ .r = 211, .g = 176, .b = 131, .a = 255 }
#endif
#ifndef PNTR_BROWN
#define PNTR_BROWN      CLITERAL(pntr_color){ .r = 127, .g = 106, .b = 79,  .a = 255 }
#endif
#ifndef PNTR_DARKBROWN
#define PNTR_DARKBROWN  CLITERAL(pntr_color){ .r = 76,  .g = 63,  .b = 47,  .a = 255 }
#endif
#ifndef PNTR_WHITE
#define PNTR_WHITE      CLITERAL(pntr_color){ .r = 255, .g = 255, .b = 255, .a = 255 }
#endif
#ifndef PNTR_BLACK
#define PNTR_BLACK      CLITERAL(pntr_color){ .r = 0,   .g = 0,   .b = 0,   .a = 255 }
#endif
#ifndef PNTR_BLANK
#define PNTR_BLANK      CLITERAL(pntr_color){ .r = 0,   .g = 0,   .b = 0,   .a = 0   }
#endif
#ifndef PNTR_MAGENTA
#define PNTR_MAGENTA    CLITERAL(pntr_color){ .r = 255, .g = 0,   .b = 255, .a = 255 }
#endif
#ifndef PNTR_RAYWHITE
#define PNTR_RAYWHITE   CLITERAL(pntr_color){ .r = 245, .g = 245, .b = 245, .a = 255 }
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
#define PNTR_MALLOC(size) malloc((size_t)(size))
#endif  // PNTR_MALLOC

#ifndef PNTR_FREE
#include <stdlib.h>
#define PNTR_FREE(obj) free((void*)(obj))
#endif  // PNTR_FREE

#ifndef PNTR_MEMCPY
#include <string.h>
#define PNTR_MEMCPY(dest, src, n) memcpy((void*)(dest), (const void*)(src), (size_t)(n))
#endif  // PNTR_MEMCPY

#ifndef PNTR_MAX
#ifdef MAX
#define PNTR_MAX MAX
#else
#define PNTR_MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#endif
#ifndef PNTR_MIN
#ifdef MIN
#define PNTR_MIN MIN
#else
#define PNTR_MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#endif

#ifndef PNTR_PIXELFORMAT
#if defined(PNTR_PIXELFORMAT_RGBA)
#define PNTR_PIXELFORMAT PNTR_PIXELFORMAT_RGBA8888
#elif defined(PNTR_PIXELFORMAT_ARGB)
#define PNTR_PIXELFORMAT PNTR_PIXELFORMAT_ARGB8888
#endif
#endif  // PNTR_PIXELFORMAT

#ifndef PNTR_LOAD_FILE
#include <stdio.h>
#endif  // PNTR_LOAD_FILE

/**
 * Draws a pixel on the canvas, ignoring sanity checks.
 */
#define pntr_draw_pixel_unsafe(dst, x, y, color) dst->data[(y) * (dst->pitch >> 2) + x] = color

// stb_image
// TODO: Allow selective inclusion with stb_image. And use STBI_NO_STDIO
#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STBI_ONLY_PNG
#define STBI_NO_SIMD
#define STBI_NO_HDR
#define STBI_NO_LINEAR
#define STBI_NO_GIF
#define STBI_NO_THREAD_LOCALS

#ifndef PNTR_NO_STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif  // PNTR_NO_STB_IMAGE_IMPLEMENTATION

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#include "external/stb_image.h"
#pragma GCC diagnostic pop
#endif  // STBI_INCLUDE_STB_IMAGE_H

/**
 * The last error that was reported from pntr.
 *
 * @see pntr_get_error
 * @see pntr_set_error
 */
const char* pntr_error;

const char* pntr_get_error() {
    return pntr_error;
}

void* pntr_set_error(const char* error) {
    pntr_error = error;
    return NULL;
}

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

pntr_image* pntr_gen_image_color(int width, int height, pntr_color color) {
    pntr_image* image = pntr_new_image(width, height);
    pntr_clear_background(image, color);
    return image;
}

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

pntr_rectangle pntr_rectangle_intersect(pntr_rectangle *a, pntr_rectangle *b) {
    int left   = PNTR_MAX(a->x, b->x);
    int right  = PNTR_MIN(a->x + a->width, b->x + b->width);
    int top    = PNTR_MAX(a->y, b->y);
    int bottom = PNTR_MIN(a->y + a->height, b->y + b->height);
    int width  = right - left;
    int height = bottom - top;
    return CLITERAL(pntr_rectangle){ left, top, PNTR_MAX(width, 0), PNTR_MAX(height, 0) };
}

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

void pntr_draw_horizontal_line_unsafe(pntr_image* dst, int posX, int posY, int width, pntr_color color) {
    pntr_color *row  = dst->data + posY * (dst->pitch >> 2);
    for (int x = posX; x < posX + width; ++x) {
        row[x] = color;
    }
}

void pntr_clear_background(pntr_image* image, pntr_color color) {
    if (image == NULL) {
        return;
    }

    // Draw the first line
    pntr_draw_horizontal_line_unsafe(image, 0, 0, image->width, color);

    // Copy the line for the rest of the screen
    for (int y = 1; y < image->height; y++) {
        PNTR_MEMCPY(pntr_image_get_color_pointer(image, 0, y), image->data, image->pitch);
    }
}

inline pntr_color pntr_new_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    return CLITERAL(pntr_color){
        .r = r,
        .g = g,
        .b = b,
        .a = a
    };
}

/**
 * Get a pntr_color from a RGBA hexadecimal value
 *
 * Example: 0x052c46ff
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

void pntr_draw_pixel(pntr_image* dst, int x, int y, pntr_color color) {
    // TODO: Allow drawing Alpha-Transparency pixels
    if ((color.a == 0) || (dst == NULL) || (x < 0) || (x >= dst->width) || (y < 0) || (y >= dst->height)) {
        return;
    }

    pntr_draw_pixel_unsafe(dst, x, y, color);
}

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

inline void pntr_draw_rectangle(pntr_image* dst, int posX, int posY, int width, int height, pntr_color color) {
    pntr_draw_rectangle_rec(dst, CLITERAL(pntr_rectangle){posX, posY, width, height}, color);
}

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

pntr_color pntr_image_get_color(pntr_image* image, int x, int y) {
    if (image == NULL) {
        return PNTR_BLANK;
    }
    if (x < 0 || y < 0 || x >= image->width || y >= image->height) {
        return PNTR_BLANK;
    }
    return image->data[y * (image->pitch >> 2) + x];
}

inline pntr_color* pntr_image_get_color_pointer(pntr_image* image, int x, int y) {
    return image->data + y * (image->pitch >> 2) + x;
}

pntr_image* pntr_load_image_from_memory(const unsigned char *fileData, unsigned int dataSize) {
    if (fileData == NULL || dataSize <= 0) {
        return pntr_set_error("pntr_load_image_from_memory() requires valid file data");
    }

    int width, height, channels_in_file;
    void* output = (void*)stbi_load_from_memory(fileData, (int)dataSize, &width, &height, &channels_in_file, 4);
    if (output == NULL) {
        return pntr_set_error("pntr_load_image_from_memory() failed to load image from memory");
    }

    return pntr_image_from_pixelformat(output, width, height, PNTR_PIXELFORMAT_RGBA8888);
}

pntr_image* pntr_load_image(const char* fileName) {
    if (fileName == NULL) {
        return pntr_set_error("pntr_load_image() requires a valid fileName");
    }

    int width, height, channels_in_file;
    // TODO: Implement an abstracted file system and use pntr_load_image_from_memory() instead.
    void* output = (void*)stbi_load(fileName, &width, &height, &channels_in_file, 4);
    if (output == NULL) {
        return pntr_set_error("pntr_load_image() failed to load image with stbi_load");
    }

    return pntr_image_from_pixelformat(output, width, height, PNTR_PIXELFORMAT_RGBA8888);
}

#define COMPOSE_FAST(S, D, A) (((S * A) + (D * (256U - A))) >> 8U)

inline void pntr_draw_image(pntr_image* dst, pntr_image* src, int posX, int posY) {
    pntr_draw_image_rec(dst, src, CLITERAL(pntr_rectangle){0, 0, src->width, src->height}, posX, posY);
}

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

#ifdef PNTR_HAVE_COMPOSITION
    pntr_color s, d;
    while (rows_left-- > 0) {
        for (int x = 0; x < cols; ++x) {
            s = srcPixel[x];
            d = dstPixel[x];
            if (s.a == 0) {
                continue;
            }
            dstPixel[x].data = ((s.a + d.a * (255 - s.a)) << 24) | (COMPOSE_FAST(s.r, d.r, s.a) << 16) | (COMPOSE_FAST(s.g, d.g, s.a) << 8) | (COMPOSE_FAST(s.b, d.b, s.a));
        }

        dstPixel += dst_skip;
        srcPixel += src_skip;
    }
#else
    while (rows_left-- > 0) {
        for (int x = 0; x < cols; ++x) {
            // Alpha transparency threshold
            if (srcPixel[x].a <= 128) {
                continue;
            }
            dstPixel[x] = srcPixel[x];
        }

        dstPixel += dst_skip;
        srcPixel += src_skip;
    }
#endif
}

/**
 * Converts the given data to a pntr_image.
 *
 * This will free the original data, when needed.
 */
pntr_image* pntr_image_from_pixelformat(void* data, int width, int height, pntr_pixelformat pixelFormat) {
    if (data == NULL || width <= 0 || height <= 0 || pixelFormat < 0) {
        return pntr_set_error("pntr_image_from_data() requires valid data");
    }

    pntr_image* output = (pntr_image*)PNTR_MALLOC(sizeof(pntr_image));
    if (output == NULL) {
        return pntr_set_error("pntr_image_from_pixelformat() failed to allocate memory");
    }

    output->width = width;
    output->height = height;
    output->pitch = width * (int)sizeof(pntr_color);
    output->data = (pntr_color*)data;

    // Check if we have to convert the pixel format.
    if (pixelFormat != PNTR_PIXELFORMAT) {
        switch (pixelFormat) {
            case PNTR_PIXELFORMAT_ARGB8888:
            case PNTR_PIXELFORMAT_RGBA8888:
            default:
            for (int i = 0; i < width * height; i++) {
                output->data[i] = pntr_get_pixel_color((void*)(output->data + i), pixelFormat);
            }
            break;
        }
    }

    return output;
}

pntr_image* pntr_image_resize(pntr_image* image, int newWidth, int newHeight, pntr_filter filter) {
    if (image == NULL || newWidth <= 0 || newHeight <= 0 || filter < 0) {
        return pntr_set_error("pntr_image_resize() requires a valid image and width/height");
    }

    pntr_image* output = pntr_new_image(newWidth, newHeight);

    switch (filter) {
        // TODO: pntr_image_resize: Add more filters to resize using. stb_image_resize?
        case PNTR_FILTER_NEARESTNEIGHBOR:
        default: {
            int xRatio = (image->width << 16) / newWidth + 1;
            int yRatio = (image->height << 16) / newHeight + 1;

            for (int y = 0; y < newHeight; y++) {
                for (int x = 0; x < newWidth; x++) {
                    int x2 = (x * xRatio) >> 16;
                    int y2 = (y * yRatio) >> 16;

                    output->data[(y * newWidth) + x] = image->data[(y2 * image->width) + x2];
                }
            }
        }
        break;
    }

    return output;
}

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

pntr_color pntr_color_tint(pntr_color color, pntr_color tint) {
    float cR = (float)tint.r / 255.0f;
    float cG = (float)tint.g / 255.0f;
    float cB = (float)tint.b / 255.0f;
    float cA = (float)tint.a / 255.0f;

    return CLITERAL(pntr_color) {
        .r = (unsigned char)(((float)color.r / 255 * cR) * 255.0f),
        .g = (unsigned char)(((float)color.g / 255 * cG) * 255.0f),
        .b = (unsigned char)(((float)color.b / 255 * cB) * 255.0f),
        .a = (unsigned char)(((float)color.a / 255 * cA) * 255.0f)
    };
}

pntr_color pntr_color_fade(pntr_color color, float alpha) {
    if (alpha < 0.0f) {
        alpha = 0;
    }
    if (alpha > 1.0f) {
        alpha = 1.0f;
    }

    return CLITERAL(pntr_color){
        .r = color.r,
        .g = color.g,
        .b = color.b,
        .a = (unsigned char)(255.0f * alpha)
    };
}

void pntr_set_pixel_color(void* dstPtr, pntr_color color, pntr_pixelformat dstPixelFormat) {
    switch (dstPixelFormat) {
        case PNTR_PIXELFORMAT_RGBA8888:
            ((uint32_t*)(dstPtr))[0] = ((int)color.r << 24) | ((int)color.g << 16) | ((int)color.b << 8) | (int)color.a;
            break;
        case PNTR_PIXELFORMAT_ARGB8888:
            ((uint32_t*)(dstPtr))[0] = ((int)color.a << 24) | ((int)color.r << 16) | ((int)color.g << 8) | (int)color.b;
            break;
    }
}

pntr_color pntr_get_pixel_color(void* srcPtr, pntr_pixelformat srcPixelFormat) {
    switch (srcPixelFormat) {
        case PNTR_PIXELFORMAT_RGBA8888:
            return CLITERAL(pntr_color) {
                .r = ((unsigned char *)srcPtr)[0],
                .g = ((unsigned char *)srcPtr)[1],
                .b = ((unsigned char *)srcPtr)[2],
                .a = ((unsigned char *)srcPtr)[3]
            };
        break;
        case PNTR_PIXELFORMAT_ARGB8888:
            return CLITERAL(pntr_color) {
                .a = ((unsigned char *)srcPtr)[0],
                .r = ((unsigned char *)srcPtr)[1],
                .g = ((unsigned char *)srcPtr)[2],
                .b = ((unsigned char *)srcPtr)[3]
            };
        break;
    }

    return PNTR_BLANK;
}

void pntr_image_color_tint(pntr_image* image, pntr_color tint) {
    if (image == NULL) {
        return;
    }

    for (int i = 0; i < image->width * image->height; i++) {
        image->data[i] = pntr_color_tint(image->data[i], tint);
    }
}

pntr_font* pntr_load_bmfont(const char* fileName, const char* characters) {
    pntr_image* image = pntr_load_image(fileName);
    if (image == NULL) {
        return NULL;
    }

    return pntr_load_bmfont_from_image(image, characters);
}

pntr_font* pntr_load_bmfont_from_memory(const unsigned char* fileData, unsigned int dataSize, const char* characters) {
    pntr_image* image = pntr_load_image_from_memory(fileData, dataSize);
    if (image == NULL) {
        return NULL;
    }

    return pntr_load_bmfont_from_image(image, characters);
}

pntr_font* pntr_load_bmfont_from_image(pntr_image* image, const char* characters) {
    if (image == NULL || characters == NULL) {
        return pntr_set_error("pntr_load_bmfont_from_image() requires a valid image and characters");
    }

    pntr_font* font = PNTR_MALLOC(sizeof(pntr_font));
    if (font == NULL) {
        return pntr_set_error("pntr_load_bmfont_from_image() failed to allocate pntr_font memory");
    }

    pntr_color seperator = pntr_image_get_color(image, 0, 0);
    pntr_rectangle currentRectangle = CLITERAL(pntr_rectangle){1,0,0, image->height};
    int currentCharacter = 0;

    for (int i = 1; i < image->width && currentCharacter < PNTR_MAX_FONTS; i++) {
        if (pntr_image_get_color(image, i, 0).data == seperator.data) {
            font->characters[currentCharacter] = characters[currentCharacter];
            font->rectangles[currentCharacter] = currentRectangle;
            currentRectangle.width = 0;
            currentRectangle.x = i + 1;
            currentCharacter++;
        }
        else {
            currentRectangle.width++;
        }
    }

    font->type = PNTR_FONTTYPE_BM;
    font->atlas = image;
    font->charactersFound = currentCharacter;

    return font;
}

pntr_font* pntr_load_ttyfont(const char* fileName, int glyphWidth, int glyphHeight, const char* characters) {
    pntr_image* image = pntr_load_image(fileName);
    if (image == NULL) {
        return NULL;
    }

    return pntr_load_ttyfont_from_image(image, glyphWidth, glyphHeight, characters);
}

pntr_font* pntr_load_ttyfont_from_memory(const unsigned char* fileData, unsigned int dataSize, int glyphWidth, int glyphHeight, const char* characters) {
    pntr_image* image = pntr_load_image_from_memory(fileData, dataSize);
    if (image == NULL) {
        return NULL;
    }

    return pntr_load_ttyfont_from_image(image, glyphWidth, glyphHeight, characters);
}

pntr_font* pntr_load_ttyfont_from_image(pntr_image* image, int glyphWidth, int glyphHeight, const char* characters) {
    if (image == NULL || characters == NULL || glyphWidth <= 0 || glyphHeight <= 0) {
        return pntr_set_error("pntr_load_ttyfont_from_image() requires a valid image and characters");
    }

    pntr_font* font = PNTR_MALLOC(sizeof(pntr_font));
    if (font == NULL) {
        return pntr_set_error("pntr_load_ttyfont_from_image() failed to allocate pntr_font memory");
    }

    int currentCharIndex = 0;
    const char * currentChar = characters;
    while (currentChar != NULL && *currentChar != '\0' && currentCharIndex < PNTR_MAX_FONTS) {
        pntr_rectangle rect;
        rect.x = (currentCharIndex % (image->width / glyphWidth)) * glyphWidth;
        rect.y = (currentCharIndex / (image->width / glyphWidth)) * glyphHeight;

        rect.width = glyphWidth;
        rect.height = glyphHeight;

        font->rectangles[currentCharIndex] = rect;
        font->characters[currentCharIndex] = characters[currentCharIndex];

        currentCharIndex++;
    }

    font->type = PNTR_FONTTYPE_TTY;
    font->atlas = image;
    font->charactersFound = currentCharIndex;

    return font;
}

#ifdef PNTR_SUPPORT_BDF

void pntr_bdf_put_pixel(const pntr_image* canvas, const int x, const int y, pntr_color color) {
    printf("FSDAFASDSDFAAFSD\n");
    pntr_draw_pixel((pntr_image*)canvas, (int)x, (int)y, color);
}

#define AL_BDF_IMPLEMENTATION
#define AL_BDF_CANVAS_TYPE pntr_image*
#define AL_BDF_COLOR_TYPE pntr_color
#define AL_BDF_PUT_PIXEL pntr_bdf_put_pixel
#define AL_BDF_MALLOC PNTR_MALLOC
#define AL_BDF_FREE PNTR_FREE
#ifndef PNTR_REALLOC
#include <stdlib.h>
#define PNTR_REALLOC realloc
#endif
#define AL_BDF_REALLOC PNTR_REALLOC

#include "external/al_bdf.h"

typedef struct pntr_bdf_reader {
    unsigned char* fileData;
    unsigned int dataSize;
    int current;
} pntr_bdf_reader;

int pntr_bdf_read(void* const userdata, void* const buffer, size_t const count) {
    pntr_bdf_reader* bdfReader = (pntr_bdf_reader*)userdata;

    unsigned char* buf = (unsigned char*)buffer;
    buf = bdfReader->fileData + bdfReader->current;
    int readAmount = count;
    int dataSize = bdfReader->dataSize;
    if (bdfReader->current + readAmount > dataSize) {
        readAmount = dataSize - bdfReader->current;
    }
    return readAmount;
}

void pntr_bdf_render(al_bdf_Font* const font, char const* text, int posX, int posY, AL_BDF_CANVAS_TYPE const canvas, AL_BDF_COLOR_TYPE const color) {

    int x = 0, y = font->baseline;

    for (;;) {
        int const code = AL_BDF_UTF8_DECODE(&text);

        if (code == 0) {
            return;
        }

        al_bdf_Char const* const chr = al_bdf_find_char(font, code);

        if (chr != NULL) {
            int const dx = x + chr->bbxoff0x;
            int const dy = y - (chr->bbyoff0y + chr->bbh);

            al_bdf_draw_char(font, chr, canvas, posX + dx, posY + dy, color);

            x += chr->dwx0;
            y += chr->dwy0;
        }
    }
}
#endif

unsigned char *pntr_load_file(const char *fileName, unsigned int *bytesRead) {
    *bytesRead = 0;
    if (fileName == NULL) {
        return pntr_set_error("pntr_load_file() requires a valid fileName");
    }

    #ifdef PNTR_LOAD_FILE
    return PNTR_LOAD_FILE(fileName, bytesRead);
    #endif

    FILE *file = fopen(fileName, "rb");
    if (file == NULL) {
        return pntr_set_error("Failed to open file");
    }

    fseek(file, 0, SEEK_END);
    unsigned int size = (unsigned int)ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size <= 0) {
        fclose(file);
        return pntr_set_error("Failed to read file");
    }

    unsigned char *data = (unsigned char *)PNTR_MALLOC((size_t)size * sizeof(unsigned char));
    unsigned int count = (unsigned int)fread(data, sizeof(unsigned char), (size_t)size, file);
    *bytesRead = count;

    if (count != size) {
        pntr_set_error("File only partially loaded.");
    }

    fclose(file);

    return data;
}

pntr_font* pntr_load_bdffont(const char* fileName, pntr_color defaultColor) {
    unsigned int bytesRead;
    const unsigned char* fileData = pntr_load_file(fileName, &bytesRead);
    if (fileData == NULL || bytesRead <= 0) {
        return NULL;
    }

    return pntr_load_bdffont_from_memory(fileData, bytesRead, defaultColor);
}

pntr_font* pntr_load_bdffont_from_memory(const unsigned char* fileData, unsigned int dataSize, pntr_color defaultColor) {
    #ifdef PNTR_SUPPORT_BDF
    if (fileData == NULL || dataSize <= 0) {
        return pntr_set_error("Loading BDFFont requires valid fileData and dataSize");
    }
    al_bdf_Font* bdfFont = PNTR_MALLOC(sizeof(al_bdf_Font));
    if (bdfFont == NULL) {
        return pntr_set_error("Failed to allocate al_bdf_Font");
    }

    pntr_bdf_reader bdfReader;
    bdfReader.fileData = (unsigned char*)fileData;
    bdfReader.dataSize = dataSize;
    bdfReader.current = 0;

    al_bdf_Result result = al_bdf_load(bdfFont, pntr_bdf_read, &bdfReader);
    if (result != AL_BDF_OK) {
        PNTR_FREE(bdfFont);
        printf("%i %i\n", result, dataSize);
        return pntr_set_error("Failed to load BDF");
    }

    pntr_font* font = PNTR_MALLOC(sizeof(pntr_font));
    if (font == NULL) {
        al_bdf_unload(bdfFont);
        return pntr_set_error("Failed to allocate font memory");
    }
    font->color = defaultColor;
    font->userdata = bdfFont;
    font->type = PNTR_FONTTYPE_BDF;
    return font;

    #else
    (void*)fileData;
    (void)dataSize;
    (void)defaultColor;
    return pntr_set_error("pntr_load_bdf() requires PNTR_SUPPORT_BDF");
    #endif
}

void pntr_unload_font(pntr_font* font) {
    if (font == NULL) {
        return;
    }

    // Atlas
    if (font->atlas != NULL) {
        pntr_unload_image(font->atlas);
        font->atlas = NULL;
    }

    // BDF UserData
    #ifdef PNTR_SUPPORT_BDF
    if (font->type == PNTR_FONTTYPE_BDF) {
        al_bdf_unload((al_bdf_Font*)font->userdata);
        font->userdata = NULL;
    }
    #endif

    PNTR_FREE(font);
}

void pntr_draw_text(pntr_image* dst, pntr_font* font, const char* text, int posX, int posY) {
    if (dst == NULL || font == NULL || font->atlas == NULL || text == NULL) {
        return;
    }

    #ifdef PNTR_SUPPORT_BDF
    if (font->type == PNTR_FONTTYPE_BDF) {
        al_bdf_Font* bdfFont = (al_bdf_Font*)font->userdata;
        pntr_bdf_render(bdfFont, text, posX, posY, dst, font->color);
        return;
    }
    #endif

    int x = posX;
    int y = posY;

    const char * currentChar = text;
    while (currentChar != NULL && *currentChar != '\0') {
        if (*currentChar == '\n') {
            // TODO: pntr_draw_text(): Allow for center/right alignment
            x = posX;
            y += font->atlas->height;
        }
        else {
            for (int i = 0; i < font->charactersFound; i++) {
                if (font->characters[i] == *currentChar) {
                    pntr_draw_image_rec(dst, font->atlas, font->rectangles[i], x, y);
                    x += font->rectangles[i].width;
                    break;
                }
            }
        }

        currentChar++;
    }
}

inline int pntr_measure_text(pntr_font* font, const char* text) {
    return pntr_measure_text_ex(font, text).x;
}

pntr_vector pntr_measure_text_ex(pntr_font* font, const char* text) {
    if (font == NULL || text == NULL) {
        return CLITERAL(pntr_vector){0, 0};
    }

    pntr_vector output;
    output.x = 0;
    output.y = font->atlas->height;
    int x = 0;
    const char * currentChar = text;

    while (currentChar != NULL && *currentChar != '\0') {
        if (*currentChar == '\n') {
            output.y += font->atlas->height;
            x = 0;
        }
        else {
            for (int i = 0; i < font->charactersFound; i++) {
                if (font->characters[i] == *currentChar) {
                    x += font->rectangles[i].width;
                    if (output.x < x) {
                        output.x = x;
                    }
                    break;
                }
            }
        }
        currentChar++;
    }

    return output;
}

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
 * You can change this by defining your own PNTR_DEFAULT_FONT.
 *
 * #define PNTR_DEFAULT_FONT pntr_load_ttyfont("myfont.png", 10, 10, "abc")
 */
pntr_font* pntr_load_default_font() {
#ifdef PNTR_SUPPORT_DEFAULT_FONT
    // https://github.com/Grumbel/SDL_tty/blob/master/src/font8x8.h
    #include "external/font8x8.h"

    // Port the font8x8 data to a pntr_image
    pntr_image* sourceImage = pntr_image_from_pixelformat((void*)font8x8_data, font8x8_width, font8x8_height, PNTR_PIXELFORMAT_RGBA8888);
    if (sourceImage == NULL) {
        return pntr_set_error("pntr_load_default_font() failed to convert default image");
    }

    // Create a copy since the original source data is held locally
    pntr_image* newImage = pntr_image_copy(sourceImage);

    // Since the source data is defined locally, don't free() it
    sourceImage->data = NULL;
    pntr_unload_image(sourceImage);

    if (newImage == NULL) {
        return pntr_set_error("pntr_load_default_font() failed to copy the source image");
    }

    // Load the font from the new image.
    pntr_font* font = pntr_load_ttyfont_from_image(newImage, font8x8_glyph_width, font8x8_glyph_height, font8x8_glyphs);
    if (font == NULL) {
        pntr_unload_image(newImage);
        return pntr_set_error("Failed to load default font from image");
    }

    return font;
#elif defined(PNTR_DEFAULT_FONT)
    return PNTR_DEFAULT_FONT;
#else
    return pntr_set_error("pntr_load_default_font() requires PNTR_SUPPORT_DEFAULT_FONT");
#endif
}

#ifdef __cplusplus
}
#endif

#endif  // PNTR_IMPLEMENTATION_ONCE
#endif  // PNTR_IMPLEMENTATION
