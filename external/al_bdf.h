/*
# al_bdf

**al_bdf** is a header-only library that renders text using
[Adobe BDF Fonts](https://wwwimages2.adobe.com/content/dam/acom/en/devnet/font/pdfs/5005.BDF_Spec.pdf).

## Instructions

Define `AL_BDF_IMPLEMENTATION` and include this file in one C or C++ file to
create the implementation. If you need to declare the functions somewhere
else, include this file without defining `AL_BDF_IMPLEMENTATION`; the same
configuration used in the implementation must be used.

**al_bdf** needs some user defined configuration to build:

* `AL_BDF_CANVAS_TYPE`: The type of the canvas where text will be rendered to.
  I.e. this would be a `SDL_Surface` when rendering to a SDL surface.
* `AL_BDF_COLOR_TYPE`: The type of the color to use when rendering. I.e. for a
  32 bits per pixel canvas, this would be an `uint32_t`.
* `AL_BDF_PUT_PIXEL`: A macro that will be called to render each individual
  pixel in the canvas. It'll receive the following parameters:
    1. `AL_BDF_CANVAS_TYPE canvas`: Where to render the text to
    1. `int x`: The horizontal coordinate of the pixel to render (0 is left)
    1. `int y`: The vertical coordinate of the pixel to render (0 is top)
    1. `AL_BDF_COLOR_TYPE color`: The color to set the pixel
* `AL_BDF_PUT_PIXELS`: A macro to be used instead of `AL_BDF_PUT_PIXEL`. This
  one will receive a bit pattern for eight horizontal pixels to be drawn
  starting at `x` and `y`. It takes the same parameters, plus `uint8_t pattern`
  with each bit set corresponding to a pixel that must be rendered, from most
  significant bit to less significant.

In addition to the mandatory macros, the following macros can be defined to
further configure **al_bdf**:

* `AL_BDF_MALLOC`: A function that allocates memory, compatible with `malloc`.
* `AL_BDF_REALLOC`: A function that realocates memory, compatible with
  `realloc`.
* `AL_BDF_FREE`: A function that frees memory, compatible with `free`

The memory management macros must either be all defined or not defined. If not
defined, they will default to the `libc` functions.

The API is fully documented in the `al_bdf.h` file.

## TL;DR

```cpp
// al_sfxr config and inclusion
#define AL_BDF_IMPLEMENTATION
#define AL_BDF_CANVAS_TYPE uint16_t*
#define AL_BDF_COLOR_TYPE uint16_t
#define AL_BDF_PUT_PIXEL put_pixel
#include <al_bdf.h>

static int width;

static void put_pixel(uint16_t* const canvas, int const x, int const y, uint16_t color) {
    canvas[y * width + x] = color;
}

static int fpreader(void* const userdata, void* const buffer, size_t const count) {
    FILE* const fp = (FILE*)userdata;
    return fread(buffer, 1, count, fp);
}

static void render(void) {
    // Load a BDF font
    FILE* const fp = fopen("b10.bdf", "rb");

    if (al_bdf_load(font, fpreader, fp) != AL_BDF_OK) {
        // error
    }

    // Evaluate the size of the "al_bdf" text
    int x0, y0, height;
    al_bdf_size(&font, &x0, &y0, &width, &height, "al_bdf");

    // Allocate a canvas big enough to render the text
    size_t const count = width * height;
    uint16_t* const pixels = (uint16_t*)malloc(count * 2);

    // Clear the canvas, as al_bdf_render only sets pixels
    memset(pixels, 0, count * 2);

    // Render the text
    al_bdf_render(&font, "al_bdf", pixels, 0xff00);
}
```

## License

The MIT License (MIT)

Copyright (c) 2020 Andre Leiradella

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef AL_BDF_H
#define AL_BDF_H

#include <stdint.h>

typedef struct {
    int code;
    int dwx0, dwy0;
    int bbw, bbh, bbxoff0x, bbyoff0y, wbytes;

    size_t bits;
}
al_bdf_Char;

typedef struct {
    int num_chars, height, baseline;
    al_bdf_Char const* chars;
    uint8_t const* bits;
}
al_bdf_Font;

typedef enum {
    AL_BDF_OK,
    AL_BDF_READ_ERROR,
    AL_BDF_DIGIT_EXPECTED,
    AL_BDF_IDENTIFIER_EXPECTED,
    AL_BDF_MALFORMED_VERSION,
    AL_BDF_INVALID_VERSION,
    AL_BDF_INVALID_DIRECTION,
    AL_BDF_OUT_OF_MEMORY,
    AL_BDF_CHARACTER_NOT_ENDED,
    AL_BDF_TOO_MANY_CHARACTERS,
    AL_BDF_CHARACTER_NOT_STARTED,
    AL_BDF_XDIGIT_EXPECTED,
}
al_bdf_Result;

/**
 * Reads bytes in an user defined way into the provided buffer. The count
 * parameter must contain a value less than or equal to INT_MAX.
 *
 * @param userdata a pointer to the implementation specific data
 * @param buffer where the bytes will be put
 * @param count number of bytes to read
 *
 * @result the number of bytes read if successful, a negative value on error
 */
