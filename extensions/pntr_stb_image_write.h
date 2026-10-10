#ifndef PNTR_STB_IMAGE_WRITE_H__
#define PNTR_STB_IMAGE_WRITE_H__

/**
 * @defgroup pntr_stb_image_write pntr_stb_image_write
 * @{
 *
 * @brief [stb_image_write](https://github.com/nothings/stb/blob/master/stb_image_write.h) integration with pntr for saving images.
 *
 * To use *stb_image_write* for saving images, define `PNTR_STB_IMAGE` prior to including `pntr.h`. This is provided by default.
 *
 * @code
 * #define PNTR_STB_IMAGE
 * #define PNTR_IMPLEMENTATION
 * #include "pntr.h"
 * @endcode
 *
 * Image saving can be completely disabled with `PNTR_NO_SAVE_IMAGE`.
 *
 * PNG, BMP and JPEG can all be saved. PNG and BMP can also be loaded back in, while
 * JPEG loading requires `PNTR_ENABLE_JPEG`.
 *
 * @see https://github.com/nothings/stb/blob/master/stb_image_write.h
 * @see PNTR_STB_IMAGE
 * @see PNTR_SAVE_IMAGE_TO_MEMORY
 * @see PNTR_NO_SAVE_IMAGE
 */

/**
 * Save an image using stb_image.
 */
unsigned char* pntr_stb_image_save_image_to_memory(pntr_image* image, pntr_image_type type, unsigned int* dataSize);

/**
 * @}
 */

#endif  // PNTR_STB_IMAGE_WRITE_H__

#ifdef PNTR_IMPLEMENTATION
#ifndef PNTR_STB_IMAGE_WRITE_IMPLEMENTATION
#define PNTR_STB_IMAGE_WRITE_IMPLEMENTATION

#ifndef PNTR_MEMMOVE
    #include <string.h>
    /**
     * Copies the values of num bytes from the location pointed by source to the memory block pointed by destination.
     *
     * @note If not defined, will use `memmove()`.
     *
     * @param destination Pointer to the destination array where the content is to be copied, type-casted to a pointer of type `void*`.
     * @param source Pointer to the source of data to be copied, type-casted to a pointer of type `const void*`.
     * @param num Number of bytes to copy.
     */
    #define PNTR_MEMMOVE(destination, source, num) memmove((destination), (source), (num))
#endif  // PNTR_MEMMOVE

#ifndef PNTR_NO_STB_IMAGE_WRITE_IMPLEMENTATION
    #define STB_IMAGE_WRITE_IMPLEMENTATION
    #define STBIW_MALLOC PNTR_MALLOC
    #define STBIW_REALLOC PNTR_REALLOC
    #define STBIW_FREE PNTR_FREE
    #define STBIW_MEMMOVE PNTR_MEMMOVE
    #define STBI_WRITE_NO_STDIO
    #define STBIW_ASSERT(x)
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

#ifndef PNTR_STB_IMAGE_WRITE_H
#define PNTR_STB_IMAGE_WRITE_H "../external/stb_image_write.h"
#endif
#include PNTR_STB_IMAGE_WRITE_H

#ifndef PNTR_NO_STB_IMAGE_WRITE_IMPLEMENTATION
    #define PNTR_NO_STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#ifdef STB_IMAGE_WRITE_IMPLEMENTATION
    #undef STB_IMAGE_WRITE_IMPLEMENTATION
#endif

#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
#endif // defined(__GNUC__) || defined(__clang__)

/**
 * The initial size of the growing buffer that collects the written image data.
 *
 * @see pntr_stb_image_write_func()
 */
#ifndef PNTR_STB_IMAGE_WRITE_INITIAL_CAPACITY
#define PNTR_STB_IMAGE_WRITE_INITIAL_CAPACITY 4096
#endif

/**
 * The state that is carried across the many calls to pntr_stb_image_write_func().
 *
 * @see pntr_stb_image_write_func()
 */
typedef struct pntr_stb_image_context {
    unsigned int dataSize; /** How many bytes have been written to `data` so far. */
    unsigned int capacity; /** How many bytes are currently allocated for `data`. */
    void* data; /** The growing buffer holding the written image data. */
    bool failed; /** Whether an allocation failed, which stops any further writing. */
} pntr_stb_image_context;

