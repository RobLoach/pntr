#define UNIT_TEST_PREFIX ""
#define UNIT_STATIC
#include "unit.h"

#define PNTR_ENABLE_DEFAULT_FONT
#define PNTR_ENABLE_TTF
#define PNTR_ENABLE_UTF8

#define PNTR_IMPLEMENTATION
#define PNTR_ASSERT(condition) EQUALS((bool)(condition), true)
#include "../pntr.h"

#define COLOREQUALS PNTR_ASSERT_COLOR_EQUALS
#define IMAGEEQUALS PNTR_ASSERT_IMAGE_EQUALS
#define RECTEQUALS PNTR_ASSERT_RECT_EQUALS
#include "../pntr_assert.h"

/**
 * The 70 characters that are available on resources/font.png, which is a 588x17
 * BMFont atlas holding exactly 70 glyphs.
 */
#define PNTR_TEST_BMF_CHARACTERS " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/"

/**
 * The 95 characters that are available on resources/font-tty-8x8.png, which is a
 * 760x8 atlas holding exactly 95 8x8 glyphs.
 */
#define PNTR_TEST_TTY_CHARACTERS "\x7f !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}"

/**
 * The color that the pixel format tests push through every conversion, chosen so that
 * all four of its channels are different.
 */
#define PNTR_TEST_PIXEL_COLOR pntr_new_color(18, 52, 86, 120)

/**
 * The bytes of PNTR_TEST_PIXEL_COLOR as pntr_color holds them in memory.
 *
 * Unlike a pixel format, pntr_color's own byte order follows the build's
 * PNTR_PIXELFORMAT, which is why storing the struct is never a valid shortcut for
 * pntr_set_pixel_color().
 */
#if defined(PNTR_PIXELFORMAT_ARGB)
    #define PNTR_TEST_PIXEL_COLOR_BYTES { 86, 52, 18, 120 } /* blue, green, red, alpha */
#else
    #define PNTR_TEST_PIXEL_COLOR_BYTES { 18, 52, 86, 120 } /* red, green, blue, alpha */
#endif

bool pntr_utf8() {
    #ifdef PNTR_ENABLE_UTF8
        return true;
    #else
        return false;
    #endif
}

/**
 * An alpha value that is unique to the coordinate it came from.
 *
 * This is what makes an alpha mask misalignment of even a single pixel
 * detectable. A uniform mask would report the same alpha no matter how far the
 * mask was offset by.
 */
unsigned char pntr_test_alpha(int x, int y) {
    return (unsigned char)(((x * 7 + y * 13) % 251) + 1);
}

/**
 * Builds an alpha mask where every pixel carries the alpha of pntr_test_alpha().
 */
pntr_image* pntr_test_alpha_mask(int width, int height) {
    pntr_image* mask = pntr_gen_image_color(width, height, PNTR_WHITE);
    if (mask == NULL) {
        return NULL;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            PNTR_PIXEL(mask, x, y).rgba.a = pntr_test_alpha(x, y);
        }
    }

    return mask;
}

/**
 * The given color, with its alpha replaced by the given alpha.
 */
pntr_color pntr_test_color_alpha(pntr_color color, unsigned char alpha) {
    pntr_color_set_a(&color, alpha);

    return color;
}

/**
 * The name of a file that accepts any write but never has room to store it, or NULL
 * when the platform doesn't provide one.
 *
 * Used to exercise a genuine write failure, as opposed to a failure to open.
 */
const char* pntr_test_unwritable_file() {
    #ifdef __linux__
        return "/dev/full";
    #else
        return NULL;
    #endif
}

/**
 * Whether JPEG images can be loaded back in. Saving them is always available.
 */
bool pntr_jpeg() {
    #if defined(PNTR_ENABLE_JPEG) && !defined(PNTR_NO_LOAD_IMAGE)
        return true;
    #else
        return false;
    #endif
}

/**
 * Whether the selected image backend handles BMP images.
 *
 * cute_png is a PNG-only backend, so it answers every other image type with
 * `PNTR_ERROR_NOT_SUPPORTED` rather than saving or loading it.
 *
 * @see PNTR_CUTE_PNG
 */
bool pntr_bmp() {
    #ifdef PNTR_CUTE_PNG
        return false;
    #else
        return true;
    #endif
}

/**
 * Every value of pntr_error, so that each one can be checked for a message of its own.
 *
 * PNTR_ERROR_NONE is first, as it's the only one that has no message.
 */
const pntr_error pntr_test_errors[] = {
    PNTR_ERROR_NONE,
    PNTR_ERROR_INVALID_ARGS,
    PNTR_ERROR_NO_MEMORY,
    PNTR_ERROR_NOT_SUPPORTED,
    PNTR_ERROR_FAILED_TO_OPEN,
    PNTR_ERROR_FAILED_TO_WRITE,
    PNTR_ERROR_UNKNOWN,
    PNTR_ERROR_FAILED_TO_READ,
    PNTR_ERROR_INVALID_DATA
};

/**
 * How many values pntr_test_errors holds.
 */
#define PNTR_TEST_ERRORS_LEN ((int)(sizeof(pntr_test_errors) / sizeof(pntr_test_errors[0])))

/**
 * Whether two strings hold the same characters.
 *
 * STREQUALS() can only assert that two strings match, which is the opposite of what the
 * distinct error messages need.
 */
bool pntr_test_string_equals(const char* a, const char* b) {
    if (a == NULL || b == NULL) {
        return a == b;
    }

    while (*a != '\0' && *a == *b) {
        a++;
        b++;
    }

    return *a == *b;
}

/**
 * Whether a directory can be handed to `fopen()` on this platform.
 *
 * Where it can, loading a directory fails at the read rather than at the open, which is
 * what makes it report a different error than a file that isn't there at all. Windows
 * refuses the open outright, so both report a failure to open there.
 */
bool pntr_test_directory_opens() {
    FILE* directory = fopen(".", "rb");
    if (directory == NULL) {
        return false;
    }

    fclose(directory);

    return true;
}

/**
 * The color that the drawing bounds tests use as their background.
 *
 * Anything that is not this color counts as painted, which is what lets a test state the
 * exact bounds a primitive is expected to cover.
 */
#define PNTR_TEST_BACKGROUND PNTR_WHITE

/**
 * The exact bounds of everything that has been painted onto the given image.
 *
 * @details Returns the smallest rectangle that holds every pixel which is not the given
 * background color, so asserting it against a rectangle states where a primitive starts
 * and where it stops, in both directions, in a single assertion. An image that has
 * nothing painted on it reports a rectangle of no size at the origin.
 *
 * This is what pins down the bounds convention: a primitive given endpoints covers them
 * both, and one given an extent stops one short of it. Canvases from
 * pntr_test_canvas() pass PNTR_TEST_BACKGROUND; the rotated draws land on their own
 * background, so they pass that instead.
 */
pntr_rectangle pntr_test_painted_bounds(pntr_image* image, pntr_color background) {
    pntr_rectangle bounds = PNTR_CLITERAL(pntr_rectangle) {0, 0, 0, 0};
    if (image == NULL) {
        return bounds;
    }

    int left = image->width;
    int top = image->height;
    int right = -1;
    int bottom = -1;

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            if (pntr_image_get_color(image, x, y).value == background.value) {
                continue;
            }

            if (x < left) {
                left = x;
            }
            if (x > right) {
                right = x;
            }
            if (y < top) {
                top = y;
            }
            if (y > bottom) {
                bottom = y;
            }
        }
    }

    if (right < left || bottom < top) {
        return bounds;
    }

    bounds.x = left;
    bounds.y = top;
    bounds.width = right - left + 1;
    bounds.height = bottom - top + 1;

    return bounds;
}

/**
 * How many pixels of the given PNTR_TEST_BACKGROUND canvas have been painted on.
 *
 * @see pntr_test_painted_bounds()
 */
int pntr_test_painted_count(pntr_image* image) {
    if (image == NULL) {
        return 0;
    }

    int count = 0;
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            if (pntr_image_get_color(image, x, y).value != PNTR_TEST_BACKGROUND.value) {
                count++;
            }
        }
    }

    return count;
}

/**
 * Whether the given pixel of a PNTR_TEST_BACKGROUND canvas has been painted on.
 *
 * @see pntr_test_painted_bounds()
 */
bool pntr_test_painted(pntr_image* image, int x, int y) {
    return pntr_image_get_color(image, x, y).value != PNTR_TEST_BACKGROUND.value;
}

/**
 * A fresh background-colored image for the drawing bounds tests to paint onto.
 */
pntr_image* pntr_test_canvas(int width, int height) {
    return pntr_gen_image_color(width, height, PNTR_TEST_BACKGROUND);
}

/**
 * How many pixels of one image are painted where the other is not.
 *
 * @details Zero means the second image covers everything the first one painted. The two
 * have to be the same size, which is what lets a test compare an outline against its own
 * fill pixel for pixel rather than only comparing their bounds.
 */
int pntr_test_uncovered(pntr_image* painted, pntr_image* cover) {
    if (painted == NULL || cover == NULL || painted->width != cover->width || painted->height != cover->height) {
        return -1;
    }

    int uncovered = 0;
    for (int y = 0; y < painted->height; y++) {
        for (int x = 0; x < painted->width; x++) {
            if (pntr_test_painted(painted, x, y) && !pntr_test_painted(cover, x, y)) {
                uncovered++;
            }
        }
    }

    return uncovered;
}

/**
 * How many pixels of an ellipse's outline the matching fill leaves unpainted.
 *
 * @details Draws pntr_draw_ellipse() and pntr_draw_ellipse_fill() onto their own canvas
 * at the same center and radii, and counts the outline pixels the fill misses, so zero
 * means the fill registers exactly with the outline. Both walk the same midpoint
 * traversal, so sweeping the radii through this is what keeps them in step.
 */
int pntr_test_unfilled_ellipse(int radiusX, int radiusY) {
    int width = radiusX * 2 + 5;
    int height = radiusY * 2 + 5;

    pntr_image* outline = pntr_test_canvas(width, height);
    pntr_image* fill = pntr_test_canvas(width, height);
    if (outline == NULL || fill == NULL) {
        pntr_unload_image(outline);
        pntr_unload_image(fill);
        return -1;
    }

    pntr_draw_ellipse(outline, width / 2, height / 2, radiusX, radiusY, PNTR_RED);
    pntr_draw_ellipse_fill(fill, width / 2, height / 2, radiusX, radiusY, PNTR_RED);

    int uncovered = pntr_test_uncovered(outline, fill);

    pntr_unload_image(outline);
    pntr_unload_image(fill);

    return uncovered;
}

/**
 * How many pixels of a circle's outline the matching fill leaves unpainted.
 *
 * @see pntr_test_unfilled_ellipse()
 */
int pntr_test_unfilled_circle(int radius) {
    int size = radius * 2 + 5;

    pntr_image* outline = pntr_test_canvas(size, size);
    pntr_image* fill = pntr_test_canvas(size, size);
    if (outline == NULL || fill == NULL) {
        pntr_unload_image(outline);
        pntr_unload_image(fill);
        return -1;
    }

    pntr_draw_circle(outline, size / 2, size / 2, radius, PNTR_RED);
    pntr_draw_circle_fill(fill, size / 2, size / 2, radius, PNTR_RED);

    int uncovered = pntr_test_uncovered(outline, fill);

    pntr_unload_image(outline);
    pntr_unload_image(fill);

    return uncovered;
}

/**
 * A blue source image carrying a red marker centered on the given pivot.
 *
 * The rotated drawing paths place the source pixel at their offset onto the destination
 * position they were given, so following this marker is how a test holds a draw to its
 * pivot without having to know which path, bounding box or filter the angle took.
 *
 * The marker is three pixels across so that at least one of its pixels is sure to survive
 * nearest neighbor sampling, whatever angle the draw lands on.
 */
pntr_image* pntr_test_pivot_source(int width, int height, int pivotX, int pivotY) {
    pntr_image* src = pntr_gen_image_color(width, height, PNTR_BLUE);
    if (src == NULL) {
        return NULL;
    }

    pntr_draw_rectangle_fill(src, pivotX - 1, pivotY - 1, 3, 3, PNTR_RED);

    return src;
}

/**
 * How far the marker of pntr_test_pivot_source() landed from the given position.
 *
 * Measured in whole pixels, from the average position of the marker, as the larger of the
 * two axes. A rotation that honours its pivot keeps this within a pixel of zero.
 *
 * @return The distance, or the width of the image when none of the marker was drawn.
 */
int pntr_test_pivot_offset(pntr_image* image, int posX, int posY) {
    long sumX = 0;
    long sumY = 0;
    long count = 0;

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            if (pntr_image_get_color(image, x, y).value == PNTR_RED.value) {
                sumX += x;
                sumY += y;
                count++;
            }
        }
    }

    if (count == 0) {
        return image->width;
    }

    int deltaX = (int)(sumX / count) - posX;
    int deltaY = (int)(sumY / count) - posY;
    if (deltaX < 0) {
        deltaX = -deltaX;
    }
    if (deltaY < 0) {
        deltaY = -deltaY;
    }

    return deltaX > deltaY ? deltaX : deltaY;
}

/**
 * The largest difference, in pixels, between the edges of two rectangles.
 */
int pntr_test_rectangle_distance(pntr_rectangle a, pntr_rectangle b) {
    int deltas[4];
    deltas[0] = a.x - b.x;
    deltas[1] = a.y - b.y;
    deltas[2] = a.x + a.width - (b.x + b.width);
    deltas[3] = a.y + a.height - (b.y + b.height);

    int largest = 0;
    for (int i = 0; i < 4; i++) {
        int delta = deltas[i] < 0 ? -deltas[i] : deltas[i];
        if (delta > largest) {
            largest = delta;
        }
    }

    return largest;
}

/**
 * Where a rotated draw of the given source lands on a destination, at the given pivot.
 *
 * The destination is large enough that nothing is ever clipped away, which is what lets the
 * returned rectangle be compared from one angle to the next. Sweeping the angle and watching
 * it is how the tests hold the rotated drawing paths to a continuous position: a pivot that
 * is rotated along with the image moves the drawn area by at most a pixel between
 * neighbouring angles, while one that isn't jumps the moment an exact quarter turn hands the
 * draw to a different path.
 */
pntr_rectangle pntr_test_rotated_bounds(pntr_image* src, float degrees, float offsetX, float offsetY) {
    pntr_rectangle empty = PNTR_CLITERAL(pntr_rectangle) { .x = 0, .y = 0, .width = 0, .height = 0 };

    pntr_image* dst = pntr_gen_image_color(160, 160, PNTR_GREEN);
    if (dst == NULL) {
        return empty;
    }

    pntr_rectangle srcRect = PNTR_CLITERAL(pntr_rectangle) { .x = 0, .y = 0, .width = src->width, .height = src->height };
    pntr_draw_image_rotated_rec(dst, src, srcRect, 80, 80, degrees, offsetX, offsetY, PNTR_FILTER_NEARESTNEIGHBOR);

    pntr_rectangle bounds = pntr_test_painted_bounds(dst, PNTR_GREEN);
    pntr_unload_image(dst);

    return bounds;
}

/**
 * Where a rotozoomed draw of the given source lands on a destination, at the given pivot and scale.
 *
 * @details The scaled counterpart to pntr_test_rotated_bounds(), and read the same way.
 * The destination is big enough for twice the source at any angle, so the rectangle stays
 * comparable from one angle to the next even when nothing is clipped.
 */
pntr_rectangle pntr_test_rotozoom_bounds(pntr_image* src, float degrees, float originX, float originY, float scale) {
    pntr_rectangle empty = PNTR_CLITERAL(pntr_rectangle) { .x = 0, .y = 0, .width = 0, .height = 0 };

    pntr_image* dst = pntr_gen_image_color(260, 260, PNTR_GREEN);
    if (dst == NULL) {
        return empty;
    }

    pntr_rectangle srcRect = PNTR_CLITERAL(pntr_rectangle) { .x = 0, .y = 0, .width = src->width, .height = src->height };
    pntr_draw_image_rotozoom(dst, src, srcRect, 130, 130, degrees, scale, scale, originX, originY, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);

    pntr_rectangle bounds = pntr_test_painted_bounds(dst, PNTR_GREEN);
    pntr_unload_image(dst);

    return bounds;
}

MODULE(pntr_math, {
    IT("PNTR_SINF", {
        EQUALS((int)PNTR_SINF(PNTR_PI / 2.0f), 1);
    });

    IT("PNTR_COSF", {
        float value = PNTR_COSF(PNTR_PI * 2.0f);
        if (value < 0.9f || value > 1.1f) {
            EQUALS(0, 1);
        }
    });

    IT("PNTR_CEILF", {
        EQUALS((int)PNTR_CEILF(2.4f), 3);
        EQUALS((int)PNTR_CEILF(-2.0f), -2);
        EQUALS((int)PNTR_CEILF(0.0f), 0);
    });

    IT("PNTR_FABS", {
        EQUALS((int)PNTR_FABSF(3.0f), 3);
        EQUALS((int)PNTR_FABSF(-3.0f), 3);
        EQUALS((int)PNTR_FABSF(0.0f), 0);
    });

    IT("PNTR_FLOORF", {
        EQUALS((int)PNTR_FLOORF(2.7f), 2);
        EQUALS((int)PNTR_FLOORF(-2.7f), -3);
        EQUALS((int)PNTR_FLOORF(0.0f), 0);
    });

    IT("PNTR_FMODF", {
        EQUALS((int)PNTR_FMODF(10.0f, 3.0f), 1);
        EQUALS((int)PNTR_FMODF(9.0f, 3.0f), 0);
    });
})