typedef int (*al_bdf_Read)(void* const userdata, void* const buffer, size_t const count);

/**
 * Filter to apply when reading a BDF font. For each glyph present in the font,
 * the filter will be called with the userdata passed to the function that
 * loads the BDF font, the Adobe Standard Encoding value, and the non-standard
 * encoding value. The non-standard encoding value is only valid ifthe Adobe
 * Standard Encoding is -1. The filter function must return 0 if the glyph
 * should be loaded, and any other value if it should.
 *
 * @param userdata a pointer to the implementation specific data
 * @param encoding the Adobe Standard Encoding value
 * @param non_standard the non-standard encoding value, only valid if encoding
 *                     is -1
 *
 * @return 0 if the glyph should be loaded, something else otherwise
 */
typedef int (*al_bdf_Filter)(void* const userdata, int const encoding, int const non_standard);

/**
 * Loads a BDF font from a reader. The reader will be called for each byte that
 * this function needs to read, and will receive the userdata that is passed
 * to this function. All glyphs present in the font will be read and will be
 * available for rendering.
 *
 * @param font the BDF font
 * @param reader the function used to read bytes
 * @param userdata the userdata that will be passed to the reader
 *
 * @return 0 if successful, something else on error
 *
 * @see al_bdf_load_filter
 */
al_bdf_Result al_bdf_load(al_bdf_Font* const font, al_bdf_Read const reader, void* const userdata);

/**
 * Loads a BDF font from a reader. The reader will be called for each byte that
 * this function needs to read, and will receive the userdata that is passed
 * to this function.
 *
 * @param font the BDF font
 * @param reader the function used to read bytes
 * @param filter the filter function to limit which glyphs will be loaded
 * @param userdata the userdata that will be passed to the reader
 *
 * @return 0 if successful, something else on error
 *
 * @see al_bdf_load
 */
al_bdf_Result al_bdf_load_filter(al_bdf_Font* const font,
                                 al_bdf_Read const reader,
                                 al_bdf_Filter const filter,
                                 void* const userdata);

/**
 * Unloads a font from memory.
 *
 * @param font the BDF font
 *
 * @see al_bdf_load
 * @see al_bdf_load_filter
 */
void al_bdf_unload(al_bdf_Font* const font);

/**
 * Evaluates the dimensions in pixels of the given text for the given font,
 * which are stored into width and height. x0 and y0 will have the offsets to
 * the pixel which is at the start of the baseline of the first character.
 *
 * @param font the BDF font
 * @param x0 the horizontal coordinate of the baseline for the first character
 * @param y0 the vertical coordinate of the baseline for the first character
 * @param width the horizontal size of the text
 * @param height the vertical size of the text
 * @param text the string with the text to evaluate the size
 */
void al_bdf_size(al_bdf_Font* const font, int* const x0, int* const y0, int* const width, int* const height, char const* text);

/**
 * Draws the text using the given font. AL_BDF_CANVAS_TYPE must be defined with
 * the type of the canvas argument, and AL_BDF_COLOR_TYPE with the type of the
 * color compatible with the canvas. This function will call AL_BDF_PUT_PIXEL
 * to set each individual pixel to draw the text onto the canvas.
 * AL_BDF_PUT_PIXELS can be defined instead, and will receive a bit pattern
 * with eight pixels that can be drawn in one call. Only pixels that are part
 * of the text are drawn, i.e. the canvas won't be cleared or zeroed as part of
 * the rendering.
 *
 * AL_BDF_PUT_PIXEL must have the following signature: (AL_BDF_CANVAS_TYPE, int, int, AL_BDF_COLOR_TYPE).
 * AL_BDF_PUT_PIXELS must have the following signature: (AL_BDF_CANVAS_TYPE, int, int, AL_BDF_COLOR_TYPE, uint8_t).
 *
 * @param font the BDF font
 * @param text the text to be rendered with the given font
 * @param canvas the canvas where the text will be rendered to
 * @param color the color that will be used to render the text
 */
