#ifndef PNTR_H__
#define PNTR_H__

#include <stdint.h>  // pntr_color

#ifndef PNTR_API
#define PNTR_API
#endif

typedef union pntr_color_t {
  uint32_t data;
  struct {
    // TODO: Verify the order here. What about big endian?
    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char a;
  };
} pntr_color_t;

typedef union pntr_color_t pntr_color;

typedef struct pntr_image {
  pntr_color* data;
  int width;
  int height;
  int pitch;
} pntr_image;

typedef struct pntr_rectangle {
  int x;
  int y;
  int width;
  int height;
} pntr_rectangle;

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
PNTR_API void pntr_draw_rectangle(pntr_image* dst, int posX, int posY, int width, int height, pntr_color color);
PNTR_API void pntr_draw_circle(pntr_image* dst, int centerX, int centerY, int radius, pntr_color color);
PNTR_API void pntr_draw_image(pntr_image* dst, pntr_image* src, int posX, int posY);
PNTR_API void pntr_draw_image_rec(pntr_image* dst, pntr_image* src, pntr_rectangle srcRect, int posX, int posY);
PNTR_API pntr_color pntr_new_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
PNTR_API pntr_color pntr_get_color(unsigned int hexValue);
PNTR_API void pntr_color_get_rgba(pntr_color color, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a);
PNTR_API unsigned char pntr_color_get_r(pntr_color color);
PNTR_API unsigned char pntr_color_get_g(pntr_color color);
PNTR_API unsigned char pntr_color_get_b(pntr_color color);
PNTR_API unsigned char pntr_color_get_a(pntr_color color);
PNTR_API pntr_color pntr_color_set_r(pntr_color color, unsigned char r);
PNTR_API pntr_color pntr_color_set_g(pntr_color color, unsigned char g);
PNTR_API pntr_color pntr_color_set_b(pntr_color color, unsigned char b);
PNTR_API pntr_color pntr_color_set_a(pntr_color color, unsigned char a);
PNTR_API pntr_color pntr_image_get_color(pntr_image* image, int x, int y);
PNTR_API pntr_color* pntr_image_get_color_pointer(pntr_image* image, int x, int y);
PNTR_API pntr_image* pntr_load_image(const char* fileName);
PNTR_API const char* pntr_get_error();
PNTR_API void* pntr_set_error(const char* error);
PNTR_API pntr_image* pntr_image_from_pixelformat(void* data, int width, int height, int pixelFormat);

#ifdef __cplusplus
}
#endif

#if defined(__cplusplus)
#define CLITERAL(type) type
#else
#define CLITERAL(type) (type)
#endif

#define PNTR_LIGHTGRAY \
  CLITERAL(pntr_color) { .r = 200, .g = 200, .b = 200, .a = 255 }
#define PNTR_GRAY \
  CLITERAL(pntr_color) { .r = 130, .g = 130, .b = 130, .a = 255 }
#define PNTR_DARKGRAY \
  CLITERAL(pntr_color) { .r = 80, .g = 80, .b = 80, .a = 255 }
#define PNTR_YELLOW \
  CLITERAL(pntr_color) { .r = 253, .g = 249, .b = 0, .a = 255 }
#define PNTR_GOLD \
  CLITERAL(pntr_color) { .r = 255, .g = 203, .b = 0, .a = 255 }
#define PNTR_ORANGE \
  CLITERAL(pntr_color) { .r = 255, .g = 161, .b = 0, .a = 255 }
#define PNTR_PINK \
  CLITERAL(pntr_color) { .r = 255, .g = 109, .b = 194, .a = 255 }
#define PNTR_RED \
  CLITERAL(pntr_color) { .r = 230, .g = 41, .b = 55, .a = 255 }
#define PNTR_MAROON \
  CLITERAL(pntr_color) { .r = 190, .g = 33, .b = 55, .a = 255 }
#define PNTR_GREEN \
  CLITERAL(pntr_color) { .r = 0, .g = 228, .b = 48, .a = 255 }
#define PNTR_LIME \
  CLITERAL(pntr_color) { .r = 0, .g = 158, .b = 47, .a = 255 }
#define PNTR_DARKGREEN \
  CLITERAL(pntr_color) { .r = 0, .g = 117, .b = 44, .a = 255 }
#define PNTR_SKYBLUE \
  CLITERAL(pntr_color) { .r = 102, .g = 191, .b = 255, .a = 255 }
#define PNTR_BLUE \
  CLITERAL(pntr_color) { .r = 0, .g = 121, .b = 241, .a = 255 }