MODULE(pntr, {
    /**
     * Pins the meaning of unit.h's comparison macros.
     *
     * LESSEREQ() and GREATEREQ() are inverted in the upstream unit.h, so each
     * asserted the opposite of its name. Every line here fails against that
     * version.
     *
     * @see unit.h
     */
    IT("unit.h comparison macros", {
        int one = 1;
        int two = 2;

        LESSER(one, two);
        GREATER(two, one);

        // The "or equal" forms have to accept the equal side as well as the unequal one.
        LESSEREQ(one, two);
        LESSEREQ(one, one);
        GREATEREQ(two, one);
        GREATEREQ(one, one);
    });

    IT("pntr_load_memory(), pntr_unload_memory()", {
        void* memory = pntr_load_memory(100);
        NEQUALS(memory, NULL);
        pntr_unload_memory(memory);
    });

    IT("pntr_set_error(), pntr_get_error(), pntr_get_error_code()", {
        pntr_set_error(PNTR_ERROR_NONE);
        EQUALS(pntr_get_error(), NULL);
        pntr_image* image = pntr_new_image(-500, -500);
        EQUALS(image, NULL);
        NEQUALS(pntr_get_error(), NULL);
        STREQUALS(pntr_get_error(), "Invalid arguments");
        EQUALS(pntr_get_error_code(), PNTR_ERROR_INVALID_ARGS);
        pntr_unload_image(image);
        pntr_set_error(PNTR_ERROR_NONE);
        EQUALS(pntr_get_error_code(), PNTR_ERROR_NONE);
    });

    IT("pntr_set_error(PNTR_ERROR_NONE) clears the error state", {
        // Any reported error is forgotten, leaving nothing behind to mislead the caller.
        pntr_set_error(PNTR_ERROR_UNKNOWN);
        NEQUALS(pntr_get_error(), NULL);
        EQUALS(pntr_get_error_code(), PNTR_ERROR_UNKNOWN);

        pntr_set_error(PNTR_ERROR_NONE);
        EQUALS(pntr_get_error(), NULL);
        EQUALS(pntr_get_error_code(), PNTR_ERROR_NONE);

        // Clearing what is already clear changes nothing.
        pntr_set_error(PNTR_ERROR_NONE);
        EQUALS(pntr_get_error(), NULL);
        EQUALS(pntr_get_error_code(), PNTR_ERROR_NONE);
    });

    IT("pntr_get_error(): Every error has a message of its own", {
        const char* messages[PNTR_TEST_ERRORS_LEN];

        for (int i = 0; i < PNTR_TEST_ERRORS_LEN; i++) {
            pntr_set_error(pntr_test_errors[i]);
            EQUALS(pntr_get_error_code(), pntr_test_errors[i]);
            messages[i] = pntr_get_error();
        }

        // PNTR_ERROR_NONE is the absence of an error, so it's the only one without words.
        EQUALS(pntr_test_errors[0], PNTR_ERROR_NONE);
        EQUALS(messages[0], NULL);

        for (int i = 1; i < PNTR_TEST_ERRORS_LEN; i++) {
            NEQUALS(messages[i], NULL);
            EQUALS(pntr_test_string_equals(messages[i], ""), false);

            // Two errors that read the same way can't be told apart by what they say.
            for (int j = 1; j < i; j++) {
                EQUALS(pntr_test_string_equals(messages[i], messages[j]), false);
            }
        }

        pntr_set_error(PNTR_ERROR_NONE);
        EQUALS(pntr_get_error(), NULL);
    });

    IT("pntr_load_file(): A missing file and a directory differ", {
        pntr_set_error(PNTR_ERROR_NONE);

        // Nothing can be opened at a name that isn't there.
        EQUALS(pntr_load_file("FileNotFound.txt", NULL), NULL);
        pntr_error missing = pntr_get_error_code();
        EQUALS(missing, PNTR_ERROR_FAILED_TO_OPEN);
        pntr_set_error(PNTR_ERROR_NONE);

        // A directory can't be loaded either, but for its own reason, and in no case
        // because there wasn't enough memory for it.
        EQUALS(pntr_load_file(".", NULL), NULL);
        pntr_error directory = pntr_get_error_code();
        NEQUALS(directory, PNTR_ERROR_NONE);
        NEQUALS(directory, PNTR_ERROR_NO_MEMORY);

        // Where the directory opens, the read is what fails, and the caller can tell the
        // two apart.
        if (pntr_test_directory_opens()) {
            EQUALS(directory, PNTR_ERROR_FAILED_TO_READ);
            NEQUALS(directory, missing);
        }

        pntr_set_error(PNTR_ERROR_NONE);
    });

    IT("pntr_load_image(): Reports the file's error, not its own", {
        pntr_set_error(PNTR_ERROR_NONE);

        // A missing image file must surface the same error pntr_load_file() reported,
        // rather than a blanket failure to open that hides it.
        EQUALS(pntr_load_image("NotFoundImage.png"), NULL);
        EQUALS(pntr_get_error_code(), PNTR_ERROR_FAILED_TO_OPEN);
        pntr_set_error(PNTR_ERROR_NONE);

        // A directory isn't a missing file, and is never a lack of memory.
        EQUALS(pntr_load_image("."), NULL);
        NEQUALS(pntr_get_error_code(), PNTR_ERROR_NONE);
        NEQUALS(pntr_get_error_code(), PNTR_ERROR_NO_MEMORY);
        if (pntr_test_directory_opens()) {
            EQUALS(pntr_get_error_code(), PNTR_ERROR_FAILED_TO_READ);
        }
        pntr_set_error(PNTR_ERROR_NONE);

        // A file that loads but holds no bytes is about the data, not the file.
        const char* emptyName = "tempFileEmptyImage.png";
        FILE* emptyFile = fopen(emptyName, "wb");
        NEQUALS(emptyFile, NULL);
        fclose(emptyFile);

        EQUALS(pntr_load_image(emptyName), NULL);
        EQUALS(pntr_get_error_code(), PNTR_ERROR_INVALID_DATA);
        pntr_set_error(PNTR_ERROR_NONE);
        remove(emptyName);

        // So is a file full of bytes that aren't an image.
        const char* notAnImage = "this is not an image";
        EQUALS(pntr_load_image_from_memory(PNTR_IMAGE_TYPE_PNG, (const unsigned char*)notAnImage, 20), NULL);
        EQUALS(pntr_get_error_code(), PNTR_ERROR_INVALID_DATA);
        pntr_set_error(PNTR_ERROR_NONE);
    });

    IT("pntr_color_rgba()", {
        pntr_color color = PNTR_RED;
        EQUALS(pntr_color_r(color), 230);
        EQUALS(pntr_color_g(color), 41);
        EQUALS(pntr_color_b(color), 55);
        EQUALS(pntr_color_a(color), 255);
        EQUALS(color.rgba.r, 230);
        EQUALS(color.rgba.g, 41);
        EQUALS(color.rgba.b, 55);
        EQUALS(color.rgba.a, 255);
    });

    IT("pntr_color_set_*()", {
        pntr_color blank = PNTR_BLANK;
        pntr_color_set_r(&blank, 10);
        pntr_color_set_g(&blank, 20);
        pntr_color_set_b(&blank, 30);
        pntr_color_set_a(&blank, 40);
        EQUALS(blank.rgba.r, 10);
        EQUALS(blank.rgba.g, 20);
        EQUALS(blank.rgba.b, 30);
        EQUALS(blank.rgba.a, 40);
    });

    IT("pntr_get_color()", {
        pntr_color color = pntr_get_color(0x052c46ff);
        EQUALS(color.rgba.r, 5);
        EQUALS(color.rgba.g, 44);
        EQUALS(color.rgba.b, 70);
        EQUALS(color.rgba.a, 255);
    });

    IT("pntr_new_color()", {
        pntr_color color = pntr_new_color(100, 120, 130, 200);
        EQUALS(color.rgba.r, 100);
        EQUALS(color.rgba.g, 120);
        EQUALS(color.rgba.b, 130);
        EQUALS(color.rgba.a, 200);
    });

    IT("pntr_gen_image_color(), pntr_image_get_color()", {
        pntr_image* image = pntr_gen_image_color(640, 480, PNTR_SKYBLUE);
        NEQUALS(image, NULL);
        EQUALS(image->width, 640);
        EQUALS(image->height, 480);

        COLOREQUALS(pntr_image_get_color(image, 10, 10), PNTR_SKYBLUE);

        pntr_draw_point(image, 10, 10, PNTR_PURPLE);
        COLOREQUALS(pntr_image_get_color(image, 10, 10), PNTR_PURPLE);

        pntr_unload_image(image);
    });

    IT("pntr_color_bilinear_interpolate()", {
        pntr_color color00 = pntr_new_color(0, 0, 0, 255);
        pntr_color color01 = pntr_new_color(0, 100, 0, 255);
        pntr_color color10 = pntr_new_color(100, 0, 0, 255);
        pntr_color color11 = pntr_new_color(100, 100, 0, 255);

        COLOREQUALS(pntr_color_bilinear_interpolate(color00, color01, color10, color11, 0.0f, 0.0f), color00);
        COLOREQUALS(pntr_color_bilinear_interpolate(color00, color01, color10, color11, 0.0f, 1.0f), color01);
        COLOREQUALS(pntr_color_bilinear_interpolate(color00, color01, color10, color11, 1.0f, 0.0f), color10);
        COLOREQUALS(pntr_color_bilinear_interpolate(color00, color01, color10, color11, 1.0f, 1.0f), color11);
        COLOREQUALS(pntr_color_bilinear_interpolate(color00, color01, color10, color11, 0.5f, 0.5f), pntr_new_color(50, 50, 0, 255));
    });

    IT("pntr_image_get_color_bilinear()", {
        pntr_color topLeft = pntr_new_color(0, 0, 0, 255);
        pntr_color topRight = pntr_new_color(100, 0, 0, 255);
        pntr_color bottomLeft = pntr_new_color(0, 100, 0, 255);
        pntr_color bottomRight = pntr_new_color(100, 100, 0, 255);

        pntr_image* image = pntr_gen_image_color(2, 2, PNTR_BLANK);
        NEQUALS(image, NULL);
        pntr_draw_point(image, 0, 0, topLeft);
        pntr_draw_point(image, 1, 0, topRight);
        pntr_draw_point(image, 0, 1, bottomLeft);
        pntr_draw_point(image, 1, 1, bottomRight);

        IT("pntr_image_get_color_bilinear() on a pixel center", {
            COLOREQUALS(pntr_image_get_color_bilinear(image, 0.0f, 0.0f), topLeft);
            COLOREQUALS(pntr_image_get_color_bilinear(image, 1.0f, 0.0f), topRight);
            COLOREQUALS(pntr_image_get_color_bilinear(image, 0.0f, 1.0f), bottomLeft);
            COLOREQUALS(pntr_image_get_color_bilinear(image, 1.0f, 1.0f), bottomRight);
        });

        IT("pntr_image_get_color_bilinear() between pixels", {
            COLOREQUALS(pntr_image_get_color_bilinear(image, 0.5f, 0.0f), pntr_new_color(50, 0, 0, 255));
            COLOREQUALS(pntr_image_get_color_bilinear(image, 0.0f, 0.5f), pntr_new_color(0, 50, 0, 255));
            COLOREQUALS(pntr_image_get_color_bilinear(image, 0.5f, 0.5f), pntr_new_color(50, 50, 0, 255));
        });

        IT("pntr_image_get_color_bilinear() clamps to the edges", {
            // The last pixel, and anything past it, resolves to the edge pixel.
            COLOREQUALS(pntr_image_get_color_bilinear(image, (float)(image->width - 1), (float)(image->height - 1)), bottomRight);
            COLOREQUALS(pntr_image_get_color_bilinear(image, 1.5f, 1.5f), bottomRight);
            COLOREQUALS(pntr_image_get_color_bilinear(image, 100.0f, 100.0f), bottomRight);
            COLOREQUALS(pntr_image_get_color_bilinear(image, 100.0f, 0.0f), topRight);
            COLOREQUALS(pntr_image_get_color_bilinear(image, 0.0f, 100.0f), bottomLeft);

            // Negative coordinates clamp to the first pixel.
            COLOREQUALS(pntr_image_get_color_bilinear(image, -100.0f, -100.0f), topLeft);
        });

        IT("pntr_image_get_color_bilinear() on a 1x1 image", {
            pntr_image* single = pntr_gen_image_color(1, 1, PNTR_RED);
            NEQUALS(single, NULL);
            COLOREQUALS(pntr_image_get_color_bilinear(single, 0.0f, 0.0f), PNTR_RED);
            COLOREQUALS(pntr_image_get_color_bilinear(single, 0.5f, 0.75f), PNTR_RED);
            COLOREQUALS(pntr_image_get_color_bilinear(single, 50.0f, 50.0f), PNTR_RED);
            COLOREQUALS(pntr_image_get_color_bilinear(single, -50.0f, -50.0f), PNTR_RED);
            pntr_unload_image(single);
        });

        IT("pntr_image_get_color_bilinear() with a NULL image", {
            COLOREQUALS(pntr_image_get_color_bilinear(NULL, 0.0f, 0.0f), PNTR_BLANK);
        });

        pntr_unload_image(image);
    });

    IT("pntr_image_resize(PNTR_FILTER_BILINEAR) of a solid image stays solid", {
        pntr_image* image = pntr_gen_image_color(10, 10, PNTR_BLUE);
        NEQUALS(image, NULL);

        pntr_image* resized = pntr_image_resize(image, 20, 20, PNTR_FILTER_BILINEAR);
        NEQUALS(resized, NULL);
        EQUALS(resized->width, 20);
        EQUALS(resized->height, 20);

        pntr_image* expected = pntr_gen_image_color(20, 20, PNTR_BLUE);
        NEQUALS(expected, NULL);
        IMAGEEQUALS(resized, expected);

        pntr_unload_image(expected);
        pntr_unload_image(resized);
        pntr_unload_image(image);
    });

    IT("pntr_draw_image_scaled_rec(PNTR_FILTER_BILINEAR) does not sample outside the source rectangle", {
        // A 2x2 tile sheet, where only the top left tile is red.
        pntr_image* sheet = pntr_gen_image_color(4, 4, PNTR_RED);
        NEQUALS(sheet, NULL);
        pntr_draw_rectangle_fill(sheet, 2, 0, 2, 4, PNTR_BLUE);
        pntr_draw_rectangle_fill(sheet, 0, 2, 2, 2, PNTR_BLUE);

        pntr_image* dst = pntr_gen_image_color(8, 8, PNTR_BLANK);
        NEQUALS(dst, NULL);
        pntr_draw_image_scaled_rec(dst, sheet,
            PNTR_CLITERAL(pntr_rectangle) { .x = 0, .y = 0, .width = 2, .height = 2 },
            0, 0, 4.0f, 4.0f, 0.0f, 0.0f, PNTR_FILTER_BILINEAR, PNTR_WHITE);

        pntr_image* expected = pntr_gen_image_color(8, 8, PNTR_RED);
        NEQUALS(expected, NULL);
        IMAGEEQUALS(dst, expected);

        pntr_unload_image(expected);
        pntr_unload_image(dst);
        pntr_unload_image(sheet);
    });

    IT("pntr_draw_image_scaled_rec() with a scale that truncates the image away", {
        pntr_image* src = pntr_gen_image_color(8, 8, PNTR_RED);
        NEQUALS(src, NULL);
        pntr_image* dst = pntr_gen_image_color(16, 16, PNTR_BLUE);
        NEQUALS(dst, NULL);
        pntr_image* expected = pntr_gen_image_color(16, 16, PNTR_BLUE);
        NEQUALS(expected, NULL);

        // 8 * 0.1f truncates down to a 0x0 draw, which used to divide by zero while
        // working out the nearest neighbor ratios.
        pntr_draw_image_scaled(dst, src, 0, 0, 0.1f, 0.1f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
        IMAGEEQUALS(dst, expected);

        pntr_draw_image_scaled(dst, src, 0, 0, 0.1f, 0.1f, 0.0f, 0.0f, PNTR_FILTER_BILINEAR, PNTR_WHITE);
        IMAGEEQUALS(dst, expected);

        pntr_draw_image_scaled_rec(dst, src,
            PNTR_CLITERAL(pntr_rectangle) { .x = 0, .y = 0, .width = 8, .height = 8 },
            0, 0, 0.01f, 0.01f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
        IMAGEEQUALS(dst, expected);

        pntr_unload_image(expected);
        pntr_unload_image(dst);
        pntr_unload_image(src);
    });

    IT("pntr_draw_image_dest_rec()", {
        IT("pntr_draw_image_dest_rec() with a 1:1 destination rectangle copies the source region", {
            // An 8x8 source where every pixel carries a unique color.
            pntr_image* src = pntr_gen_image_color(8, 8, PNTR_BLANK);
            NEQUALS(src, NULL);
            for (int y = 0; y < 8; y++) {
                for (int x = 0; x < 8; x++) {
                    pntr_draw_point(src, x, y, pntr_new_color(
                        (unsigned char)(x * 32 + 7),
                        (unsigned char)(y * 32 + 3),
                        (unsigned char)(x * y + 1),
                        255));
                }
            }

            // The source rectangle has x != y, and width != height, to catch mixing them up.
            pntr_rectangle srcRect = {2, 1, 4, 3};
            pntr_rectangle dstRect = {0, 0, 4, 3};

            // The same region, drawn by the plain 1:1 blit.
            pntr_image* expected = pntr_gen_image_color(4, 3, PNTR_BLANK);
            NEQUALS(expected, NULL);
            pntr_draw_image_rec(expected, src, srcRect, 0, 0);

            IT("with the nearest neighbor filter", {
                pntr_image* dst = pntr_gen_image_color(4, 3, PNTR_BLANK);
                NEQUALS(dst, NULL);
                pntr_draw_image_dest_rec(dst, src, srcRect, dstRect, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
                IMAGEEQUALS(dst, expected);
                pntr_unload_image(dst);
            });

            IT("with the bilinear filter", {
                pntr_image* dst = pntr_gen_image_color(4, 3, PNTR_BLANK);
                NEQUALS(dst, NULL);
                pntr_draw_image_dest_rec(dst, src, srcRect, dstRect, PNTR_FILTER_BILINEAR, PNTR_WHITE);
                IMAGEEQUALS(dst, expected);
                pntr_unload_image(dst);
            });

            pntr_unload_image(expected);
            pntr_unload_image(src);
        });

        IT("pntr_draw_image_dest_rec() upscales to the destination rectangle", {
            // A 4x4 source split into four 2x2 quadrants.
            pntr_image* src = pntr_gen_image_color(4, 4, PNTR_RED);
            NEQUALS(src, NULL);
            pntr_draw_rectangle_fill(src, 2, 0, 2, 2, PNTR_BLUE);
            pntr_draw_rectangle_fill(src, 0, 2, 2, 2, PNTR_GREEN);
            pntr_draw_rectangle_fill(src, 2, 2, 2, 2, PNTR_YELLOW);

            pntr_rectangle srcRect = {0, 0, 4, 4};

            // Drawn at an offset, so each source pixel becomes a 2x2 block from (3, 5) to (10, 12).
            pntr_rectangle dstRect = {3, 5, 8, 8};

            pntr_image* expected = pntr_gen_image_color(16, 16, PNTR_MAGENTA);
            NEQUALS(expected, NULL);
            pntr_draw_rectangle_fill(expected, 3, 5, 4, 4, PNTR_RED);
            pntr_draw_rectangle_fill(expected, 7, 5, 4, 4, PNTR_BLUE);
            pntr_draw_rectangle_fill(expected, 3, 9, 4, 4, PNTR_GREEN);
            pntr_draw_rectangle_fill(expected, 7, 9, 4, 4, PNTR_YELLOW);

            IT("with the nearest neighbor filter", {
                pntr_image* dst = pntr_gen_image_color(16, 16, PNTR_MAGENTA);
                NEQUALS(dst, NULL);
                pntr_draw_image_dest_rec(dst, src, srcRect, dstRect, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);

                // Nothing outside of the destination rectangle was touched.
                IMAGEEQUALS(dst, expected);
                pntr_unload_image(dst);
            });

            IT("with the bilinear filter", {
                pntr_image* dst = pntr_gen_image_color(16, 16, PNTR_MAGENTA);
                NEQUALS(dst, NULL);
                pntr_draw_image_dest_rec(dst, src, srcRect, dstRect, PNTR_FILTER_BILINEAR, PNTR_WHITE);

                // The bilinear filter blends the middle, but the corners of the
                // destination rectangle still land on the corners of the source.
                COLOREQUALS(pntr_image_get_color(dst, 3, 5), PNTR_RED);
                COLOREQUALS(pntr_image_get_color(dst, 10, 5), PNTR_BLUE);
                COLOREQUALS(pntr_image_get_color(dst, 3, 12), PNTR_GREEN);
                COLOREQUALS(pntr_image_get_color(dst, 10, 12), PNTR_YELLOW);

                // Everything just outside of the destination rectangle is untouched.
                COLOREQUALS(pntr_image_get_color(dst, 2, 5), PNTR_MAGENTA);
                COLOREQUALS(pntr_image_get_color(dst, 11, 5), PNTR_MAGENTA);
                COLOREQUALS(pntr_image_get_color(dst, 3, 4), PNTR_MAGENTA);
                COLOREQUALS(pntr_image_get_color(dst, 3, 13), PNTR_MAGENTA);
                pntr_unload_image(dst);
            });

            pntr_unload_image(expected);
            pntr_unload_image(src);
        });

        IT("pntr_draw_image_dest_rec() downscales to the destination rectangle", {
            // An 8x8 source, red on the left half and blue on the right half.
            pntr_image* src = pntr_gen_image_color(8, 8, PNTR_RED);
            NEQUALS(src, NULL);
            pntr_draw_rectangle_fill(src, 4, 0, 4, 8, PNTR_BLUE);

            pntr_rectangle srcRect = {0, 0, 8, 8};
            pntr_rectangle dstRect = {0, 0, 2, 2};

            pntr_image* expected = pntr_gen_image_color(4, 4, PNTR_MAGENTA);
            NEQUALS(expected, NULL);
            pntr_draw_rectangle_fill(expected, 0, 0, 1, 2, PNTR_RED);
            pntr_draw_rectangle_fill(expected, 1, 0, 1, 2, PNTR_BLUE);

            IT("with the nearest neighbor filter", {
                pntr_image* dst = pntr_gen_image_color(4, 4, PNTR_MAGENTA);
                NEQUALS(dst, NULL);
                pntr_draw_image_dest_rec(dst, src, srcRect, dstRect, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
                IMAGEEQUALS(dst, expected);
                pntr_unload_image(dst);
            });

            IT("with the bilinear filter", {
                pntr_image* dst = pntr_gen_image_color(4, 4, PNTR_MAGENTA);
                NEQUALS(dst, NULL);
                pntr_draw_image_dest_rec(dst, src, srcRect, dstRect, PNTR_FILTER_BILINEAR, PNTR_WHITE);
                IMAGEEQUALS(dst, expected);
                pntr_unload_image(dst);
            });

            IT("down to a single pixel", {
                pntr_image* dst = pntr_gen_image_color(4, 4, PNTR_MAGENTA);
                NEQUALS(dst, NULL);
                pntr_draw_image_dest_rec(dst, src, srcRect,
                    PNTR_CLITERAL(pntr_rectangle) { .x = 1, .y = 2, .width = 1, .height = 1 },
                    PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);

                pntr_image* single = pntr_gen_image_color(4, 4, PNTR_MAGENTA);
                NEQUALS(single, NULL);
                pntr_draw_point(single, 1, 2, PNTR_RED);
                IMAGEEQUALS(dst, single);

                pntr_unload_image(single);
                pntr_unload_image(dst);
            });

            pntr_unload_image(expected);
            pntr_unload_image(src);
        });

        IT("pntr_draw_image_dest_rec() scales each axis independently", {
            // A 2x2 source, with a different color in every pixel.
            pntr_image* src = pntr_gen_image_color(2, 2, PNTR_RED);
            NEQUALS(src, NULL);
            pntr_draw_point(src, 1, 0, PNTR_BLUE);
            pntr_draw_point(src, 0, 1, PNTR_GREEN);
            pntr_draw_point(src, 1, 1, PNTR_YELLOW);

            // 3x along the x axis, 1x along the y axis.
            pntr_rectangle srcRect = {0, 0, 2, 2};
            pntr_rectangle dstRect = {0, 0, 6, 2};

            pntr_image* expected = pntr_gen_image_color(8, 4, PNTR_MAGENTA);
            NEQUALS(expected, NULL);
            pntr_draw_rectangle_fill(expected, 0, 0, 3, 1, PNTR_RED);
            pntr_draw_rectangle_fill(expected, 3, 0, 3, 1, PNTR_BLUE);
            pntr_draw_rectangle_fill(expected, 0, 1, 3, 1, PNTR_GREEN);
            pntr_draw_rectangle_fill(expected, 3, 1, 3, 1, PNTR_YELLOW);

            pntr_image* dst = pntr_gen_image_color(8, 4, PNTR_MAGENTA);
            NEQUALS(dst, NULL);
            pntr_draw_image_dest_rec(dst, src, srcRect, dstRect, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
            IMAGEEQUALS(dst, expected);

            pntr_unload_image(dst);
            pntr_unload_image(expected);
            pntr_unload_image(src);
        });

        IT("pntr_draw_image_dest_rec() does not bleed in neighbouring tiles", {
            // A 6x6 sheet of 2x2 tiles, where only the middle tile is red.
            pntr_image* sheet = pntr_gen_image_color(6, 6, PNTR_BLUE);
            NEQUALS(sheet, NULL);
            pntr_draw_rectangle_fill(sheet, 2, 2, 2, 2, PNTR_RED);

            pntr_rectangle srcRect = {2, 2, 2, 2};
            pntr_rectangle dstRect = {0, 0, 8, 8};

            pntr_image* expected = pntr_gen_image_color(8, 8, PNTR_RED);
            NEQUALS(expected, NULL);

            IT("with the nearest neighbor filter", {
                pntr_image* dst = pntr_gen_image_color(8, 8, PNTR_BLANK);
                NEQUALS(dst, NULL);
                pntr_draw_image_dest_rec(dst, sheet, srcRect, dstRect, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
                IMAGEEQUALS(dst, expected);
                pntr_unload_image(dst);
            });

            IT("with the bilinear filter", {
                pntr_image* dst = pntr_gen_image_color(8, 8, PNTR_BLANK);
                NEQUALS(dst, NULL);
                pntr_draw_image_dest_rec(dst, sheet, srcRect, dstRect, PNTR_FILTER_BILINEAR, PNTR_WHITE);
                IMAGEEQUALS(dst, expected);
                pntr_unload_image(dst);
            });

            pntr_unload_image(expected);
            pntr_unload_image(sheet);
        });

        IT("pntr_draw_image_dest_rec() applies the tint", {
            pntr_image* src = pntr_gen_image_color(2, 2, PNTR_WHITE);
            NEQUALS(src, NULL);

            pntr_rectangle srcRect = {0, 0, 2, 2};
            pntr_rectangle dstRect = {0, 0, 4, 4};
            pntr_color tint = pntr_new_color(255, 0, 0, 255);

            pntr_image* untinted = pntr_gen_image_color(4, 4, PNTR_WHITE);
            NEQUALS(untinted, NULL);
            pntr_image* tinted = pntr_gen_image_color(4, 4, tint);
            NEQUALS(tinted, NULL);

            IT("with the nearest neighbor filter", {
                pntr_image* dst = pntr_gen_image_color(4, 4, PNTR_BLANK);
                NEQUALS(dst, NULL);
                pntr_draw_image_dest_rec(dst, src, srcRect, dstRect, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
                IMAGEEQUALS(dst, untinted);

                pntr_draw_image_dest_rec(dst, src, srcRect, dstRect, PNTR_FILTER_NEARESTNEIGHBOR, tint);
                IMAGEEQUALS(dst, tinted);
                pntr_unload_image(dst);
            });

            IT("with the bilinear filter", {
                pntr_image* dst = pntr_gen_image_color(4, 4, PNTR_BLANK);
                NEQUALS(dst, NULL);
                pntr_draw_image_dest_rec(dst, src, srcRect, dstRect, PNTR_FILTER_BILINEAR, PNTR_WHITE);
                IMAGEEQUALS(dst, untinted);

                pntr_draw_image_dest_rec(dst, src, srcRect, dstRect, PNTR_FILTER_BILINEAR, tint);
                IMAGEEQUALS(dst, tinted);
                pntr_unload_image(dst);
            });

            pntr_unload_image(tinted);
            pntr_unload_image(untinted);
            pntr_unload_image(src);
        });

        IT("pntr_draw_image_dest_rec() clips the destination rectangle", {
            pntr_image* src = pntr_gen_image_color(4, 4, PNTR_RED);
            NEQUALS(src, NULL);
            pntr_rectangle srcRect = {0, 0, 4, 4};

            IT("partially outside of the destination image", {
                pntr_image* dst = pntr_gen_image_color(8, 8, PNTR_BLUE);
                NEQUALS(dst, NULL);

                // The top left half of the destination rectangle is off of the image.
                pntr_draw_image_dest_rec(dst, src, srcRect,
                    PNTR_CLITERAL(pntr_rectangle) { .x = -2, .y = -2, .width = 4, .height = 4 },
                    PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);

                pntr_image* expected = pntr_gen_image_color(8, 8, PNTR_BLUE);
                NEQUALS(expected, NULL);
                pntr_draw_rectangle_fill(expected, 0, 0, 2, 2, PNTR_RED);
                IMAGEEQUALS(dst, expected);

                // The bottom right half of the destination rectangle is off of the image.
                pntr_draw_image_dest_rec(dst, src, srcRect,
                    PNTR_CLITERAL(pntr_rectangle) { .x = 6, .y = 6, .width = 4, .height = 4 },
                    PNTR_FILTER_BILINEAR, PNTR_WHITE);
                pntr_draw_rectangle_fill(expected, 6, 6, 2, 2, PNTR_RED);
                IMAGEEQUALS(dst, expected);

                pntr_unload_image(expected);
                pntr_unload_image(dst);
            });

            IT("entirely outside of the destination image", {
                pntr_image* dst = pntr_gen_image_color(8, 8, PNTR_BLUE);
                NEQUALS(dst, NULL);
                pntr_image* expected = pntr_gen_image_color(8, 8, PNTR_BLUE);
                NEQUALS(expected, NULL);

                pntr_draw_image_dest_rec(dst, src, srcRect,
                    PNTR_CLITERAL(pntr_rectangle) { .x = -10, .y = -10, .width = 4, .height = 4 },
                    PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
                IMAGEEQUALS(dst, expected);

                pntr_draw_image_dest_rec(dst, src, srcRect,
                    PNTR_CLITERAL(pntr_rectangle) { .x = 20, .y = 20, .width = 4, .height = 4 },
                    PNTR_FILTER_BILINEAR, PNTR_WHITE);
                IMAGEEQUALS(dst, expected);

                pntr_unload_image(expected);
                pntr_unload_image(dst);
            });

            IT("against the clip rectangle of the destination", {
                pntr_image* dst = pntr_gen_image_color(8, 8, PNTR_BLUE);
                NEQUALS(dst, NULL);
                pntr_image_set_clip(dst, 2, 2, 4, 4);
                pntr_rectangle clip = {2, 2, 4, 4};
                RECTEQUALS(pntr_image_get_clip(dst), clip);

                pntr_draw_image_dest_rec(dst, src, srcRect,
                    PNTR_CLITERAL(pntr_rectangle) { .x = 0, .y = 0, .width = 8, .height = 8 },
                    PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
                pntr_image_reset_clip(dst);

                pntr_image* expected = pntr_gen_image_color(8, 8, PNTR_BLUE);
                NEQUALS(expected, NULL);
                pntr_draw_rectangle_fill(expected, 2, 2, 4, 4, PNTR_RED);
                IMAGEEQUALS(dst, expected);

                pntr_unload_image(expected);
                pntr_unload_image(dst);
            });

            pntr_unload_image(src);
        });

        IT("pntr_draw_image_dest_rec() ignores invalid arguments", {
            pntr_image* src = pntr_gen_image_color(4, 4, PNTR_RED);
            NEQUALS(src, NULL);
            pntr_image* dst = pntr_gen_image_color(8, 8, PNTR_BLUE);
            NEQUALS(dst, NULL);
            pntr_image* expected = pntr_gen_image_color(8, 8, PNTR_BLUE);
            NEQUALS(expected, NULL);

            pntr_rectangle srcRect = {0, 0, 4, 4};
            pntr_rectangle dstRect = {0, 0, 8, 8};

            // NULL images do not crash, and draw nothing.
            pntr_draw_image_dest_rec(NULL, src, srcRect, dstRect, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
            pntr_draw_image_dest_rec(dst, NULL, srcRect, dstRect, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
            pntr_draw_image_dest_rec(NULL, NULL, srcRect, dstRect, PNTR_FILTER_BILINEAR, PNTR_WHITE);
            IMAGEEQUALS(dst, expected);

            // A destination rectangle without a size draws nothing.
            pntr_draw_image_dest_rec(dst, src, srcRect, PNTR_CLITERAL(pntr_rectangle) { .x = 0, .y = 0, .width = 0, .height = 8 }, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
            pntr_draw_image_dest_rec(dst, src, srcRect, PNTR_CLITERAL(pntr_rectangle) { .x = 0, .y = 0, .width = 8, .height = 0 }, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
            pntr_draw_image_dest_rec(dst, src, srcRect, PNTR_CLITERAL(pntr_rectangle) { .x = 0, .y = 0, .width = -8, .height = 8 }, PNTR_FILTER_BILINEAR, PNTR_WHITE);
            pntr_draw_image_dest_rec(dst, src, srcRect, PNTR_CLITERAL(pntr_rectangle) { .x = 0, .y = 0, .width = 8, .height = -8 }, PNTR_FILTER_BILINEAR, PNTR_WHITE);
            IMAGEEQUALS(dst, expected);

            // A source rectangle without a size draws nothing.
            pntr_draw_image_dest_rec(dst, src, PNTR_CLITERAL(pntr_rectangle) { .x = 0, .y = 0, .width = 0, .height = 4 }, dstRect, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
            pntr_draw_image_dest_rec(dst, src, PNTR_CLITERAL(pntr_rectangle) { .x = 0, .y = 0, .width = 4, .height = 0 }, dstRect, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
            pntr_draw_image_dest_rec(dst, src, PNTR_CLITERAL(pntr_rectangle) { .x = 0, .y = 0, .width = -4, .height = 4 }, dstRect, PNTR_FILTER_BILINEAR, PNTR_WHITE);
            pntr_draw_image_dest_rec(dst, src, PNTR_CLITERAL(pntr_rectangle) { .x = 0, .y = 0, .width = 4, .height = -4 }, dstRect, PNTR_FILTER_BILINEAR, PNTR_WHITE);
            IMAGEEQUALS(dst, expected);

            // A source rectangle that is entirely off of the source image draws nothing.
            pntr_draw_image_dest_rec(dst, src, PNTR_CLITERAL(pntr_rectangle) { .x = 10, .y = 10, .width = 4, .height = 4 }, dstRect, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
            pntr_draw_image_dest_rec(dst, src, PNTR_CLITERAL(pntr_rectangle) { .x = -8, .y = -8, .width = 4, .height = 4 }, dstRect, PNTR_FILTER_BILINEAR, PNTR_WHITE);
            IMAGEEQUALS(dst, expected);

            pntr_unload_image(expected);
            pntr_unload_image(dst);
            pntr_unload_image(src);
        });
    });

    IT("pntr_image_rotate(PNTR_FILTER_BILINEAR) draws a single pixel source", {
        pntr_image* image = pntr_gen_image_color(1, 1, PNTR_RED);
        NEQUALS(image, NULL);

        pntr_image* rotated = pntr_image_rotate(image, 45.0f, PNTR_FILTER_BILINEAR);
        NEQUALS(rotated, NULL);

        // Count how much of the source actually made it onto the rotated image.
        int drawn = 0;
        for (int y = 0; y < rotated->height; y++) {
            for (int x = 0; x < rotated->width; x++) {
                if (pntr_image_get_color(rotated, x, y).value == PNTR_RED.value) {
                    drawn++;
                }
            }
        }
        GREATER(drawn, 0);

        pntr_unload_image(rotated);
        pntr_unload_image(image);
    });

    IT("pntr_clear_background(), pntr_draw_rectangle_fill()", {
        pntr_image* image = pntr_new_image(100, 100);
        NEQUALS(image, NULL);
        pntr_clear_background(image, PNTR_RED);

        COLOREQUALS(pntr_image_get_color(image, 10, 10), PNTR_RED);

        pntr_clear_background(image, PNTR_BLANK);
        COLOREQUALS(pntr_image_get_color(image, 10, 10), PNTR_BLANK);

        pntr_draw_rectangle_fill(image, 9, 9, 3, 3, PNTR_BLUE);
        COLOREQUALS(pntr_image_get_color(image, 10, 10), PNTR_BLUE);
        pntr_unload_image(image);
    });

    IT("pntr_draw_point()", {
        pntr_image* image = pntr_gen_image_color(50, 50, PNTR_WHITE);
        COLOREQUALS(pntr_image_get_color(image, 10, 10), PNTR_WHITE);
        COLOREQUALS(pntr_image_get_color(image, 10, 9), PNTR_WHITE);
        pntr_draw_point(image, 10, 10, PNTR_RED);
        COLOREQUALS(pntr_image_get_color(image, 10, 10), PNTR_RED);
        COLOREQUALS(pntr_image_get_color(image, 10, 9), PNTR_WHITE);
        pntr_unload_image(image);
    });

    IT("pntr_draw_points()", {
        pntr_image* image = pntr_gen_image_color(50, 50, PNTR_WHITE);
        COLOREQUALS(pntr_image_get_color(image, 10, 10), PNTR_WHITE);
        COLOREQUALS(pntr_image_get_color(image, 15, 30), PNTR_WHITE);
        COLOREQUALS(pntr_image_get_color(image, 40, 40), PNTR_WHITE);
        COLOREQUALS(pntr_image_get_color(image, 0, 5), PNTR_WHITE);
        COLOREQUALS(pntr_image_get_color(image, 0, 4), PNTR_WHITE);
        pntr_vector points[10];
        points[0] = PNTR_CLITERAL(pntr_vector) {10, 10};
        points[1] = PNTR_CLITERAL(pntr_vector) {15, 30};
        points[2] = PNTR_CLITERAL(pntr_vector) {40, 40};
        points[3] = PNTR_CLITERAL(pntr_vector) {0, 5};
        pntr_draw_points(image, points, 4, PNTR_RED);
        COLOREQUALS(pntr_image_get_color(image, 10, 10), PNTR_RED);
        COLOREQUALS(pntr_image_get_color(image, 15, 30), PNTR_RED);
        COLOREQUALS(pntr_image_get_color(image, 40, 40), PNTR_RED);
        COLOREQUALS(pntr_image_get_color(image, 0, 5), PNTR_RED);
        COLOREQUALS(pntr_image_get_color(image, 0, 4), PNTR_WHITE);
        pntr_unload_image(image);
    });

    IT("primitive bounds", {
        // Every test here asserts the exact rectangle that a primitive paints, which is
        // what states the bounds convention: a primitive given endpoints covers both of
        // them, and one given a width and a height stops one pixel short of them.

        IT("pntr_draw_line() covers both endpoints", {
            pntr_rectangle bounds;

            IT("pntr_draw_line() horizontally", {
                pntr_image* image = pntr_test_canvas(40, 40);
                NEQUALS(image, NULL);

                pntr_draw_line(image, 5, 20, 15, 20, PNTR_RED);
                bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
                pntr_rectangle expected = PNTR_CLITERAL(pntr_rectangle) {5, 20, 11, 1};
                RECTEQUALS(bounds, expected);

                // Both endpoints, and nothing past either of them.
                EQUALS(pntr_test_painted(image, 5, 20), true);
                EQUALS(pntr_test_painted(image, 15, 20), true);
                EQUALS(pntr_test_painted(image, 4, 20), false);
                EQUALS(pntr_test_painted(image, 16, 20), false);
                EQUALS(pntr_test_painted_count(image), 11);

                pntr_unload_image(image);
            });

            IT("pntr_draw_line() horizontally, from right to left", {
                pntr_image* image = pntr_test_canvas(40, 40);
                NEQUALS(image, NULL);

                // Reversing the endpoints paints the very same pixels.
                pntr_draw_line(image, 15, 20, 5, 20, PNTR_RED);
                bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
                pntr_rectangle expected = PNTR_CLITERAL(pntr_rectangle) {5, 20, 11, 1};
                RECTEQUALS(bounds, expected);
                EQUALS(pntr_test_painted_count(image), 11);

                pntr_unload_image(image);
            });

            IT("pntr_draw_line() vertically", {
                pntr_image* image = pntr_test_canvas(40, 40);
                NEQUALS(image, NULL);

                pntr_draw_line(image, 20, 5, 20, 15, PNTR_RED);
                bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
                pntr_rectangle expected = PNTR_CLITERAL(pntr_rectangle) {20, 5, 1, 11};
                RECTEQUALS(bounds, expected);

                EQUALS(pntr_test_painted(image, 20, 5), true);
                EQUALS(pntr_test_painted(image, 20, 15), true);
                EQUALS(pntr_test_painted(image, 20, 4), false);
                EQUALS(pntr_test_painted(image, 20, 16), false);
                EQUALS(pntr_test_painted_count(image), 11);

                pntr_unload_image(image);
            });

            IT("pntr_draw_line() vertically, from bottom to top", {
                pntr_image* image = pntr_test_canvas(40, 40);
                NEQUALS(image, NULL);

                pntr_draw_line(image, 20, 15, 20, 5, PNTR_RED);
                bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
                pntr_rectangle expected = PNTR_CLITERAL(pntr_rectangle) {20, 5, 1, 11};
                RECTEQUALS(bounds, expected);
                EQUALS(pntr_test_painted_count(image), 11);

                pntr_unload_image(image);
            });

            IT("pntr_draw_line() diagonally", {
                pntr_image* image = pntr_test_canvas(40, 40);
                NEQUALS(image, NULL);

                pntr_draw_line(image, 5, 5, 15, 15, PNTR_RED);
                bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
                pntr_rectangle expected = PNTR_CLITERAL(pntr_rectangle) {5, 5, 11, 11};
                RECTEQUALS(bounds, expected);

                EQUALS(pntr_test_painted(image, 5, 5), true);
                EQUALS(pntr_test_painted(image, 15, 15), true);
                EQUALS(pntr_test_painted(image, 4, 4), false);
                EQUALS(pntr_test_painted(image, 16, 16), false);
                EQUALS(pntr_test_painted_count(image), 11);

                pntr_unload_image(image);
            });

            IT("pntr_draw_line() on a shallow diagonal", {
                pntr_image* image = pntr_test_canvas(40, 40);
                NEQUALS(image, NULL);

                pntr_draw_line(image, 5, 10, 25, 14, PNTR_RED);
                bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
                pntr_rectangle expected = PNTR_CLITERAL(pntr_rectangle) {5, 10, 21, 5};
                RECTEQUALS(bounds, expected);

                pntr_unload_image(image);
            });

            IT("pntr_draw_line() with both endpoints on the same pixel", {
                pntr_image* image = pntr_test_canvas(40, 40);
                NEQUALS(image, NULL);

                // A line of no length still covers the one pixel it is given.
                pntr_draw_line(image, 20, 20, 20, 20, PNTR_RED);
                bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
                pntr_rectangle expected = PNTR_CLITERAL(pntr_rectangle) {20, 20, 1, 1};
                RECTEQUALS(bounds, expected);
                EQUALS(pntr_test_painted_count(image), 1);

                pntr_unload_image(image);
            });

            IT("pntr_draw_line_aa() covers both endpoints", {
                pntr_image* image = pntr_test_canvas(40, 40);
                NEQUALS(image, NULL);

                // The axis-aligned cases of the anti-aliased line run the same lengths.
                pntr_draw_line_aa(image, 5, 20, 15, 20, PNTR_RED);
                bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
                pntr_rectangle expected = PNTR_CLITERAL(pntr_rectangle) {5, 20, 11, 1};
                RECTEQUALS(bounds, expected);

                pntr_clear_background(image, PNTR_TEST_BACKGROUND);
                pntr_draw_line_aa(image, 20, 5, 20, 15, PNTR_RED);
                bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
                pntr_rectangle expectedVertical = PNTR_CLITERAL(pntr_rectangle) {20, 5, 1, 11};
                RECTEQUALS(bounds, expectedVertical);

                pntr_unload_image(image);
            });

            IT("pntr_draw_polyline() covers its first and last point", {
                pntr_image* image = pntr_test_canvas(40, 40);
                NEQUALS(image, NULL);

                pntr_vector points[3];
                points[0] = PNTR_CLITERAL(pntr_vector) {5, 5};
                points[1] = PNTR_CLITERAL(pntr_vector) {25, 5};
                points[2] = PNTR_CLITERAL(pntr_vector) {25, 25};
                pntr_draw_polyline(image, points, 3, PNTR_RED);

                bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
                pntr_rectangle expected = PNTR_CLITERAL(pntr_rectangle) {5, 5, 21, 21};
                RECTEQUALS(bounds, expected);

                // The corner where the two lines meet, and both loose ends.
                EQUALS(pntr_test_painted(image, 5, 5), true);
                EQUALS(pntr_test_painted(image, 25, 5), true);
                EQUALS(pntr_test_painted(image, 25, 25), true);

                // The polyline is not closed, so the fourth corner stays clear.
                EQUALS(pntr_test_painted(image, 5, 25), false);

                pntr_unload_image(image);
            });

            IT("pntr_draw_line_curve() covers its first and last point", {
                pntr_image* image = pntr_test_canvas(40, 40);
                NEQUALS(image, NULL);

                pntr_vector start = PNTR_CLITERAL(pntr_vector) {5, 30};
                pntr_vector control1 = PNTR_CLITERAL(pntr_vector) {5, 5};
                pntr_vector control2 = PNTR_CLITERAL(pntr_vector) {30, 5};
                pntr_vector end = PNTR_CLITERAL(pntr_vector) {30, 30};
                pntr_draw_line_curve(image, start, control1, control2, end, 24, PNTR_RED);

                EQUALS(pntr_test_painted(image, 5, 30), true);
                EQUALS(pntr_test_painted(image, 30, 30), true);

                pntr_unload_image(image);
            });
        });

        IT("pntr_draw_polygon() closes its corners", {
            pntr_image* image = pntr_test_canvas(40, 40);
            NEQUALS(image, NULL);

            pntr_vector points[4];
            points[0] = PNTR_CLITERAL(pntr_vector) {5, 5};
            points[1] = PNTR_CLITERAL(pntr_vector) {25, 5};
            points[2] = PNTR_CLITERAL(pntr_vector) {25, 25};
            points[3] = PNTR_CLITERAL(pntr_vector) {5, 25};
            pntr_draw_polygon(image, points, 4, PNTR_RED);

            pntr_rectangle bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
            pntr_rectangle expected = PNTR_CLITERAL(pntr_rectangle) {5, 5, 21, 21};
            RECTEQUALS(bounds, expected);

            // All four corners of an axis-aligned square, with no hole in any of them.
            EQUALS(pntr_test_painted(image, 5, 5), true);
            EQUALS(pntr_test_painted(image, 25, 5), true);
            EQUALS(pntr_test_painted(image, 25, 25), true);
            EQUALS(pntr_test_painted(image, 5, 25), true);

            // And no hole anywhere along the four edges either.
            for (int i = 5; i <= 25; i++) {
                EQUALS(pntr_test_painted(image, i, 5), true);
                EQUALS(pntr_test_painted(image, i, 25), true);
                EQUALS(pntr_test_painted(image, 5, i), true);
                EQUALS(pntr_test_painted(image, 25, i), true);
            }

            // The outline is hollow.
            EQUALS(pntr_test_painted(image, 15, 15), false);
            EQUALS(pntr_test_painted_count(image), 21 * 4 - 4);

            pntr_unload_image(image);
        });

        IT("pntr_draw_polygon_fill() covers the bounds of its points", {
            pntr_image* image = pntr_test_canvas(40, 40);
            NEQUALS(image, NULL);

            pntr_vector points[4];
            points[0] = PNTR_CLITERAL(pntr_vector) {5, 5};
            points[1] = PNTR_CLITERAL(pntr_vector) {25, 5};
            points[2] = PNTR_CLITERAL(pntr_vector) {25, 25};
            points[3] = PNTR_CLITERAL(pntr_vector) {5, 25};
            pntr_draw_polygon_fill(image, points, 4, PNTR_RED);

            pntr_rectangle bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
            pntr_rectangle expected = PNTR_CLITERAL(pntr_rectangle) {5, 5, 21, 21};
            RECTEQUALS(bounds, expected);

            // A square's fill is solid, so it covers every pixel of its own bounds.
            EQUALS(pntr_test_painted_count(image), 21 * 21);

            pntr_unload_image(image);
        });

        IT("pntr_draw_polygon_fill() reaches pntr_draw_polygon()", {
            pntr_image* outline = pntr_test_canvas(40, 40);
            pntr_image* fill = pntr_test_canvas(40, 40);
            NEQUALS(outline, NULL);
            NEQUALS(fill, NULL);

            pntr_vector points[5];
            points[0] = PNTR_CLITERAL(pntr_vector) {20, 4};
            points[1] = PNTR_CLITERAL(pntr_vector) {34, 18};
            points[2] = PNTR_CLITERAL(pntr_vector) {26, 34};
            points[3] = PNTR_CLITERAL(pntr_vector) {12, 32};
            points[4] = PNTR_CLITERAL(pntr_vector) {4, 14};
            pntr_draw_polygon(outline, points, 5, PNTR_RED);
            pntr_draw_polygon_fill(fill, points, 5, PNTR_RED);

            // The fill occupies the same bounds as the outline it belongs to.
            pntr_rectangle outlineBounds = pntr_test_painted_bounds(outline, PNTR_TEST_BACKGROUND);
            pntr_rectangle fillBounds = pntr_test_painted_bounds(fill, PNTR_TEST_BACKGROUND);
            RECTEQUALS(fillBounds, outlineBounds);

            // And reaches every corner, rather than stopping one short of them. The rest
            // of the outline is not compared pixel for pixel, because a line stepped
            // along a shallow edge rounds out as far as half a pixel past the edge
            // itself, which puts it outside the shape the fill covers.
            for (int i = 0; i < 5; i++) {
                EQUALS(pntr_test_painted(fill, points[i].x, points[i].y), true);
            }

            pntr_unload_image(outline);
            pntr_unload_image(fill);
        });

        IT("pntr_draw_polygon_fill() covers pntr_draw_polygon() of a square", {
            pntr_image* outline = pntr_test_canvas(40, 40);
            pntr_image* fill = pntr_test_canvas(40, 40);
            NEQUALS(outline, NULL);
            NEQUALS(fill, NULL);

            pntr_vector points[4];
            points[0] = PNTR_CLITERAL(pntr_vector) {5, 5};
            points[1] = PNTR_CLITERAL(pntr_vector) {25, 5};
            points[2] = PNTR_CLITERAL(pntr_vector) {25, 25};
            points[3] = PNTR_CLITERAL(pntr_vector) {5, 25};
            pntr_draw_polygon(outline, points, 4, PNTR_RED);
            pntr_draw_polygon_fill(fill, points, 4, PNTR_RED);

            // An axis-aligned square has no shallow edges to round out, so here the fill
            // does cover every single pixel of the outline.
            for (int y = 0; y < 40; y++) {
                for (int x = 0; x < 40; x++) {
                    if (pntr_test_painted(outline, x, y)) {
                        EQUALS(pntr_test_painted(fill, x, y), true);
                    }
                }
            }

            pntr_unload_image(outline);
            pntr_unload_image(fill);
        });

        IT("pntr_draw_triangle_fill() covers the bounds of its points", {
            pntr_image* image = pntr_test_canvas(40, 40);
            NEQUALS(image, NULL);

            pntr_draw_triangle_fill(image, 15, 5, 5, 25, 25, 25, PNTR_RED);

            pntr_rectangle bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
            pntr_rectangle expected = PNTR_CLITERAL(pntr_rectangle) {5, 5, 21, 21};
            RECTEQUALS(bounds, expected);

            // The apex row and the base row are both filled.
            EQUALS(pntr_test_painted(image, 15, 5), true);
            EQUALS(pntr_test_painted(image, 5, 25), true);
            EQUALS(pntr_test_painted(image, 25, 25), true);
            EQUALS(pntr_test_painted(image, 15, 25), true);

            // Outside the triangle, in the corners its edges cut off.
            EQUALS(pntr_test_painted(image, 5, 5), false);
            EQUALS(pntr_test_painted(image, 25, 5), false);

            pntr_unload_image(image);
        });

        IT("pntr_draw_triangle_fill() reaches pntr_draw_triangle()", {
            pntr_image* outline = pntr_test_canvas(40, 40);
            pntr_image* fill = pntr_test_canvas(40, 40);
            NEQUALS(outline, NULL);
            NEQUALS(fill, NULL);

            pntr_draw_triangle(outline, 15, 5, 5, 25, 32, 20, PNTR_RED);
            pntr_draw_triangle_fill(fill, 15, 5, 5, 25, 32, 20, PNTR_RED);

            pntr_rectangle outlineBounds = pntr_test_painted_bounds(outline, PNTR_TEST_BACKGROUND);
            pntr_rectangle fillBounds = pntr_test_painted_bounds(fill, PNTR_TEST_BACKGROUND);
            RECTEQUALS(fillBounds, outlineBounds);

            pntr_unload_image(outline);
            pntr_unload_image(fill);
        });

        IT("pntr_draw_circle_fill() reaches pntr_draw_circle()", {
            pntr_image* outline = pntr_test_canvas(40, 40);
            pntr_image* fill = pntr_test_canvas(40, 40);
            NEQUALS(outline, NULL);
            NEQUALS(fill, NULL);

            pntr_draw_circle(outline, 20, 20, 12, PNTR_RED);
            pntr_draw_circle_fill(fill, 20, 20, 12, PNTR_RED);

            // A radius covers its far pixel, so both are 12 * 2 + 1 across.
            pntr_rectangle expected = PNTR_CLITERAL(pntr_rectangle) {8, 8, 25, 25};
            pntr_rectangle outlineBounds = pntr_test_painted_bounds(outline, PNTR_TEST_BACKGROUND);
            pntr_rectangle fillBounds = pntr_test_painted_bounds(fill, PNTR_TEST_BACKGROUND);
            RECTEQUALS(outlineBounds, expected);
            RECTEQUALS(fillBounds, expected);

            // The fill reaches the outline on all four sides.
            EQUALS(pntr_test_painted(fill, 8, 20), true);
            EQUALS(pntr_test_painted(fill, 32, 20), true);
            EQUALS(pntr_test_painted(fill, 20, 8), true);
            EQUALS(pntr_test_painted(fill, 20, 32), true);

            // And covers every pixel of it.
            EQUALS(pntr_test_uncovered(outline, fill), 0);

            pntr_unload_image(outline);
            pntr_unload_image(fill);

            // At every radius, the same way the ellipse does. The fill walks out from the
            // same circle equation the outline does, so this already held; asserting it
            // across a sweep is what stops it from drifting.
            for (int radius = 1; radius <= 48; radius++) {
                EQUALS(pntr_test_unfilled_circle(radius), 0);
            }

            EQUALS(pntr_test_unfilled_circle(120), 0);
        });

        IT("pntr_draw_circle() grows one pixel a side with its radius", {
            // The radius is inclusive, so every radius is two pixels wider than the one
            // before it, with no size skipped and none repeated. A radius of zero is the
            // center point on its own and a radius of one is three across; both used to
            // be special cased, which left the smallest circle a single pixel and no
            // three pixel circle at all.
            int outlineWidth[5] = {0};
            int outlineHeight[5] = {0};
            int outlinePainted[5] = {0};
            int fillWidth[5] = {0};
            int fillHeight[5] = {0};
            int fillPainted[5] = {0};

            for (int radius = 0; radius <= 4; radius++) {
                pntr_image* outline = pntr_test_canvas(20, 20);
                pntr_image* fill = pntr_test_canvas(20, 20);
                NEQUALS(outline, NULL);
                NEQUALS(fill, NULL);

                pntr_draw_circle(outline, 10, 10, radius, PNTR_RED);
                pntr_draw_circle_fill(fill, 10, 10, radius, PNTR_RED);

                pntr_rectangle outlineBounds = pntr_test_painted_bounds(outline, PNTR_TEST_BACKGROUND);
                pntr_rectangle fillBounds = pntr_test_painted_bounds(fill, PNTR_TEST_BACKGROUND);
                outlineWidth[radius] = outlineBounds.width;
                outlineHeight[radius] = outlineBounds.height;
                outlinePainted[radius] = pntr_test_painted_count(outline);
                fillWidth[radius] = fillBounds.width;
                fillHeight[radius] = fillBounds.height;
                fillPainted[radius] = pntr_test_painted_count(fill);

                // The outline and the fill are the same size as each other, and the fill
                // covers every pixel of the outline, at the small radii too.
                RECTEQUALS(fillBounds, outlineBounds);
                EQUALS(pntr_test_uncovered(outline, fill), 0);
                EQUALS(pntr_test_unfilled_circle(radius), 0);

                pntr_unload_image(outline);
                pntr_unload_image(fill);
            }

            for (int radius = 0; radius <= 4; radius++) {
                EQUALS(outlineWidth[radius], radius * 2 + 1);
                EQUALS(outlineHeight[radius], radius * 2 + 1);
                EQUALS(fillWidth[radius], radius * 2 + 1);
                EQUALS(fillHeight[radius], radius * 2 + 1);
            }

            // A radius of zero is the center point, the same as pntr_draw_arc() draws.
            EQUALS(outlinePainted[0], 1);
            EQUALS(fillPainted[0], 1);

            // A radius of one is the four pixels around the center, which the fill turns
            // into a plus by covering the center as well.
            EQUALS(outlinePainted[1], 4);
            EQUALS(fillPainted[1], 5);

            // And every radius paints more than the one before it.
            for (int radius = 1; radius <= 4; radius++) {
                GREATER(outlinePainted[radius], outlinePainted[radius - 1]);
                GREATER(fillPainted[radius], fillPainted[radius - 1]);
            }
        });

        IT("pntr_draw_circle_thick() grows with its thickness", {
            // pntr_draw_circle_thick() plots the outline with the same round brush the
            // thick lines use, rather than dispatching to pntr_draw_circle_fill() at half
            // the thickness, so the radius of one case can no longer flatten a thickness
            // of two or three down to a single pixel.
            int painted[4] = {0};
            int width[4] = {0};
            int height[4] = {0};

            for (int thickness = 1; thickness <= 3; thickness++) {
                pntr_image* image = pntr_test_canvas(40, 40);
                NEQUALS(image, NULL);

                pntr_draw_circle_thick(image, 20, 20, 8, thickness, PNTR_RED);

                pntr_rectangle bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
                painted[thickness] = pntr_test_painted_count(image);
                width[thickness] = bounds.width;
                height[thickness] = bounds.height;

                pntr_unload_image(image);
            }

            // The brush straddles the outline, so each extra pixel of thickness reaches
            // one pixel further out.
            for (int thickness = 1; thickness <= 3; thickness++) {
                EQUALS(width[thickness], 8 * 2 + thickness);
                EQUALS(height[thickness], 8 * 2 + thickness);
            }

            GREATER(painted[1], 0);
            for (int thickness = 2; thickness <= 3; thickness++) {
                GREATER(painted[thickness], painted[thickness - 1]);
                GREATER(width[thickness], width[thickness - 1]);
            }
        });

        IT("pntr_draw_line_thick() keeps its brush at a thickness of two and three", {
            // The same knock-on, on the path that used to call pntr_draw_circle_fill()
            // with thickness / 2: a thickness of two and three both came out as a single
            // pixel, which made them render the same as a thickness of one.
            int painted[4] = {0};
            int brush[4] = {0};

            for (int thickness = 1; thickness <= 3; thickness++) {
                pntr_image* image = pntr_test_canvas(40, 40);
                NEQUALS(image, NULL);

                pntr_draw_line_thick(image, 10, 10, 30, 20, thickness, PNTR_RED);
                painted[thickness] = pntr_test_painted_count(image);
                pntr_clear_background(image, PNTR_TEST_BACKGROUND);

                // A line of no length is the brush on its own.
                pntr_draw_line_thick(image, 20, 20, 20, 20, thickness, PNTR_RED);
                brush[thickness] = pntr_test_painted_count(image);

                pntr_unload_image(image);
            }

            EQUALS(brush[1], 1);
            for (int thickness = 2; thickness <= 3; thickness++) {
                GREATER(brush[thickness], brush[thickness - 1]);
                GREATER(painted[thickness], painted[thickness - 1]);
            }
        });

        IT("pntr_draw_ellipse_fill() reaches pntr_draw_ellipse()", {
            pntr_image* outline = pntr_test_canvas(40, 40);
            pntr_image* fill = pntr_test_canvas(40, 40);
            NEQUALS(outline, NULL);
            NEQUALS(fill, NULL);

            pntr_draw_ellipse(outline, 20, 20, 15, 9, PNTR_RED);
            pntr_draw_ellipse_fill(fill, 20, 20, 15, 9, PNTR_RED);

            pntr_rectangle expected = PNTR_CLITERAL(pntr_rectangle) {5, 11, 31, 19};
            pntr_rectangle outlineBounds = pntr_test_painted_bounds(outline, PNTR_TEST_BACKGROUND);
            pntr_rectangle fillBounds = pntr_test_painted_bounds(fill, PNTR_TEST_BACKGROUND);
            RECTEQUALS(outlineBounds, expected);
            RECTEQUALS(fillBounds, expected);

            // The fill reaches the outline on all four sides.
            EQUALS(pntr_test_painted(fill, 5, 20), true);
            EQUALS(pntr_test_painted(fill, 35, 20), true);
            EQUALS(pntr_test_painted(fill, 20, 11), true);
            EQUALS(pntr_test_painted(fill, 20, 29), true);

            // And covers every pixel of it. Both walk the same midpoint traversal, so the
            // fill never leaves the outline outside it, not even along the flat runs at
            // the extremes where testing each pixel against the ellipse used to.
            EQUALS(pntr_test_uncovered(outline, fill), 0);

            pntr_unload_image(outline);
            pntr_unload_image(fill);

            // Every size registers the same way: odd and even radii, both orientations,
            // and the degenerate radius of one that is a single pixel across.
            for (int radiusX = 1; radiusX <= 24; radiusX++) {
                for (int radiusY = 1; radiusY <= 24; radiusY++) {
                    EQUALS(pntr_test_unfilled_ellipse(radiusX, radiusY), 0);
                }
            }

            // Far enough out that the two regions of the midpoint walk are both long.
            EQUALS(pntr_test_unfilled_ellipse(120, 7), 0);
            EQUALS(pntr_test_unfilled_ellipse(7, 120), 0);
            EQUALS(pntr_test_unfilled_ellipse(120, 120), 0);
        });

        IT("pntr_draw_ellipse() covers both radii at a radius of one", {
            // The ellipse never special cased a radius of one, so a radius of one on
            // either axis is already three pixels across on that axis. A radius of zero
            // collapses it onto a line through the center, and a radius of zero on both
            // axes is the center point on its own, which is what pntr_draw_circle()
            // draws for a radius of zero.
            int radii[5][2] = {{0, 0}, {1, 1}, {1, 3}, {3, 1}, {0, 3}};

            for (int i = 0; i < 5; i++) {
                int radiusX = radii[i][0];
                int radiusY = radii[i][1];

                pntr_image* outline = pntr_test_canvas(20, 20);
                pntr_image* fill = pntr_test_canvas(20, 20);
                NEQUALS(outline, NULL);
                NEQUALS(fill, NULL);

                pntr_draw_ellipse(outline, 10, 10, radiusX, radiusY, PNTR_RED);
                pntr_draw_ellipse_fill(fill, 10, 10, radiusX, radiusY, PNTR_RED);

                pntr_rectangle expected = PNTR_CLITERAL(pntr_rectangle) {
                    10 - radiusX, 10 - radiusY, radiusX * 2 + 1, radiusY * 2 + 1
                };
                pntr_rectangle outlineBounds = pntr_test_painted_bounds(outline, PNTR_TEST_BACKGROUND);
                pntr_rectangle fillBounds = pntr_test_painted_bounds(fill, PNTR_TEST_BACKGROUND);
                RECTEQUALS(outlineBounds, expected);
                RECTEQUALS(fillBounds, expected);

                // And the fill still covers every pixel of the outline at these sizes.
                EQUALS(pntr_test_uncovered(outline, fill), 0);
                EQUALS(pntr_test_unfilled_ellipse(radiusX, radiusY), 0);

                pntr_unload_image(outline);
                pntr_unload_image(fill);
            }

            // A radius of one on both axes is the same four pixels the circle of radius
            // one draws, and the fill covers the center on top of them.
            pntr_image* ellipse = pntr_test_canvas(20, 20);
            pntr_image* circle = pntr_test_canvas(20, 20);
            NEQUALS(ellipse, NULL);
            NEQUALS(circle, NULL);
            pntr_draw_ellipse(ellipse, 10, 10, 1, 1, PNTR_RED);
            pntr_draw_circle(circle, 10, 10, 1, PNTR_RED);
            IMAGEEQUALS(ellipse, circle);
            pntr_unload_image(ellipse);
            pntr_unload_image(circle);

            ellipse = pntr_test_canvas(20, 20);
            circle = pntr_test_canvas(20, 20);
            NEQUALS(ellipse, NULL);
            NEQUALS(circle, NULL);
            pntr_draw_ellipse_fill(ellipse, 10, 10, 1, 1, PNTR_RED);
            pntr_draw_circle_fill(circle, 10, 10, 1, PNTR_RED);
            IMAGEEQUALS(ellipse, circle);
            pntr_unload_image(ellipse);
            pntr_unload_image(circle);
        });

        IT("pntr_draw_rectangle() stops short of its width and height", {
            pntr_image* image = pntr_test_canvas(40, 40);
            NEQUALS(image, NULL);

            pntr_draw_rectangle(image, 5, 5, 20, 12, PNTR_RED);

            pntr_rectangle bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
            pntr_rectangle expected = PNTR_CLITERAL(pntr_rectangle) {5, 5, 20, 12};
            RECTEQUALS(bounds, expected);

            EQUALS(pntr_test_painted(image, 24, 16), true);
            EQUALS(pntr_test_painted(image, 25, 16), false);
            EQUALS(pntr_test_painted(image, 24, 17), false);

            // Four edges of a hollow rectangle, with each corner counted once.
            EQUALS(pntr_test_painted_count(image), 20 * 2 + 12 * 2 - 4);

            pntr_unload_image(image);
        });

        IT("pntr_draw_rectangle_rounded() matches pntr_draw_rectangle()", {
            pntr_image* plain = pntr_test_canvas(40, 40);
            pntr_image* rounded = pntr_test_canvas(40, 40);
            NEQUALS(plain, NULL);
            NEQUALS(rounded, NULL);

            pntr_draw_rectangle(plain, 5, 5, 24, 16, PNTR_RED);
            pntr_draw_rectangle_rounded(rounded, 5, 5, 24, 16, 4, 4, 4, 4, PNTR_RED);

            // Rounding the corners does not change which pixels the rectangle occupies.
            pntr_rectangle plainBounds = pntr_test_painted_bounds(plain, PNTR_TEST_BACKGROUND);
            pntr_rectangle roundedBounds = pntr_test_painted_bounds(rounded, PNTR_TEST_BACKGROUND);
            RECTEQUALS(roundedBounds, plainBounds);

            // The corners themselves are the part that is cut away.
            EQUALS(pntr_test_painted(plain, 5, 5), true);
            EQUALS(pntr_test_painted(rounded, 5, 5), false);
            EQUALS(pntr_test_painted(rounded, 28, 20), false);

            // The middle of each edge is still on the edge of the rectangle.
            EQUALS(pntr_test_painted(rounded, 17, 5), true);
            EQUALS(pntr_test_painted(rounded, 17, 20), true);
            EQUALS(pntr_test_painted(rounded, 5, 13), true);
            EQUALS(pntr_test_painted(rounded, 28, 13), true);

            pntr_unload_image(plain);
            pntr_unload_image(rounded);
        });

        IT("pntr_draw_rectangle_rounded_fill() matches pntr_draw_rectangle_fill()", {
            pntr_image* plain = pntr_test_canvas(40, 40);
            pntr_image* rounded = pntr_test_canvas(40, 40);
            pntr_image* outline = pntr_test_canvas(40, 40);
            NEQUALS(plain, NULL);
            NEQUALS(rounded, NULL);
            NEQUALS(outline, NULL);

            pntr_draw_rectangle_fill(plain, 5, 5, 24, 16, PNTR_RED);
            pntr_draw_rectangle_rounded_fill(rounded, 5, 5, 24, 16, 4, PNTR_RED);
            pntr_draw_rectangle_rounded(outline, 5, 5, 24, 16, 4, 4, 4, 4, PNTR_RED);

            pntr_rectangle plainBounds = pntr_test_painted_bounds(plain, PNTR_TEST_BACKGROUND);
            pntr_rectangle roundedBounds = pntr_test_painted_bounds(rounded, PNTR_TEST_BACKGROUND);
            RECTEQUALS(roundedBounds, plainBounds);

            // The rightmost column and the bottom row are filled, not left behind.
            EQUALS(pntr_test_painted(rounded, 28, 13), true);
            EQUALS(pntr_test_painted(rounded, 17, 20), true);

            // The fill reaches every pixel of the outline of the same rectangle.
            for (int y = 0; y < 40; y++) {
                for (int x = 0; x < 40; x++) {
                    if (pntr_test_painted(outline, x, y)) {
                        EQUALS(pntr_test_painted(rounded, x, y), true);
                    }
                }
            }

            pntr_unload_image(plain);
            pntr_unload_image(rounded);
            pntr_unload_image(outline);
        });

        IT("pntr_draw_rectangle_rounded() clamps an over-large radius", {
            pntr_image* image = pntr_test_canvas(40, 40);
            pntr_image* fill = pntr_test_canvas(40, 40);
            NEQUALS(image, NULL);
            NEQUALS(fill, NULL);

            // A radius of 18 on a 20x20 rectangle cannot fit. It used to give the
            // straight edges a negative length, which drew them backwards through the
            // middle of the shape.
            pntr_draw_rectangle_rounded(image, 5, 5, 20, 20, 18, 18, 18, 18, PNTR_RED);
            pntr_draw_rectangle_rounded_fill(fill, 5, 5, 20, 20, 18, PNTR_RED);

            pntr_rectangle expected = PNTR_CLITERAL(pntr_rectangle) {5, 5, 20, 20};
            pntr_rectangle bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
            pntr_rectangle fillBounds = pntr_test_painted_bounds(fill, PNTR_TEST_BACKGROUND);
            RECTEQUALS(bounds, expected);
            RECTEQUALS(fillBounds, expected);

            // The outline is still hollow, so nothing ran back through the middle.
            EQUALS(pntr_test_painted(image, 15, 15), false);

            // And the corners are still cut away.
            EQUALS(pntr_test_painted(image, 5, 5), false);
            EQUALS(pntr_test_painted(fill, 5, 5), false);

            pntr_unload_image(image);
            pntr_unload_image(fill);
        });

        IT("pntr_draw_line_thick() grows with its thickness", {
            int axisAligned[5] = {0};
            int diagonal[5] = {0};
            int brushWidth[5] = {0};
            int brushHeight[5] = {0};
            int lineHeight[5] = {0};
            int lineLeft[5] = {0};
            int lineWidth[5] = {0};
            int stepLeft[5] = {0};
            int stepWidth[5] = {0};

            for (int thickness = 1; thickness <= 4; thickness++) {
                pntr_image* image = pntr_test_canvas(60, 60);
                pntr_draw_line_thick(image, 10, 30, 40, 30, thickness, PNTR_RED);
                pntr_rectangle bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
                axisAligned[thickness] = pntr_test_painted_count(image);
                lineHeight[thickness] = bounds.height;
                lineLeft[thickness] = bounds.x;
                lineWidth[thickness] = bounds.width;
                pntr_unload_image(image);

                image = pntr_test_canvas(60, 60);
                pntr_draw_line_thick(image, 10, 10, 40, 40, thickness, PNTR_RED);
                diagonal[thickness] = pntr_test_painted_count(image);
                pntr_unload_image(image);

                // A line of no length is the brush on its own.
                image = pntr_test_canvas(60, 60);
                pntr_draw_line_thick(image, 30, 30, 30, 30, thickness, PNTR_RED);
                bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
                brushWidth[thickness] = bounds.width;
                brushHeight[thickness] = bounds.height;
                pntr_unload_image(image);

                // One step along a diagonal, which uses the brush rather than the
                // axis-aligned path.
                image = pntr_test_canvas(60, 60);
                pntr_draw_line_thick(image, 30, 30, 31, 31, thickness, PNTR_RED);
                bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
                stepLeft[thickness] = bounds.x;
                stepWidth[thickness] = bounds.width;
                pntr_unload_image(image);
            }

            for (int thickness = 1; thickness <= 4; thickness++) {
                // The brush is exactly as many pixels across as the thickness asks for,
                // which is what makes 2 and 3 render differently from 1.
                EQUALS(brushWidth[thickness], thickness);
                EQUALS(brushHeight[thickness], thickness);

                // An axis-aligned line is exactly that many pixels thick too.
                EQUALS(lineHeight[thickness], thickness);

                // And the brush reaches the same distance past the endpoints whether the
                // line is axis-aligned or not, so both run the same thickness.
                EQUALS(lineLeft[thickness], 10 - thickness / 2);
                EQUALS(stepLeft[thickness], 30 - thickness / 2);
                EQUALS(lineWidth[thickness], 30 + thickness);
                EQUALS(stepWidth[thickness], thickness + 1);
            }

            // Every extra pixel of thickness paints more than the one before it, on both
            // an axis-aligned line and a diagonal one.
            for (int thickness = 2; thickness <= 4; thickness++) {
                GREATER(axisAligned[thickness], axisAligned[thickness - 1]);
                GREATER(diagonal[thickness], diagonal[thickness - 1]);
            }
        });

        IT("a semi-transparent primitive blends each pixel once", {
            // Blending onto a transparent pixel leaves the color as it is, so an alpha
            // that is still 128 is a pixel that was painted exactly once. A pixel that
            // was painted twice reports 192 instead.
            pntr_color semi = pntr_new_color(230, 41, 55, 128);

            IT("pntr_draw_rectangle() one pixel tall", {
                pntr_image* image = pntr_new_image(40, 40);
                NEQUALS(image, NULL);
                pntr_clear_background(image, PNTR_BLANK);

                pntr_draw_rectangle(image, 5, 5, 20, 1, semi);
                for (int x = 5; x < 25; x++) {
                    EQUALS((int)pntr_color_a(pntr_image_get_color(image, x, 5)), 128);
                }

                pntr_unload_image(image);
            });

            IT("pntr_draw_rectangle() one pixel wide", {
                pntr_image* image = pntr_new_image(40, 40);
                NEQUALS(image, NULL);
                pntr_clear_background(image, PNTR_BLANK);

                pntr_draw_rectangle(image, 5, 5, 1, 20, semi);
                for (int y = 5; y < 25; y++) {
                    EQUALS((int)pntr_color_a(pntr_image_get_color(image, 5, y)), 128);
                }

                pntr_unload_image(image);
            });

            IT("pntr_draw_rectangle() with a hollow middle", {
                pntr_image* image = pntr_new_image(40, 40);
                NEQUALS(image, NULL);
                pntr_clear_background(image, PNTR_BLANK);

                // The corners are where the horizontal and vertical edges used to meet.
                pntr_draw_rectangle(image, 5, 5, 20, 12, semi);
                EQUALS((int)pntr_color_a(pntr_image_get_color(image, 5, 5)), 128);
                EQUALS((int)pntr_color_a(pntr_image_get_color(image, 24, 5)), 128);
                EQUALS((int)pntr_color_a(pntr_image_get_color(image, 5, 16)), 128);
                EQUALS((int)pntr_color_a(pntr_image_get_color(image, 24, 16)), 128);

                pntr_unload_image(image);
            });

            IT("pntr_draw_circle_fill()", {
                pntr_image* image = pntr_new_image(40, 40);
                NEQUALS(image, NULL);
                pntr_clear_background(image, PNTR_BLANK);

                pntr_draw_circle_fill(image, 20, 20, 12, semi);
                for (int y = 0; y < 40; y++) {
                    for (int x = 0; x < 40; x++) {
                        unsigned char alpha = pntr_color_a(pntr_image_get_color(image, x, y));
                        if (alpha != 0) {
                            EQUALS((int)alpha, 128);
                        }
                    }
                }

                // Including the center row, which both halves of the fill run along.
                EQUALS((int)pntr_color_a(pntr_image_get_color(image, 20, 20)), 128);

                pntr_unload_image(image);
            });

            IT("pntr_draw_ellipse_fill()", {
                pntr_image* image = pntr_new_image(40, 40);
                NEQUALS(image, NULL);
                pntr_clear_background(image, PNTR_BLANK);

                pntr_draw_ellipse_fill(image, 20, 20, 15, 9, semi);
                for (int y = 0; y < 40; y++) {
                    for (int x = 0; x < 40; x++) {
                        unsigned char alpha = pntr_color_a(pntr_image_get_color(image, x, y));
                        if (alpha != 0) {
                            EQUALS((int)alpha, 128);
                        }
                    }
                }

                pntr_unload_image(image);
            });

            IT("pntr_draw_circle()", {
                pntr_image* image = pntr_new_image(40, 40);
                NEQUALS(image, NULL);
                pntr_clear_background(image, PNTR_BLANK);

                // The outline mirrors one eighth of the circle into the other seven, and
                // the cardinal and diagonal points are where that mirroring folds over
                // onto itself.
                pntr_draw_circle(image, 20, 20, 12, semi);
                for (int y = 0; y < 40; y++) {
                    for (int x = 0; x < 40; x++) {
                        unsigned char alpha = pntr_color_a(pntr_image_get_color(image, x, y));
                        if (alpha != 0) {
                            EQUALS((int)alpha, 128);
                        }
                    }
                }

                pntr_unload_image(image);
            });

            IT("pntr_draw_circle() and pntr_draw_circle_fill() at a radius of one", {
                // The smallest circle is where the eightfold mirroring folds over the
                // most, so it is the radius most likely to paint a pixel twice. A radius
                // of zero folds every one of the eight onto the center.
                int expectedOutline[2] = {1, 4};
                int expectedFill[2] = {1, 5};
                for (int radius = 0; radius <= 1; radius++) {
                    pntr_image* outline = pntr_new_image(16, 16);
                    pntr_image* fill = pntr_new_image(16, 16);
                    NEQUALS(outline, NULL);
                    NEQUALS(fill, NULL);
                    pntr_clear_background(outline, PNTR_BLANK);
                    pntr_clear_background(fill, PNTR_BLANK);

                    pntr_draw_circle(outline, 8, 8, radius, semi);
                    pntr_draw_circle_fill(fill, 8, 8, radius, semi);

                    int outlinePainted = 0;
                    int fillPainted = 0;
                    for (int y = 0; y < 16; y++) {
                        for (int x = 0; x < 16; x++) {
                            unsigned char outlineAlpha = pntr_color_a(pntr_image_get_color(outline, x, y));
                            unsigned char fillAlpha = pntr_color_a(pntr_image_get_color(fill, x, y));
                            if (outlineAlpha != 0) {
                                EQUALS((int)outlineAlpha, 128);
                                outlinePainted++;
                            }
                            if (fillAlpha != 0) {
                                EQUALS((int)fillAlpha, 128);
                                fillPainted++;
                            }
                        }
                    }

                    // Which is only worth checking because something was painted.
                    EQUALS(outlinePainted, expectedOutline[radius]);
                    EQUALS(fillPainted, expectedFill[radius]);

                    pntr_unload_image(outline);
                    pntr_unload_image(fill);
                }
            });

            IT("pntr_draw_polygon_fill()", {
                pntr_image* image = pntr_new_image(40, 40);
                NEQUALS(image, NULL);
                pntr_clear_background(image, PNTR_BLANK);

                pntr_vector points[4];
                points[0] = PNTR_CLITERAL(pntr_vector) {20, 5};
                points[1] = PNTR_CLITERAL(pntr_vector) {34, 20};
                points[2] = PNTR_CLITERAL(pntr_vector) {20, 34};
                points[3] = PNTR_CLITERAL(pntr_vector) {6, 20};
                pntr_draw_polygon_fill(image, points, 4, semi);

                for (int y = 0; y < 40; y++) {
                    for (int x = 0; x < 40; x++) {
                        unsigned char alpha = pntr_color_a(pntr_image_get_color(image, x, y));
                        if (alpha != 0) {
                            EQUALS((int)alpha, 128);
                        }
                    }
                }

                pntr_unload_image(image);
            });

            IT("pntr_draw_rectangle_rounded_fill()", {
                pntr_image* image = pntr_new_image(40, 40);
                NEQUALS(image, NULL);
                pntr_clear_background(image, PNTR_BLANK);

                pntr_draw_rectangle_rounded_fill(image, 5, 5, 24, 16, 4, semi);
                for (int y = 0; y < 40; y++) {
                    for (int x = 0; x < 40; x++) {
                        unsigned char alpha = pntr_color_a(pntr_image_get_color(image, x, y));
                        if (alpha != 0) {
                            EQUALS((int)alpha, 128);
                        }
                    }
                }

                pntr_unload_image(image);
            });
        });

        IT("pntr_test_painted_bounds() reports nothing for an empty image", {
            pntr_image* image = pntr_test_canvas(10, 10);
            NEQUALS(image, NULL);

            pntr_rectangle bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
            pntr_rectangle expected = PNTR_CLITERAL(pntr_rectangle) {0, 0, 0, 0};
            RECTEQUALS(bounds, expected);
            EQUALS(pntr_test_painted_count(image), 0);

            pntr_unload_image(image);
        });
    });

    IT("pntr_draw_arc_fill()", {
        // Note: pntr_draw_polygon_fill() covers the whole bounding box of the points it
        // is given, so the wedge includes the rows of its topmost and bottommost points.
        IT("pntr_draw_arc_fill() fills the wedge", {
            pntr_image* image = pntr_gen_image_color(50, 50, PNTR_WHITE);
            NEQUALS(image, NULL);

            // A quarter wedge, sweeping clockwise from 0 to 90 degrees.
            pntr_draw_arc_fill(image, 25, 25, 20.0f, 0.0f, 90.0f, 8, PNTR_RED);

            // Clearly inside the wedge.
            COLOREQUALS(pntr_image_get_color(image, 30, 30), PNTR_RED);
            COLOREQUALS(pntr_image_get_color(image, 27, 35), PNTR_RED);
            COLOREQUALS(pntr_image_get_color(image, 40, 27), PNTR_RED);

            // Clearly outside the wedge.
            COLOREQUALS(pntr_image_get_color(image, 10, 10), PNTR_WHITE);
            COLOREQUALS(pntr_image_get_color(image, 20, 30), PNTR_WHITE);
            COLOREQUALS(pntr_image_get_color(image, 45, 45), PNTR_WHITE);
            COLOREQUALS(pntr_image_get_color(image, 49, 49), PNTR_WHITE);

            pntr_unload_image(image);
        });

        IT("pntr_draw_arc_fill() with one segment", {
            pntr_image* image = pntr_gen_image_color(50, 50, PNTR_WHITE);
            NEQUALS(image, NULL);

            // One segment is a single arc point plus the center, which is a
            // degenerate line rather than a fillable shape, so nothing is drawn.
            pntr_draw_arc_fill(image, 25, 25, 20.0f, 0.0f, 90.0f, 1, PNTR_RED);
            COLOREQUALS(pntr_image_get_color(image, 10, 10), PNTR_WHITE);
            COLOREQUALS(pntr_image_get_color(image, 45, 45), PNTR_WHITE);

            pntr_unload_image(image);
        });

        IT("pntr_draw_arc_fill() with two segments", {
            pntr_image* image = pntr_gen_image_color(50, 50, PNTR_WHITE);
            NEQUALS(image, NULL);

            pntr_draw_arc_fill(image, 25, 25, 20.0f, 0.0f, 90.0f, 2, PNTR_RED);
            COLOREQUALS(pntr_image_get_color(image, 35, 30), PNTR_RED);
            COLOREQUALS(pntr_image_get_color(image, 10, 10), PNTR_WHITE);
            COLOREQUALS(pntr_image_get_color(image, 45, 45), PNTR_WHITE);

            pntr_unload_image(image);
        });

        IT("pntr_draw_arc_fill() with eight segments", {
            pntr_image* image = pntr_gen_image_color(50, 50, PNTR_WHITE);
            NEQUALS(image, NULL);

            pntr_draw_arc_fill(image, 25, 25, 20.0f, 0.0f, 90.0f, 8, PNTR_RED);
            COLOREQUALS(pntr_image_get_color(image, 30, 30), PNTR_RED);
            COLOREQUALS(pntr_image_get_color(image, 10, 10), PNTR_WHITE);
            COLOREQUALS(pntr_image_get_color(image, 45, 45), PNTR_WHITE);

            pntr_unload_image(image);
        });

        IT("pntr_draw_arc_fill() with sixty four segments", {
            pntr_image* image = pntr_gen_image_color(50, 50, PNTR_WHITE);
            NEQUALS(image, NULL);

            pntr_draw_arc_fill(image, 25, 25, 20.0f, 0.0f, 90.0f, 64, PNTR_RED);
            COLOREQUALS(pntr_image_get_color(image, 30, 30), PNTR_RED);
            COLOREQUALS(pntr_image_get_color(image, 26, 44), PNTR_RED);
            COLOREQUALS(pntr_image_get_color(image, 10, 10), PNTR_WHITE);
            COLOREQUALS(pntr_image_get_color(image, 45, 45), PNTR_WHITE);

            pntr_unload_image(image);
        });

        IT("pntr_draw_arc_fill() with a full sweep", {
            pntr_image* image = pntr_gen_image_color(50, 50, PNTR_WHITE);
            NEQUALS(image, NULL);

            // A full sweep leaves a thin seam where the trailing center vertex
            // cuts back through the center, so stay away from the center row.
            pntr_draw_arc_fill(image, 25, 25, 20.0f, 0.0f, 360.0f, 64, PNTR_RED);
            COLOREQUALS(pntr_image_get_color(image, 25, 15), PNTR_RED);
            COLOREQUALS(pntr_image_get_color(image, 25, 35), PNTR_RED);
            COLOREQUALS(pntr_image_get_color(image, 15, 20), PNTR_RED);
            COLOREQUALS(pntr_image_get_color(image, 35, 30), PNTR_RED);

            COLOREQUALS(pntr_image_get_color(image, 25, 2), PNTR_WHITE);
            COLOREQUALS(pntr_image_get_color(image, 25, 47), PNTR_WHITE);
            COLOREQUALS(pntr_image_get_color(image, 1, 1), PNTR_WHITE);
            COLOREQUALS(pntr_image_get_color(image, 48, 48), PNTR_WHITE);

            pntr_unload_image(image);
        });

        IT("pntr_draw_arc_fill() with a zero or negative radius", {
            pntr_image* image = pntr_gen_image_color(50, 50, PNTR_WHITE);
            NEQUALS(image, NULL);

            // Matches pntr_draw_arc(): a radius with no size draws the center point.
            pntr_draw_arc_fill(image, 25, 25, 0.0f, 0.0f, 90.0f, 8, PNTR_RED);
            COLOREQUALS(pntr_image_get_color(image, 25, 25), PNTR_RED);
            COLOREQUALS(pntr_image_get_color(image, 26, 25), PNTR_WHITE);
            COLOREQUALS(pntr_image_get_color(image, 25, 26), PNTR_WHITE);

            pntr_draw_arc_fill(image, 10, 10, -5.0f, 0.0f, 90.0f, 8, PNTR_BLUE);
            COLOREQUALS(pntr_image_get_color(image, 10, 10), PNTR_BLUE);
            COLOREQUALS(pntr_image_get_color(image, 11, 10), PNTR_WHITE);
            COLOREQUALS(pntr_image_get_color(image, 10, 11), PNTR_WHITE);

            pntr_unload_image(image);
        });

        IT("pntr_draw_arc_fill() with zero or negative segments", {
            pntr_image* image = pntr_gen_image_color(50, 50, PNTR_WHITE);
            NEQUALS(image, NULL);

            pntr_draw_arc_fill(image, 25, 25, 20.0f, 0.0f, 90.0f, 0, PNTR_RED);
            pntr_draw_arc_fill(image, 25, 25, 20.0f, 0.0f, 90.0f, -4, PNTR_RED);
            COLOREQUALS(pntr_image_get_color(image, 25, 25), PNTR_WHITE);
            COLOREQUALS(pntr_image_get_color(image, 30, 30), PNTR_WHITE);
            COLOREQUALS(pntr_image_get_color(image, 10, 10), PNTR_WHITE);

            pntr_unload_image(image);
        });

        IT("pntr_draw_arc_fill() with a matching start and end angle", {
            pntr_image* image = pntr_gen_image_color(50, 50, PNTR_WHITE);
            NEQUALS(image, NULL);

            // Every arc point lands on the same spot, so the wedge collapses onto the
            // line between that spot and the center. A fill covers the bounding box of
            // the points it is given, which for a collapsed shape is that line.
            pntr_draw_arc_fill(image, 25, 25, 20.0f, 45.0f, 45.0f, 8, PNTR_RED);
            COLOREQUALS(pntr_image_get_color(image, 25, 25), PNTR_RED);
            COLOREQUALS(pntr_image_get_color(image, 30, 30), PNTR_RED);
            COLOREQUALS(pntr_image_get_color(image, 39, 39), PNTR_RED);

            // Still nothing on either side of that line.
            COLOREQUALS(pntr_image_get_color(image, 25, 39), PNTR_WHITE);
            COLOREQUALS(pntr_image_get_color(image, 39, 25), PNTR_WHITE);

            pntr_unload_image(image);
        });

        IT("pntr_draw_arc_fill() with a NULL destination", {
            // A NULL destination does not crash, for any of the early exits.
            pntr_draw_arc_fill(NULL, 25, 25, 20.0f, 0.0f, 90.0f, 8, PNTR_RED);
            pntr_draw_arc_fill(NULL, 25, 25, 0.0f, 0.0f, 90.0f, 8, PNTR_RED);
            pntr_draw_arc_fill(NULL, 25, 25, 20.0f, 0.0f, 90.0f, 0, PNTR_RED);

            // Drawing still works afterwards.
            pntr_image* image = pntr_gen_image_color(50, 50, PNTR_WHITE);
            NEQUALS(image, NULL);
            pntr_draw_arc_fill(image, 25, 25, 20.0f, 0.0f, 90.0f, 8, PNTR_RED);
            COLOREQUALS(pntr_image_get_color(image, 30, 30), PNTR_RED);
            pntr_unload_image(image);
        });
    });

    IT("pntr_draw_arc()", {
        IT("pntr_draw_arc() turns clockwise from the positive X axis", {
            pntr_image* image = pntr_test_canvas(60, 60);
            NEQUALS(image, NULL);

            // A quarter sweep from 0 to 90 degrees stays in the quarter below and to the
            // right of the center, which is what makes the angles clockwise on screen.
            pntr_draw_arc(image, 30, 30, 20.0f, 0.0f, 90.0f, 64, PNTR_RED);

            pntr_rectangle bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
            LESSER(bounds.x, 31);
            GREATER(bounds.x, 28);
            LESSER(bounds.y, 31);
            GREATER(bounds.y, 28);
            GREATER(bounds.width, 15);
            GREATER(bounds.height, 15);

            pntr_unload_image(image);
        });

        IT("pntr_draw_arc() sweeps a half-open range of angles", {
            pntr_image* image = pntr_test_canvas(60, 60);
            NEQUALS(image, NULL);

            // Four points over a full turn land on the four cardinal directions, with
            // the one at 360 degrees left out because it is the one at 0 degrees.
            pntr_draw_arc(image, 30, 30, 10.0f, 0.0f, 360.0f, 4, PNTR_RED);
            EQUALS(pntr_test_painted_count(image), 4);

            pntr_rectangle bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);

            // The built in trigonometry is an approximation, so the radius can land a
            // pixel short of 10 in either direction.
            GREATER(bounds.width, 18);
            LESSER(bounds.width, 22);
            GREATER(bounds.height, 18);
            LESSER(bounds.height, 22);

            pntr_unload_image(image);
        });

        IT("pntr_draw_arc() with a zero or negative radius", {
            pntr_image* image = pntr_test_canvas(60, 60);
            NEQUALS(image, NULL);

            // Matches pntr_draw_arc_fill(): a radius with no size draws the center.
            pntr_draw_arc(image, 30, 30, 0.0f, 0.0f, 90.0f, 8, PNTR_RED);
            EQUALS(pntr_test_painted_count(image), 1);
            EQUALS(pntr_test_painted(image, 30, 30), true);

            pntr_unload_image(image);
        });

        IT("pntr_draw_arc() with zero or negative segments", {
            pntr_image* image = pntr_test_canvas(60, 60);
            NEQUALS(image, NULL);

            pntr_draw_arc(image, 30, 30, 20.0f, 0.0f, 90.0f, 0, PNTR_RED);
            pntr_draw_arc(image, 30, 30, 20.0f, 0.0f, 90.0f, -8, PNTR_RED);
            EQUALS(pntr_test_painted_count(image), 0);

            pntr_unload_image(image);
        });

        IT("pntr_draw_arc_thick() grows with its thickness", {
            int thin = 0;
            int thick = 0;

            pntr_image* image = pntr_test_canvas(60, 60);
            NEQUALS(image, NULL);
            pntr_draw_arc_thick(image, 30, 30, 20.0f, 0.0f, 90.0f, 16, 2, PNTR_RED);
            thin = pntr_test_painted_count(image);
            pntr_clear_background(image, PNTR_TEST_BACKGROUND);
            pntr_draw_arc_thick(image, 30, 30, 20.0f, 0.0f, 90.0f, 16, 4, PNTR_RED);
            thick = pntr_test_painted_count(image);

            GREATER(thin, 0);
            GREATER(thick, thin);

            pntr_unload_image(image);
        });

        IT("pntr_draw_arc() with a NULL destination", {
            pntr_draw_arc(NULL, 30, 30, 20.0f, 0.0f, 90.0f, 8, PNTR_RED);
            pntr_draw_arc(NULL, 30, 30, 0.0f, 0.0f, 90.0f, 8, PNTR_RED);
            pntr_draw_arc_thick(NULL, 30, 30, 20.0f, 0.0f, 90.0f, 8, 3, PNTR_RED);
            pntr_draw_arc_thick(NULL, 30, 30, 0.0f, 0.0f, 90.0f, 8, 3, PNTR_RED);
        });
    });

    IT("pntr_get_file_image_type()", {
        EQUALS(pntr_get_file_image_type("myimage.png"), PNTR_IMAGE_TYPE_PNG);
        EQUALS(pntr_get_file_image_type("my/path/ima.ge.png"), PNTR_IMAGE_TYPE_PNG);
        EQUALS(pntr_get_file_image_type("myimage.jpg"), PNTR_IMAGE_TYPE_JPG);
        EQUALS(pntr_get_file_image_type("myimage.jpeg"), PNTR_IMAGE_TYPE_JPG);
        EQUALS(pntr_get_file_image_type("myimage.bmp"), PNTR_IMAGE_TYPE_BMP);
        EQUALS(pntr_get_file_image_type("myimage.exe"), PNTR_IMAGE_TYPE_UNKNOWN);
        EQUALS(pntr_get_file_image_type(NULL), PNTR_IMAGE_TYPE_UNKNOWN);
        EQUALS(pntr_get_file_image_type(""), PNTR_IMAGE_TYPE_UNKNOWN);
    });

    IT("pntr_load_image()", {
        pntr_image* image = pntr_load_image("NotFoundImage.png");
        EQUALS(image, NULL);
        pntr_set_error(PNTR_ERROR_NONE);

        image = pntr_load_image("resources/image.png");
        NEQUALS(image, NULL);
        EQUALS(image->width, 128);
        EQUALS(image->height, 128);
        NEQUALS(image->data, NULL);
        pntr_unload_image(image);
    });

    IT("pntr_load_image_from_memory()", {
        unsigned int bytes;
        unsigned char* fileData = pntr_load_file("resources/image.png", &bytes);

        pntr_image* image = pntr_load_image_from_memory(PNTR_IMAGE_TYPE_PNG, fileData, bytes);
        NEQUALS(image, NULL);
        EQUALS(image->width, 128);
        EQUALS(image->height, 128);

        pntr_unload_image(image);
        pntr_unload_file(fileData);
    });

    IT("pntr_load_font_bmf(), pntr_unload_font(), pntr_draw_text()", {
        pntr_font* font = pntr_load_font_bmf("resources/font.png", PNTR_TEST_BMF_CHARACTERS);
        NEQUALS(font, NULL);
        GREATER(font->charactersLen, 10);

        pntr_image* image = pntr_gen_image_color(200, 200, PNTR_DARKBROWN);
        NEQUALS(image, NULL);
        pntr_draw_text(image, font, "Hello World!", 10, 10, PNTR_WHITE);
        pntr_draw_text_wrapped(image, font, "The quick brown fox jumped over the lazy dog.", 10, 10, 100, PNTR_BLUE);

        pntr_unload_image(image);
        pntr_unload_font(font);
    });

    IT("pntr_measure_text(), pntr_measure_text_ex(), pntr_gen_image_text()", {
        pntr_font* font = pntr_load_font_bmf("resources/font.png", PNTR_TEST_BMF_CHARACTERS);
        GREATER(pntr_measure_text(font, "Hello World!"), 50);
        pntr_vector size = pntr_measure_text_ex(font, "Hello World!", 0);
        GREATER(size.x, 50);
        EQUALS(size.y, font->atlas->height);

        pntr_image* textImage = pntr_gen_image_text(font, "Hello World!", PNTR_WHITE, PNTR_BLANK);
        NEQUALS(textImage, NULL);
        EQUALS(textImage->width, size.x);
        EQUALS(textImage->height, size.y);
        pntr_unload_image(textImage);

        size = pntr_measure_text_ex(font, "On\nNew\nLines", 0);
        EQUALS(size.y, font->atlas->height * 3);

        pntr_unload_font(font);
    });

    IT("pntr_load_font_tty()", {
        // The atlas is 760x8, which is exactly 95 8x8 glyphs.
        pntr_font* font = pntr_load_font_tty("resources/font-tty-8x8.png", 8, 8, PNTR_TEST_TTY_CHARACTERS);
        NEQUALS(font, NULL);
        EQUALS(font->charactersLen, 95);
        pntr_rectangle firstGlyph = PNTR_CLITERAL(pntr_rectangle) {0, 0, 8, 8};
        pntr_rectangle lastGlyph = PNTR_CLITERAL(pntr_rectangle) {94 * 8, 0, 8, 8};
        RECTEQUALS(font->srcRects[0], firstGlyph);
        RECTEQUALS(font->srcRects[94], lastGlyph);
        EQUALS(pntr_measure_text(font, "Hello World!"), 96);
        pntr_unload_font(font);
    });

    IT("pntr_load_font_bmf_from_memory(), pntr_load_font_bmf_from_image()", {
        unsigned int bytes;
        unsigned char* fileData = pntr_load_file("resources/font.png", &bytes);
        NEQUALS(fileData, NULL);

        // Loading from memory matches loading from the file system.
        pntr_font* fromFile = pntr_load_font_bmf("resources/font.png", PNTR_TEST_BMF_CHARACTERS);
        NEQUALS(fromFile, NULL);
        pntr_font* fromMemory = pntr_load_font_bmf_from_memory(fileData, bytes, PNTR_TEST_BMF_CHARACTERS);
        NEQUALS(fromMemory, NULL);
        EQUALS(fromFile->charactersLen, 70);
        EQUALS(pntr_measure_text(fromFile, "Hello World!"), 83);
        EQUALS(fromMemory->charactersLen, fromFile->charactersLen);
        RECTEQUALS(fromMemory->srcRects[0], fromFile->srcRects[0]);
        RECTEQUALS(fromMemory->glyphRects[fromFile->charactersLen - 1], fromFile->glyphRects[fromFile->charactersLen - 1]);

        // Both fonts measure and draw the same text identically.
        pntr_vector sizeFromFile = pntr_measure_text_ex(fromFile, "Hello World!", 0);
        pntr_vector sizeFromMemory = pntr_measure_text_ex(fromMemory, "Hello World!", 0);
        EQUALS(sizeFromMemory.x, sizeFromFile.x);
        EQUALS(sizeFromMemory.y, sizeFromFile.y);

        pntr_image* expected = pntr_gen_image_text(fromFile, "Hello World!", PNTR_WHITE, PNTR_BLANK);
        NEQUALS(expected, NULL);
        pntr_image* actual = pntr_gen_image_text(fromMemory, "Hello World!", PNTR_WHITE, PNTR_BLANK);
        NEQUALS(actual, NULL);
        IMAGEEQUALS(actual, expected);
        pntr_unload_image(actual);
        pntr_unload_image(expected);

        pntr_unload_font(fromMemory);
        pntr_unload_font(fromFile);

        // Asking for more characters than the atlas has glyphs is rejected.
        pntr_font* tooMany = pntr_load_font_bmf_from_memory(fileData, bytes, PNTR_TEST_BMF_CHARACTERS "*=");
        EQUALS(tooMany, NULL);
        EQUALS(pntr_get_error_code(), PNTR_ERROR_INVALID_ARGS);
        pntr_set_error(PNTR_ERROR_NONE);

        // Invalid arguments.
        EQUALS(pntr_load_font_bmf_from_memory(NULL, bytes, PNTR_TEST_BMF_CHARACTERS), NULL);
        EQUALS(pntr_load_font_bmf_from_memory(fileData, 0, PNTR_TEST_BMF_CHARACTERS), NULL);
        EQUALS(pntr_load_font_bmf_from_memory(fileData, bytes, NULL), NULL);
        EQUALS(pntr_load_font_bmf_from_image(NULL, PNTR_TEST_BMF_CHARACTERS), NULL);
        pntr_set_error(PNTR_ERROR_NONE);

        // A rejected pntr_load_font_bmf_from_image() leaves the image to the caller.
        pntr_image* atlas = pntr_load_image_from_memory(PNTR_IMAGE_TYPE_PNG, fileData, bytes);
        NEQUALS(atlas, NULL);
        EQUALS(pntr_load_font_bmf_from_image(atlas, PNTR_TEST_BMF_CHARACTERS "*="), NULL);
        EQUALS(pntr_get_error_code(), PNTR_ERROR_INVALID_ARGS);
        pntr_set_error(PNTR_ERROR_NONE);
        EQUALS(atlas->width, 588);
        pntr_unload_image(atlas);

        pntr_unload_file(fileData);
    });

    IT("pntr_load_font_tty_from_memory(), pntr_load_font_tty_from_image()", {
        unsigned int bytes;
        unsigned char* fileData = pntr_load_file("resources/font-tty-8x8.png", &bytes);
        NEQUALS(fileData, NULL);

        // Loading from memory matches loading from the file system.
        pntr_font* fromFile = pntr_load_font_tty("resources/font-tty-8x8.png", 8, 8, PNTR_TEST_TTY_CHARACTERS);
        NEQUALS(fromFile, NULL);
        pntr_font* fromMemory = pntr_load_font_tty_from_memory(fileData, bytes, 8, 8, PNTR_TEST_TTY_CHARACTERS);
        NEQUALS(fromMemory, NULL);
        EQUALS(fromMemory->charactersLen, fromFile->charactersLen);
        RECTEQUALS(fromMemory->srcRects[94], fromFile->srcRects[94]);

        pntr_image* expected = pntr_gen_image_text(fromFile, "Hello World!", PNTR_WHITE, PNTR_BLANK);
        NEQUALS(expected, NULL);
        pntr_image* actual = pntr_gen_image_text(fromMemory, "Hello World!", PNTR_WHITE, PNTR_BLANK);
        NEQUALS(actual, NULL);
        IMAGEEQUALS(actual, expected);
        pntr_unload_image(actual);
        pntr_unload_image(expected);

        pntr_unload_font(fromMemory);
        pntr_unload_font(fromFile);

        // A glyph that is wider than the atlas has no grid to index into.
        EQUALS(pntr_load_font_tty("resources/font-tty-8x8.png", 1000, 8, "ABC"), NULL);
        EQUALS(pntr_get_error_code(), PNTR_ERROR_INVALID_ARGS);
        pntr_set_error(PNTR_ERROR_NONE);

        // Same for a glyph that is taller than the atlas.
        EQUALS(pntr_load_font_tty("resources/font-tty-8x8.png", 8, 1000, "ABC"), NULL);
        EQUALS(pntr_get_error_code(), PNTR_ERROR_INVALID_ARGS);
        pntr_set_error(PNTR_ERROR_NONE);

        EQUALS(pntr_load_font_tty_from_memory(fileData, bytes, 8, 1000, "ABC"), NULL);
        EQUALS(pntr_get_error_code(), PNTR_ERROR_INVALID_ARGS);
        pntr_set_error(PNTR_ERROR_NONE);

        // Asking for more characters than the atlas has glyphs is rejected.
        EQUALS(pntr_load_font_tty_from_memory(fileData, bytes, 8, 8, PNTR_TEST_TTY_CHARACTERS "~"), NULL);
        EQUALS(pntr_get_error_code(), PNTR_ERROR_INVALID_ARGS);
        pntr_set_error(PNTR_ERROR_NONE);

        // Invalid arguments.
        EQUALS(pntr_load_font_tty_from_memory(fileData, bytes, 0, 8, "ABC"), NULL);
        EQUALS(pntr_load_font_tty_from_memory(fileData, bytes, 8, -8, "ABC"), NULL);
        EQUALS(pntr_load_font_tty_from_memory(fileData, bytes, 8, 8, NULL), NULL);
        EQUALS(pntr_load_font_tty_from_image(NULL, 8, 8, "ABC"), NULL);
        pntr_set_error(PNTR_ERROR_NONE);

        // A rejected pntr_load_font_tty_from_image() leaves the image to the caller.
        pntr_image* atlas = pntr_load_image_from_memory(PNTR_IMAGE_TYPE_PNG, fileData, bytes);
        NEQUALS(atlas, NULL);
        EQUALS(pntr_load_font_tty_from_image(atlas, 8, 16, "ABC"), NULL);
        pntr_set_error(PNTR_ERROR_NONE);
        EQUALS(atlas->height, 8);
        pntr_unload_image(atlas);

        pntr_unload_file(fileData);
    });

    IT("pntr_load_font_ttf_from_memory()", {
        // Data that isn't a font must not produce a font.
        const char* notAFont = "this is not a ttf";
        pntr_font* font = pntr_load_font_ttf_from_memory((const unsigned char*)notAFont, 17, 20);
        EQUALS(font, NULL);
        NEQUALS(pntr_get_error(), NULL);
        pntr_set_error(PNTR_ERROR_NONE);

        // Neither should a truncated font.
        unsigned int bytes;
        unsigned char* fileData = pntr_load_file("resources/tuffy.ttf", &bytes);
        NEQUALS(fileData, NULL);
        GREATER(bytes, 100);

        EQUALS(pntr_load_font_ttf_from_memory(fileData, 64, 20), NULL);
        NEQUALS(pntr_get_error(), NULL);
        pntr_set_error(PNTR_ERROR_NONE);

        // Invalid arguments.
        EQUALS(pntr_load_font_ttf_from_memory(NULL, bytes, 20), NULL);
        EQUALS(pntr_load_font_ttf_from_memory(fileData, 0, 20), NULL);
        EQUALS(pntr_load_font_ttf_from_memory(fileData, bytes, 0), NULL);
        pntr_set_error(PNTR_ERROR_NONE);

        // The real font still loads, and matches the file system loader.
        pntr_font* fromFile = pntr_load_font_ttf("resources/tuffy.ttf", 20);
        NEQUALS(fromFile, NULL);
        pntr_font* fromMemory = pntr_load_font_ttf_from_memory(fileData, bytes, 20);
        NEQUALS(fromMemory, NULL);
        EQUALS(fromMemory->charactersLen, fromFile->charactersLen);
        EQUALS(fromMemory->atlas->width, fromFile->atlas->width);
        EQUALS(fromMemory->atlas->height, fromFile->atlas->height);
        IMAGEEQUALS(fromMemory->atlas, fromFile->atlas);

        pntr_vector sizeFromFile = pntr_measure_text_ex(fromFile, "Hello World!", 0);
        pntr_vector sizeFromMemory = pntr_measure_text_ex(fromMemory, "Hello World!", 0);
        GREATER(sizeFromFile.x, 10);
        EQUALS(sizeFromMemory.x, sizeFromFile.x);
        EQUALS(sizeFromMemory.y, sizeFromFile.y);

        pntr_image* expected = pntr_gen_image_text(fromFile, "Hello World!", PNTR_RED, PNTR_BLANK);
        NEQUALS(expected, NULL);
        pntr_image* actual = pntr_gen_image_text(fromMemory, "Hello World!", PNTR_RED, PNTR_BLANK);
        NEQUALS(actual, NULL);
        IMAGEEQUALS(actual, expected);
        pntr_unload_image(actual);
        pntr_unload_image(expected);

        pntr_unload_font(fromMemory);
        pntr_unload_font(fromFile);
        pntr_unload_file(fileData);
    });

    IT("pntr_draw_text(): unknown characters", {
        // Characters that aren't in the font are skipped rather than indexed.
        pntr_font* font = pntr_load_font_bmf("resources/font.png", PNTR_TEST_BMF_CHARACTERS);
        NEQUALS(font, NULL);

        EQUALS(pntr_measure_text(font, "~~~"), 0);

        pntr_image* image = pntr_gen_image_color(40, 40, PNTR_DARKBROWN);
        NEQUALS(image, NULL);
        pntr_image* expected = pntr_gen_image_color(40, 40, PNTR_DARKBROWN);
        NEQUALS(expected, NULL);
        pntr_draw_text(image, font, "~~~", 0, 0, PNTR_WHITE);
        IMAGEEQUALS(image, expected);

        pntr_unload_image(expected);
        pntr_unload_image(image);
        pntr_unload_font(font);
    });

    IT("pntr_load_font_default()", {
        pntr_font* font = pntr_load_font_default();
        NEQUALS(font, NULL);
        NEQUALS(font->atlas, NULL);
        GREATER(font->charactersLen, 10);
        pntr_unload_font(font);
    });

    IT("pntr_image_resize()", {
        pntr_image* image = pntr_new_image(300, 100);
        NEQUALS(image, NULL);

        IT("pntr_image_resize(PNTR_FILTER_NEARESTNEIGHBOR)", {
            pntr_image* resized = pntr_image_resize(image, 100, 100, PNTR_FILTER_NEARESTNEIGHBOR);
            NEQUALS(resized, NULL);
            EQUALS(resized->width, 100);
            EQUALS(resized->height, 100);
            pntr_unload_image(resized);
        });

        IT("pntr_image_resize(PNTR_FILTER_BILINEAR)", {
            pntr_image* resized = pntr_image_resize(image, 400, 300, PNTR_FILTER_BILINEAR);
            NEQUALS(resized, NULL);
            EQUALS(resized->width, 400);
            EQUALS(resized->height, 300);
            pntr_unload_image(resized);
        });

        pntr_unload_image(image);
    });

    IT("pntr_image_scale()", {
        pntr_image* image = pntr_new_image(100, 200);
        NEQUALS(image, NULL);

        pntr_image* scaled = pntr_image_scale(image, 1.5f, 2.5f, PNTR_FILTER_BILINEAR);
        NEQUALS(scaled, NULL);
        EQUALS(scaled->width, 150);
        EQUALS(scaled->height, 500);
        pntr_unload_image(scaled);

        scaled = pntr_image_scale(image, -2.0f, -3.0f, PNTR_FILTER_NEARESTNEIGHBOR);
        EQUALS(scaled, NULL);
        EQUALS(pntr_get_error_code(), PNTR_ERROR_INVALID_ARGS);

        pntr_unload_image(image);
    });

    IT("pntr_image_copy()", {
        pntr_image* image = pntr_gen_image_color(10, 10, PNTR_RED);
        pntr_draw_point(image, 5, 5, PNTR_BLUE);
        COLOREQUALS(pntr_image_get_color(image, 5, 5), PNTR_BLUE);
        COLOREQUALS(pntr_image_get_color(image, 2, 2), PNTR_RED);

        pntr_image* copy = pntr_image_copy(image);
        NEQUALS(image, copy);
        IMAGEEQUALS(image, copy);
        COLOREQUALS(pntr_image_get_color(copy, 5, 5), PNTR_BLUE);
        COLOREQUALS(pntr_image_get_color(copy, 2, 2), PNTR_RED);

        pntr_unload_image(copy);
        pntr_unload_image(image);
    });

    IT("pntr_image_copy() with a non-square image", {
        // A tall image catches an IMAGEEQUALS that bounds its rows by the width.
        pntr_image* image = pntr_gen_image_color(4, 16, PNTR_RED);
        pntr_draw_point(image, 1, 12, PNTR_BLUE);
        COLOREQUALS(pntr_image_get_color(image, 1, 12), PNTR_BLUE);

        pntr_image* copy = pntr_image_copy(image);
        NEQUALS(copy, NULL);
        IMAGEEQUALS(image, copy);
        COLOREQUALS(pntr_image_get_color(copy, 1, 12), PNTR_BLUE);

        pntr_unload_image(copy);
        pntr_unload_image(image);

        // A wide image covers the other axis.
        pntr_image* wide = pntr_gen_image_color(16, 4, PNTR_RED);
        pntr_draw_point(wide, 12, 1, PNTR_BLUE);
        pntr_image* wideCopy = pntr_image_copy(wide);
        NEQUALS(wideCopy, NULL);
        IMAGEEQUALS(wide, wideCopy);

        pntr_unload_image(wideCopy);
        pntr_unload_image(wide);
    });

    IT("pntr_image_color_replace()", {
        pntr_image* image = pntr_gen_image_color(100, 100, PNTR_BLUE);
        NEQUALS(image, NULL);
        COLOREQUALS(pntr_image_get_color(image, 10, 10), PNTR_BLUE);

        pntr_image_color_replace(image, PNTR_BLUE, PNTR_RED);
        COLOREQUALS(pntr_image_get_color(image, 10, 10), PNTR_RED);

        pntr_unload_image(image);
    });

    IT("pntr_image_color_replace() respects the clip region", {
        pntr_image* image = pntr_gen_image_color(100, 100, PNTR_BLUE);
        NEQUALS(image, NULL);

        // A clip that starts away from column 0 must only affect its own columns.
        pntr_rectangle expectedClip = PNTR_CLITERAL(pntr_rectangle) {50, 0, 10, 100};
        pntr_image_set_clip(image, 50, 0, 10, 100);
        RECTEQUALS(pntr_image_get_clip(image), expectedClip);
        pntr_image_color_replace(image, PNTR_BLUE, PNTR_RED);
        pntr_image_reset_clip(image);

        for (int y = 0; y < image->height; y++) {
            for (int x = 0; x < image->width; x++) {
                pntr_color expected = (x >= 50 && x < 60) ? PNTR_RED : PNTR_BLUE;
                COLOREQUALS(pntr_image_get_color(image, x, y), expected);
            }
        }

        pntr_unload_image(image);
    });

    IT("pntr_color_invert()", {
        pntr_color color = pntr_new_color(21, 16, 171, 255);
        COLOREQUALS(pntr_color_invert(color), pntr_new_color(234, 239, 84, 255));
        color = pntr_new_color(64, 148, 81, 255);
        COLOREQUALS(pntr_color_invert(color), pntr_new_color(191, 107, 174, 255));
    });

    IT("pntr_image_color_invert()", {
        pntr_color color = pntr_new_color(21, 16, 171, 255);
        pntr_color invert = pntr_new_color(234, 239, 84, 255);
        pntr_image* image = pntr_gen_image_color(100, 100, color);
        COLOREQUALS(pntr_image_get_color(image, 10, 10), color);
        pntr_image_color_invert(image);
        COLOREQUALS(pntr_image_get_color(image, 10, 10), invert);
        pntr_unload_image(image);
    });

    IT("pntr_color_tint()", {
        pntr_color color = PNTR_WHITE;
        pntr_color tinted = pntr_color_tint(color, PNTR_RED);
        COLOREQUALS(tinted, PNTR_RED);
    });

    IT("pntr_image_color_tint()", {
        pntr_image* image = pntr_gen_image_color(100, 100, PNTR_WHITE);
        COLOREQUALS(pntr_image_get_color(image, 10, 10), PNTR_WHITE);
        pntr_image_color_tint(image, PNTR_RED);
        COLOREQUALS(pntr_image_get_color(image, 10, 10), PNTR_RED);
        pntr_unload_image(image);
    });

    IT("pntr_color_fade()", {
        pntr_color color = PNTR_RED;
        EQUALS(color.rgba.a, 255);
        EQUALS(color.rgba.r, 230);

        pntr_color faded = pntr_color_fade(color, -0.5f);
        EQUALS(faded.rgba.a, 127);
        EQUALS(faded.rgba.r, 230);

        faded = pntr_color_fade(faded, 0.5f);
        EQUALS(faded.rgba.a, 191);
        EQUALS(faded.rgba.r, 230);
    });

    IT("pntr_image_color_fade()", {
        pntr_color red = PNTR_RED;
        pntr_image* image = pntr_gen_image_color(50, 50, red);
        NEQUALS(image, NULL);
        COLOREQUALS(pntr_image_get_color(image, 10, 10), red);
        pntr_image_color_fade(image, -0.5f);
        red.rgba.a = 127;
        COLOREQUALS(pntr_image_get_color(image, 10, 10), red);
        pntr_unload_image(image);
    });

    IT("pntr_image_color_fade() matches pntr_color_fade()", {
        // The per-image fade must agree with the per-color fade, including for pixels that
        // start out fully transparent.
        pntr_image* image = pntr_gen_image_color(8, 8, PNTR_BLANK);
        NEQUALS(image, NULL);
        pntr_image_color_fade(image, 1.0f);
        COLOREQUALS(pntr_image_get_color(image, 4, 4), pntr_color_fade(PNTR_BLANK, 1.0f));
        EQUALS(pntr_image_get_color(image, 4, 4).rgba.a, 255);
        pntr_unload_image(image);
    });

    IT("pntr_color_brightness()", {
        pntr_color color = pntr_new_color(100, 150, 200, 128);

        // 0.0f is the identity, and the alpha channel is never touched.
        COLOREQUALS(pntr_color_brightness(color, 0.0f), color);

        // Positive factors move each channel towards white, negative towards black.
        pntr_color brighter = pntr_color_brightness(color, 0.5f);
        GREATER(brighter.rgba.r, color.rgba.r);
        GREATER(brighter.rgba.g, color.rgba.g);
        GREATER(brighter.rgba.b, color.rgba.b);
        EQUALS(brighter.rgba.a, color.rgba.a);

        pntr_color darker = pntr_color_brightness(color, -0.5f);
        LESSER(darker.rgba.r, color.rgba.r);
        LESSER(darker.rgba.g, color.rgba.g);
        LESSER(darker.rgba.b, color.rgba.b);
        EQUALS(darker.rgba.a, color.rgba.a);

        // The endpoints saturate to white and black.
        COLOREQUALS(pntr_color_brightness(color, 1.0f), pntr_new_color(255, 255, 255, 128));
        COLOREQUALS(pntr_color_brightness(color, -1.0f), pntr_new_color(0, 0, 0, 128));

        // Out-of-range factors behave as the clamped endpoints.
        COLOREQUALS(pntr_color_brightness(color, 5.0f), pntr_color_brightness(color, 1.0f));
        COLOREQUALS(pntr_color_brightness(color, -5.0f), pntr_color_brightness(color, -1.0f));
    });

    IT("pntr_color_grayscale()", {
        // Every channel ends up equal, and the alpha is preserved.
        pntr_color gray = pntr_color_grayscale(pntr_new_color(100, 150, 200, 128));
        EQUALS(gray.rgba.r, gray.rgba.g);
        EQUALS(gray.rgba.g, gray.rgba.b);
        EQUALS(gray.rgba.a, 128);

        // Grays are unchanged, and the extremes stay at the extremes.
        COLOREQUALS(pntr_color_grayscale(pntr_new_color(60, 60, 60, 255)), pntr_new_color(60, 60, 60, 255));
        COLOREQUALS(pntr_color_grayscale(PNTR_BLACK), PNTR_BLACK);
        COLOREQUALS(pntr_color_grayscale(PNTR_WHITE), PNTR_WHITE);

        // Green is weighted the heaviest, then red, then blue.
        GREATER(pntr_color_grayscale(PNTR_GREEN).rgba.r, pntr_color_grayscale(PNTR_RED).rgba.r);
        GREATER(pntr_color_grayscale(PNTR_RED).rgba.r, pntr_color_grayscale(PNTR_BLUE).rgba.r);
    });

    IT("pntr_color_alpha_blend()", {
        // A fully opaque source replaces the destination outright.
        COLOREQUALS(pntr_color_alpha_blend(PNTR_BLUE, PNTR_RED), PNTR_RED);

        // Blending onto a fully transparent destination is just the source.
        COLOREQUALS(pntr_color_alpha_blend(PNTR_BLANK, PNTR_RED), PNTR_RED);

        // A partially transparent source pulls the destination towards it, and raises alpha.
        pntr_color half = pntr_color_fade(PNTR_RED, -0.5f);
        EQUALS(half.rgba.a, 127);
        pntr_color blended = pntr_color_alpha_blend(pntr_color_fade(PNTR_BLUE, -0.5f), half);
        GREATER(blended.rgba.r, PNTR_BLUE.rgba.r);
        GREATER(blended.rgba.a, 127);
        LESSER(blended.rgba.b, PNTR_BLUE.rgba.b);
    });

    IT("pntr_color_contrast() is the identity at 0.0f", {
        // The documented range is -1.0f to 1.0f, so 0.0f must leave the color alone. The
        // old formula multiplied by (contrast * contrast + contrast), which is 0 at 0.0f,
        // flattening every color to mid-gray.
        COLOREQUALS(pntr_color_contrast(PNTR_RED, 0.0f), PNTR_RED);
        COLOREQUALS(pntr_color_contrast(PNTR_WHITE, 0.0f), PNTR_WHITE);
        COLOREQUALS(pntr_color_contrast(PNTR_BLACK, 0.0f), PNTR_BLACK);
        COLOREQUALS(pntr_color_contrast(PNTR_BLUE, 0.0f), PNTR_BLUE);

        // Every channel value round-trips, including the low values where computing in
        // 0.0f-1.0f space used to lose a step to rounding.
        for (int i = 0; i < 256; i++) {
            pntr_color color = pntr_new_color((unsigned char)i, (unsigned char)i, (unsigned char)i, 255);
            COLOREQUALS(pntr_color_contrast(color, 0.0f), color);
        }
    });

    IT("pntr_color_contrast()", {
        pntr_color color = PNTR_RED;
        EQUALS(color.rgba.r, 230);

        // -1.0f gives a multiplier of 0, collapsing every channel to mid-gray.
        pntr_color flat = pntr_color_contrast(color, -1.0f);
        COLOREQUALS(flat, pntr_new_color(127, 127, 127, 255));
        COLOREQUALS(pntr_color_contrast(PNTR_WHITE, -1.0f), flat);
        COLOREQUALS(pntr_color_contrast(PNTR_BLACK, -1.0f), flat);

        // Increasing contrast spreads channels away from mid-gray, decreasing pulls them
        // in. Use a color near mid-gray so that no channel saturates and the ordering is
        // strict. Red sits above mid-gray and green below it, so they move opposite ways.
        pntr_color mid = pntr_new_color(140, 110, 130, 255);
        pntr_color midFlat = pntr_color_contrast(mid, -1.0f);
        pntr_color less = pntr_color_contrast(mid, -0.5f);
        pntr_color more = pntr_color_contrast(mid, 0.5f);
        pntr_color most = pntr_color_contrast(mid, 1.0f);

        // -1.0f < -0.5f < 0.0f < 0.5f < 1.0f pushes red steadily further up.
        GREATER(less.rgba.r, midFlat.rgba.r);
        GREATER(mid.rgba.r, less.rgba.r);
        GREATER(more.rgba.r, mid.rgba.r);
        GREATER(most.rgba.r, more.rgba.r);

        // ... and green, which starts below mid-gray, steadily further down.
        LESSER(less.rgba.g, midFlat.rgba.g);
        LESSER(mid.rgba.g, less.rgba.g);
        LESSER(more.rgba.g, mid.rgba.g);
        LESSER(most.rgba.g, more.rgba.g);

        // The alpha channel is never touched.
        EQUALS(less.rgba.a, 255);
        EQUALS(more.rgba.a, 255);
        EQUALS(most.rgba.a, 255);
        EQUALS(pntr_color_contrast(pntr_new_color(10, 20, 30, 64), 0.5f).rgba.a, 64);

        // Out-of-range values behave as the clamped endpoints.
        COLOREQUALS(pntr_color_contrast(mid, 5.0f), most);
        COLOREQUALS(pntr_color_contrast(mid, -5.0f), midFlat);
    });

    IT("pntr_image_color_contrast()", {
        pntr_image* image = pntr_gen_image_color(20, 20, PNTR_RED);
        NEQUALS(image, NULL);
        pntr_draw_rectangle_fill(image, 2, 2, 5, 5, PNTR_BLUE);

        // 0.0f must leave the image pixel-identical.
        pntr_image* expected = pntr_image_copy(image);
        NEQUALS(expected, NULL);
        pntr_image_color_contrast(image, 0.0f);
        IMAGEEQUALS(image, expected);
        pntr_unload_image(expected);

        // -1.0f collapses the whole clip region to mid-gray.
        pntr_image_color_contrast(image, -1.0f);
        COLOREQUALS(pntr_image_get_color(image, 0, 0), pntr_new_color(127, 127, 127, 255));
        COLOREQUALS(pntr_image_get_color(image, 4, 4), pntr_new_color(127, 127, 127, 255));

        pntr_unload_image(image);
    });

    IT("pntr_image_color_contrast() respects the clip region", {
        pntr_image* image = pntr_gen_image_color(20, 20, PNTR_RED);
        NEQUALS(image, NULL);

        pntr_image_set_clip(image, 10, 0, 5, 20);
        pntr_image_color_contrast(image, -1.0f);
        pntr_image_reset_clip(image);

        COLOREQUALS(pntr_image_get_color(image, 9, 5), PNTR_RED);
        COLOREQUALS(pntr_image_get_color(image, 10, 5), pntr_new_color(127, 127, 127, 255));
        COLOREQUALS(pntr_image_get_color(image, 14, 5), pntr_new_color(127, 127, 127, 255));
        COLOREQUALS(pntr_image_get_color(image, 15, 5), PNTR_RED);

        pntr_unload_image(image);
    });

    IT("pntr_load_file(), pntr_unload_file()", {
        unsigned int bytesRead;
        unsigned char* fileData = pntr_load_file("resources/text.txt", &bytesRead);

        GREATER(bytesRead, 5);
        STRCEQUALS((const char*)fileData, "Hello", 5);
        pntr_unload_file(fileData);

        // Try to load a file that doesn't exist.
        bytesRead = 1234;
        unsigned char* fileNotFound = pntr_load_file("FileNotFound.txt", &bytesRead);
        EQUALS(fileNotFound, NULL);
        EQUALS(bytesRead, 0);
        EQUALS(pntr_get_error_code(), PNTR_ERROR_FAILED_TO_OPEN);

        // Reporting the size back is optional.
        fileNotFound = pntr_load_file("FileNotFound.txt", NULL);
        EQUALS(fileNotFound, NULL);

        // Expect an error to result.
        const char* error = pntr_get_error();
        NEQUALS(error, NULL);
        pntr_set_error(PNTR_ERROR_NONE);
    });

    IT("pntr_load_file(): Empty file", {
        // A file that exists but holds no data loads successfully.
        const char* fileName = "tempFileEmpty.txt";
        FILE* emptyFile = fopen(fileName, "wb");
        NEQUALS(emptyFile, NULL);
        fclose(emptyFile);

        unsigned int bytesRead = 1234;
        unsigned char* fileData = pntr_load_file(fileName, &bytesRead);
        NEQUALS(fileData, NULL);
        EQUALS(bytesRead, 0);
        EQUALS(pntr_get_error(), NULL);
        pntr_unload_file(fileData);

        remove(fileName);
    });

    IT("pntr_load_file(): Directory", {
        // A directory can't be loaded, but it isn't a lack of memory either.
        unsigned int bytesRead = 1234;
        unsigned char* fileData = pntr_load_file(".", &bytesRead);
        EQUALS(fileData, NULL);
        EQUALS(bytesRead, 0);
        NEQUALS(pntr_get_error_code(), PNTR_ERROR_NONE);
        NEQUALS(pntr_get_error_code(), PNTR_ERROR_NO_MEMORY);
        pntr_set_error(PNTR_ERROR_NONE);
    });

    IT("pntr_load_file_text()", {
        const char* text = pntr_load_file_text("resources/text.txt");
        STRCEQUALS(text, "Hello, World!", 13);
        pntr_unload_file_text(text);
    });

    IT("pntr_load_file_text(): Empty file", {
        // Unlike pntr_load_file(), the text is always null terminated.
        const char* fileName = "tempFileEmptyText.txt";
        FILE* emptyFile = fopen(fileName, "wb");
        NEQUALS(emptyFile, NULL);
        fclose(emptyFile);

        const char* text = pntr_load_file_text(fileName);
        NEQUALS(text, NULL);
        STREQUALS(text, "");
        pntr_unload_file_text(text);

        remove(fileName);
    });

    IT("pntr_load_font_ttf()", {
        pntr_font* font = pntr_load_font_ttf("resources/tuffy.ttf", 20);
        NEQUALS(font, NULL);
        GREATER(font->charactersLen, 20);

        pntr_image* canvas = pntr_gen_image_text(font, "Hello World!", PNTR_RED, PNTR_BLANK);
        NEQUALS(canvas, NULL);
        GREATER(canvas->width, 10);
        GREATER(canvas->height, 10);

        IT("pntr_measure_text_ex()", {
            pntr_vector size = pntr_measure_text_ex(font, "Hello!!", 0);
            GREATER(size.x, 20);
            GREATER(size.y, 5);
        });

        pntr_unload_image(canvas);
        pntr_unload_font(font);
    });

    IT("pntr_save_file()", {
        const char* fileName = "tempFile.txt";
        const char* fileData = "Hello World!";
        unsigned int bytes = 12;
        bool result = pntr_save_file(fileName, (unsigned char*)fileData, bytes);
        EQUALS(result, true);

        unsigned char* fileDataResult = pntr_load_file(fileName, &bytes);
        GREATER(bytes, 5);
        STRCEQUALS((const char*)fileDataResult, "Hello", 5);
        pntr_unload_file(fileDataResult);

        remove(fileName);
    });

    IT("pntr_save_file(): No data", {
        // Saving nothing still creates and truncates the file, which is a success.
        const char* fileName = "tempFileNoData.txt";
        const char* fileData = "Hello World!";

        bool result = pntr_save_file(fileName, (unsigned char*)fileData, 12);
        EQUALS(result, true);

        result = pntr_save_file(fileName, (unsigned char*)fileData, 0);
        EQUALS(result, true);
        EQUALS(pntr_get_error_code(), PNTR_ERROR_NONE);

        // The file still exists, and is now empty.
        unsigned int bytes = 1234;
        unsigned char* fileDataResult = pntr_load_file(fileName, &bytes);
        NEQUALS(fileDataResult, NULL);
        EQUALS(bytes, 0);
        pntr_unload_file(fileDataResult);

        remove(fileName);
    });

    IT("pntr_save_file(): Failures", {
        const char* fileData = "Hello World!";

        // A file in a directory that doesn't exist can't be opened.
        bool result = pntr_save_file("MissingDirectory/tempFile.txt", (unsigned char*)fileData, 12);
        EQUALS(result, false);
        EQUALS(pntr_get_error_code(), PNTR_ERROR_FAILED_TO_OPEN);
        pntr_set_error(PNTR_ERROR_NONE);

        // A file that opens, but that can't hold what is written to it, reports the
        // write as the thing that failed.
        const char* unwritableFile = pntr_test_unwritable_file();
        if (unwritableFile != NULL) {
            FILE* exists = fopen(unwritableFile, "wb");
            if (exists != NULL) {
                fclose(exists);

                result = pntr_save_file(unwritableFile, (unsigned char*)fileData, 12);
                EQUALS(result, false);
                EQUALS(pntr_get_error_code(), PNTR_ERROR_FAILED_TO_WRITE);
                pntr_set_error(PNTR_ERROR_NONE);
            }
        }
    });

    IT("pntr_save_file(), pntr_load_file(): Round trip", {
        const char* fileName = "tempFileRoundTrip.bin";
        unsigned char fileData[4096];
        for (int i = 0; i < 4096; i++) {
            fileData[i] = (unsigned char)(i % 256);
        }

        unsigned int sizes[3];
        sizes[0] = 0;
        sizes[1] = 1;
        sizes[2] = 4096;

        for (int i = 0; i < 3; i++) {
            bool result = pntr_save_file(fileName, fileData, sizes[i]);
            EQUALS(result, true);

            unsigned int bytes = 1234;
            unsigned char* fileDataResult = pntr_load_file(fileName, &bytes);
            NEQUALS(fileDataResult, NULL);
            EQUALS(bytes, sizes[i]);
            EQUALS(memcmp(fileDataResult, fileData, sizes[i]), 0);
            pntr_unload_file(fileDataResult);
        }

        EQUALS(pntr_get_error_code(), PNTR_ERROR_NONE);
        remove(fileName);
    });

    IT("pntr_save_image()", {
        int width = 400;
        int height = 300;
        pntr_image* saveImage = pntr_gen_image_color(width, height, PNTR_RED);
        NEQUALS(saveImage, NULL);
        pntr_draw_circle_fill(saveImage, 200, 150, 80, PNTR_BLUE);
        pntr_draw_rectangle_fill(saveImage, 10, 10, 20, 20, PNTR_GREEN);
        bool result = pntr_save_image(saveImage, "saveImage.png");
        EQUALS(result, true);
        pntr_unload_image(saveImage);

        pntr_image* loadedImage = pntr_load_image("saveImage.png");
        NEQUALS(loadedImage, NULL);
        EQUALS(loadedImage->width, 400);
        EQUALS(loadedImage->height, height);
        COLOREQUALS(pntr_image_get_color(loadedImage, 15, 15), PNTR_GREEN);
        pntr_unload_image(loadedImage);
    });

    if (pntr_bmp()) {
        IT("pntr_save_image(): .bmp", {
            pntr_image* saveImage = pntr_gen_image_color(64, 64, PNTR_RED);
            NEQUALS(saveImage, NULL);
            pntr_draw_rectangle_fill(saveImage, 8, 8, 16, 16, PNTR_GREEN);

            bool result = pntr_save_image(saveImage, "saveImage.bmp");
            EQUALS(result, true);

            // A 64x64 RGBA bitmap is a 14 byte file header, a 108 byte V4 info
            // header, then 32 bits per pixel.
            unsigned int expectedSize = 14 + 108 + (64 * 64 * 4);

            unsigned int bytesRead = 0;
            unsigned char* fileData = pntr_load_file("saveImage.bmp", &bytesRead);
            NEQUALS(fileData, NULL);
            EQUALS(bytesRead, expectedSize);

            // The "BM" magic, along with the file size and pixel offset that the header declares.
            EQUALS(fileData[0], 'B');
            EQUALS(fileData[1], 'M');
            unsigned int declaredSize = (unsigned int)fileData[2] | ((unsigned int)fileData[3] << 8) |
                ((unsigned int)fileData[4] << 16) | ((unsigned int)fileData[5] << 24);
            EQUALS(declaredSize, expectedSize);
            unsigned int pixelOffset = (unsigned int)fileData[10] | ((unsigned int)fileData[11] << 8) |
                ((unsigned int)fileData[12] << 16) | ((unsigned int)fileData[13] << 24);
            EQUALS(pixelOffset, 14 + 108);

            // Saving to memory has to produce the exact same thing as the file.
            unsigned int memorySize = 0;
            unsigned char* memoryData = pntr_save_image_to_memory(saveImage, PNTR_IMAGE_TYPE_BMP, &memorySize);
            NEQUALS(memoryData, NULL);
            EQUALS(memorySize, bytesRead);
            EQUALS(memcmp(memoryData, fileData, (size_t)memorySize), 0);
            pntr_unload_memory(memoryData);

            // The bitmap has to load back in, pixel for pixel.
            pntr_image* loadedImage = pntr_load_image("saveImage.bmp");
            NEQUALS(loadedImage, NULL);
            EQUALS(loadedImage->width, 64);
            EQUALS(loadedImage->height, 64);
            COLOREQUALS(pntr_image_get_color(loadedImage, 0, 0), PNTR_RED);
            COLOREQUALS(pntr_image_get_color(loadedImage, 10, 10), PNTR_GREEN);
            IMAGEEQUALS(loadedImage, saveImage);

            pntr_unload_image(loadedImage);
            pntr_unload_file(fileData);
            pntr_unload_image(saveImage);
            remove("saveImage.bmp");
        });
    }
    else {
        IT("pntr_save_image(): .bmp: the PNG-only backend does not support BMP", {
            pntr_image* saveImage = pntr_gen_image_color(64, 64, PNTR_RED);
            NEQUALS(saveImage, NULL);

            // A PNG-only backend has to refuse the bitmap rather than write one.
            unsigned int memorySize = 100;
            unsigned char* memoryData = pntr_save_image_to_memory(saveImage, PNTR_IMAGE_TYPE_BMP, &memorySize);
            EQUALS(memoryData, NULL);
            EQUALS(memorySize, 0);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
            pntr_set_error(PNTR_ERROR_NONE);

            EQUALS(pntr_save_image(saveImage, "saveImage.bmp"), false);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
            pntr_set_error(PNTR_ERROR_NONE);

            pntr_unload_image(saveImage);
        });
    }

    IT("pntr_save_image_to_memory()", {
        pntr_image* image = pntr_gen_image_color(64, 64, PNTR_BLUE);
        NEQUALS(image, NULL);
        pntr_draw_rectangle_fill(image, 4, 4, 10, 10, PNTR_WHITE);

        // PNG is the default, and must still work the same as it always has.
        unsigned int pngSize = 0;
        unsigned char* png = pntr_save_image_to_memory(image, PNTR_IMAGE_TYPE_PNG, &pngSize);
        NEQUALS(png, NULL);
        GREATER(pngSize, 8);
        EQUALS(png[0], 0x89);
        EQUALS(png[1], 'P');
        EQUALS(png[2], 'N');
        EQUALS(png[3], 'G');

        pntr_image* loadedPng = pntr_load_image_from_memory(PNTR_IMAGE_TYPE_PNG, png, pngSize);
        NEQUALS(loadedPng, NULL);
        IMAGEEQUALS(loadedPng, image);
        pntr_unload_image(loadedPng);
        pntr_unload_memory(png);

        // An unknown type falls back to PNG.
        unsigned int unknownSize = 0;
        unsigned char* unknown = pntr_save_image_to_memory(image, PNTR_IMAGE_TYPE_UNKNOWN, &unknownSize);
        NEQUALS(unknown, NULL);
        EQUALS(unknownSize, pngSize);
        pntr_unload_memory(unknown);

        // An unsupported type reports an error rather than handing back a buffer.
        unsigned int badSize = 100;
        unsigned char* bad = pntr_save_image_to_memory(image, (pntr_image_type)100, &badSize);
        EQUALS(bad, NULL);
        EQUALS(badSize, 0);
        EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
        pntr_set_error(PNTR_ERROR_NONE);

        // A NULL image is invalid.
        unsigned char* nothing = pntr_save_image_to_memory(NULL, PNTR_IMAGE_TYPE_PNG, NULL);
        EQUALS(nothing, NULL);
        EQUALS(pntr_get_error_code(), PNTR_ERROR_INVALID_ARGS);
        pntr_set_error(PNTR_ERROR_NONE);

        pntr_unload_image(image);
    });

    if (pntr_jpeg()) {
        IT("pntr_save_image(): .jpg", {
            pntr_image* image = pntr_gen_image_color(64, 64, PNTR_BLUE);
            NEQUALS(image, NULL);
            pntr_draw_rectangle_fill(image, 4, 4, 10, 10, PNTR_WHITE);

            // JPEG is lossy, so only the markers and a plausible size are checked.
            unsigned int jpgSize = 0;
            unsigned char* jpg = pntr_save_image_to_memory(image, PNTR_IMAGE_TYPE_JPG, &jpgSize);
            NEQUALS(jpg, NULL);
            GREATER(jpgSize, 600);
            EQUALS(jpg[0], 0xFF);
            EQUALS(jpg[1], 0xD8);
            EQUALS(jpg[jpgSize - 2], 0xFF);
            EQUALS(jpg[jpgSize - 1], 0xD9);

            // Saving a JPEG file has to match what was saved to memory.
            bool result = pntr_save_image(image, "saveImage.jpg");
            EQUALS(result, true);

            unsigned int bytesRead = 0;
            unsigned char* fileData = pntr_load_file("saveImage.jpg", &bytesRead);
            NEQUALS(fileData, NULL);
            EQUALS(bytesRead, jpgSize);
            EQUALS(memcmp(fileData, jpg, (size_t)jpgSize), 0);
            pntr_unload_file(fileData);

            pntr_image* loadedJpg = pntr_load_image_from_memory(PNTR_IMAGE_TYPE_JPG, jpg, jpgSize);
            NEQUALS(loadedJpg, NULL);
            EQUALS(loadedJpg->width, 64);
            EQUALS(loadedJpg->height, 64);
            pntr_unload_image(loadedJpg);

            pntr_unload_memory(jpg);
            pntr_unload_image(image);
            remove("saveImage.jpg");
        });
    }
    else {
        IT("pntr_save_image(): .jpg: PNTR_ENABLE_JPEG not enabled", {
            // Nothing
        });
    }

    IT("pntr_get_pixel_data_size()", {
        EQUALS(pntr_get_pixel_data_size(1, 1, PNTR_PIXELFORMAT_RGBA8888), 4);
        EQUALS(pntr_get_pixel_data_size(2, 3, PNTR_PIXELFORMAT_RGBA8888), 24);
        EQUALS(pntr_get_pixel_data_size(3, 2, PNTR_PIXELFORMAT_ARGB8888), 24);
        EQUALS(pntr_get_pixel_data_size(4, 4, PNTR_PIXELFORMAT_GRAYSCALE), 16);
    });

    IT("pntr_set_pixel_color(), pntr_get_pixel_color()", {
        // These colors are built through pntr_new_color() so that the test says nothing
        // about how pntr_color is laid out in memory, which depends on the build's
        // PNTR_PIXELFORMAT.
        pntr_color colors[5];
        colors[0] = pntr_new_color(255, 0, 0, 255);
        colors[1] = pntr_new_color(0, 255, 0, 255);
        colors[2] = pntr_new_color(10, 20, 30, 40);
        colors[3] = PNTR_TEST_PIXEL_COLOR;
        colors[4] = pntr_new_color(0, 0, 0, 0);

        for (int i = 0; i < 5; i++) {
            unsigned char pixel[4];

            // RGBA8888 has to round trip exactly, in every build.
            PNTR_MEMSET(pixel, 0, sizeof(pixel));
            pntr_set_pixel_color(pixel, PNTR_PIXELFORMAT_RGBA8888, colors[i]);
            COLOREQUALS(pntr_get_pixel_color(pixel, PNTR_PIXELFORMAT_RGBA8888), colors[i]);

            // ARGB8888 has to round trip exactly too. It used to lose the channel order
            // in a PNTR_PIXELFORMAT_ARGB build, where opaque red came back as
            // transparent cyan.
            PNTR_MEMSET(pixel, 0, sizeof(pixel));
            pntr_set_pixel_color(pixel, PNTR_PIXELFORMAT_ARGB8888, colors[i]);
            COLOREQUALS(pntr_get_pixel_color(pixel, PNTR_PIXELFORMAT_ARGB8888), colors[i]);
        }

        // Grayscale is one byte per pixel, so it only keeps the luminance. The setter
        // writes that luminance and the getter reports white with the luminance as its
        // alpha, which makes white the one color that round trips exactly.
        unsigned char gray = 0;
        pntr_set_pixel_color(&gray, PNTR_PIXELFORMAT_GRAYSCALE, pntr_new_color(255, 255, 255, 255));
        EQUALS((int)gray, 255);
        COLOREQUALS(pntr_get_pixel_color(&gray, PNTR_PIXELFORMAT_GRAYSCALE), pntr_new_color(255, 255, 255, 255));

        pntr_set_pixel_color(&gray, PNTR_PIXELFORMAT_GRAYSCALE, pntr_new_color(255, 0, 0, 255));
        EQUALS((int)gray, 76);
        COLOREQUALS(pntr_get_pixel_color(&gray, PNTR_PIXELFORMAT_GRAYSCALE), pntr_new_color(255, 255, 255, 76));

        pntr_set_pixel_color(&gray, PNTR_PIXELFORMAT_GRAYSCALE, pntr_new_color(0, 255, 0, 255));
        EQUALS((int)gray, 149);
        COLOREQUALS(pntr_get_pixel_color(&gray, PNTR_PIXELFORMAT_GRAYSCALE), pntr_new_color(255, 255, 255, 149));

        pntr_set_pixel_color(&gray, PNTR_PIXELFORMAT_GRAYSCALE, pntr_new_color(0, 0, 255, 255));
        EQUALS((int)gray, 29);
        COLOREQUALS(pntr_get_pixel_color(&gray, PNTR_PIXELFORMAT_GRAYSCALE), pntr_new_color(255, 255, 255, 29));
    });

    IT("pntr_set_pixel_color() byte layout", {
        // A pixel format names the order of the bytes on the wire, so the bytes that
        // pntr_set_pixel_color() writes are the same in every build, on every host.
        pntr_color color = PNTR_TEST_PIXEL_COLOR;
        unsigned char pixel[4];

        PNTR_MEMSET(pixel, 0, sizeof(pixel));
        pntr_set_pixel_color(pixel, PNTR_PIXELFORMAT_RGBA8888, color);
        EQUALS((int)pixel[0], 18);  // red
        EQUALS((int)pixel[1], 52);  // green
        EQUALS((int)pixel[2], 86);  // blue
        EQUALS((int)pixel[3], 120); // alpha

        PNTR_MEMSET(pixel, 0, sizeof(pixel));
        pntr_set_pixel_color(pixel, PNTR_PIXELFORMAT_ARGB8888, color);
        EQUALS((int)pixel[0], 120); // alpha
        EQUALS((int)pixel[1], 18);  // red
        EQUALS((int)pixel[2], 52);  // green
        EQUALS((int)pixel[3], 86);  // blue

        // pntr_color's own bytes, on the other hand, are ordered by the build's
        // PNTR_PIXELFORMAT. Storing the struct is therefore never a valid shortcut for
        // pntr_set_pixel_color(): it only lines up with RGBA8888 in a
        // PNTR_PIXELFORMAT_RGBA build, and lines up with nothing at all otherwise.
        unsigned char raw[4];
        unsigned char expected[4] = PNTR_TEST_PIXEL_COLOR_BYTES;
        PNTR_MEMCPY(raw, &color, sizeof(raw));
        EQUALS((int)raw[0], (int)expected[0]);
        EQUALS((int)raw[1], (int)expected[1]);
        EQUALS((int)raw[2], (int)expected[2]);
        EQUALS((int)raw[3], (int)expected[3]);
    });

    IT("pntr_image_to_pixelformat(), pntr_image_from_pixelformat()", {
        pntr_image* image = pntr_gen_image_color(4, 3, pntr_new_color(200, 100, 50, 255));
        NEQUALS(image, NULL);

        // Write the pixels directly so that they are exactly these colors, including a
        // partially transparent one and a fully transparent one.
        PNTR_PIXEL(image, 0, 0) = pntr_new_color(255, 0, 0, 255);
        PNTR_PIXEL(image, 1, 1) = pntr_new_color(0, 255, 0, 128);
        PNTR_PIXEL(image, 3, 2) = pntr_new_color(1, 2, 3, 0);

        pntr_pixelformat formats[2];
        formats[0] = PNTR_PIXELFORMAT_RGBA8888;
        formats[1] = PNTR_PIXELFORMAT_ARGB8888;

        for (int i = 0; i < 2; i++) {
            unsigned int dataSize = 0;
            void* data = pntr_image_to_pixelformat(image, &dataSize, formats[i]);
            NEQUALS(data, NULL);
            EQUALS((int)dataSize, 4 * 3 * 4);

            pntr_image* result = pntr_image_from_pixelformat(data, image->width, image->height, formats[i]);
            IMAGEEQUALS(result, image);

            pntr_unload_image(result);
            pntr_unload_memory(data);
        }

        // Grayscale only carries the luminance, so it comes back as white with the
        // luminance as the alpha. A white image is the one that survives it untouched.
        pntr_image* white = pntr_gen_image_color(4, 3, pntr_new_color(255, 255, 255, 255));
        NEQUALS(white, NULL);

        unsigned int graySize = 0;
        void* grayData = pntr_image_to_pixelformat(white, &graySize, PNTR_PIXELFORMAT_GRAYSCALE);
        NEQUALS(grayData, NULL);
        EQUALS((int)graySize, 4 * 3);

        pntr_image* grayResult = pntr_image_from_pixelformat(grayData, white->width, white->height, PNTR_PIXELFORMAT_GRAYSCALE);
        IMAGEEQUALS(grayResult, white);

        pntr_unload_image(grayResult);
        pntr_unload_memory(grayData);
        pntr_unload_image(white);
        pntr_unload_image(image);
    });

    IT("pntr_image_alpha_border(), pntr_image_alpha_crop()", {
        pntr_image* image = pntr_gen_image_color(400, 400, PNTR_BLANK);
        NEQUALS(image, NULL);
        EQUALS(image->width, 400);
        EQUALS(image->height, 400);

        pntr_draw_rectangle_fill(image, 100, 100, 200, 200, PNTR_BLUE);

        pntr_rectangle crop = pntr_image_alpha_border(image, 0);
        EQUALS(crop.x, 100);
        EQUALS(crop.y, 100);
        EQUALS(crop.width, 200);
        EQUALS(crop.height, 200);

        pntr_image_alpha_crop(image, 0);
        NEQUALS(image, NULL);
        EQUALS(image->width, 200);
        EQUALS(image->height, 200);

        COLOREQUALS(pntr_image_get_color(image, 50, 50), PNTR_BLUE);

        pntr_unload_image(image);
    });

    IT("pntr_image_crop()", {
        pntr_image* image = pntr_gen_image_color(200, 200, PNTR_RED);
        NEQUALS(image, NULL);
        pntr_image_crop(image, 10, 30, 20, 50);
        NEQUALS(image, NULL);
        EQUALS(image->width, 20);
        EQUALS(image->height, 50);
        COLOREQUALS(pntr_image_get_color(image, 10, 20), PNTR_RED);
        pntr_unload_image(image);
    });

    IT("pntr_image_flip()", {
        pntr_image* image = pntr_gen_image_color(100, 100, PNTR_RED);
        NEQUALS(image, NULL);
        pntr_draw_rectangle_fill(image, 0, 0, 20, 20, PNTR_BLUE);
        COLOREQUALS(pntr_image_get_color(image, 10, 10), PNTR_BLUE);
        COLOREQUALS(pntr_image_get_color(image, 90, 10), PNTR_RED);
        COLOREQUALS(pntr_image_get_color(image, 10, 90), PNTR_RED);
        COLOREQUALS(pntr_image_get_color(image, 90, 90), PNTR_RED);
        pntr_image_flip(image, true, true);
        COLOREQUALS(pntr_image_get_color(image, 10, 10), PNTR_RED);
        COLOREQUALS(pntr_image_get_color(image, 90, 10), PNTR_RED);
        COLOREQUALS(pntr_image_get_color(image, 10, 90), PNTR_RED);
        COLOREQUALS(pntr_image_get_color(image, 90, 90), PNTR_BLUE);
        pntr_unload_image(image);
    });

    IT("pntr_image_resize_canvas()", {
        pntr_image* image = pntr_gen_image_color(200, 200, PNTR_BLUE);
        NEQUALS(image, NULL);
        EQUALS(image->width, 200);
        EQUALS(image->height, 200);
        pntr_image_resize_canvas(image, 400, 400, 100, 100, PNTR_RED);
        NEQUALS(image, NULL);
        EQUALS(image->width, 400);
        EQUALS(image->height, 400);
        COLOREQUALS(pntr_image_get_color(image, 50, 50), PNTR_RED);
        COLOREQUALS(pntr_image_get_color(image, 150, 150), PNTR_BLUE);
        pntr_rectangle expectedClip = {0, 0, 400, 400};
        RECTEQUALS(pntr_image_get_clip(image), expectedClip);
        pntr_unload_image(image);
    });

    IT("pntr_image_resize_canvas() preserves custom clip", {
        pntr_image* image = pntr_gen_image_color(200, 200, PNTR_BLUE);
        pntr_image_set_clip(image, 10, 10, 50, 50);
        pntr_image_resize_canvas(image, 300, 300, 20, 20, PNTR_RED);
        pntr_rectangle expectedCustomClip = {30, 30, 50, 50};
        RECTEQUALS(pntr_image_get_clip(image), expectedCustomClip);
        pntr_unload_image(image);
    });

    IT("pntr_image_rotate()", {
        pntr_image* image = pntr_gen_image_color(40, 30, PNTR_BLUE);
        NEQUALS(image, NULL);
        pntr_draw_rectangle_fill(image, 9, 9, 3, 3, PNTR_RED);

        IT("pntr_image_rotate(image, 0.0f)", {
            pntr_image* rotated = pntr_image_rotate(image, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR);
            NEQUALS(rotated, NULL);
            EQUALS(rotated->width, 40);
            EQUALS(rotated->height, 30);
            pntr_unload_image(rotated);
        });

        IT("pntr_image_rotate(image, 90.0f)", {
            pntr_image* rotated = pntr_image_rotate(image, 90.0f, PNTR_FILTER_BILINEAR);
            NEQUALS(rotated, NULL);
            EQUALS(rotated->width, image->height);
            EQUALS(rotated->height, image->width);
            COLOREQUALS(pntr_image_get_color(rotated, 10, 10), PNTR_BLUE);
            COLOREQUALS(pntr_image_get_color(rotated, 10, 30), PNTR_RED);
            pntr_unload_image(rotated);
        });

        IT("pntr_image_rotate(image, 180.0f)", {
            pntr_image* rotated = pntr_image_rotate(image, 180.0f, PNTR_FILTER_BILINEAR);
            NEQUALS(rotated, NULL);
            EQUALS(rotated->width, image->width);
            EQUALS(rotated->height, image->height);
            COLOREQUALS(pntr_image_get_color(rotated, 10, 10), PNTR_BLUE);
            COLOREQUALS(pntr_image_get_color(rotated, 30, 20), PNTR_RED);
            pntr_unload_image(rotated);
        });

        IT("pntr_image_rotate(image, 270.0f)", {
            pntr_image* rotated = pntr_image_rotate(image, 270.0f, PNTR_FILTER_NEARESTNEIGHBOR);
            NEQUALS(rotated, NULL);
            EQUALS(rotated->width, image->height);
            EQUALS(rotated->height, image->width);
            COLOREQUALS(pntr_image_get_color(rotated, 10, 10), PNTR_BLUE);
            COLOREQUALS(pntr_image_get_color(rotated, 20, 10), PNTR_RED);
            pntr_unload_image(rotated);
        });

        IT("pntr_image_rotate(image, 48.0f)", {
            pntr_image* rotated = pntr_image_rotate(image, 48.0f, PNTR_FILTER_BILINEAR);
            NEQUALS(rotated, NULL);
            NEQUALS(rotated->width, image->height);
            NEQUALS(rotated->height, image->width);
            COLOREQUALS(pntr_image_get_color(rotated, 5, 5), PNTR_BLANK);
            COLOREQUALS(pntr_image_get_color(rotated, rotated->width / 2, rotated->height / 2), PNTR_BLUE);
            pntr_unload_image(rotated);
        });

        IT("pntr_draw_image_rotated_rec() keeps the source rectangle in bounds", {
            pntr_image* dst = pntr_gen_image_color(60, 60, PNTR_BLANK);
            NEQUALS(dst, NULL);

            IT("with a sprite sheet rectangle that runs off the right edge", {
                // A 16x16 sprite rect at x=10 on a 20x16 sheet only has 10 columns left.
                pntr_image* sheet = pntr_gen_image_color(20, 16, PNTR_RED);
                NEQUALS(sheet, NULL);
                pntr_rectangle sheetRect = {10, 0, 16, 16};

                pntr_draw_image_rotated_rec(dst, sheet, sheetRect, 4, 4, 90.0f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR);
                COLOREQUALS(pntr_image_get_color(dst, 4, 4), PNTR_RED);

                pntr_draw_image_rotated_rec(dst, sheet, sheetRect, 4, 4, 180.0f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR);
                pntr_draw_image_rotated_rec(dst, sheet, sheetRect, 4, 4, 270.0f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR);
                pntr_draw_image_rotated_rec(dst, sheet, sheetRect, 20, 20, 45.0f, 0.0f, 0.0f, PNTR_FILTER_BILINEAR);
                pntr_draw_image_rotated_rec(dst, sheet, sheetRect, 20, 20, 45.0f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR);
                COLOREQUALS(pntr_image_get_color(dst, 25, 25), PNTR_RED);

                pntr_unload_image(sheet);
            });

            IT("with a source rectangle that is partially outside the source", {
                pntr_image* src = pntr_gen_image_color(10, 10, PNTR_RED);
                NEQUALS(src, NULL);
                pntr_rectangle partial = {5, 5, 10, 10};

                pntr_draw_image_rotated_rec(dst, src, partial, 30, 0, 180.0f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR);
                pntr_draw_image_rotated_rec(dst, src, partial, 30, 0, 270.0f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR);
                pntr_draw_image_rotated_rec(dst, src, partial, 30, 20, 45.0f, 0.0f, 0.0f, PNTR_FILTER_BILINEAR);
                pntr_draw_image_rotated_rec(dst, src, partial, 30, 20, 45.0f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR);

                pntr_draw_image_rotated_rec(dst, src, partial, 30, 0, 90.0f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR);
                COLOREQUALS(pntr_image_get_color(dst, 30, 0), PNTR_RED);

                pntr_unload_image(src);
            });

            IT("with a source rectangle that is entirely outside the source", {
                pntr_image* src = pntr_gen_image_color(10, 10, PNTR_RED);
                NEQUALS(src, NULL);
                pntr_image* untouched = pntr_gen_image_color(20, 20, PNTR_GREEN);
                NEQUALS(untouched, NULL);
                pntr_image* expected = pntr_gen_image_color(20, 20, PNTR_GREEN);
                NEQUALS(expected, NULL);
                pntr_rectangle outside = {20, 20, 5, 5};

                pntr_draw_image_rotated_rec(untouched, src, outside, 0, 0, 90.0f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR);
                pntr_draw_image_rotated_rec(untouched, src, outside, 0, 0, 45.0f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR);
                IMAGEEQUALS(untouched, expected);

                pntr_unload_image(expected);
                pntr_unload_image(untouched);
                pntr_unload_image(src);
            });

            pntr_unload_image(dst);
        });

        IT("pntr_image_rotate(image, -360.0f) is a full rotation", {
            // A negative multiple of 360 normalizes to 0, so the image is unchanged.
            pntr_image* rotated = pntr_image_rotate(image, -360.0f, PNTR_FILTER_NEARESTNEIGHBOR);
            NEQUALS(rotated, NULL);
            EQUALS(rotated->width, image->width);
            EQUALS(rotated->height, image->height);
            IMAGEEQUALS(rotated, image);
            pntr_unload_image(rotated);

            pntr_image* rotatedTwice = pntr_image_rotate(image, -720.0f, PNTR_FILTER_NEARESTNEIGHBOR);
            NEQUALS(rotatedTwice, NULL);
            EQUALS(rotatedTwice->width, image->width);
            EQUALS(rotatedTwice->height, image->height);
            IMAGEEQUALS(rotatedTwice, image);
            pntr_unload_image(rotatedTwice);
        });

        IT("pntr_image_rotate(image, -90.0f) normalizes to 270 degrees", {
            pntr_image* rotated = pntr_image_rotate(image, -90.0f, PNTR_FILTER_NEARESTNEIGHBOR);
            NEQUALS(rotated, NULL);
            EQUALS(rotated->width, image->height);
            EQUALS(rotated->height, image->width);
            pntr_unload_image(rotated);
        });

        IT("pntr_image_rotate() maps the corners of a non-square image", {
            // A distinct color in every corner, so the exact mapping is verified
            // rather than just the pixel being non-blank.
            pntr_image* corners = pntr_gen_image_color(7, 4, PNTR_BLUE);
            NEQUALS(corners, NULL);
            pntr_draw_rectangle_fill(corners, 0, 0, 1, 1, PNTR_RED);
            pntr_draw_rectangle_fill(corners, 6, 0, 1, 1, PNTR_GREEN);
            pntr_draw_rectangle_fill(corners, 0, 3, 1, 1, PNTR_GOLD);
            pntr_draw_rectangle_fill(corners, 6, 3, 1, 1, PNTR_WHITE);

            IT("90 degrees puts the source's top right corner at (0, 0)", {
                pntr_image* rotated = pntr_image_rotate(corners, 90.0f, PNTR_FILTER_NEARESTNEIGHBOR);
                NEQUALS(rotated, NULL);
                EQUALS(rotated->width, 4);
                EQUALS(rotated->height, 7);
                COLOREQUALS(pntr_image_get_color(rotated, 0, 0), PNTR_GREEN);
                COLOREQUALS(pntr_image_get_color(rotated, 3, 0), PNTR_WHITE);
                COLOREQUALS(pntr_image_get_color(rotated, 0, 6), PNTR_RED);
                COLOREQUALS(pntr_image_get_color(rotated, 3, 6), PNTR_GOLD);
                pntr_unload_image(rotated);
            });

            IT("180 degrees puts the source's bottom right corner at (0, 0)", {
                pntr_image* rotated = pntr_image_rotate(corners, 180.0f, PNTR_FILTER_NEARESTNEIGHBOR);
                NEQUALS(rotated, NULL);
                EQUALS(rotated->width, 7);
                EQUALS(rotated->height, 4);
                COLOREQUALS(pntr_image_get_color(rotated, 0, 0), PNTR_WHITE);
                COLOREQUALS(pntr_image_get_color(rotated, 6, 0), PNTR_GOLD);
                COLOREQUALS(pntr_image_get_color(rotated, 0, 3), PNTR_GREEN);
                COLOREQUALS(pntr_image_get_color(rotated, 6, 3), PNTR_RED);
                pntr_unload_image(rotated);
            });

            IT("270 degrees puts the source's bottom left corner at (0, 0)", {
                pntr_image* rotated = pntr_image_rotate(corners, 270.0f, PNTR_FILTER_NEARESTNEIGHBOR);
                NEQUALS(rotated, NULL);
                EQUALS(rotated->width, 4);
                EQUALS(rotated->height, 7);
                COLOREQUALS(pntr_image_get_color(rotated, 0, 0), PNTR_GOLD);
                COLOREQUALS(pntr_image_get_color(rotated, 3, 0), PNTR_RED);
                COLOREQUALS(pntr_image_get_color(rotated, 0, 6), PNTR_WHITE);
                COLOREQUALS(pntr_image_get_color(rotated, 3, 6), PNTR_GREEN);
                pntr_unload_image(rotated);
            });

            IT("four 90 degree rotations return the original image", {
                pntr_image* rotated = pntr_image_copy(corners);
                NEQUALS(rotated, NULL);
                for (int i = 0; i < 4; i++) {
                    pntr_image* next = pntr_image_rotate(rotated, 90.0f, PNTR_FILTER_NEARESTNEIGHBOR);
                    NEQUALS(next, NULL);
                    pntr_unload_image(rotated);
                    rotated = next;
                }
                IMAGEEQUALS(rotated, corners);
                pntr_unload_image(rotated);
            });

            IT("two 180 degree rotations return the original image", {
                pntr_image* once = pntr_image_rotate(corners, 180.0f, PNTR_FILTER_NEARESTNEIGHBOR);
                NEQUALS(once, NULL);
                pntr_image* twice = pntr_image_rotate(once, 180.0f, PNTR_FILTER_NEARESTNEIGHBOR);
                NEQUALS(twice, NULL);
                IMAGEEQUALS(twice, corners);
                pntr_unload_image(once);
                pntr_unload_image(twice);
            });

            IT("four 270 degree rotations return the original image", {
                pntr_image* rotated = pntr_image_copy(corners);
                NEQUALS(rotated, NULL);
                for (int i = 0; i < 4; i++) {
                    pntr_image* next = pntr_image_rotate(rotated, 270.0f, PNTR_FILTER_NEARESTNEIGHBOR);
                    NEQUALS(next, NULL);
                    pntr_unload_image(rotated);
                    rotated = next;
                }
                IMAGEEQUALS(rotated, corners);
                pntr_unload_image(rotated);
            });

            IT("90 and 270 degrees are inverses of each other", {
                pntr_image* rotated = pntr_image_rotate(corners, 90.0f, PNTR_FILTER_NEARESTNEIGHBOR);
                NEQUALS(rotated, NULL);
                pntr_image* back = pntr_image_rotate(rotated, 270.0f, PNTR_FILTER_NEARESTNEIGHBOR);
                NEQUALS(back, NULL);
                IMAGEEQUALS(back, corners);
                pntr_unload_image(rotated);
                pntr_unload_image(back);
            });

            pntr_unload_image(corners);
        });

        IT("pntr_image_rotate(image, 90.0f) keeps the source's first column", {
            // The first column of the source becomes the last row of the result.
            pntr_image* source = pntr_gen_image_color(7, 4, PNTR_BLUE);
            NEQUALS(source, NULL);
            pntr_draw_rectangle_fill(source, 0, 0, 1, 4, PNTR_RED);

            pntr_image* rotated = pntr_image_rotate(source, 90.0f, PNTR_FILTER_NEARESTNEIGHBOR);
            NEQUALS(rotated, NULL);
            EQUALS(rotated->width, 4);
            EQUALS(rotated->height, 7);

            for (int x = 0; x < rotated->width; x++) {
                COLOREQUALS(pntr_image_get_color(rotated, x, rotated->height - 1), PNTR_RED);
            }

            // Every pixel of the result is written, so no row is left blank.
            for (int y = 0; y < rotated->height; y++) {
                for (int x = 0; x < rotated->width; x++) {
                    NEQUALS(pntr_image_get_color(rotated, x, y).value, PNTR_BLANK.value);
                }
            }

            pntr_unload_image(rotated);
            pntr_unload_image(source);
        });

        IT("pntr_draw_image_rotozoom()", {
            IT("pntr_draw_image_rotozoom() with no rotation delegates to the scaled drawing path", {
                // Source image: blue, with a red column from x=6 to x=9.
                // The source rectangle has x != y to catch mixing up srcRect.x and srcRect.y.
                pntr_image* src = pntr_gen_image_color(10, 10, PNTR_BLUE);
                NEQUALS(src, NULL);
                pntr_draw_rectangle_fill(src, 6, 0, 4, 10, PNTR_RED);
                pntr_rectangle srcRect = {6, 0, 4, 4};

                IT("with the bilinear filter", {
                    pntr_image* dst = pntr_gen_image_color(20, 20, PNTR_GREEN);
                    NEQUALS(dst, NULL);
                    pntr_draw_image_rotozoom(dst, src, srcRect, 2, 2, 0.0f, 2.0f, 2.0f, 0.0f, 0.0f, PNTR_FILTER_BILINEAR, PNTR_WHITE);
                    COLOREQUALS(pntr_image_get_color(dst, 2, 2), PNTR_RED);
                    COLOREQUALS(pntr_image_get_color(dst, 5, 5), PNTR_RED);
                    COLOREQUALS(pntr_image_get_color(dst, 9, 9), PNTR_RED);
                    COLOREQUALS(pntr_image_get_color(dst, 0, 0), PNTR_GREEN);
                    COLOREQUALS(pntr_image_get_color(dst, 10, 10), PNTR_GREEN);
                    pntr_unload_image(dst);
                });

                IT("with the nearest neighbor filter", {
                    pntr_image* dst = pntr_gen_image_color(20, 20, PNTR_GREEN);
                    NEQUALS(dst, NULL);
                    pntr_draw_image_rotozoom(dst, src, srcRect, 2, 2, 0.0f, 2.0f, 2.0f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
                    COLOREQUALS(pntr_image_get_color(dst, 2, 2), PNTR_RED);
                    COLOREQUALS(pntr_image_get_color(dst, 9, 9), PNTR_RED);
                    COLOREQUALS(pntr_image_get_color(dst, 0, 0), PNTR_GREEN);
                    pntr_unload_image(dst);
                });

                pntr_unload_image(src);
            });

            IT("pntr_draw_image_rotozoom() with a 180 degree rotation", {
                // Source image: blue, with a 2x2 red square in the top left corner.
                pntr_image* src = pntr_gen_image_color(10, 10, PNTR_BLUE);
                NEQUALS(src, NULL);
                pntr_draw_rectangle_fill(src, 0, 0, 2, 2, PNTR_RED);

                pntr_image* dst = pntr_gen_image_color(12, 12, PNTR_GREEN);
                NEQUALS(dst, NULL);
                pntr_rectangle srcRect = {0, 0, 10, 10};

                // The origin is a pivot, so the way to fill the top left 10x10 of the
                // destination is to spin the center of the source onto the center of it.
                pntr_draw_image_rotozoom(dst, src, srcRect, 5, 5, 180.0f, 1.0f, 1.0f, 4.5f, 4.5f, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);

                // After a 180 degree rotation, the red square lands in the bottom right of the drawn area.
                COLOREQUALS(pntr_image_get_color(dst, 9, 9), PNTR_RED);
                COLOREQUALS(pntr_image_get_color(dst, 2, 2), PNTR_BLUE);
                COLOREQUALS(pntr_image_get_color(dst, 11, 11), PNTR_GREEN);

                pntr_unload_image(dst);
                pntr_unload_image(src);
            });

            IT("pntr_draw_image_rotozoom() with a negative full rotation is unrotated", {
                // Source image: blue, with a red column from x=6 to x=9, so that a flip shows.
                pntr_image* src = pntr_gen_image_color(10, 10, PNTR_BLUE);
                NEQUALS(src, NULL);
                pntr_draw_rectangle_fill(src, 6, 0, 4, 10, PNTR_RED);
                pntr_rectangle srcRect = {0, 0, 10, 10};

                // An exact negative multiple of 360 is a full rotation, which is 0 rather
                // than 360, so it has to take the same unrotated path that 0 degrees does
                // instead of running the general rotation on a no-op angle.
                IT("with the nearest neighbor filter", {
                    pntr_image* expected = pntr_gen_image_color(20, 20, PNTR_GREEN);
                    NEQUALS(expected, NULL);
                    pntr_draw_image_rotozoom(expected, src, srcRect, 4, 4, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);

                    pntr_image* once = pntr_gen_image_color(20, 20, PNTR_GREEN);
                    NEQUALS(once, NULL);
                    pntr_draw_image_rotozoom(once, src, srcRect, 4, 4, -360.0f, 1.0f, 1.0f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
                    IMAGEEQUALS(once, expected);
                    pntr_unload_image(once);

                    pntr_image* twice = pntr_gen_image_color(20, 20, PNTR_GREEN);
                    NEQUALS(twice, NULL);
                    pntr_draw_image_rotozoom(twice, src, srcRect, 4, 4, -720.0f, 1.0f, 1.0f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
                    IMAGEEQUALS(twice, expected);
                    pntr_unload_image(twice);

                    pntr_unload_image(expected);
                });

                IT("with the bilinear filter", {
                    pntr_image* expected = pntr_gen_image_color(20, 20, PNTR_GREEN);
                    NEQUALS(expected, NULL);
                    pntr_draw_image_rotozoom(expected, src, srcRect, 4, 4, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, PNTR_FILTER_BILINEAR, PNTR_WHITE);

                    pntr_image* once = pntr_gen_image_color(20, 20, PNTR_GREEN);
                    NEQUALS(once, NULL);
                    pntr_draw_image_rotozoom(once, src, srcRect, 4, 4, -360.0f, 1.0f, 1.0f, 0.0f, 0.0f, PNTR_FILTER_BILINEAR, PNTR_WHITE);
                    IMAGEEQUALS(once, expected);
                    pntr_unload_image(once);

                    pntr_image* twice = pntr_gen_image_color(20, 20, PNTR_GREEN);
                    NEQUALS(twice, NULL);
                    pntr_draw_image_rotozoom(twice, src, srcRect, 4, 4, -720.0f, 1.0f, 1.0f, 0.0f, 0.0f, PNTR_FILTER_BILINEAR, PNTR_WHITE);
                    IMAGEEQUALS(twice, expected);
                    pntr_unload_image(twice);

                    pntr_unload_image(expected);
                });

                pntr_unload_image(src);
            });

            IT("pntr_draw_image_rotozoom() ignores invalid arguments", {
                pntr_image* src = pntr_gen_image_color(10, 10, PNTR_BLUE);
                NEQUALS(src, NULL);
                pntr_image* dst = pntr_gen_image_color(10, 10, PNTR_GREEN);
                NEQUALS(dst, NULL);
                pntr_rectangle srcRect = {0, 0, 10, 10};

                // NULL arguments do not crash.
                pntr_draw_image_rotozoom(NULL, src, srcRect, 0, 0, 45.0f, 1.0f, 1.0f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
                pntr_draw_image_rotozoom(dst, NULL, srcRect, 0, 0, 45.0f, 1.0f, 1.0f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);

                // A zero scale draws nothing.
                pntr_draw_image_rotozoom(dst, src, srcRect, 0, 0, 45.0f, 0.0f, 0.0f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
                COLOREQUALS(pntr_image_get_color(dst, 5, 5), PNTR_GREEN);

                // A negative scale with no rotation draws nothing.
                pntr_draw_image_rotozoom(dst, src, srcRect, 0, 0, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
                COLOREQUALS(pntr_image_get_color(dst, 5, 5), PNTR_GREEN);

                // A negative scale with a rotation does not crash.
                pntr_draw_image_rotozoom(dst, src, srcRect, 0, 0, 45.0f, -1.0f, -1.0f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);

                // A source rectangle with no width or height uses the full image.
                pntr_image* dst2 = pntr_gen_image_color(10, 10, PNTR_GREEN);
                NEQUALS(dst2, NULL);
                pntr_rectangle emptyRect = {0, 0, 0, 0};
                pntr_draw_image_rotozoom(dst2, src, emptyRect, 0, 0, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
                COLOREQUALS(pntr_image_get_color(dst2, 0, 0), PNTR_BLUE);
                COLOREQUALS(pntr_image_get_color(dst2, 9, 9), PNTR_BLUE);
                pntr_unload_image(dst2);

                pntr_unload_image(dst);
                pntr_unload_image(src);
            });
        });

        IT("the rotation offset is a pivot", {
            // A 40x24 source with its pivot near the left edge and halfway down, which is the
            // shape of offset that the rotation paths used to disagree about. A centered
            // pivot is the one case they already agreed on, so it can't tell them apart.
            pntr_image* pivotSource = pntr_test_pivot_source(40, 24, 2, 12);
            NEQUALS(pivotSource, NULL);
            pntr_rectangle pivotRect = {0, 0, 40, 24};

            // 0, 180 and the quarter turns each have a drawing path of their own, and 45, 135
            // and 359 fall to the general rotation.
            float pivotAngles[7] = {0.0f, 45.0f, 90.0f, 135.0f, 180.0f, 270.0f, 359.0f};

            IT("pntr_draw_image_rotated_rec() lands its offset on the position", {
                for (int i = 0; i < 7; i++) {
                    pntr_image* dst = pntr_gen_image_color(160, 160, PNTR_GREEN);
                    NEQUALS(dst, NULL);

                    pntr_draw_image_rotated_rec(dst, pivotSource, pivotRect, 80, 80, pivotAngles[i], 2.0f, 12.0f, PNTR_FILTER_NEARESTNEIGHBOR);

                    // Rasterizing the marker can land it a pixel out, but no further.
                    LESSER(pntr_test_pivot_offset(dst, 80, 80), 2);

                    pntr_unload_image(dst);
                }
            });

            IT("pntr_draw_image_rotozoom() lands its origin on the position", {
                for (int i = 0; i < 7; i++) {
                    pntr_image* dst = pntr_gen_image_color(160, 160, PNTR_GREEN);
                    NEQUALS(dst, NULL);

                    pntr_draw_image_rotozoom(dst, pivotSource, pivotRect, 80, 80, pivotAngles[i], 1.0f, 1.0f, 2.0f, 12.0f, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);

                    LESSER(pntr_test_pivot_offset(dst, 80, 80), 2);

                    pntr_unload_image(dst);
                }
            });

            IT("pntr_draw_image_rotozoom() lands its origin on the position when scaled", {
                for (int i = 0; i < 7; i++) {
                    pntr_image* dst = pntr_gen_image_color(240, 240, PNTR_GREEN);
                    NEQUALS(dst, NULL);

                    // A scale of 2 makes the marker 6x6, so its center can sit a pixel and a
                    // half away from the pivot it grew out of.
                    pntr_draw_image_rotozoom(dst, pivotSource, pivotRect, 120, 120, pivotAngles[i], 2.0f, 2.0f, 2.0f, 12.0f, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);

                    LESSER(pntr_test_pivot_offset(dst, 120, 120), 3);

                    pntr_unload_image(dst);
                }
            });

            IT("pntr_draw_image_rotated_rec() moves continuously past the quarter turns", {
                // Each quarter turn has a drawing path of its own, and the general rotation
                // runs a thousandth of a degree either side of it. Both have to arrive at the
                // same place, or a sprite jumps as it spins past the exact angle.
                float corners[5] = {0.0f, 90.0f, 180.0f, 270.0f, 360.0f};

                for (int i = 0; i < 5; i++) {
                    pntr_rectangle before = pntr_test_rotated_bounds(pivotSource, corners[i] - 0.001f, 2.0f, 12.0f);
                    pntr_rectangle exact = pntr_test_rotated_bounds(pivotSource, corners[i], 2.0f, 12.0f);
                    pntr_rectangle after = pntr_test_rotated_bounds(pivotSource, corners[i] + 0.001f, 2.0f, 12.0f);

                    // Something was actually drawn, so an empty rectangle can't pass as still.
                    GREATER(exact.width, 0);
                    GREATER(exact.height, 0);

                    LESSER(pntr_test_rectangle_distance(before, exact), 2);
                    LESSER(pntr_test_rectangle_distance(exact, after), 2);
                }
            });

            IT("pntr_draw_image_rotated_rec() and pntr_draw_image_rotozoom() agree", {
                pntr_filter filters[2] = {PNTR_FILTER_NEARESTNEIGHBOR, PNTR_FILTER_BILINEAR};

                for (int f = 0; f < 2; f++) {
                    for (int i = 0; i < 7; i++) {
                        pntr_image* rotated = pntr_gen_image_color(160, 160, PNTR_GREEN);
                        NEQUALS(rotated, NULL);
                        pntr_image* zoomed = pntr_gen_image_color(160, 160, PNTR_GREEN);
                        NEQUALS(zoomed, NULL);

                        pntr_draw_image_rotated_rec(rotated, pivotSource, pivotRect, 80, 80, pivotAngles[i], 2.0f, 12.0f, filters[f]);
                        pntr_draw_image_rotozoom(zoomed, pivotSource, pivotRect, 80, 80, pivotAngles[i], 1.0f, 1.0f, 2.0f, 12.0f, filters[f], PNTR_WHITE);

                        // At a scale of 1 and the same pivot the two draw the very same
                        // pixels, at every angle: the quarter turns take the same shortcut,
                        // and every other angle shares the general rotation and the pivot
                        // derived from it.
                        pntr_rectangle rotatedBounds = pntr_test_painted_bounds(rotated, PNTR_GREEN);
                        GREATER(rotatedBounds.width, 0);
                        IMAGEEQUALS(zoomed, rotated);

                        pntr_unload_image(zoomed);
                        pntr_unload_image(rotated);
                    }
                }

                // A scale other than one has no unscaled draw to be identical to, so there
                // it keeps the looser bound instead: the drawn area still moves by at most a
                // pixel across a quarter turn, which is what stops a scaled sprite from
                // jumping as it spins past the exact angle.
                float corners[5] = {0.0f, 90.0f, 180.0f, 270.0f, 360.0f};
                for (int i = 0; i < 5; i++) {
                    pntr_rectangle before = pntr_test_rotozoom_bounds(pivotSource, corners[i] - 0.001f, 2.0f, 12.0f, 2.0f);
                    pntr_rectangle exact = pntr_test_rotozoom_bounds(pivotSource, corners[i], 2.0f, 12.0f, 2.0f);
                    pntr_rectangle after = pntr_test_rotozoom_bounds(pivotSource, corners[i] + 0.001f, 2.0f, 12.0f, 2.0f);

                    GREATER(exact.width, 0);
                    LESSER(pntr_test_rectangle_distance(before, exact), 2);
                    LESSER(pntr_test_rectangle_distance(exact, after), 2);
                }

                // And at a scale of one the new shortcut has to be continuous with the
                // general rotation either side of it, the same way the unscaled path is.
                for (int i = 0; i < 5; i++) {
                    pntr_rectangle before = pntr_test_rotozoom_bounds(pivotSource, corners[i] - 0.001f, 2.0f, 12.0f, 1.0f);
                    pntr_rectangle exact = pntr_test_rotozoom_bounds(pivotSource, corners[i], 2.0f, 12.0f, 1.0f);
                    pntr_rectangle after = pntr_test_rotozoom_bounds(pivotSource, corners[i] + 0.001f, 2.0f, 12.0f, 1.0f);

                    GREATER(exact.width, 0);
                    LESSER(pntr_test_rectangle_distance(before, exact), 2);
                    LESSER(pntr_test_rectangle_distance(exact, after), 2);
                }
            });

            IT("pntr_draw_image_rotated_rec() and pntr_draw_image_rotozoom() match away from the quarter turns", {
                // 45 degrees hands both of them to their general rotation, where a shared
                // pivot has to produce not just the same placement but the same pixels.
                pntr_image* rotated = pntr_gen_image_color(160, 160, PNTR_GREEN);
                NEQUALS(rotated, NULL);
                pntr_image* zoomed = pntr_gen_image_color(160, 160, PNTR_GREEN);
                NEQUALS(zoomed, NULL);

                pntr_draw_image_rotated_rec(rotated, pivotSource, pivotRect, 80, 80, 45.0f, 2.0f, 12.0f, PNTR_FILTER_NEARESTNEIGHBOR);
                pntr_draw_image_rotozoom(zoomed, pivotSource, pivotRect, 80, 80, 45.0f, 1.0f, 1.0f, 2.0f, 12.0f, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
                IMAGEEQUALS(zoomed, rotated);

                pntr_unload_image(zoomed);
                pntr_unload_image(rotated);
            });

            IT("pntr_draw_image_rotated_rec() with a centered offset spins in place", {
                // The case every convention already agreed on, kept here as a guard: a pivot
                // at the center of the source keeps the drawn area centered on the position,
                // whatever the angle.
                pntr_image* centered = pntr_test_pivot_source(40, 24, 20, 12);
                NEQUALS(centered, NULL);

                for (int i = 0; i < 7; i++) {
                    pntr_rectangle bounds = pntr_test_rotated_bounds(centered, pivotAngles[i], 20.0f, 12.0f);
                    GREATER(bounds.width, 0);

                    int centerX = bounds.x + bounds.width / 2 - 80;
                    int centerY = bounds.y + bounds.height / 2 - 80;
                    if (centerX < 0) {
                        centerX = -centerX;
                    }
                    if (centerY < 0) {
                        centerY = -centerY;
                    }

                    LESSER(centerX, 2);
                    LESSER(centerY, 2);
                }

                pntr_unload_image(centered);
            });

            pntr_unload_image(pivotSource);
        });

        IT("pntr_gen_image_gradient", {
            pntr_image* image = pntr_gen_image_gradient(500, 500, PNTR_RED, PNTR_GREEN, PNTR_BLUE, PNTR_GOLD);
            NEQUALS(image, NULL);

            pntr_color red = pntr_image_get_color(image, 0, 0);
            COLOREQUALS(red, PNTR_RED);
            pntr_color green = pntr_image_get_color(image, image->width - 1, 0);
            GREATER(green.rgba.g, 220);
            pntr_color blue = pntr_image_get_color(image, 0, image->height - 1);
            GREATER(blue.rgba.b, 230);
            pntr_color gold = pntr_image_get_color(image, image->width - 1, image->height - 1);
            GREATER(gold.rgba.r, 230);
            GREATER(gold.rgba.g, 180);
            pntr_unload_image(image);
        });

        pntr_unload_image(image);
    });

    IT("pntr_font_copy()", {
        pntr_font* font = pntr_load_font_default();
        NEQUALS(font, NULL);

        pntr_font* copy = pntr_font_copy(font);
        NEQUALS(copy, NULL);

        EQUALS(font->charactersLen, copy->charactersLen);
        EQUALS(font->atlas->width, copy->atlas->width);
        EQUALS(font->atlas->height, copy->atlas->height);
        NEQUALS(font->atlas, copy->atlas);

        pntr_unload_font(copy);
        pntr_unload_font(font);
    });

    IT("pntr_font_scale()", {
        pntr_font* font = pntr_load_font_default();
        NEQUALS(font, NULL);

        int scaleX = 5;
        int scaleY = 2;
        pntr_font* resized = pntr_font_scale(font, (float)scaleX, (float)scaleY, PNTR_FILTER_BILINEAR);
        NEQUALS(resized, NULL);

        EQUALS(font->charactersLen, resized->charactersLen);
        EQUALS(font->atlas->width * scaleX, resized->atlas->width);
        EQUALS(font->atlas->height * scaleY, resized->atlas->height);
        EQUALS(resized->glyphRects[0].width, font->glyphRects[0].width * scaleX);
        NEQUALS(font->atlas, resized->atlas);

        pntr_unload_font(font);
        pntr_unload_font(resized);
    });

    IT("pntr_image_subimage", {
        pntr_image* image = pntr_gen_image_color(300, 300, PNTR_RED);
        pntr_draw_rectangle_fill(image, 100, 100, 100, 100, PNTR_BLUE);

        COLOREQUALS(pntr_image_get_color(image, 50, 50), PNTR_RED);
        COLOREQUALS(pntr_image_get_color(image, 150, 150), PNTR_BLUE);

        pntr_image* subimage = pntr_image_subimage(image, 100, 100, 100, 100);
        COLOREQUALS(pntr_image_get_color(subimage, 50, 50), PNTR_BLUE);

        pntr_unload_image(subimage);
        pntr_unload_image(image);
    });

    IT("pntr_clear_background() on a whole image", {
        pntr_image* image = pntr_new_image(37, 11);
        NEQUALS(image, NULL);

        // A whole image's pitch matches its row width, which is what allows
        // pntr_clear_background() to replicate rows by width rather than pitch.
        EQUALS(image->pitch, image->width * (int)sizeof(pntr_color));

        pntr_clear_background(image, PNTR_RED);
        for (int y = 0; y < image->height; y++) {
            for (int x = 0; x < image->width; x++) {
                COLOREQUALS(pntr_image_get_color(image, x, y), PNTR_RED);
            }
        }

        // The white fast path.
        pntr_clear_background(image, PNTR_WHITE);
        for (int y = 0; y < image->height; y++) {
            for (int x = 0; x < image->width; x++) {
                COLOREQUALS(pntr_image_get_color(image, x, y), PNTR_WHITE);
            }
        }

        // The blank fast path.
        pntr_clear_background(image, PNTR_BLANK);
        for (int y = 0; y < image->height; y++) {
            for (int x = 0; x < image->width; x++) {
                COLOREQUALS(pntr_image_get_color(image, x, y), PNTR_BLANK);
            }
        }

        pntr_unload_image(image);
    });

    IT("pntr_clear_background() on a subimage", {
        // Give every pixel of the parent a color of its own. A uniformly
        // colored parent would hide the damage, since copying a full parent
        // pitch per row wraps onto the next row and would just copy the same
        // color back over itself.
        pntr_image* parent = pntr_new_image(100, 100);
        NEQUALS(parent, NULL);
        for (int y = 0; y < parent->height; y++) {
            for (int x = 0; x < parent->width; x++) {
                pntr_draw_point(parent, x, y, pntr_new_color((unsigned char)x, (unsigned char)y, 128, 255));
            }
        }

        pntr_image* subimage = pntr_image_subimage(parent, 20, 30, 50, 10);
        NEQUALS(subimage, NULL);
        EQUALS(subimage->width, 50);
        EQUALS(subimage->height, 10);

        // A subimage shares the pitch of its parent, so the pitch is wider
        // than the subimage's own rows.
        EQUALS(subimage->pitch, parent->pitch);
        EQUALS((int)subimage->subimage, 1);

        pntr_clear_background(subimage, PNTR_RED);

        // Every pixel inside the subimage rectangle takes the new color.
        for (int y = 0; y < subimage->height; y++) {
            for (int x = 0; x < subimage->width; x++) {
                COLOREQUALS(pntr_image_get_color(subimage, x, y), PNTR_RED);
                COLOREQUALS(pntr_image_get_color(parent, 20 + x, 30 + y), PNTR_RED);
            }
        }

        // Every pixel of the parent outside the rectangle is left alone.
        for (int y = 0; y < parent->height; y++) {
            for (int x = 0; x < parent->width; x++) {
                if (x >= 20 && x < 70 && y >= 30 && y < 40) {
                    continue;
                }
                COLOREQUALS(pntr_image_get_color(parent, x, y),
                    pntr_new_color((unsigned char)x, (unsigned char)y, 128, 255));
            }
        }

        // The columns immediately to the left and right of the rectangle.
        COLOREQUALS(pntr_image_get_color(parent, 19, 30), pntr_new_color(19, 30, 128, 255));
        COLOREQUALS(pntr_image_get_color(parent, 19, 39), pntr_new_color(19, 39, 128, 255));
        COLOREQUALS(pntr_image_get_color(parent, 70, 30), pntr_new_color(70, 30, 128, 255));
        COLOREQUALS(pntr_image_get_color(parent, 70, 39), pntr_new_color(70, 39, 128, 255));

        // The rows immediately above and below the rectangle.
        COLOREQUALS(pntr_image_get_color(parent, 20, 29), pntr_new_color(20, 29, 128, 255));
        COLOREQUALS(pntr_image_get_color(parent, 69, 29), pntr_new_color(69, 29, 128, 255));
        COLOREQUALS(pntr_image_get_color(parent, 20, 40), pntr_new_color(20, 40, 128, 255));
        COLOREQUALS(pntr_image_get_color(parent, 69, 40), pntr_new_color(69, 40, 128, 255));

        // Copying a parent pitch rather than a subimage width runs off the
        // right of the rectangle and wraps into the row after it.
        COLOREQUALS(pntr_image_get_color(parent, 70, 31), pntr_new_color(70, 31, 128, 255));
        COLOREQUALS(pntr_image_get_color(parent, 99, 31), pntr_new_color(99, 31, 128, 255));
        COLOREQUALS(pntr_image_get_color(parent, 0, 32), pntr_new_color(0, 32, 128, 255));
        COLOREQUALS(pntr_image_get_color(parent, 19, 32), pntr_new_color(19, 32, 128, 255));

        pntr_unload_image(subimage);
        pntr_unload_image(parent);
    });

    IT("pntr_clear_background() on a subimage at the bottom right corner", {
        pntr_image* parent = pntr_new_image(100, 100);
        NEQUALS(parent, NULL);
        for (int y = 0; y < parent->height; y++) {
            for (int x = 0; x < parent->width; x++) {
                pntr_draw_point(parent, x, y, pntr_new_color((unsigned char)x, (unsigned char)y, 128, 255));
            }
        }

        // Anchored at the very end of the parent's pixel data, so copying a
        // full parent pitch for each row would run past the allocation.
        pntr_image* subimage = pntr_image_subimage(parent, 50, 90, 50, 10);
        NEQUALS(subimage, NULL);
        EQUALS(subimage->width, 50);
        EQUALS(subimage->height, 10);

        pntr_clear_background(subimage, PNTR_RED);

        for (int y = 0; y < parent->height; y++) {
            for (int x = 0; x < parent->width; x++) {
                COLOREQUALS(pntr_image_get_color(parent, x, y),
                    (x >= 50 && y >= 90)
                        ? PNTR_RED
                        : pntr_new_color((unsigned char)x, (unsigned char)y, 128, 255));
            }
        }

        pntr_unload_image(subimage);
        pntr_unload_image(parent);
    });

    IT("pntr_clear_background() on a subimage with white and blank", {
        pntr_image* parent = pntr_new_image(100, 100);
        NEQUALS(parent, NULL);
        for (int y = 0; y < parent->height; y++) {
            for (int x = 0; x < parent->width; x++) {
                pntr_draw_point(parent, x, y, pntr_new_color((unsigned char)x, (unsigned char)y, 128, 255));
            }
        }

        pntr_image* subimage = pntr_image_subimage(parent, 50, 90, 50, 10);
        NEQUALS(subimage, NULL);

        // White and blank have whole image memset fast paths that a subimage
        // must not take, as they would cover the whole of the parent.
        pntr_clear_background(subimage, PNTR_WHITE);
        for (int y = 0; y < parent->height; y++) {
            for (int x = 0; x < parent->width; x++) {
                COLOREQUALS(pntr_image_get_color(parent, x, y),
                    (x >= 50 && y >= 90)
                        ? PNTR_WHITE
                        : pntr_new_color((unsigned char)x, (unsigned char)y, 128, 255));
            }
        }

        pntr_clear_background(subimage, PNTR_BLANK);
        for (int y = 0; y < parent->height; y++) {
            for (int x = 0; x < parent->width; x++) {
                COLOREQUALS(pntr_image_get_color(parent, x, y),
                    (x >= 50 && y >= 90)
                        ? PNTR_BLANK
                        : pntr_new_color((unsigned char)x, (unsigned char)y, 128, 255));
            }
        }

        pntr_unload_image(subimage);
        pntr_unload_image(parent);
    });

    IT("pntr_image_set_clip", {
        pntr_image* image = pntr_gen_image_color(300, 300, PNTR_RED);
        COLOREQUALS(pntr_image_get_color(image, 50, 50), PNTR_RED);
        COLOREQUALS(pntr_image_get_color(image, 125, 125), PNTR_RED);

        pntr_image_set_clip(image, 100, 100, 50, 50);
        pntr_draw_rectangle_fill(image, 0, 0, image->width, image->height, PNTR_BLUE);
        COLOREQUALS(pntr_image_get_color(image, 50, 50), PNTR_RED);
        COLOREQUALS(pntr_image_get_color(image, 125, 125), PNTR_BLUE);

        pntr_unload_image(image);
    });

    IT("pntr_image_alpha_mask()", {
        pntr_image* image = pntr_gen_image_color(40, 30, PNTR_RED);
        NEQUALS(image, NULL);
        pntr_image* mask = pntr_test_alpha_mask(40, 30);
        NEQUALS(mask, NULL);

        pntr_image_alpha_mask(image, mask, 0, 0);

        // Every destination pixel keeps its color, and takes the alpha of the
        // mask pixel that sits directly on top of it.
        for (int y = 0; y < image->height; y++) {
            for (int x = 0; x < image->width; x++) {
                COLOREQUALS(pntr_image_get_color(image, x, y),
                    pntr_test_color_alpha(PNTR_RED, pntr_test_alpha(x, y)));
            }
        }

        // A NULL image or mask is a no-op.
        pntr_image_alpha_mask(NULL, mask, 0, 0);
        pntr_image_alpha_mask(image, NULL, 0, 0);

        pntr_unload_image(mask);
        pntr_unload_image(image);
    });

    IT("pntr_image_alpha_mask() with a custom clip", {
        pntr_image* image = pntr_gen_image_color(100, 100, PNTR_RED);
        NEQUALS(image, NULL);
        pntr_image* mask = pntr_test_alpha_mask(100, 100);
        NEQUALS(mask, NULL);

        pntr_image_set_clip(image, 20, 20, 50, 50);
        pntr_image_alpha_mask(image, mask, 0, 0);

        // The clip only limits what gets written, it must not shift the mask,
        // so the pixel at the clip origin still takes the mask pixel above it.
        COLOREQUALS(pntr_image_get_color(image, 20, 20),
            pntr_test_color_alpha(PNTR_RED, pntr_test_alpha(20, 20)));
        COLOREQUALS(pntr_image_get_color(image, 69, 69),
            pntr_test_color_alpha(PNTR_RED, pntr_test_alpha(69, 69)));

        for (int y = 0; y < image->height; y++) {
            for (int x = 0; x < image->width; x++) {
                // Everything outside of the clip rectangle is left alone.
                bool clipped = x >= 20 && x < 70 && y >= 20 && y < 70;
                COLOREQUALS(pntr_image_get_color(image, x, y),
                    pntr_test_color_alpha(PNTR_RED, clipped ? pntr_test_alpha(x, y) : 255));
            }
        }

        pntr_unload_image(mask);
        pntr_unload_image(image);
    });

    IT("pntr_image_alpha_mask() at a negative position", {
        pntr_image* image = pntr_gen_image_color(100, 100, PNTR_RED);
        NEQUALS(image, NULL);
        pntr_image* mask = pntr_test_alpha_mask(10, 10);
        NEQUALS(mask, NULL);

        pntr_image_alpha_mask(image, mask, -5, 0);

        // The five columns of the mask that do overlap are still applied.
        for (int y = 0; y < image->height; y++) {
            for (int x = 0; x < image->width; x++) {
                bool masked = x < 5 && y < 10;
                COLOREQUALS(pntr_image_get_color(image, x, y),
                    pntr_test_color_alpha(PNTR_RED, masked ? pntr_test_alpha(x + 5, y) : 255));
            }
        }

        pntr_unload_image(mask);
        pntr_unload_image(image);
    });

    IT("pntr_image_alpha_mask() at a negative position on both axes", {
        pntr_image* image = pntr_gen_image_color(100, 100, PNTR_RED);
        NEQUALS(image, NULL);
        pntr_image* mask = pntr_test_alpha_mask(10, 10);
        NEQUALS(mask, NULL);

        pntr_image_alpha_mask(image, mask, -3, -4);

        for (int y = 0; y < image->height; y++) {
            for (int x = 0; x < image->width; x++) {
                bool masked = x < 7 && y < 6;
                COLOREQUALS(pntr_image_get_color(image, x, y),
                    pntr_test_color_alpha(PNTR_RED, masked ? pntr_test_alpha(x + 3, y + 4) : 255));
            }
        }

        pntr_unload_image(mask);
        pntr_unload_image(image);
    });

    IT("pntr_image_alpha_mask() overhanging the right and bottom edges", {
        pntr_image* image = pntr_gen_image_color(100, 100, PNTR_RED);
        NEQUALS(image, NULL);
        pntr_image* mask = pntr_test_alpha_mask(10, 10);
        NEQUALS(mask, NULL);

        pntr_image_alpha_mask(image, mask, 95, 97);

        for (int y = 0; y < image->height; y++) {
            for (int x = 0; x < image->width; x++) {
                bool masked = x >= 95 && y >= 97;
                COLOREQUALS(pntr_image_get_color(image, x, y),
                    pntr_test_color_alpha(PNTR_RED, masked ? pntr_test_alpha(x - 95, y - 97) : 255));
            }
        }

        pntr_unload_image(mask);
        pntr_unload_image(image);
    });

    IT("pntr_image_alpha_mask() fully outside the image is a no-op", {
        pntr_image* image = pntr_gen_image_color(100, 100, PNTR_RED);
        NEQUALS(image, NULL);
        pntr_image* mask = pntr_test_alpha_mask(10, 10);
        NEQUALS(mask, NULL);

        pntr_image* expected = pntr_image_copy(image);
        NEQUALS(expected, NULL);

        pntr_image_alpha_mask(image, mask, 100, 0);
        pntr_image_alpha_mask(image, mask, 0, 100);
        pntr_image_alpha_mask(image, mask, -10, 0);
        pntr_image_alpha_mask(image, mask, 0, -10);
        pntr_image_alpha_mask(image, mask, -200, -200);
        pntr_image_alpha_mask(image, mask, 200, 200);
        IMAGEEQUALS(image, expected);

        pntr_unload_image(expected);
        pntr_unload_image(mask);
        pntr_unload_image(image);
    });

    IT("pntr_image_alpha_mask() leaves transparent pixels alone", {
        pntr_image* image = pntr_gen_image_color(20, 20, PNTR_RED);
        NEQUALS(image, NULL);
        pntr_image* mask = pntr_test_alpha_mask(20, 20);
        NEQUALS(mask, NULL);

        // Clear the alpha of every other column.
        for (int y = 0; y < image->height; y++) {
            for (int x = 0; x < image->width; x += 2) {
                PNTR_PIXEL(image, x, y).rgba.a = 0;
            }
        }

        pntr_image_alpha_mask(image, mask, 0, 0);

        for (int y = 0; y < image->height; y++) {
            for (int x = 0; x < image->width; x++) {
                COLOREQUALS(pntr_image_get_color(image, x, y),
                    pntr_test_color_alpha(PNTR_RED, (x % 2 == 0) ? 0 : pntr_test_alpha(x, y)));
            }
        }

        pntr_unload_image(mask);
        pntr_unload_image(image);
    });

    IT("pntr_image_get_clip", {
        pntr_image* image = pntr_gen_image_color(300, 100, PNTR_RED);

        pntr_rectangle expected;
        expected.width = 300;
        expected.height = 100;
        expected.x = 0;
        expected.y = 0;
        RECTEQUALS(expected, pntr_image_get_clip(image));

        expected.width = 0;
        expected.height = 0;
        expected.x = 0;
        expected.y = 0;
        RECTEQUALS(expected, pntr_image_get_clip(NULL));

        pntr_unload_image(image);
    });

    IT("pntr_image_reset_clip", {
        pntr_image* image = pntr_gen_image_color(300, 300, PNTR_RED);

        EQUALS(image->clip.x, 0);
        EQUALS(image->clip.y, 0);
        EQUALS(image->clip.width, image->width);
        EQUALS(image->clip.height, image->height);

        pntr_image_set_clip(image, 100, 200, 50, 60);

        EQUALS(image->clip.x, 100);
        EQUALS(image->clip.y, 200);
        EQUALS(image->clip.width, 50);
        EQUALS(image->clip.height, 60);

        pntr_image_reset_clip(image);

        EQUALS(image->clip.x, 0);
        EQUALS(image->clip.y, 0);
        EQUALS(image->clip.width, image->width);
        EQUALS(image->clip.height, image->height);

        pntr_unload_image(image);
    });

    IT("_pntr_rectangle_intersect", {
        pntr_rectangle out;
        EQUALS(_pntr_rectangle_intersect(-10, -10, 5, 5, 0, 0, 100, 100, &out), false);
        EQUALS(_pntr_rectangle_intersect(5, 6, 10, 5, 0, 0, 100, 100, &out), true);
        EQUALS(out.x, 5);
        EQUALS(out.y, 6);
        EQUALS(out.width, 10);
        EQUALS(out.height, 5);
        EQUALS(_pntr_rectangle_intersect(-5, -5, 10, 10, 0, 0, 20, 20, &out), true);
        EQUALS(out.x, 0);
        EQUALS(out.y, 0);
        EQUALS(out.width, 5);
        EQUALS(out.height, 5);

        EQUALS(_pntr_rectangle_intersect(10, 10, 50, 50, 20, 20, 10, 10, &out), true);
        pntr_rectangle expected = (pntr_rectangle) {20, 20, 10, 10};
        RECTEQUALS(out, expected);
    });

    if (pntr_utf8()) {
        IT("PNTR_ENABLE_UTF8", {
            pntr_font* font = pntr_load_font_ttf("resources/tuffy.ttf", 38);
            NEQUALS(font, NULL);

            // Generate the image displaying UTF-8 text.
            const char* text = "Добрий день!";
            pntr_image* image = pntr_gen_image_text(font, text, PNTR_BLACK, PNTR_WHITE);
            NEQUALS(image, NULL);
            pntr_save_image(image, "pntr_test_utf8.png");

            EQUALS(image->width, 190);
            EQUALS(image->height, 37);

            pntr_unload_font(font);
            pntr_unload_image(image);
        });
    }
    else {
        IT("PNTR_ENABLE_UTF8: Not enabled", {
            // Nothing
        });
    }

    IT("No reported errors", {
        const char* err = "";
        if (pntr_get_error() != NULL) {
            err = pntr_get_error();
        }

        STREQUALS(err, "");
    });
})

int main() {
    UNIT_CREATE("pntr");
    UNIT_MODULE(pntr);
    UNIT_MODULE(pntr_math);
    return UNIT_RUN();
}