void al_bdf_render(al_bdf_Font* const font, char const* text, AL_BDF_CANVAS_TYPE const canvas, AL_BDF_COLOR_TYPE const color);

#endif /* !AL_BDF_H */

#if defined(AL_BDF_IMPLEMENTATION)

#include <string.h>
#include <setjmp.h>
#include <stdlib.h>

#if defined(AL_BDF_MALLOC) && defined(AL_BDF_REALLOC) && defined(AL_BDF_FREE)
/* Ok */
#elif !defined(AL_BDF_MALLOC) && !defined(AL_BDF_REALLOC) && !defined(AL_BDF_FREE)
#define AL_BDF_MALLOC malloc
#define AL_BDF_REALLOC realloc
#define AL_BDF_FREE free
#else
#error "Must define all or none of AL_BDF_MALLOC, AL_BDF_REALLOC, and AL_BDF_FREE."
#endif

/*
"Lines may be of unlimited length."

"In this version, the new maximum length of a value of the type string is 65535
characters, and hence lines may now be at least this long."

https://wwwimages2.adobe.com/content/dam/acom/en/devnet/font/pdfs/5005.BDF_Spec.pdf

We limit a line at MAXLEN characters at most. If a line is greater than MAXLEN,
it's likely that the font is too big for use with this engine anyway.
*/
#define AL_BDF_MAXLEN 1024

/* Dynamic vector helpers. */
typedef struct {
    void* elements;
    size_t element_size;
    size_t num_elements;
    size_t allocated_elements;
}
al_bdf_Vector;

static void al_bdf_vector_init(al_bdf_Vector* const vector, size_t const element_size) {
    vector->elements = NULL;
    vector->element_size = element_size;
    vector->num_elements = 0;
    vector->allocated_elements = 0;
}

static void al_bdf_vector_destroy(al_bdf_Vector* const vector) {
    AL_BDF_FREE(vector->elements);
    al_bdf_vector_init(vector, vector->element_size);
}

static void* al_bdf_vector_reserve(al_bdf_Vector* const vector, size_t const required_elements) {
    size_t const unused_elements = vector->allocated_elements - vector->num_elements;

    if (required_elements <= unused_elements) {
        size_t const old_num_elements = vector->num_elements;
        vector->num_elements += required_elements;
        return (uint8_t*)vector->elements + old_num_elements * vector->element_size;
    }

    size_t new_allocated = vector->allocated_elements != 0 ? vector->allocated_elements * 2 : 16;

    while (required_elements > new_allocated - vector->num_elements) {
        new_allocated *= 2;
    }

    void* const new_elements = AL_BDF_REALLOC(vector->elements, new_allocated * vector->element_size);

    if (new_elements == NULL) {
        return NULL;
    }

    vector->elements = new_elements;
    vector->allocated_elements = new_allocated;
    size_t const old_num_elements = vector->num_elements;
    vector->num_elements += required_elements;
    return (uint8_t*)vector->elements + old_num_elements * vector->element_size;
}

static int al_bdf_vector_fit(al_bdf_Vector* const vector) {
    void* new_elements = AL_BDF_REALLOC(vector->elements, vector->num_elements * vector->element_size);

    if (new_elements == NULL) {
        return 0;
    }

    vector->elements = new_elements;
    vector->allocated_elements = vector->num_elements;
    return 1;
}

typedef struct {
    char line[AL_BDF_MAXLEN];
    size_t length;
    char const* str;
    al_bdf_Read read;
    void* userdata;
    jmp_buf on_error;
}
al_bdf_Context;

#define AL_BDF_BBX             UINT32_C(0x0b87d481)
#define AL_BDF_ENDFONT         UINT32_C(0x7e1a9db3)
#define AL_BDF_ENCODING        UINT32_C(0x3fb5ea0c)
#define AL_BDF_STARTFONT       UINT32_C(0x8cf73faa)
#define AL_BDF_METRICSSET      UINT32_C(0xaf5b3888)
#define AL_BDF_DWIDTH          UINT32_C(0xaccd5769)
#define AL_BDF_BITMAP          UINT32_C(0xa72bdb22)
#define AL_BDF_ENDCHAR         UINT32_C(0x7e18d91a)
#define AL_BDF_STARTCHAR       UINT32_C(0x8cf57b11)
#define AL_BDF_CHARS           UINT32_C(0x0ce40496)
#define AL_BDF_FONTBOUNDINGBOX UINT32_C(0x3344af7b)

#define AL_BDF_UNICODE_UNKNOWN 0xfffd