#define PNTR_DARKBLUE \
  CLITERAL(pntr_color) { .r = 0, .g = 82, .b = 172, .a = 255 }
#define PNTR_PURPLE \
  CLITERAL(pntr_color) { .r = 200, .g = 122, .b = 255, .a = 255 }
#define PNTR_VIOLET \
  CLITERAL(pntr_color) { .r = 135, .g = 60, .b = 190, .a = 255 }
#define PNTR_DARKPURPLE \
  CLITERAL(pntr_color) { .r = 112, .g = 31, .b = 126, .a = 255 }
#define PNTR_BEIGE \
  CLITERAL(pntr_color) { .r = 211, .g = 176, .b = 131, .a = 255 }
#define PNTR_BROWN \
  CLITERAL(pntr_color) { .r = 127, .g = 106, .b = 79, .a = 255 }
#define PNTR_DARKBROWN \
  CLITERAL(pntr_color) { .r = 76, .g = 63, .b = 47, .a = 255 }
#define PNTR_WHITE \
  CLITERAL(pntr_color) { .r = 255, .g = 255, .b = 255, .a = 255 }
#define PNTR_BLACK \
  CLITERAL(pntr_color) { .r = 0, .g = 0, .b = 0, .a = 255 }
#define PNTR_BLANK \
  CLITERAL(pntr_color) { .r = 0, .g = 0, .b = 0, .a = 0 }
#define PNTR_MAGENTA \
  CLITERAL(pntr_color) { .r = 255, .g = 0, .b = 255, .a = 255 }
#define PNTR_RAYWHITE \
  CLITERAL(pntr_color) { .r = 245, .g = 245, .b = 245, .a = 255 }

#define PNTR_PIXELFORMAT_ARGB8888 0
#define PNTR_PIXELFORMAT_RGBA8888 1
#define PNTR_PIXELFORMAT_LAST PNTR_PIXELFORMAT_RGBA8888

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
#define PNTR_MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef PNTR_MIN
#define PNTR_MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

// stb_image
// TODO: Allow selective inclusion with stb_image. And use STBI_NO_STDIO
#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STBI_ONLY_PNG
#define STBI_NO_SIMD
#define STBI_NO_HDR
#define STBI_NO_LINEAR
#define STBI_NO_GIF
#define STBI_NO_THREAD_LOCALS
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"
#endif  // STBI_INCLUDE_STB_IMAGE_H

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
    return pntr_set_error("pntr_new_image() requires a width or height > 0");
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
  if (image == NULL || image->data == NULL) {
    return pntr_set_error("pntr_image_copy() requires valid image data");
  }

  pntr_image* newImage = pntr_new_image(image->width, image->height);
  if (newImage == NULL) {
    return NULL;
  }

  PNTR_MEMCPY(newImage->data, image->data, newImage->pitch * newImage->height);

  return newImage;
}

pntr_rectangle pntr_rectangle_intersect(pntr_rectangle* a, pntr_rectangle* b) {
  int left = PNTR_MAX(a->x, b->x);
  int right = PNTR_MIN(a->x + a->width, b->x + b->width);
  int top = PNTR_MAX(a->y, b->y);
  int bottom = PNTR_MIN(a->y + a->height, b->y + b->height);
  int width = right - left;
  int height = bottom - top;
  return CLITERAL(pntr_rectangle){left, top, PNTR_MAX(width, 0), PNTR_MAX(height, 0)};
}

