#ifndef PNTR_STB_IMAGE_WRITE_H__
#define PNTR_STB_IMAGE_WRITE_H__

/**
 * Save an image using stb_image.
 */
unsigned char* pntr_stb_image_save_image_to_memory(pntr_image* image, unsigned int* dataSize);

#endif  // PNTR_STB_IMAGE_WRITE_H__

#ifdef PNTR_IMPLEMENTATION
#ifndef PNTR_STB_IMAGE_WRITE_IMPLEMENTATION
#define PNTR_STB_IMAGE_WRITE_IMPLEMENTATION

#ifndef PNTR_NO_STB_IMAGE_WRITE_IMPLEMENTATION
    #define STB_IMAGE_WRITE_IMPLEMENTATION
    #define STBIW_MALLOC PNTR_MALLOC
    #define STBIW_REALLOC PNTR_REALLOC
    #define STBIW_FREE PNTR_FREE
    #define STBIW_MEMMOVE PNTR_MEMMOVE
    #define STBI_WRITE_NO_STDIO
#endif  // PNTR_NO_STB_IMAGE_IMPLEMENTATION

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

#include "../external/stb_image_write.h"

#define PNTR_NO_STB_IMAGE_WRITE_IMPLEMENTATION
#ifdef STB_IMAGE_WRITE_IMPLEMENTATION
    #undef STB_IMAGE_WRITE_IMPLEMENTATION
#endif

#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
#endif // defined(__GNUC__) || defined(__clang__)

// Early declaration of the function we'll use.
unsigned char *stbi_write_png_to_mem(const unsigned char *pixels, int stride_bytes, int x, int y, int n, int *out_len);

unsigned char* pntr_stb_image_save_image_to_memory(pntr_image* image, unsigned int* dataSize) {
    const unsigned char* pixels = (const unsigned char*)pntr_image_to_pixelformat(image, NULL, PNTR_PIXELFORMAT_RGBA8888);
    if (pixels == NULL) {
        return NULL;
    }

    int stride_bytes = pntr_get_pixel_data_size(image->width, 1, PNTR_PIXELFORMAT_RGBA8888);
    int out_len;
    unsigned char* output = stbi_write_png_to_mem(pixels, stride_bytes, image->width, image->height, 4, &out_len);
    if (dataSize != NULL) {
        *dataSize = (unsigned int)out_len;
    }
    pntr_unload_memory((void*)pixels);
    return output;
}

#ifdef PNTR_SAVE_IMAGE_TO_MEMORY
#undef PNTR_SAVE_IMAGE_TO_MEMORY
#endif
#define PNTR_SAVE_IMAGE_TO_MEMORY pntr_stb_image_save_image_to_memory

#endif  // PNTR_STB_IMAGE_WRITE_IMPLEMENTATION
#endif  // PNTR_IMPLEMENTATION