#if !defined(AL_BDF_UTF8_DECODE)
/* Based off from PHYSFS. */
static uint32_t AL_BDF_UTF8_DECODE(char const** const utf8) {
    const uint8_t* str = (const uint8_t*)*utf8;
    uint32_t const octet = *str;

    if (octet == 0) {
        /* null terminator, end of string. */
        return 0;
    }
    else if (octet < 0x80) {
        /* one octet char: 0 to 127 */
        (*utf8)++;  /* skip to next possible start of codepoint. */
        return octet;
    }
    else if ((octet >= 0x80) && (octet < 0xc0)) {
        /* bad (starts with 10xxxxxx). */

        /*
        * Apparently each of these is supposed to be flagged as a bogus
        * char, instead of just resyncing to the next valid codepoint.
        */
        (*utf8)++; /* skip to next possible start of codepoint. */
        return AL_BDF_UNICODE_UNKNOWN;
    }
    else if (octet < 0xe0) {
        /* two octets */
        (*utf8)++; /* advance at least one byte in case of an error */
        uint32_t const octet1 = octet - 0xc0;
        uint32_t const octet2 = *++str;

        if ((octet2 & 0xc0) != 0x80) {
            /* Format isn't 10xxxxxx? */
            return AL_BDF_UNICODE_UNKNOWN;
        }

        (*utf8)++; /* skip to next possible start of codepoint. */
        uint32_t const retval = (octet1 << 6) | (octet2 - 0x80);

        if ((retval >= 0x80) && (retval <= 0x7FF)) {
            return retval;
        }
    }
    else if (octet < 0xf0) {
        /* three octets */
        (*utf8)++; /* advance at least one byte in case of an error */
        uint32_t const octet1 = octet - 0xe0;
        uint32_t const octet2 = *++str;
        uint32_t const octet3 = *++str;

        if ((octet2 & 0xc0) != 0x80 || (octet3 & 0xc0) != 0x80) {
            /* Format isn't 10xxxxxx? */
            return AL_BDF_UNICODE_UNKNOWN;
        }

        *utf8 += 2; /* skip to next possible start of codepoint. */
        const uint32_t retval = (octet1 << 12) | ((octet2 - 0x80) << 6) | (octet3 - 0x80);

        /* There are seven "UTF-16 surrogates" that are illegal in UTF-8. */
        switch (retval) {
            case 0xd800U:
            case 0xdb7fU:
            case 0xdb80U:
            case 0xdbffU:
            case 0xdc00U:
            case 0xdf80U:
            case 0xdfffU:
            return AL_BDF_UNICODE_UNKNOWN;
        }

        /* 0xFFFE and 0xFFFF are illegal, too, so we check them at the edge. */
        if (retval >= 0x0800U && retval <= 0xfffdU) {
            return retval;
        }
    }
    else if (octet < 0xf8) {
        /* four octets */
        (*utf8)++; /* advance at least one byte in case of an error */
        uint32_t const octet1 = octet - 0xf0;
        uint32_t const octet2 = *++str;
        uint32_t const octet3 = *++str;
        uint32_t const octet4 = *++str;

        if ((octet2 & 0xc0) != 0x80 || (octet3 & 0xc0) != 0x80 || (octet4 & 0xc0) != 0x80) {
            /* Format isn't 10xxxxxx? */
            return AL_BDF_UNICODE_UNKNOWN;
        }

        *utf8 += 3; /* skip to next possible start of codepoint. */
        uint32_t const retval = (octet1 << 18) | ((octet2 - 0x80) << 12) | ((octet3 - 0x80) << 6) | (octet4 - 0x80);

        if (retval >= 0x10000U && retval <= 0x10ffffU) {
            return retval;
        }
    }
    /*
    * Five and six octet sequences became illegal in rfc3629.
    * We throw the codepoint away, but parse them to make sure we move
    * ahead the right number of bytes and don't overflow the buffer.
    */
    else if (octet < 0xfc) {
        /* five octets */
        (*utf8)++; /* advance at least one byte in case of an error */

        int ok = (*++str & 0xc0) == 0x80;
        ok = ok && (*++str & 0xc0) == 0x80;
        ok = ok && (*++str & 0xc0) == 0x80;
        ok = ok && (*++str & 0xc0) == 0x80;

        if (!ok) {
            /* Format isn't 10xxxxxx? */
            return AL_BDF_UNICODE_UNKNOWN;
        }

        *utf8 += 4;  /* skip to next possible start of codepoint. */
        return AL_BDF_UNICODE_UNKNOWN;
    }
    else {
        /* six octets */
        (*utf8)++; /* advance at least one byte in case of an error */

        int ok = (*++str & 0xc0) == 0x80;
        ok = ok && (*++str & 0xc0) == 0x80;
        ok = ok && (*++str & 0xc0) == 0x80;
        ok = ok && (*++str & 0xc0) == 0x80;
        ok = ok && (*++str & 0xc0) == 0x80;

        if (!ok) {
            /* Format isn't 10xxxxxx? */
            return AL_BDF_UNICODE_UNKNOWN;
        }

        *utf8 += 5; /* skip to next possible start of codepoint. */
        return AL_BDF_UNICODE_UNKNOWN;
    }

    return AL_BDF_UNICODE_UNKNOWN;
}
#endif /* AL_BDF_UTF8_DECODE */

