#ifndef PNTR_H__
#define PNTR_H__

#include <stdint.h> // uint32_t

#ifndef PNTR_API
#define PNTR_API
#endif

typedef struct pntr_image {
    uint32_t* data;
    int width;
    int height;
    int pitch;
} pntr_image;

typedef struct pntr_color {
    unsigned char a;
    unsigned char r;
    unsigned char g;
    unsigned char b;
} pntr_color;

#ifdef __cplusplus
extern "C" {
#endif

PNTR_API pntr_image* pntr_new_image(int width, int height, pntr_color color);
PNTR_API void pntr_unload_image(pntr_image* image);
PNTR_API void pntr_clear(pntr_image* image, pntr_color color);
PNTR_API uint32_t pntr_color_to_uint32(pntr_color color);
PNTR_API pntr_color pntr_uint32_to_color(uint32_t color);
PNTR_API pntr_color pntr_new_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
PNTR_API void pntr_draw_pixel_uint32(pntr_image* dst, int x, int y, uint32_t col);
PNTR_API void pntr_draw_pixel(pntr_image* dst, int x, int y, pntr_color color);
PNTR_API void pntr_draw_rectangle(pntr_image* dst, int posX, int posY, int width, int height, pntr_color color);
PNTR_API void pntr_draw_pixel(pntr_image* dst, int x, int y, pntr_color color);
PNTR_API void pntr_draw_pixel_uint32(pntr_image* dst, int x, int y, uint32_t col);

#ifdef __cplusplus
}
#endif

#if defined(__cplusplus)
    #define PNTR_CLITERAL(type) type
#else
    #define PNTR_CLITERAL(type) (type)
#endif

#define PNTR_LIGHTGRAY  PNTR_CLITERAL(pntr_color){255, 200, 200, 200}   // Light Gray
#define PNTR_GRAY       PNTR_CLITERAL(pntr_color){255, 130, 130, 130}   // Gray
#define PNTR_DARKGRAY   PNTR_CLITERAL(pntr_color){255, 80, 80, 80}      // Dark Gray
#define PNTR_YELLOW     PNTR_CLITERAL(pntr_color){255, 253, 249, 0}     // Yellow
#define PNTR_GOLD       PNTR_CLITERAL(pntr_color){255, 255, 203, 0}     // Gold
#define PNTR_ORANGE     PNTR_CLITERAL(pntr_color){255, 255, 161, 0}     // Orange
#define PNTR_PINK       PNTR_CLITERAL(pntr_color){255, 255, 109, 194}   // Pink
#define PNTR_RED        PNTR_CLITERAL(pntr_color){255, 230, 41, 55}     // Red
#define PNTR_MAROON     PNTR_CLITERAL(pntr_color){255, 190, 33, 55}     // Maroon
#define PNTR_GREEN      PNTR_CLITERAL(pntr_color){255, 0, 228, 48}      // Green
#define PNTR_LIME       PNTR_CLITERAL(pntr_color){255, 0, 158, 47}      // Lime
#define PNTR_DARKGREEN  PNTR_CLITERAL(pntr_color){255, 0, 117, 44}      // Dark Green
#define PNTR_SKYBLUE    PNTR_CLITERAL(pntr_color){255, 102, 191, 255}   // Sky Blue
#define PNTR_BLUE       PNTR_CLITERAL(pntr_color){255, 0, 121, 241}     // Blue
#define PNTR_DARKBLUE   PNTR_CLITERAL(pntr_color){255, 0, 82, 172}      // Dark Blue
#define PNTR_PURPLE     PNTR_CLITERAL(pntr_color){255, 200, 122, 255}   // Purple
#define PNTR_VIOLET     PNTR_CLITERAL(pntr_color){255, 135, 60, 190}    // Violet
#define PNTR_DARKPURPLE PNTR_CLITERAL(pntr_color){255, 112, 31, 126}    // Dark Purple
#define PNTR_BEIGE      PNTR_CLITERAL(pntr_color){255, 211, 176, 131}   // Beige
#define PNTR_BROWN      PNTR_CLITERAL(pntr_color){255, 127, 106, 79}    // Brown
#define PNTR_DARKBROWN  PNTR_CLITERAL(pntr_color){255, 76, 63, 47}      // Dark Brown
#define PNTR_WHITE      PNTR_CLITERAL(pntr_color){255, 255, 255, 255}   // White
#define PNTR_BLACK      PNTR_CLITERAL(pntr_color){255, 0, 0, 0}         // Black
#define PNTR_BLANK      PNTR_CLITERAL(pntr_color){0, 0, 0, 0}           // Blank (Transparent)
#define PNTR_MAGENTA    PNTR_CLITERAL(pntr_color){255, 255, 0, 255}     // Magenta
#define PNTR_RAYWHITE   PNTR_CLITERAL(pntr_color){255, 245, 245, 245}   // raylib logo