/**
 * The stb_image_write callback, appending every written chunk to the context's buffer.
 *
 * @note *stb_image_write* only invokes this once for PNG, but the BMP and JPEG writers
 * stream their output, calling this many times with as little as one byte at a time. The
 * data is therefore accumulated rather than replaced, growing the buffer geometrically.
 *
 * @param context The `pntr_stb_image_context` collecting the data.
 * @param data The chunk of image data that was written.
 * @param size The amount of bytes available in `data`.
 *
 * @see pntr_stb_image_context
 */
void pntr_stb_image_write_func(void *context, void *data, int size) {
    pntr_stb_image_context* ctx = (pntr_stb_image_context*)context;
    if (ctx == NULL || ctx->failed) {
        return;
    }

    // stb_image_write will flush empty buffers, which is nothing to collect.
    if (data == NULL || size <= 0) {
        return;
    }

    // Grow the buffer geometrically, as JPEG can write a single byte at a time.
    size_t needed = (size_t)ctx->dataSize + (size_t)size;
    if (needed > (size_t)ctx->capacity) {
        size_t capacity = (ctx->capacity == 0) ? (size_t)PNTR_STB_IMAGE_WRITE_INITIAL_CAPACITY : (size_t)ctx->capacity;
        while (capacity < needed) {
            capacity *= 2;
        }

        // Copy the stb_image data, because stb_image will free() it afterwards.
        void* newData = PNTR_REALLOC(ctx->data, capacity);
        if (newData == NULL) {
            PNTR_FREE(ctx->data);
            ctx->data = NULL;
            ctx->dataSize = 0;
            ctx->capacity = 0;
            ctx->failed = true;
            pntr_set_error(PNTR_ERROR_NO_MEMORY);
            return;
        }

        ctx->data = newData;
        ctx->capacity = (unsigned int)capacity;
    }

    PNTR_MEMCPY((unsigned char*)ctx->data + ctx->dataSize, data, (size_t)size);
    ctx->dataSize += (unsigned int)size;
}

unsigned char* pntr_stb_image_save_image_to_memory(pntr_image* image, pntr_image_type type, unsigned int* dataSize) {
    if (dataSize != NULL) {
        *dataSize = 0;
    }

    const unsigned char* pixels = (const unsigned char*)pntr_image_to_pixelformat(image, NULL, PNTR_PIXELFORMAT_RGBA8888);
    if (pixels == NULL) {
        return NULL;
    }

    pntr_stb_image_context context;
    context.data = NULL;
    context.dataSize = 0;
    context.capacity = 0;
    context.failed = false;

    switch (type) {
        case PNTR_IMAGE_TYPE_UNKNOWN:
        case PNTR_IMAGE_TYPE_PNG: {
            int stride_bytes = pntr_get_pixel_data_size(image->width, 1, PNTR_PIXELFORMAT_RGBA8888);
            stbi_write_png_to_func(pntr_stb_image_write_func, &context, image->width, image->height, 4, pixels, stride_bytes);
        }
        break;
        case PNTR_IMAGE_TYPE_JPG: {
            stbi_write_jpg_to_func(pntr_stb_image_write_func, &context, image->width, image->height, 4, pixels, 100);
        }
        break;
        case PNTR_IMAGE_TYPE_BMP: {
            stbi_write_bmp_to_func(pntr_stb_image_write_func, &context, image->width, image->height, 4, pixels);
        }
        break;
        default:
            context.failed = true;
            pntr_set_error(PNTR_ERROR_NOT_SUPPORTED);
        break;
    }

    pntr_unload_memory((void*)pixels);

    if (context.failed || context.dataSize == 0) {
        if (context.data != NULL) {
            PNTR_FREE(context.data);
        }

        // An empty result without an allocation failure means nothing was written.
        if (!context.failed) {
            pntr_set_error(PNTR_ERROR_FAILED_TO_WRITE);
        }

        return NULL;
    }

    if (dataSize != NULL) {
        *dataSize = context.dataSize;
    }

    return (unsigned char*)context.data;
}

#ifndef PNTR_SAVE_IMAGE_TO_MEMORY
#define PNTR_SAVE_IMAGE_TO_MEMORY pntr_stb_image_save_image_to_memory
#endif

#endif  // PNTR_STB_IMAGE_WRITE_IMPLEMENTATION
#endif  // PNTR_IMPLEMENTATION