static void al_bdf_skip_spaces(al_bdf_Context* const ctx) {
    for (;;) {
        switch (*ctx->str) {
            case ' ': case '\f': case '\r': case '\t': case '\v':
                ctx->str++;
                continue;

            default:
                return;
        }
    }
}

static uint32_t al_bdf_hash_id(al_bdf_Context* const ctx) {
    uint32_t hash = 0;

    switch (*ctx->str) {
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
        case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
        case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
        case 'V': case 'W': case 'X': case 'Y': case 'Z': case '_':
            hash = 5381 * 33 + (uint8_t)*ctx->str++;
            break;

        default:
            return 0;
    }

    for (;;) {
        switch (*ctx->str) {
            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
            case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
            case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
            case 'V': case 'W': case 'X': case 'Y': case 'Z': case '_':
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                hash = hash * 33 + (uint8_t)*ctx->str++;
                continue;

            default:
                return hash;
        }
    }
}

static void al_bdf_next_line(al_bdf_Context* const ctx) {
    size_t end = 0;

    if (ctx->length != 0) {
        memmove(ctx->line, ctx->line + ctx->length + 1, AL_BDF_MAXLEN - ctx->length - 1);
        end = AL_BDF_MAXLEN - ctx->length - 2;
    }

    int num_read = ctx->read(ctx->userdata, ctx->line + end, AL_BDF_MAXLEN - end - 1);

    if (num_read < 0) {
        longjmp(ctx->on_error, AL_BDF_READ_ERROR);
    }

    ctx->line[num_read + end] = 0;
    char* const newline = strchr(ctx->line, '\n');

    if (newline != NULL) {
        *newline = 0;
        ctx->length = newline - ctx->line;

        if (newline > ctx->line && newline[-1] == '\r') {
            newline[-1] = 0;
        }
    }
    else {
        ctx->length = AL_BDF_MAXLEN - 1;
    }

    ctx->str = ctx->line;
}

/* Parse an optional integer, updating the pointer. Sets found accordingly. */
static int al_bdf_optional_int(al_bdf_Context* ctx, int* found) {
    al_bdf_skip_spaces(ctx);
    int const has_int = *ctx->str != 0;

    if (found != NULL) {
        *found = has_int;
    }

    if (!has_int) {
        return -1;
    }

    /* Adobe's spec doesn't say numbers can be preceeded by '+' but we never know. */
    int const signal = *ctx->str == '-' ? -1 : 1;

    if (*ctx->str == '-' || *ctx->str == '+') {
        ctx->str++;
    }

    /* Skip spaces between signal and first digit. */
    al_bdf_skip_spaces(ctx);

    /* Now we start reading digits. */
    int value = 0;

    switch (*ctx->str) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            value = *ctx->str++ - '0';
            break;

        default:
            longjmp(ctx->on_error, AL_BDF_DIGIT_EXPECTED);
    }

    for (;;) {
        switch (*ctx->str) {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                value = value * 10 + *ctx->str++ - '0';
                break;

            default:
                return signal * value;
        }
    }
}

/* Parse an integer, updating the pointer. */
static int al_bdf_read_int(al_bdf_Context* ctx) {
    int found = 0;
    int const value = al_bdf_optional_int(ctx, &found);

    if (!found) {
        longjmp(ctx->on_error, AL_BDF_DIGIT_EXPECTED);
    }

    return value;
}

static uint8_t al_bdf_read_hex2(al_bdf_Context* const ctx) {
    uint8_t value = 0;

    switch (*ctx->str) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            value = (*ctx->str++ - '0') * 16;
            break;

        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            value = (*ctx->str++ - 'A' + 10) * 16;
            break;

        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            value = (*ctx->str++ - 'a' + 10) * 16;
            break;

        default:
            longjmp(ctx->on_error, AL_BDF_XDIGIT_EXPECTED);
    }

    switch (*ctx->str) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            value |= (*ctx->str++ - '0');
            return value;

        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            value |= (*ctx->str++ - 'A' + 10);
            return value;

        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            value |= (*ctx->str++ - 'a' + 10);
            return value;

        default:
            longjmp(ctx->on_error, AL_BDF_XDIGIT_EXPECTED);
    }
}