#endif  // PNTR_H__

#ifdef PNTR_IMPLEMENTATION
#ifndef PNTR_IMPLEMENTATION_ONCE
#define PNTR_IMPLEMENTATION_ONCE

#ifndef PNTR_MALLOC
#include <stdlib.h>
#define PNTR_MALLOC(size) malloc((size_t)(size))
#endif  // PNTR_MALLOC

#ifndef PNTR_FREE
#include <stdlib.h>
#define PNTR_FREE(obj) free((void*)(obj))
#endif  // PNTR_FREE

// #ifndef PNTR_MEMCPY
// #include <string.h>
// #define PNTR_MEMCPY(dest, src, n) memcpy((void*)(dest), (const void*)(src), (size_t)(n))
// #endif  // PNTR_MEMCPY

#ifdef __cplusplus
extern "C" {
#endif

pntr_image* pntr_new_image(int width, int height, pntr_color color) {
    pntr_image* image = (pntr_image*)PNTR_MALLOC(sizeof(pntr_image));
    if (image == NULL) {
        return NULL;
    }

    image->pitch = width * (int)sizeof(uint32_t);
    image->width = width;
    image->height = height;
    image->data = (uint32_t*)PNTR_MALLOC(image->pitch * height);

    pntr_clear(image, color);

    return image;
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

void pntr_clear(pntr_image* image, pntr_color color) {
    if (image == NULL || image->data == NULL) {
        return;
    }

    uint32_t *begin = image->data;
    uint32_t *end   = image->data + image->height * (image->pitch >> 2);
    uint32_t col  = pntr_color_to_uint32(color);

    while (begin < end) {
        *begin++ = col;
    }
}

uint32_t pntr_color_to_uint32(pntr_color color) {
    // TODO: Type cast directly to a uint32?
    //return *(uint32_t*)&color;
    return (((uint32_t)color.a << 24) | ((uint32_t)color.r << 16) | ((uint32_t)color.g << 8) | (uint32_t)color.b);
}

pntr_color pntr_uint32_to_color(uint32_t hexValue) {
    // TODO: Type cast directly to a pntr_color?
    pntr_color color;

    color.a = (unsigned char)(hexValue >> 24) & 0xFF;
    color.r = (unsigned char)(hexValue >> 16) & 0xFF;
    color.g = (unsigned char)(hexValue >> 8) & 0xFF;
    color.b = (unsigned char)hexValue & 0xFF;

    return color;
}

inline pntr_color pntr_new_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    return PNTR_CLITERAL(pntr_color){a, r, g, b};
}

void pntr_draw_pixel_uint32(pntr_image* dst, int x, int y, uint32_t col) {
    dst->data[y * (dst->pitch >> 2) + x] = col;
}

void pntr_draw_pixel(pntr_image* dst, int x, int y, pntr_color color) {
    if ((dst->data == NULL) || (x < 0) || (x >= dst->width) || (y < 0) || (y >= dst->height)) {
        return;
    }

    if (color.a == 0) {
        return;
    }

    // TODO: Allow drawing Alpha-Transparency pixels
    pntr_draw_pixel_uint32(dst, x, y, pntr_color_to_uint32(color));
}

void pntr_draw_hline_uint32(pntr_image* dst, int x, int y, int width, uint32_t color) {
    uint32_t* row  = dst->data + y * (dst->pitch >> 2);
    while (x < width) {
        row[x++] = color;
    }
}

void pntr_draw_rectangle(pntr_image* dst, int posX, int posY, int width, int height, pntr_color color) {
    // Fully transparent
    if (color.a == 0) {
        return;
    }

    if (dst == NULL || dst->data == NULL) {
        return;
    }
    if (posX < 0) {
        width = width - posX;
        posX = 0;
    }
    if (posY < 0) {
        height = height - posY;
        posY = 0;
    }
    if (posX > dst->width || posY > dst->height) {
        return;
    }
    if (posX + width >= dst->width) {
        width = dst->width - posX;
    }
    if (posY + height >= dst->height) {
        height = dst->height - posY;
    }

    if (width == 0 || height == 0) {
        return;
    }

    // Semi-transparent
    if (color.a != 255) {
        // TODO: Draw with composition.
        color.a = 255;
    }

    int row_size = dst->pitch >> 2;
    uint32_t col = pntr_color_to_uint32(color);

    int x;
    int xend = posX + width;
    uint32_t *row  = dst->data + posY * row_size;
    uint32_t *end  = row + row_size * height;

    do {
        for (x = posX; x < xend; ++x) {
            row[x] = col;
        }
        row += row_size;
    } while (row < end);
}

#ifdef __cplusplus
}
#endif

#endif  // PNTR_IMPLEMENTATION_ONCE
#endif  // PNTR_IMPLEMENTATION
