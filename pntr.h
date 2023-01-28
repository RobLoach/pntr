#ifndef PNTR_H__
#define PNTR_H__

#include <stdint.h> // pntr_color

#ifndef PNTR_API
#define PNTR_API
#endif

typedef uint32_t pntr_color;

typedef struct pntr_image {
    pntr_color* data;
    int width;
    int height;
    int pitch;
} pntr_image;

#ifdef __cplusplus
extern "C" {
#endif

PNTR_API pntr_image* pntr_new_image(int width, int height);
pntr_image* pntr_gen_image_color(int width, int height, pntr_color color);
PNTR_API void pntr_unload_image(pntr_image* image);
PNTR_API void pntr_clear_background(pntr_image* image, pntr_color color);
PNTR_API void pntr_draw_pixel(pntr_image* dst, int x, int y, pntr_color color);
PNTR_API void pntr_draw_rectangle(pntr_image* dst, int posX, int posY, int width, int height, pntr_color color);
PNTR_API pntr_color pntr_new_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
PNTR_API void pntr_color_rgba(pntr_color color, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a);
PNTR_API unsigned char pntr_color_r(pntr_color color);
PNTR_API unsigned char pntr_color_g(pntr_color color);
PNTR_API unsigned char pntr_color_b(pntr_color color);
PNTR_API unsigned char pntr_color_a(pntr_color color);
PNTR_API pntr_color pntr_color_set_r(pntr_color color, unsigned char r);
PNTR_API pntr_color pntr_color_set_g(pntr_color color, unsigned char g);
PNTR_API pntr_color pntr_color_set_b(pntr_color color, unsigned char b);
PNTR_API pntr_color pntr_color_set_a(pntr_color color, unsigned char a);
PNTR_API pntr_color pntr_image_get_color(pntr_image* image, int x, int y);

#ifdef __cplusplus
}
#endif

#define PNTR_LIGHTGRAY  pntr_new_color(200, 200, 200, 255)   // Light Gray
#define PNTR_GRAY       pntr_new_color(130, 130, 130, 255)   // Gray
#define PNTR_DARKGRAY   pntr_new_color(80, 80, 80, 255)      // Dark Gray
#define PNTR_YELLOW     pntr_new_color(253, 249, 0, 255)     // Yellow
#define PNTR_GOLD       pntr_new_color(255, 203, 0, 255)     // Gold
#define PNTR_ORANGE     pntr_new_color(255, 161, 0, 255)     // Orange
#define PNTR_PINK       pntr_new_color(255, 109, 194, 255)   // Pink
#define PNTR_RED        pntr_new_color(230, 41, 55, 255)     // Red
#define PNTR_MAROON     pntr_new_color(190, 33, 55, 255)     // Maroon
#define PNTR_GREEN      pntr_new_color(0, 228, 48, 255)      // Green
#define PNTR_LIME       pntr_new_color(0, 158, 47, 255)      // Lime
#define PNTR_DARKGREEN  pntr_new_color(0, 117, 44, 255)      // Dark Green
#define PNTR_SKYBLUE    pntr_new_color(102, 191, 255, 255)   // Sky Blue
#define PNTR_BLUE       pntr_new_color(0, 121, 241, 255)     // Blue
#define PNTR_DARKBLUE   pntr_new_color(0, 82, 172, 255)      // Dark Blue
#define PNTR_PURPLE     pntr_new_color(200, 122, 255, 255)   // Purple
#define PNTR_VIOLET     pntr_new_color(135, 60, 190, 255)    // Violet
#define PNTR_DARKPURPLE pntr_new_color(112, 31, 126, 255)    // Dark Purple
#define PNTR_BEIGE      pntr_new_color(211, 176, 131, 255)   // Beige
#define PNTR_BROWN      pntr_new_color(127, 106, 79, 255)    // Brown
#define PNTR_DARKBROWN  pntr_new_color(76, 63, 47, 255)      // Dark Brown
#define PNTR_WHITE      pntr_new_color(255, 255, 255, 255)   // White
#define PNTR_BLACK      pntr_new_color(0, 0, 0, 255)         // Black
#define PNTR_BLANK      pntr_new_color(0, 0, 0, 0)           // Blank (Transparent)
#define PNTR_MAGENTA    pntr_new_color(255, 0, 255, 255)     // Magenta
#define PNTR_RAYWHITE   pntr_new_color(245, 245, 245, 255)   // raylib logo

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