/* Compare function to sort characters by their codes. */
static int al_bdf_compare(void const* const e1, void const* const e2) {
    al_bdf_Char const* const c1 = (al_bdf_Char const*)e1;
    al_bdf_Char const* const c2 = (al_bdf_Char const*)e2;

    if (c1->code < c2->code) {
        return -1;
    }
    else if (c1->code > c2->code) {
        return 1;
    }

    return 0;
}

static int al_bdf_pass_all(void* const userdata, int const encoding, int const non_standard) {
    (void)userdata;
    (void)encoding;
    (void)non_standard;

    /* TODO: validate this assumption. */
    return encoding != -1 ? encoding : non_standard;
}

al_bdf_Result al_bdf_load(al_bdf_Font* const font, al_bdf_Read const reader, void* const userdata) {
    return al_bdf_load_filter(font, reader, al_bdf_pass_all, userdata);
}

al_bdf_Result al_bdf_load_filter(al_bdf_Font* const font,
                                 al_bdf_Read const reader,
                                 al_bdf_Filter const filter,
                                 void* const userdata) {
    font->chars = NULL;
    font->num_chars = 0;

    al_bdf_Context ctx = {0};
    ctx.length = 0;
    ctx.read = reader;
    ctx.userdata = userdata;

    int dwx0 = 0, dwy0 = 0;
    int bbw = 0, bbh = 0;
    int bbxoff0x = 0, bbyoff0y = 0;

    al_bdf_Char current_char = {0};
    int in_char = 0;
    int should_add = 0;
    int num_chars = 0;

    al_bdf_Vector chars;
    al_bdf_vector_init(&chars, sizeof(al_bdf_Char));

    al_bdf_Vector bits;
    al_bdf_vector_init(&bits, sizeof(uint8_t));

    int volatile error = 0;

    if ((error = setjmp(ctx.on_error)) != 0) {
        al_bdf_vector_destroy(&bits);
        al_bdf_vector_destroy(&chars);
        al_bdf_unload(font);
        return (al_bdf_Result)error;
    }

    for (;;) {
        al_bdf_next_line(&ctx);
        uint32_t const hash = al_bdf_hash_id(&ctx);

        switch (hash) {
            /* Couldn't find an identifier. */
            case 0:
                longjmp(ctx.on_error, AL_BDF_IDENTIFIER_EXPECTED);

            /* Starts a font. */
            case AL_BDF_STARTFONT: {
                int const major = al_bdf_read_int(&ctx);

                if (*ctx.str++ != '.') {
                    longjmp(ctx.on_error, AL_BDF_MALFORMED_VERSION);
                }

                int const minor = al_bdf_read_int(&ctx);

                /* Issue an error on versions higher than 2.2. */
                if (major > 2 || (major == 2 && minor > 2)) {
                    longjmp(ctx.on_error, AL_BDF_INVALID_VERSION);
                }

                break;
            }

            /* The FONTBOUNDINGBOX values seems to be defaults for BBX values. */
            case AL_BDF_FONTBOUNDINGBOX: {
                bbw = al_bdf_read_int(&ctx);
                bbh = al_bdf_read_int(&ctx);
                bbxoff0x = al_bdf_read_int(&ctx);
                bbyoff0y = al_bdf_read_int(&ctx);

                font->height = bbh;
                font->baseline = bbh + bbyoff0y;
                break;
            }

            case AL_BDF_METRICSSET: {
                int const metrics_set = al_bdf_read_int(&ctx);

                /* We only handle horizontal writing by now. */
                if (metrics_set != 0 ) {
                    longjmp(ctx.on_error, AL_BDF_INVALID_DIRECTION);
                }

                break;
            }

            /* This is the character's width in pixels. */
            case AL_BDF_DWIDTH: {
                int const dwx0_tmp = al_bdf_read_int(&ctx);
                int const dwy0_tmp = al_bdf_read_int(&ctx);

                if (in_char) {
                    current_char.dwx0 = dwx0_tmp;
                    current_char.dwy0 = dwy0_tmp;
                }
                else {
                    dwx0 = dwx0_tmp;
                    dwy0 = dwy0_tmp;
                }

                break;
            }

            case AL_BDF_CHARS: {
                /* Read the number of chars in this font. */
                font->num_chars = al_bdf_read_int(&ctx);
                break;
            }

            case AL_BDF_STARTCHAR: {
                /* If chr is not NULL the last character was not properly ended. */
                if (in_char) {
                    longjmp(ctx.on_error, AL_BDF_CHARACTER_NOT_ENDED);
                }

                /* Bounds check. */
                if (num_chars == font->num_chars) {
                    longjmp(ctx.on_error, AL_BDF_TOO_MANY_CHARACTERS);
                }

                in_char = 1;

                /* Copy default values. */
                current_char.code = -1;
                current_char.dwx0 = dwx0;
                current_char.dwy0 = dwy0;
                /*chr->dwx1 = dwx1;*/
                /*chr->dwy1 = dwy1;*/
                current_char.bbw = bbw;
                current_char.bbh = bbh;
                current_char.bbxoff0x = bbxoff0x;
                current_char.bbyoff0y = bbyoff0y;

                break;
            }

            case AL_BDF_ENCODING: {
                /* If chr is NULL the character was not properly started. */
                if (!in_char) {
                    longjmp(ctx.on_error, AL_BDF_CHARACTER_NOT_STARTED);
                }

                /* Read character's code, it can be -1. */
                int const adobe = al_bdf_read_int(&ctx);

                /* If the encoding is -1, try to read another integer. */
                int const non_standard = adobe == -1 ? al_bdf_optional_int(&ctx, NULL) : -1;

                /* The filter returns the final encoding to use, or -1 if the character shouldn't be added to the font. */
                int const final = filter(userdata, adobe, non_standard);

                if (final != -1) {
                    current_char.code = final;
                    should_add = 1;
                }
                else {
                    should_add = 0;
                }

                break;
            }

            /* The bounding box around the character's black pixels. */
            case AL_BDF_BBX: {
                /* If chr is NULL the character was not properly started. */
                if (!in_char) {
                    longjmp(ctx.on_error, AL_BDF_CHARACTER_NOT_STARTED);
                }

                current_char.bbw = al_bdf_read_int(&ctx);
                current_char.bbh = al_bdf_read_int(&ctx);
                current_char.bbxoff0x = al_bdf_read_int(&ctx);
                current_char.bbyoff0y = al_bdf_read_int(&ctx);

                break;
            }

            /* BITMAP signals the start of the hex data. */
            case AL_BDF_BITMAP: {
                /* If chr is NULL the character was not properly started. */
                if (!in_char) {
                    longjmp(ctx.on_error, AL_BDF_CHARACTER_NOT_STARTED);
                }

                /* Only process the character if it was not filtered out. */
                if (should_add) {
                    /* wbytes is the width of the char in bytes. */
                    current_char.wbytes = (current_char.bbw + 7) / 8;

                    /* Malloc the memory for the pixels. */
                    current_char.bits = bits.num_elements * bits.element_size;
                    uint8_t* pixels = al_bdf_vector_reserve(&bits, current_char.wbytes * current_char.bbh);

                    if (pixels == NULL) {
                        longjmp(ctx.on_error, AL_BDF_OUT_OF_MEMORY);
                    }

                    /* Read all pixels from file. */
                    for (int i = current_char.bbh; i != 0; i--) {
                        al_bdf_next_line(&ctx);

                        for (int j = current_char.wbytes; j != 0; j--) {
                            uint8_t const byte = al_bdf_read_hex2(&ctx);
                            *pixels++ = byte;
                        }
                    }

                    al_bdf_Char* const chr = al_bdf_vector_reserve(&chars, 1);

                    if (chr == NULL) {
                        longjmp(ctx.on_error, AL_BDF_OUT_OF_MEMORY);
                    }

                    *chr = current_char;
                    num_chars++;
                }
                else {
                    /* Skip the bitmap. */
                    for (int i = current_char.bbh; i != 0; i--) {
                        al_bdf_next_line(&ctx);
                    }
                }

                break;
            }

            case AL_BDF_ENDCHAR: {
                /* If chr is NULL the character was not properly started. */
                if (!in_char) {
                    longjmp(ctx.on_error, AL_BDF_CHARACTER_NOT_STARTED);
                }

                in_char = 0;
                break;
            }

            case AL_BDF_ENDFONT: {
                /* If chr is not NULL the last character was not properly ended. */
                if (in_char) {
                    longjmp(ctx.on_error, AL_BDF_CHARACTER_NOT_ENDED);
                }

                /* Adjust the allocated memory to only the necessary. */
                if (!al_bdf_vector_fit(&chars) || !al_bdf_vector_fit(&bits)) {
                    longjmp(ctx.on_error, AL_BDF_OUT_OF_MEMORY);
                }

                font->num_chars = num_chars;
                font->chars = chars.elements;
                font->bits = bits.elements;

                /* Sort font by character codes (TODO: should be an hash table). */
                qsort(chars.elements, num_chars, sizeof(al_bdf_Char), al_bdf_compare);
                return AL_BDF_OK;
            }

            default: {
                /* Unknown section, skip. */
                break;
            }
        }
    }
}

