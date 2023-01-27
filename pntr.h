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

#ifdef __cplusplus
extern "C" {
#endif

PNTR_API pntr_image* pntr_new_image(int width, int height, uint32_t color);
PNTR_API void pntr_unload_image(pntr_image* image);
PNTR_API void pntr_clear(pntr_image* image, uint32_t color);
PNTR_API void pntr_draw_pixel(pntr_image* dst, int x, int y, uint32_t color);
PNTR_API void pntr_draw_rectangle(pntr_image* dst, int posX, int posY, int width, int height, uint32_t color);
PNTR_API uint32_t pntr_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
PNTR_API void pntr_color_rgba(uint32_t color, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a);
PNTR_API unsigned char pntr_color_r(uint32_t color);
PNTR_API unsigned char pntr_color_g(uint32_t color);
PNTR_API unsigned char pntr_color_b(uint32_t color);
PNTR_API unsigned char pntr_color_a(uint32_t color);
PNTR_API uint32_t pntr_color_set_r(uint32_t color, unsigned char r);
PNTR_API uint32_t pntr_color_set_g(uint32_t color, unsigned char g);
PNTR_API uint32_t pntr_color_set_b(uint32_t color, unsigned char b);
PNTR_API uint32_t pntr_color_set_a(uint32_t color, unsigned char a);

#ifdef __cplusplus
}
#endif

#define PNTR_LIGHTGRAY  pntr_color(200, 200, 200, 255)   // Light Gray
#define PNTR_GRAY       pntr_color(130, 130, 130, 255)   // Gray
#define PNTR_DARKGRAY   pntr_color(80, 80, 80, 255)      // Dark Gray
#define PNTR_YELLOW     pntr_color(253, 249, 0, 255)     // Yellow
#define PNTR_GOLD       pntr_color(255, 203, 0, 255)     // Gold
#define PNTR_ORANGE     pntr_color(255, 161, 0, 255)     // Orange
#define PNTR_PINK       pntr_color(255, 109, 194, 255)   // Pink
#define PNTR_RED        pntr_color(230, 41, 55, 255)     // Red
#define PNTR_MAROON     pntr_color(190, 33, 55, 255)     // Maroon
#define PNTR_GREEN      pntr_color(0, 228, 48, 255)      // Green
#define PNTR_LIME       pntr_color(0, 158, 47, 255)      // Lime
#define PNTR_DARKGREEN  pntr_color(0, 117, 44, 255)      // Dark Green
#define PNTR_SKYBLUE    pntr_color(102, 191, 255, 255)   // Sky Blue
#define PNTR_BLUE       pntr_color(0, 121, 241, 255)     // Blue
#define PNTR_DARKBLUE   pntr_color(0, 82, 172, 255)      // Dark Blue
#define PNTR_PURPLE     pntr_color(200, 122, 255, 255)   // Purple
#define PNTR_VIOLET     pntr_color(135, 60, 190, 255)    // Violet
#define PNTR_DARKPURPLE pntr_color(112, 31, 126, 255)    // Dark Purple
#define PNTR_BEIGE      pntr_color(211, 176, 131, 255)   // Beige
#define PNTR_BROWN      pntr_color(127, 106, 79, 255)    // Brown
#define PNTR_DARKBROWN  pntr_color(76, 63, 47, 255)      // Dark Brown
#define PNTR_WHITE      pntr_color(255, 255, 255, 255)   // White
#define PNTR_BLACK      pntr_color(0, 0, 0, 255)         // Black
#define PNTR_BLANK      pntr_color(0, 0, 0, 0)           // Blank (Transparent)
#define PNTR_MAGENTA    pntr_color(255, 0, 255, 255)     // Magenta
#define PNTR_RAYWHITE   pntr_color(245, 245, 245, 255)   // raylib logo

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

pntr_image* pntr_new_image(int width, int height, uint32_t color) {
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

void pntr_clear(pntr_image* image, uint32_t color) {
    if (image == NULL || image->data == NULL) {
        return;
    }

    uint32_t *begin = image->data;
    uint32_t *end   = image->data + image->height * (image->pitch >> 2);

    while (begin < end) {
        *begin++ = color;
    }
}

uint32_t pntr_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    return (((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b);
}

unsigned char pntr_color_r(uint32_t color) {
    return (unsigned char)(color >> 16) & 0xFF;
}

unsigned char pntr_color_g(uint32_t color) {
    return (unsigned char)(color >> 8) & 0xFF;
}

unsigned char pntr_color_b(uint32_t color) {
    return (unsigned char)color & 0xFF;
}

unsigned char pntr_color_a(uint32_t color) {
    return (unsigned char)(color >> 24) & 0xFF;
}

uint32_t pntr_color_set_r(uint32_t color, unsigned char r) {
    return pntr_color(r, pntr_color_g(color), pntr_color_b(color), pntr_color_a(color));
}

uint32_t pntr_color_set_g(uint32_t color, unsigned char g) {
    return pntr_color(pntr_color_r(color), g, pntr_color_b(color), pntr_color_a(color));
}

uint32_t pntr_color_set_b(uint32_t color, unsigned char b) {
    return pntr_color(pntr_color_r(color), pntr_color_g(color), b, pntr_color_a(color));
}

uint32_t pntr_color_set_a(uint32_t color, unsigned char a) {
    return pntr_color(pntr_color_r(color), pntr_color_g(color), pntr_color_b(color), a);
}

void pntr_color_rgba(uint32_t color, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a) {
    *r = pntr_color_r(color);
    *g = pntr_color_g(color);
    *b = pntr_color_b(color);
    *a = pntr_color_a(color);
}

void pntr_draw_pixel_unsafe(pntr_image* dst, int x, int y, uint32_t color) {
    dst->data[y * (dst->pitch >> 2) + x] = color;
}

void pntr_draw_pixel(pntr_image* dst, int x, int y, uint32_t color) {
    if ((dst->data == NULL) || (x < 0) || (x >= dst->width) || (y < 0) || (y >= dst->height)) {
        return;
    }

    // TODO: Allow drawing Alpha-Transparency pixels
    pntr_draw_pixel_unsafe(dst, x, y, color);
}

void pntr_draw_rectangle(pntr_image* dst, int posX, int posY, int width, int height, uint32_t color) {
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

    int row_size = dst->pitch >> 2;
    int x;
    int xend = posX + width;
    uint32_t *row  = dst->data + posY * row_size;
    uint32_t *end  = row + row_size * height;

    do {
        for (x = posX; x < xend; ++x) {
            row[x] = color;
        }
        row += row_size;
    } while (row < end);
}

#ifdef __cplusplus
}
#endif

#endif  // PNTR_IMPLEMENTATION_ONCE
#endif  // PNTR_IMPLEMENTATION