pntr_image* pntr_new_image(int width, int height) {
    pntr_image* image = (pntr_image*)PNTR_MALLOC(sizeof(pntr_image));
    if (image == NULL) {
        return NULL;
    }

    image->pitch = width * (int)sizeof(pntr_color);
    image->width = width;
    image->height = height;
    image->data = (pntr_color*)PNTR_MALLOC(image->pitch * height);

    return image;
}

pntr_image* pntr_gen_image_color(int width, int height, pntr_color color) {
    pntr_image* image = pntr_new_image(width, height);
    pntr_clear_background(image, color);
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

void pntr_clear_background(pntr_image* image, pntr_color color) {
    if (image == NULL || image->data == NULL) {
        return;
    }

    pntr_color *begin = image->data;
    pntr_color *end   = image->data + image->height * (image->pitch >> 2);

    while (begin < end) {
        *begin++ = color;
    }
}

pntr_color pntr_new_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    return (((pntr_color)a << 24) | ((pntr_color)r << 16) | ((pntr_color)g << 8) | (pntr_color)b);
}

unsigned char pntr_color_r(pntr_color color) {
    return (unsigned char)(color >> 16) & 0xFF;
}

unsigned char pntr_color_g(pntr_color color) {
    return (unsigned char)(color >> 8) & 0xFF;
}

unsigned char pntr_color_b(pntr_color color) {
    return (unsigned char)color & 0xFF;
}

unsigned char pntr_color_a(pntr_color color) {
    return (unsigned char)(color >> 24) & 0xFF;
}

pntr_color pntr_color_set_r(pntr_color color, unsigned char r) {
    return pntr_new_color(r, pntr_color_g(color), pntr_color_b(color), pntr_color_a(color));
}

pntr_color pntr_color_set_g(pntr_color color, unsigned char g) {
    return pntr_new_color(pntr_color_r(color), g, pntr_color_b(color), pntr_color_a(color));
}

pntr_color pntr_color_set_b(pntr_color color, unsigned char b) {
    return pntr_new_color(pntr_color_r(color), pntr_color_g(color), b, pntr_color_a(color));
}

pntr_color pntr_color_set_a(pntr_color color, unsigned char a) {
    return pntr_new_color(pntr_color_r(color), pntr_color_g(color), pntr_color_b(color), a);
}

void pntr_color_rgba(pntr_color color, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a) {
    *r = pntr_color_r(color);
    *g = pntr_color_g(color);
    *b = pntr_color_b(color);
    *a = pntr_color_a(color);
}

void pntr_draw_pixel_unsafe(pntr_image* dst, int x, int y, pntr_color color) {
    dst->data[y * (dst->pitch >> 2) + x] = color;
}

void pntr_draw_pixel(pntr_image* dst, int x, int y, pntr_color color) {
    if ((dst->data == NULL) || (x < 0) || (x >= dst->width) || (y < 0) || (y >= dst->height)) {
        return;
    }

    // TODO: Allow drawing Alpha-Transparency pixels
    pntr_draw_pixel_unsafe(dst, x, y, color);
}

void pntr_draw_horizontal_line_unsafe(pntr_image* dst, int posX, int posY, int width, pntr_color color) {
    pntr_color *row  = dst->data + posY * (dst->pitch >> 2);
    for (int x = posX; x < posX + width; ++x) {
        row[x] = color;
    }
}

void pntr_draw_rectangle(pntr_image* dst, int posX, int posY, int width, int height, pntr_color color) {
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

    for (int y = posY; y < posY + height; y++) {
        pntr_draw_horizontal_line_unsafe(dst, posX, y, width, color);
    }
}

pntr_color pntr_image_get_color(pntr_image* image, int x, int y) {
    if (image == NULL || image->data == NULL) {
        return 0;
    }
    return image->data[y * (image->pitch >> 2) + x];
}

#ifdef __cplusplus
}
#endif

#endif  // PNTR_IMPLEMENTATION_ONCE
#endif  // PNTR_IMPLEMENTATION