void al_bdf_unload(al_bdf_Font* const font) {
    for (int i = font->num_chars; i != 0; i--) {
        AL_BDF_FREE((void*)font->chars[i].bits);
    }

    AL_BDF_FREE((void*)font->chars);
}

static al_bdf_Char const* al_bdf_find_char(al_bdf_Font* const font, int code) {
    al_bdf_Char key;
    key.code = code;

    return (al_bdf_Char*)bsearch(&key, font->chars, font->num_chars, sizeof(al_bdf_Char), al_bdf_compare);
}

void al_bdf_size(al_bdf_Font* const font,
                 int* const x0,
                 int* const y0,
                 int* const width,
                 int* const height,
                 char const* text) {

    int y = 0, minh = 0, maxh = 0;

    *x0 = *y0 = -1;
    *width = *height = -1;

    for (;;) {
        int const code = AL_BDF_UTF8_DECODE(&text);

        if (code == 0) {
            goto end;
        }

        al_bdf_Char const* const chr = al_bdf_find_char(font, code);

        if (chr != NULL) {
            *x0 = *width = -chr->bbxoff0x;

            int h = y - (chr->bbyoff0y + chr->bbh);
            minh = h;

            h += chr->bbh - 1;
            maxh = h;

            *y0 = chr->bbyoff0y;
            *width += chr->dwx0;
            y += chr->dwy0;
            break;
        }
    }

    for (;;) {
        int const code = AL_BDF_UTF8_DECODE(&text);

        if (code == 0) {
            goto end;
        }

        al_bdf_Char const* const chr = al_bdf_find_char(font, code);

        if (chr != NULL) {
            int h = y - (chr->bbyoff0y + chr->bbh);

            if (h < minh) {
                minh = h;
            }

            h += chr->bbh - 1;

            if (h > maxh) {
                maxh = h;
            }

            if (chr->bbyoff0y < *y0) {
                *y0 = chr->bbyoff0y;
            }

            *width += chr->dwx0;
            y += chr->dwy0;
        }
    }

end:
    *height = maxh - minh + 1;
    *y0 += *height;
}