pntr_image* pntr_image_from_image(pntr_image* image, int x, int y, int width, int height) {
  if (image == NULL || image->data == NULL) {
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
    PNTR_MEMCPY(((unsigned char*)result->data) + y * srcRect.width * (int)sizeof(pntr_color),
                ((unsigned char*)image->data) + ((y + srcRect.y) * image->width + srcRect.x) * (int)sizeof(pntr_color),
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
  pntr_color* row = pntr_image_get_color_pointer(dst, 0, posY);
  for (int x = posX; x < posX + width; ++x) {
    row[x] = color;
  }
}

void pntr_clear_background(pntr_image* image, pntr_color color) {
  if (image == NULL || image->data == NULL) {
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
      .a = a};
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
      .a = (unsigned char)hexValue & 0xFF};
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

inline pntr_color pntr_color_set_r(pntr_color color, unsigned char r) {
  color.r = r;
  return color;
}

inline pntr_color pntr_color_set_g(pntr_color color, unsigned char g) {
  color.g = g;
  return color;
}

inline pntr_color pntr_color_set_b(pntr_color color, unsigned char b) {
  color.b = b;
  return color;
}

inline pntr_color pntr_color_set_a(pntr_color color, unsigned char a) {
  color.a = a;
  return color;
}

inline void pntr_color_get_rgba(pntr_color color, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a) {
  *r = color.r;
  *g = color.g;
  *b = color.b;
  *a = color.a;
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

void pntr_draw_rectangle(pntr_image* dst, int posX, int posY, int width, int height, pntr_color color) {
  if (dst == NULL || dst->data == NULL) {
    return;
  }

  pntr_rectangle rect = CLITERAL(pntr_rectangle){posX, posY, width, height};
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
  if (image == NULL || image->data == NULL) {
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

pntr_image* pntr_load_image_from_memory(const unsigned char* fileData, int dataSize) {
  if (fileData == NULL || dataSize <= 0) {
    return pntr_set_error("pntr_load_image_from_memory() requires valid file data");
  }

  int width, height, channels_in_file;
  stbi_uc* output = stbi_load_from_memory(fileData, dataSize, &width, &height, &channels_in_file, 4);
  if (output == NULL) {
    return pntr_set_error("pntr_load_image_from_memory() failed to load image from memory");
  }

  return pntr_image_from_pixelformat((void*)output, width, height, PNTR_PIXELFORMAT_RGBA8888);
}

pntr_image* pntr_load_image(const char* fileName) {
  if (fileName == NULL) {
    return pntr_set_error("pntr_load_image() requires a valid fileName");
  }

  int width, height, channels_in_file;
  // TODO: Implement an abstracted file system and use pntr_load_image_from_memory() instead.
  stbi_uc* output = stbi_load(fileName, &width, &height, &channels_in_file, 4);
  if (output == NULL) {
    return pntr_set_error("pntr_load_image() failed to load image with stbi_load");
  }

  return pntr_image_from_pixelformat((void*)output, width, height, PNTR_PIXELFORMAT_RGBA8888);
}

#define COMPOSE_FAST(S, D, A) (((S * A) + (D * (256U - A))) >> 8U)

void pntr_draw_image(pntr_image* dst, pntr_image* src, int posX, int posY) {
  pntr_draw_image_rec(dst, src, CLITERAL(pntr_rectangle){0, 0, src->width, src->height}, posX, posY);
}

void pntr_draw_image_rec(pntr_image* dst, pntr_image* src, pntr_rectangle srcRect, int posX, int posY) {
  if (dst == NULL || dst->data == NULL || src == NULL || src->data == NULL) {
    return;
  }

  pntr_rectangle dstRect = CLITERAL(pntr_rectangle){posX, posY, srcRect.width, srcRect.height};

  if (srcRect.width <= 0 || srcRect.height <= 0) {
    srcRect.width = src->width;
    srcRect.height = src->height;
  }

  if (dstRect.x < 0) {
    srcRect.x += -dstRect.x;
    srcRect.width += dstRect.x;
  }
  if (dstRect.y < 0) {
    srcRect.y += -dstRect.y;
    srcRect.height += dstRect.y;
  }

  if (srcRect.width < dstRect.width) {
    dstRect.width = srcRect.width;
  }
  if (srcRect.height < dstRect.height) {
    dstRect.height = srcRect.height;
  }

  if (dstRect.x + dstRect.width > dst->width) {
    dstRect.height = dst->height - dstRect.y;
  }

  if (srcRect.width <= 0 || srcRect.width <= 0 || dstRect.width <= 0 || dstRect.height <= 0 || dstRect.x >= dst->width || dstRect.y >= dst->height) {
    return;
  }

  int dst_skip = dst->pitch >> 2;
  int src_skip = src->pitch >> 2;

  pntr_color* dstPixel = dst->data + dst_skip * dstRect.y + dstRect.x;
  pntr_color* srcPixel = src->data + src_skip * srcRect.y + srcRect.x;

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
pntr_image* pntr_image_from_pixelformat(void* data, int width, int height, int pixelFormat) {
  if (data == NULL || width <= 0 || height <= 0 || pixelFormat < 0 || pixelFormat > PNTR_PIXELFORMAT_LAST) {
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

  switch (pixelFormat) {
    case PNTR_PIXELFORMAT_RGBA8888: {
      pntr_color color;
      for (int i = 0; i < width * height; i++) {
        color = output->data[i];
        output->data[i].r = color.b;
        output->data[i].b = color.r;
      }
    } break;
  }

  return output;
}

#ifdef __cplusplus
}
#endif

#endif  // PNTR_IMPLEMENTATION_ONCE
#endif  // PNTR_IMPLEMENTATION