static void al_bdf_draw_char(al_bdf_Font const* const font,
                             al_bdf_Char const* const chr,
                             AL_BDF_CANVAS_TYPE const canvas,
                             int const x0,
                             int y0,
                             AL_BDF_COLOR_TYPE const color) {

    uint8_t const* bits = font->bits + chr->bits;
    uint8_t const* const endfont = bits + chr->wbytes * chr->bbh;

    for (; bits < endfont; y0++) {
        uint8_t const* const endline = bits + chr->wbytes;

        for (int x = x0; bits < endline; x += 8, bits++) {
            uint8_t const byte = *bits;

#if defined(AL_BDF_PUT_PIXELS)
            AL_BDF_PUT_PIXELS(canvas, x, y0, color, byte);
#else
            if (byte & 0x80) AL_BDF_PUT_PIXEL(canvas, x + 0, y0, color);
            if (byte & 0x40) AL_BDF_PUT_PIXEL(canvas, x + 1, y0, color);
            if (byte & 0x20) AL_BDF_PUT_PIXEL(canvas, x + 2, y0, color);
            if (byte & 0x10) AL_BDF_PUT_PIXEL(canvas, x + 3, y0, color);
            if (byte & 0x08) AL_BDF_PUT_PIXEL(canvas, x + 4, y0, color);
            if (byte & 0x04) AL_BDF_PUT_PIXEL(canvas, x + 5, y0, color);
            if (byte & 0x02) AL_BDF_PUT_PIXEL(canvas, x + 6, y0, color);
            if (byte & 0x01) AL_BDF_PUT_PIXEL(canvas, x + 7, y0, color);
#endif
        }
    }
}

void al_bdf_render(al_bdf_Font* const font, char const* text, AL_BDF_CANVAS_TYPE const canvas, AL_BDF_COLOR_TYPE const color) {

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

            al_bdf_draw_char(font, chr, canvas, dx, dy, color);

            x += chr->dwx0;
            y += chr->dwy0;
        }
    }
}

#endif /* AL_BDF_IMPLEMENTATION */
