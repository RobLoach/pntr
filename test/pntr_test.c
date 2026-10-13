#define UNIT_TEST_PREFIX ""
#define UNIT_STATIC
#include "unit.h"
#include <limits.h>

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
 *
 * A first image with nothing painted on it reports -1 rather than zero, because nothing
 * is covered by anything. Every caller compares this against zero, so a primitive that
 * stopped drawing entirely fails them all instead of passing them all vacuously.
 *
 * @return How many pixels are uncovered, or -1 when the comparison says nothing: either
 * image missing, the two being different sizes, or the first one being blank.
 */
int pntr_test_uncovered(pntr_image* painted, pntr_image* cover) {
    if (painted == NULL || cover == NULL || painted->width != cover->width || painted->height != cover->height) {
        return -1;
    }

    int uncovered = 0;
    int total = 0;
    for (int y = 0; y < painted->height; y++) {
        for (int x = 0; x < painted->width; x++) {
            if (!pntr_test_painted(painted, x, y)) {
                continue;
            }

            total++;
            if (!pntr_test_painted(cover, x, y)) {
                uncovered++;
            }
        }
    }

    if (total == 0) {
        return -1;
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
 *
 * An outline that painted nothing reports -1, not zero.
 *
 * @see pntr_test_uncovered()
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
 * An outline that painted nothing reports -1, not zero.
 *
 * @see pntr_test_unfilled_ellipse()
 * @see pntr_test_uncovered()
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

/**
 * Whether images can be decoded in this build.
 *
 * Each of these asks pntr which stub it settled on rather than repeating the
 * reasoning, because a custom callback paired with one of the PNTR_NO_* defines keeps
 * the feature even though the define is set.
 *
 * @see PNTR_NO_LOAD_IMAGE
 */
bool pntr_image_loading() {
    #ifdef _PNTR_LOAD_IMAGE_DISABLED
        return false;
    #else
        return true;
    #endif
}

/**
 * Whether images can be encoded in this build.
 *
 * @see PNTR_NO_SAVE_IMAGE
 */
bool pntr_image_saving() {
    #ifdef _PNTR_SAVE_IMAGE_DISABLED
        return false;
    #else
        return true;
    #endif
}

/**
 * Whether pntr can read and write files.
 *
 * Without it, pntr_load_file() and pntr_save_file() are stubs that report
 * `PNTR_ERROR_NOT_SUPPORTED`, so everything that reaches the file system through them
 * refuses rather than failing to open anything. The test suite's own `stdio.h` is
 * untouched, which is what still lets a test create the file it then asks pntr for.
 *
 * @see PNTR_NO_STDIO
 */
bool pntr_file_io() {
    #if defined(_PNTR_LOAD_FILE_DISABLED) || defined(_PNTR_SAVE_FILE_DISABLED)
        return false;
    #else
        return true;
    #endif
}

/**
 * Whether an image can be loaded from the file system.
 *
 * Reading an image out of a file needs both the bytes to be read and the image to be
 * decoded, so either one being compiled out takes it away.
 *
 * @see pntr_file_io()
 * @see pntr_image_loading()
 */
bool pntr_image_from_file() {
    return pntr_file_io() && pntr_image_loading();
}

/**
 * Whether BMP images can be loaded back in. Saving them only needs pntr_bmp().
 *
 * @see PNTR_NO_BMP
 */
bool pntr_bmp_loading() {
    #ifdef PNTR_NO_BMP
        return false;
    #else
        return pntr_bmp() && pntr_image_loading();
    #endif
}

/**
 * Whether JPEG images can be saved. Loading them back also needs pntr_jpeg().
 *
 * @see pntr_jpeg()
 */
bool pntr_jpeg_saving() {
    #ifdef PNTR_CUTE_PNG
        // cute_png is a PNG-only backend, so it never writes a JPEG.
        return false;
    #else
        return pntr_image_saving();
    #endif
}

/**
 * A source image whose every pixel is a different opaque color.
 *
 * @details The red and green channels carry the coordinates the pixel was painted at, so
 * every pixel says where in the source it came from. That is what lets the flipped drawing
 * tests tell a region read from the source rectangle apart from the same sized region read
 * from the top left corner of the image.
 */
pntr_image* pntr_test_coordinate_image(int width, int height) {
    pntr_image* image = pntr_gen_image_color(width, height, PNTR_BLANK);
    if (image == NULL) {
        return NULL;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            pntr_draw_point(image, x, y, pntr_new_color(
                (unsigned char)(10 + x * 9),
                (unsigned char)(10 + y * 9),
                180, 255));
        }
    }

    return image;
}

/**
 * The region of a source image that a flipped draw of it should produce, as its own image.
 *
 * @details Built out of a crop and pntr_image_flip() rather than out of the same arithmetic
 * pntr_draw_image_flipped_rec() uses, so it is an independent answer to compare a flipped
 * draw against. The diagonal flip transposes first and the other two then mirror the
 * transposed result, which is the order the drawing path applies them in, so a diagonal
 * flip together with a horizontal one is a quarter turn either way.
 *
 * @see pntr_test_coordinate_image()
 */
pntr_image* pntr_test_flipped_region(pntr_image* src, pntr_rectangle srcRec, bool flipHorizontal, bool flipVertical, bool flipDiagonal) {
    pntr_image* region = pntr_image_from_image(src, srcRec.x, srcRec.y, srcRec.width, srcRec.height);
    if (region == NULL) {
        return NULL;
    }

    if (flipDiagonal) {
        pntr_image* transposed = pntr_gen_image_color(region->height, region->width, PNTR_BLANK);
        if (transposed == NULL) {
            pntr_unload_image(region);
            return NULL;
        }

        for (int y = 0; y < region->height; y++) {
            for (int x = 0; x < region->width; x++) {
                pntr_draw_point(transposed, y, x, pntr_image_get_color(region, x, y));
            }
        }

        pntr_unload_image(region);
        region = transposed;
    }

    pntr_image_flip(region, flipHorizontal, flipVertical);

    return region;
}

/**
 * The height of a single line of text drawn with the given font.
 *
 * @details Taken from a measurement of a single glyph, so it is the line height for any
 * font whose glyphs are all the same height, which is every font the tests draw with.
 */
int pntr_test_line_height(pntr_font* font) {
    return pntr_measure_text_ex(font, "A", 0).y;
}

/**
 * What a horizontally aligned draw of the given multi-line text is expected to look like.
 *
 * @details Every line of the text is drawn on its own, by pntr_draw_text() alone, at the
 * offset that pntr_measure_text_ex() says that line needs for the alignment, one line
 * height below the line before it. That makes this an expectation built only out of
 * single-line measuring and single-line drawing, which is what pntr_draw_text_aligned()
 * has to reproduce for a string with newlines in it. A line with nothing on it still
 * takes up its line of the canvas.
 *
 * The text is drawn in PNTR_BLACK onto a pntr_test_canvas(), so the result can be
 * compared against another canvas of the same size pixel for pixel.
 *
 * @see pntr_test_canvas()
 */
pntr_image* pntr_test_text_lines(pntr_font* font, const char* text, int width, int height, int posX, int posY, pntr_text_align align) {
    pntr_image* canvas = pntr_test_canvas(width, height);
    if (canvas == NULL || font == NULL || text == NULL) {
        return canvas;
    }

    int lineHeight = pntr_test_line_height(font);
    int currentY = posY;
    const char* lineStart = text;

    while (lineStart != NULL) {
        // The line on its own, so that the draw being compared against is the only one
        // that is ever handed a string with a newline in it.
        const char* lineEnd = strchr(lineStart, '\n');
        size_t bytes = (lineEnd != NULL) ? (size_t)(lineEnd - lineStart) : strlen(lineStart);
        char line[64];
        if (bytes > sizeof(line) - 1) {
            bytes = sizeof(line) - 1;
        }
        memcpy(line, lineStart, bytes);
        line[bytes] = '\0';

        pntr_vector size = pntr_measure_text_ex(font, line, 0);
        int lineX = posX;
        if (align == PNTR_TEXT_ALIGN_CENTER) {
            lineX = posX - size.x / 2;
        }
        else if (align == PNTR_TEXT_ALIGN_RIGHT) {
            lineX = posX - size.x;
        }

        pntr_draw_text(canvas, font, line, lineX, currentY, PNTR_BLACK);

        currentY += lineHeight;
        lineStart = (lineEnd != NULL) ? lineEnd + 1 : NULL;
    }

    return canvas;
}

/**
 * How many pixels of the given PNTR_TEST_BACKGROUND canvas are painted between two rows.
 *
 * @details Zero means the band of rows was left alone, which is what an empty line of
 * text has to leave behind.
 */
int pntr_test_painted_rows(pntr_image* image, int top, int bottom) {
    if (image == NULL) {
        return 0;
    }

    int count = 0;
    for (int y = top; y < bottom; y++) {
        for (int x = 0; x < image->width; x++) {
            if (pntr_test_painted(image, x, y)) {
                count++;
            }
        }
    }

    return count;
}

/**
 * How wide the painted pixels between two rows of a PNTR_TEST_BACKGROUND canvas are.
 *
 * @details The first painted column of the band through to the last, which for a band one
 * line height tall is the width of the line of text drawn on it, and so the number a
 * maximum width has to hold. Zero means the band of rows was left alone.
 */
int pntr_test_painted_width(pntr_image* image, int top, int bottom) {
    if (image == NULL) {
        return 0;
    }

    int left = 0;
    int right = -1;
    for (int y = top; y < bottom; y++) {
        for (int x = 0; x < image->width; x++) {
            if (pntr_test_painted(image, x, y)) {
                if (right < 0 || x < left) {
                    left = x;
                }
                if (x > right) {
                    right = x;
                }
            }
        }
    }

    return (right < 0) ? 0 : right - left + 1;
}

/**
 * Every drawing call that another one has to agree with, pixel for pixel.
 *
 * @details pntr is full of entry points that exist only to reach another one: a `_vec`
 * form that unpacks a vector into coordinates, a `_rec` form that unpacks a rectangle, a
 * `_thick` form that is the plain primitive when the thickness is one, a whole-image draw
 * that is a sub-rectangle draw of the whole image. None of those can be right or wrong on
 * their own; each one is right exactly when it paints what the call it delegates to
 * paints. Naming each call once here and listing the pairs in the test is what lets that
 * be asserted for all of them at once, instead of a hand-written case per wrapper.
 *
 * Every variant draws at the same place, in the same color, on a canvas of the same size,
 * so any two of them can be compared, not only the pairs the test happens to list.
 *
 * @see pntr_test_draw_variant()
 */
typedef enum pntr_test_variant {
    PNTR_TEST_VARIANT_POINT,
    PNTR_TEST_VARIANT_POINT_VEC,
    PNTR_TEST_VARIANT_POINT_UNSAFE,
    PNTR_TEST_VARIANT_LINE,
    PNTR_TEST_VARIANT_LINE_VEC,
    PNTR_TEST_VARIANT_LINE_THICK_1,
    PNTR_TEST_VARIANT_LINE_THICK_3,
    PNTR_TEST_VARIANT_LINE_THICK_3_VEC,
    PNTR_TEST_VARIANT_LINE_HORIZONTAL,
    PNTR_TEST_VARIANT_LINE_HORIZONTAL_THICK_1,
    PNTR_TEST_VARIANT_LINE_HORIZONTAL_AS_LINE,
    PNTR_TEST_VARIANT_LINE_HORIZONTAL_UNSAFE,
    PNTR_TEST_VARIANT_LINE_VERTICAL,
    PNTR_TEST_VARIANT_LINE_VERTICAL_THICK_1,
    PNTR_TEST_VARIANT_LINE_VERTICAL_AS_LINE,
    PNTR_TEST_VARIANT_LINE_CURVE,
    PNTR_TEST_VARIANT_LINE_CURVE_THICK_1,
    PNTR_TEST_VARIANT_RECTANGLE,
    PNTR_TEST_VARIANT_RECTANGLE_REC,
    PNTR_TEST_VARIANT_RECTANGLE_THICK_1,
    PNTR_TEST_VARIANT_RECTANGLE_THICK_3,
    PNTR_TEST_VARIANT_RECTANGLE_THICK_3_REC,
    PNTR_TEST_VARIANT_RECTANGLE_ROUNDED_0,
    PNTR_TEST_VARIANT_RECTANGLE_ROUNDED_4,
    PNTR_TEST_VARIANT_RECTANGLE_THICK_ROUNDED_1,
    PNTR_TEST_VARIANT_RECTANGLE_FILL,
    PNTR_TEST_VARIANT_RECTANGLE_FILL_REC,
    PNTR_TEST_VARIANT_RECTANGLE_ROUNDED_FILL_0,
    PNTR_TEST_VARIANT_RECTANGLE_GRADIENT,
    PNTR_TEST_VARIANT_RECTANGLE_GRADIENT_REC,
    PNTR_TEST_VARIANT_TRIANGLE,
    PNTR_TEST_VARIANT_TRIANGLE_VEC,
    PNTR_TEST_VARIANT_TRIANGLE_THICK_1,
    PNTR_TEST_VARIANT_TRIANGLE_THICK_3,
    PNTR_TEST_VARIANT_TRIANGLE_THICK_3_VEC,
    PNTR_TEST_VARIANT_TRIANGLE_FILL,
    PNTR_TEST_VARIANT_TRIANGLE_FILL_VEC,
    PNTR_TEST_VARIANT_ELLIPSE,
    PNTR_TEST_VARIANT_ELLIPSE_THICK_1,
    PNTR_TEST_VARIANT_CIRCLE,
    PNTR_TEST_VARIANT_CIRCLE_THICK_1,
    PNTR_TEST_VARIANT_POLYGON,
    PNTR_TEST_VARIANT_POLYGON_THICK_1,
    PNTR_TEST_VARIANT_POLYLINE,
    PNTR_TEST_VARIANT_POLYLINE_THICK_1,
    PNTR_TEST_VARIANT_IMAGE,
    PNTR_TEST_VARIANT_IMAGE_REC,
    PNTR_TEST_VARIANT_IMAGE_TINT_REC_WHITE,
    PNTR_TEST_VARIANT_IMAGE_ROTATED_0,
    PNTR_TEST_VARIANT_IMAGE_FLIPPED_NONE,
    PNTR_TEST_VARIANT_IMAGE_TINT,
    PNTR_TEST_VARIANT_IMAGE_TINT_REC
} pntr_test_variant;

/**
 * The canvas size that every pntr_test_variant() draws onto.
 *
 * Big enough that none of the variants is clipped, so a difference between two of them is
 * a difference in what they drew rather than in what the canvas kept.
 */
#define PNTR_TEST_VARIANT_SIZE 48

/**
 * The tint that the tinted image variants draw with. Deliberately not PNTR_WHITE, which
 * pntr_draw_image_tint_rec() short circuits into its untinted path.
 */
#define PNTR_TEST_VARIANT_TINT pntr_new_color(128, 200, 64, 255)

/**
 * Makes the given drawing call onto the given canvas.
 *
 * @param dst A PNTR_TEST_VARIANT_SIZE square canvas to draw onto.
 * @param src The source image for the variants that draw an image. Never NULL.
 * @param variant Which call to make.
 *
 * @see pntr_test_variant
 */
void pntr_test_draw_variant(pntr_image* dst, pntr_image* src, pntr_test_variant variant) {
    pntr_vector point = PNTR_CLITERAL(pntr_vector) {11, 13};
    pntr_vector start = PNTR_CLITERAL(pntr_vector) {7, 9};
    pntr_vector end = PNTR_CLITERAL(pntr_vector) {31, 22};
    pntr_vector third = PNTR_CLITERAL(pntr_vector) {13, 37};
    pntr_vector fourth = PNTR_CLITERAL(pntr_vector) {34, 30};
    pntr_vector points[4];
    points[0] = PNTR_CLITERAL(pntr_vector) {8, 6};
    points[1] = PNTR_CLITERAL(pntr_vector) {34, 11};
    points[2] = PNTR_CLITERAL(pntr_vector) {29, 34};
    points[3] = PNTR_CLITERAL(pntr_vector) {6, 28};

    pntr_rectangle rect = PNTR_CLITERAL(pntr_rectangle) {5, 6, 23, 17};
    pntr_rectangle whole = PNTR_CLITERAL(pntr_rectangle) {0, 0, src->width, src->height};
    pntr_color color = PNTR_RED;
    pntr_color tint = PNTR_TEST_VARIANT_TINT;
    int imageX = 6;
    int imageY = 7;

    switch (variant) {
        case PNTR_TEST_VARIANT_POINT:
            pntr_draw_point(dst, point.x, point.y, color);
            break;
        case PNTR_TEST_VARIANT_POINT_VEC:
            pntr_draw_point_vec(dst, &point, color);
            break;
        case PNTR_TEST_VARIANT_POINT_UNSAFE:
            pntr_draw_point_unsafe(dst, point.x, point.y, color);
            break;

        case PNTR_TEST_VARIANT_LINE:
            pntr_draw_line(dst, start.x, start.y, end.x, end.y, color);
            break;
        case PNTR_TEST_VARIANT_LINE_VEC:
            pntr_draw_line_vec(dst, start, end, color);
            break;
        case PNTR_TEST_VARIANT_LINE_THICK_1:
            pntr_draw_line_thick(dst, start.x, start.y, end.x, end.y, 1, color);
            break;
        case PNTR_TEST_VARIANT_LINE_THICK_3:
            pntr_draw_line_thick(dst, start.x, start.y, end.x, end.y, 3, color);
            break;
        case PNTR_TEST_VARIANT_LINE_THICK_3_VEC:
            pntr_draw_line_thick_vec(dst, start, end, 3, color);
            break;

        case PNTR_TEST_VARIANT_LINE_HORIZONTAL:
            pntr_draw_line_horizontal(dst, 6, 14, 20, color);
            break;
        case PNTR_TEST_VARIANT_LINE_HORIZONTAL_THICK_1:
            pntr_draw_line_horizontal_thick(dst, 6, 14, 20, 1, color);
            break;
        case PNTR_TEST_VARIANT_LINE_HORIZONTAL_AS_LINE:
            // A width of 20 covers the 20 columns that start at 6, so the far endpoint of
            // the same run is the last of them rather than the one past it.
            pntr_draw_line(dst, 6, 14, 6 + 20 - 1, 14, color);
            break;
        case PNTR_TEST_VARIANT_LINE_HORIZONTAL_UNSAFE:
            pntr_put_horizontal_line_unsafe(dst, 6, 14, 20, color);
            break;

        case PNTR_TEST_VARIANT_LINE_VERTICAL:
            pntr_draw_line_vertical(dst, 14, 6, 20, color);
            break;
        case PNTR_TEST_VARIANT_LINE_VERTICAL_THICK_1:
            pntr_draw_line_vertical_thick(dst, 14, 6, 20, 1, color);
            break;
        case PNTR_TEST_VARIANT_LINE_VERTICAL_AS_LINE:
            pntr_draw_line(dst, 14, 6, 14, 6 + 20 - 1, color);
            break;

        case PNTR_TEST_VARIANT_LINE_CURVE:
            pntr_draw_line_curve(dst, start, end, third, fourth, 16, color);
            break;
        case PNTR_TEST_VARIANT_LINE_CURVE_THICK_1:
            pntr_draw_line_curve_thick(dst, start, end, third, fourth, 16, 1, color);
            break;

        case PNTR_TEST_VARIANT_RECTANGLE:
            pntr_draw_rectangle(dst, rect.x, rect.y, rect.width, rect.height, color);
            break;
        case PNTR_TEST_VARIANT_RECTANGLE_REC:
            pntr_draw_rectangle_rec(dst, rect, color);
            break;
        case PNTR_TEST_VARIANT_RECTANGLE_THICK_1:
            pntr_draw_rectangle_thick(dst, rect.x, rect.y, rect.width, rect.height, 1, color);
            break;
        case PNTR_TEST_VARIANT_RECTANGLE_THICK_3:
            pntr_draw_rectangle_thick(dst, rect.x, rect.y, rect.width, rect.height, 3, color);
            break;
        case PNTR_TEST_VARIANT_RECTANGLE_THICK_3_REC:
            pntr_draw_rectangle_thick_rec(dst, rect, 3, color);
            break;
        case PNTR_TEST_VARIANT_RECTANGLE_ROUNDED_0:
            pntr_draw_rectangle_rounded(dst, rect.x, rect.y, rect.width, rect.height, 0, 0, 0, 0, color);
            break;
        case PNTR_TEST_VARIANT_RECTANGLE_ROUNDED_4:
            pntr_draw_rectangle_rounded(dst, rect.x, rect.y, rect.width, rect.height, 4, 4, 4, 4, color);
            break;
        case PNTR_TEST_VARIANT_RECTANGLE_THICK_ROUNDED_1:
            pntr_draw_rectangle_thick_rounded(dst, rect.x, rect.y, rect.width, rect.height, 4, 4, 4, 4, 1, color);
            break;
        case PNTR_TEST_VARIANT_RECTANGLE_FILL:
            pntr_draw_rectangle_fill(dst, rect.x, rect.y, rect.width, rect.height, color);
            break;
        case PNTR_TEST_VARIANT_RECTANGLE_FILL_REC:
            pntr_draw_rectangle_fill_rec(dst, rect, color);
            break;
        case PNTR_TEST_VARIANT_RECTANGLE_ROUNDED_FILL_0:
            pntr_draw_rectangle_rounded_fill(dst, rect.x, rect.y, rect.width, rect.height, 0, color);
            break;
        case PNTR_TEST_VARIANT_RECTANGLE_GRADIENT:
            pntr_draw_rectangle_gradient(dst, rect.x, rect.y, rect.width, rect.height, PNTR_RED, PNTR_GREEN, PNTR_BLUE, PNTR_YELLOW);
            break;
        case PNTR_TEST_VARIANT_RECTANGLE_GRADIENT_REC:
            pntr_draw_rectangle_gradient_rec(dst, rect, PNTR_RED, PNTR_GREEN, PNTR_BLUE, PNTR_YELLOW);
            break;

        case PNTR_TEST_VARIANT_TRIANGLE:
            pntr_draw_triangle(dst, start.x, start.y, end.x, end.y, third.x, third.y, color);
            break;
        case PNTR_TEST_VARIANT_TRIANGLE_VEC:
            pntr_draw_triangle_vec(dst, start, end, third, color);
            break;
        case PNTR_TEST_VARIANT_TRIANGLE_THICK_1:
            pntr_draw_triangle_thick(dst, start.x, start.y, end.x, end.y, third.x, third.y, 1, color);
            break;
        case PNTR_TEST_VARIANT_TRIANGLE_THICK_3:
            pntr_draw_triangle_thick(dst, start.x, start.y, end.x, end.y, third.x, third.y, 3, color);
            break;
        case PNTR_TEST_VARIANT_TRIANGLE_THICK_3_VEC:
            pntr_draw_triangle_thick_vec(dst, start, end, third, 3, color);
            break;
        case PNTR_TEST_VARIANT_TRIANGLE_FILL:
            pntr_draw_triangle_fill(dst, start.x, start.y, end.x, end.y, third.x, third.y, color);
            break;
        case PNTR_TEST_VARIANT_TRIANGLE_FILL_VEC:
            pntr_draw_triangle_fill_vec(dst, start, end, third, color);
            break;

        case PNTR_TEST_VARIANT_ELLIPSE:
            pntr_draw_ellipse(dst, 24, 24, 15, 9, color);
            break;
        case PNTR_TEST_VARIANT_ELLIPSE_THICK_1:
            pntr_draw_ellipse_thick(dst, 24, 24, 15, 9, 1, color);
            break;
        case PNTR_TEST_VARIANT_CIRCLE:
            pntr_draw_circle(dst, 24, 24, 12, color);
            break;
        case PNTR_TEST_VARIANT_CIRCLE_THICK_1:
            pntr_draw_circle_thick(dst, 24, 24, 12, 1, color);
            break;

        case PNTR_TEST_VARIANT_POLYGON:
            pntr_draw_polygon(dst, points, 4, color);
            break;
        case PNTR_TEST_VARIANT_POLYGON_THICK_1:
            pntr_draw_polygon_thick(dst, points, 4, 1, color);
            break;
        case PNTR_TEST_VARIANT_POLYLINE:
            pntr_draw_polyline(dst, points, 4, color);
            break;
        case PNTR_TEST_VARIANT_POLYLINE_THICK_1:
            pntr_draw_polyline_thick(dst, points, 4, 1, color);
            break;

        case PNTR_TEST_VARIANT_IMAGE:
            pntr_draw_image(dst, src, imageX, imageY);
            break;
        case PNTR_TEST_VARIANT_IMAGE_REC:
            pntr_draw_image_rec(dst, src, whole, imageX, imageY);
            break;
        case PNTR_TEST_VARIANT_IMAGE_TINT_REC_WHITE:
            pntr_draw_image_tint_rec(dst, src, whole, imageX, imageY, PNTR_WHITE);
            break;
        case PNTR_TEST_VARIANT_IMAGE_ROTATED_0:
            pntr_draw_image_rotated(dst, src, imageX, imageY, 0.0f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR);
            break;
        case PNTR_TEST_VARIANT_IMAGE_FLIPPED_NONE:
            pntr_draw_image_flipped(dst, src, imageX, imageY, false, false, false);
            break;
        case PNTR_TEST_VARIANT_IMAGE_TINT:
            pntr_draw_image_tint(dst, src, imageX, imageY, tint);
            break;
        case PNTR_TEST_VARIANT_IMAGE_TINT_REC:
            pntr_draw_image_tint_rec(dst, src, whole, imageX, imageY, tint);
            break;
    }
}

/**
 * Every whole-image color operation that a per-pixel one has to agree with.
 *
 * @details Each pntr_image_color_*() is its matching pntr_color_*() applied to every
 * pixel inside the image's clip region, and nothing else. Naming the pairs here is what
 * lets that be asserted for all of them from one loop.
 *
 * @see pntr_test_color_op_pixel()
 * @see pntr_test_image_color_op()
 */
typedef enum pntr_test_color_op {
    PNTR_TEST_COLOR_OP_INVERT,
    PNTR_TEST_COLOR_OP_GRAYSCALE,
    PNTR_TEST_COLOR_OP_BRIGHTER,
    PNTR_TEST_COLOR_OP_DARKER,
    PNTR_TEST_COLOR_OP_CONTRAST,
    PNTR_TEST_COLOR_OP_FADE,
    PNTR_TEST_COLOR_OP_TINT,
    PNTR_TEST_COLOR_OP_REPLACE
} pntr_test_color_op;

/**
 * How many pntr_test_color_op values there are.
 */
#define PNTR_TEST_COLOR_OP_LEN (PNTR_TEST_COLOR_OP_REPLACE + 1)

/**
 * The factors and colors that the color operations are asked for.
 *
 * None of them takes a channel to zero, and none of them takes the alpha to zero, so
 * every pixel of a source image stays distinguishable from every other and from the
 * background after the operation has run.
 */
#define PNTR_TEST_COLOR_OP_FACTOR 0.35f
#define PNTR_TEST_COLOR_OP_CONTRAST_FACTOR 0.6f
#define PNTR_TEST_COLOR_OP_FADE_FACTOR (-0.4f)
#define PNTR_TEST_COLOR_OP_TINT_COLOR pntr_new_color(128, 200, 64, 255)
#define PNTR_TEST_COLOR_OP_REPLACE_FROM PNTR_RED
#define PNTR_TEST_COLOR_OP_REPLACE_TO PNTR_BLUE

/**
 * What the given color becomes under the given operation, a single pixel at a time.
 *
 * @param color The color to transform.
 * @param op Which operation to apply.
 *
 * @return The transformed color.
 *
 * @see pntr_test_image_color_op()
 */
pntr_color pntr_test_color_op_pixel(pntr_color color, pntr_test_color_op op) {
    switch (op) {
        case PNTR_TEST_COLOR_OP_INVERT:
            return pntr_color_invert(color);
        case PNTR_TEST_COLOR_OP_GRAYSCALE:
            return pntr_color_grayscale(color);
        case PNTR_TEST_COLOR_OP_BRIGHTER:
            return pntr_color_brightness(color, PNTR_TEST_COLOR_OP_FACTOR);
        case PNTR_TEST_COLOR_OP_DARKER:
            return pntr_color_brightness(color, -PNTR_TEST_COLOR_OP_FACTOR);
        case PNTR_TEST_COLOR_OP_CONTRAST:
            return pntr_color_contrast(color, PNTR_TEST_COLOR_OP_CONTRAST_FACTOR);
        case PNTR_TEST_COLOR_OP_FADE:
            return pntr_color_fade(color, PNTR_TEST_COLOR_OP_FADE_FACTOR);
        case PNTR_TEST_COLOR_OP_TINT:
            return pntr_color_tint(color, PNTR_TEST_COLOR_OP_TINT_COLOR);
        case PNTR_TEST_COLOR_OP_REPLACE:
            // pntr_image_color_replace() has no per-pixel counterpart of its own, so the
            // expectation is the swap it is documented to make.
            if (color.value == PNTR_TEST_COLOR_OP_REPLACE_FROM.value) {
                return PNTR_TEST_COLOR_OP_REPLACE_TO;
            }
            return color;
    }

    return color;
}

/**
 * Applies the given operation to a whole image at once.
 *
 * @param image The image to transform in place.
 * @param op Which operation to apply.
 *
 * @see pntr_test_color_op_pixel()
 */
void pntr_test_image_color_op(pntr_image* image, pntr_test_color_op op) {
    switch (op) {
        case PNTR_TEST_COLOR_OP_INVERT:
            pntr_image_color_invert(image);
            break;
        case PNTR_TEST_COLOR_OP_GRAYSCALE:
            pntr_image_color_grayscale(image);
            break;
        case PNTR_TEST_COLOR_OP_BRIGHTER:
            pntr_image_color_brightness(image, PNTR_TEST_COLOR_OP_FACTOR);
            break;
        case PNTR_TEST_COLOR_OP_DARKER:
            pntr_image_color_brightness(image, -PNTR_TEST_COLOR_OP_FACTOR);
            break;
        case PNTR_TEST_COLOR_OP_CONTRAST:
            pntr_image_color_contrast(image, PNTR_TEST_COLOR_OP_CONTRAST_FACTOR);
            break;
        case PNTR_TEST_COLOR_OP_FADE:
            pntr_image_color_fade(image, PNTR_TEST_COLOR_OP_FADE_FACTOR);
            break;
        case PNTR_TEST_COLOR_OP_TINT:
            pntr_image_color_tint(image, PNTR_TEST_COLOR_OP_TINT_COLOR);
            break;
        case PNTR_TEST_COLOR_OP_REPLACE:
            pntr_image_color_replace(image, PNTR_TEST_COLOR_OP_REPLACE_FROM, PNTR_TEST_COLOR_OP_REPLACE_TO);
            break;
    }
}

/**
 * A source image for the color operations, holding no two pixels the same.
 *
 * @details Every channel varies across the image, including the alpha, so an operation
 * that treats one channel differently from another, or that reaches a pixel it should
 * have left alone, shows up as a single mismatched pixel. The pixel at (0, 0) is
 * PNTR_RED exactly, which is the color pntr_image_color_replace() is asked to swap.
 */
pntr_image* pntr_test_color_source(int width, int height) {
    pntr_image* image = pntr_new_image(width, height);
    if (image == NULL) {
        return NULL;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            PNTR_PIXEL(image, x, y) = pntr_new_color(
                (unsigned char)(4 + x * 9),
                (unsigned char)(250 - y * 11),
                (unsigned char)(60 + ((x + y) % 7) * 20),
                (unsigned char)(40 + ((x * 3 + y) % 9) * 24));
        }
    }

    PNTR_PIXEL(image, 0, 0) = PNTR_RED;

    return image;
}

/**
 * The index of the first pixel at which two images differ, or -1 when they match.
 *
 * @details Counted in row-major order, so the index is `y * width + x`. Two images of
 * different sizes differ at their first pixel. This is what lets a loop over many pairs
 * of images say which pair went wrong before handing the pair itself to IMAGEEQUALS.
 */
int pntr_test_first_difference(pntr_image* a, pntr_image* b) {
    if (a == NULL || b == NULL || a->width != b->width || a->height != b->height) {
        return 0;
    }

    for (int y = 0; y < a->height; y++) {
        for (int x = 0; x < a->width; x++) {
            if (pntr_image_get_color(a, x, y).value != pntr_image_get_color(b, x, y).value) {
                return y * a->width + x;
            }
        }
    }

    return -1;
}

/**
 * How many pixels of the given image are neither fully transparent nor fully opaque.
 *
 * @details This is what proves that an anti-aliased draw anti-aliased anything: a line
 * that fell back to plotting whole pixels leaves every pixel it touched at the alpha it
 * was handed, so the count is zero.
 */
int pntr_test_partial_alpha_count(pntr_image* image) {
    if (image == NULL) {
        return 0;
    }

    int count = 0;
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            unsigned char alpha = pntr_color_a(pntr_image_get_color(image, x, y));
            if (alpha > 0 && alpha < 255) {
                count++;
            }
        }
    }

    return count;
}

/**
 * Whether a non-opaque source is blended into the destination in this build.
 *
 * Without it, pntr_blend_color() writes a fully opaque source and skips every other one,
 * leaving the destination exactly as it was, so a semi-transparent draw paints nothing at
 * all. Everything that paints through pntr_blend_color() inherits that, which is most of
 * the drawing API.
 *
 * @see PNTR_NO_ALPHABLEND
 */
bool pntr_alphablend() {
    #ifdef PNTR_NO_ALPHABLEND
        return false;
    #else
        return true;
    #endif
}

/**
 * How many pixels of a transparent image have had any alpha painted onto them.
 *
 * @details The counterpart to pntr_test_painted_count() for the images that start out as
 * PNTR_BLANK rather than at PNTR_TEST_BACKGROUND, so it says how much of a draw landed on
 * one. Zero means the image was left as it was found, which is what a semi-transparent
 * draw leaves behind when pntr_alphablend() is false.
 *
 * @see pntr_test_painted_count()
 * @see pntr_alphablend()
 */
int pntr_test_blended_count(pntr_image* image) {
    if (image == NULL) {
        return 0;
    }

    int count = 0;
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            if (pntr_color_a(pntr_image_get_color(image, x, y)) != 0) {
                count++;
            }
        }
    }

    return count;
}


/**
 * Whether pntr_draw_text_ex() is available in this build.
 *
 * @see PNTR_ENABLE_VARGS
 */
bool pntr_vargs() {
    #ifdef PNTR_ENABLE_VARGS
        return true;
    #else
        return false;
    #endif
}

/**
 * Draws `n=<number>` through pntr_draw_text_ex(), where the build has it.
 *
 * @details pntr_draw_text_ex() only exists under PNTR_ENABLE_VARGS, and a `#ifdef` cannot
 * live inside the arguments of an IT(), so the call is wrapped here instead.
 *
 * @see pntr_vargs()
 */
void pntr_test_draw_text_ex(pntr_image* dst, pntr_font* font, int posX, int posY, pntr_color tint, int maxlen, int number) {
    #ifdef PNTR_ENABLE_VARGS
        pntr_draw_text_ex(dst, font, posX, posY, tint, maxlen, "n=%d", number);
    #else
        (void)dst;
        (void)font;
        (void)posX;
        (void)posY;
        (void)tint;
        (void)maxlen;
        (void)number;
    #endif
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

    if (pntr_file_io()) {
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
    }
    else {
        IT("pntr_load_file(): Reports that it was compiled out", {
            pntr_set_error(PNTR_ERROR_NONE);

            // Without a file system there is nothing to tell a missing file from a
            // directory, so both are refused for the same reason, and the read size is
            // still reported back as the nothing that was read.
            unsigned int bytesRead = 1234;
            EQUALS(pntr_load_file("FileNotFound.txt", &bytesRead), NULL);
            EQUALS(bytesRead, 0);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
            pntr_set_error(PNTR_ERROR_NONE);

            EQUALS(pntr_load_file(".", NULL), NULL);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
            pntr_set_error(PNTR_ERROR_NONE);

            // A name is still required, which is about the arguments rather than the
            // missing file system.
            EQUALS(pntr_load_file(NULL, NULL), NULL);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_INVALID_ARGS);
            pntr_set_error(PNTR_ERROR_NONE);
        });
    }

    if (pntr_image_from_file()) {
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
    }
    else {
        IT("pntr_load_image(): Reports that it was compiled out", {
            pntr_set_error(PNTR_ERROR_NONE);

            // With either the file system or the decoder gone, no file can be reached, so
            // nothing about the file itself is ever reported.
            EQUALS(pntr_load_image("resources/image.png"), NULL);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
            pntr_set_error(PNTR_ERROR_NONE);

            // A file that isn't there is refused too, for whichever of the two reasons is
            // reached first, but never for a lack of memory.
            EQUALS(pntr_load_image("NotFoundImage.png"), NULL);
            NEQUALS(pntr_get_error_code(), PNTR_ERROR_NONE);
            NEQUALS(pntr_get_error_code(), PNTR_ERROR_NO_MEMORY);
            pntr_set_error(PNTR_ERROR_NONE);

            // A name is still required, which is about the arguments rather than what was
            // compiled out.
            EQUALS(pntr_load_image(NULL), NULL);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_INVALID_ARGS);
            pntr_set_error(PNTR_ERROR_NONE);
        });
    }

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

    IT("pntr_color_bilinear_interpolate() of four identical colors is that color", {
        // The property, rather than particular positions: the four weights always sum to
        // one, so interpolating between four copies of a color cannot be anything but
        // that color. It used to round down to one less, on every channel, because the
        // weights do not sum to exactly one in float arithmetic.
        pntr_color colors[3];
        colors[0] = pntr_new_color(200, 100, 50, 255);
        colors[1] = pntr_new_color(255, 255, 255, 255);
        colors[2] = pntr_new_color(1, 128, 254, 1);

        for (int c = 0; c < 3; c++) {
            for (int i = 0; i <= 64; i++) {
                for (int j = 0; j <= 64; j++) {
                    float coordinateX = (float)i / 64.0f;
                    float coordinateY = (float)j / 64.0f;

                    COLOREQUALS(pntr_color_bilinear_interpolate(colors[c], colors[c], colors[c], colors[c], coordinateX, coordinateY), colors[c]);
                }
            }
        }
    });

    IT("pntr_image_get_color_bilinear() of a solid image is that color", {
        // The same property through the sampling path, which computes its own fractions
        // and is where the rounding actually showed. Swept across and past the edges, so
        // the clamped fractions are covered as well as the interior ones.
        pntr_color solid = pntr_new_color(200, 100, 50, 255);
        pntr_image* image = pntr_gen_image_color(9, 7, solid);
        NEQUALS(image, NULL);

        for (int i = 0; i <= 90; i++) {
            for (int j = 0; j <= 70; j++) {
                float x = (float)i / 10.0f;
                float y = (float)j / 10.0f;

                COLOREQUALS(pntr_image_get_color_bilinear(image, x, y), solid);
            }
        }

        pntr_unload_image(image);
    });

    IT("pntr_draw_image_dest_rec(PNTR_FILTER_BILINEAR) of an opaque image stays opaque", {
        // What the rounding cost in practice: a scaled draw of an image with no
        // transparency in it anywhere produced pixels at an alpha of 254, which
        // PNTR_NO_ALPHABLEND then dropped outright and left holes in the fill. Swept over
        // source and destination sizes, since whether the unit is lost depends on the
        // fractions the ratio between them happens to land on.
        pntr_color opaque = pntr_new_color(200, 100, 50, 255);

        for (int sourceWidth = 1; sourceWidth <= 8; sourceWidth += 2) {
            for (int sourceHeight = 1; sourceHeight <= 8; sourceHeight += 2) {
                pntr_image* src = pntr_gen_image_color(sourceWidth, sourceHeight, opaque);
                NEQUALS(src, NULL);

                for (int width = 1; width <= 23; width += 2) {
                    for (int height = 1; height <= 17; height += 2) {
                        pntr_image* dst = pntr_gen_image_color(width, height, PNTR_BLANK);
                        NEQUALS(dst, NULL);

                        pntr_rectangle srcRect = PNTR_CLITERAL(pntr_rectangle) {0, 0, sourceWidth, sourceHeight};
                        pntr_rectangle dstRect = PNTR_CLITERAL(pntr_rectangle) {0, 0, width, height};
                        pntr_draw_image_dest_rec(dst, src, srcRect, dstRect, PNTR_FILTER_BILINEAR, PNTR_WHITE);

                        // Every destination pixel is the source color exactly, on all four
                        // channels. The alpha is the one that mattered, but the same
                        // truncation shaved red, green and blue in the same places.
                        int exact = 0;
                        for (int y = 0; y < height; y++) {
                            for (int x = 0; x < width; x++) {
                                if (pntr_image_get_color(dst, x, y).value == opaque.value) {
                                    exact++;
                                }
                            }
                        }
                        EQUALS(exact, width * height);

                        pntr_unload_image(dst);
                    }
                }

                pntr_unload_image(src);
            }
        }
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

    IT("pntr_draw_image_scaled_rec() with no offset starts at the position", {
        // The offset is an edge rather than a pivot, so no offset draws the source rectangle
        // starting exactly at the position, at every scale. This is the common case, and the
        // rotated draws name the center of the offset pixel instead, so this is the guard
        // that keeps the half pixel between the two conventions out of here.
        pntr_image* src = pntr_gen_image_color(8, 6, PNTR_RED);
        NEQUALS(src, NULL);

        pntr_rectangle srcRect = {0, 0, 8, 6};
        float scales[8] = {0.5f, 0.75f, 1.0f, 1.25f, 1.5f, 2.0f, 4.0f, 8.0f};

        for (int i = 0; i < 8; i++) {
            pntr_image* dst = pntr_test_canvas(100, 80);
            NEQUALS(dst, NULL);

            pntr_draw_image_scaled_rec(dst, src, srcRect, 20, 30, scales[i], scales[i],
                0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);

            // The drawn size is the source size scaled and truncated, which is what the
            // destination rectangle drawing relies on to fill a rectangle exactly.
            pntr_rectangle expected = PNTR_CLITERAL(pntr_rectangle) {
                .x = 20,
                .y = 30,
                .width = (int)(8.0f * scales[i]),
                .height = (int)(6.0f * scales[i])
            };
            pntr_rectangle bounds = pntr_test_painted_bounds(dst, PNTR_TEST_BACKGROUND);
            RECTEQUALS(bounds, expected);

            pntr_unload_image(dst);
        }

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

        IT("pntr_draw_image_dest_rec() fills the destination rectangle at every size", {
            // Every destination rectangle is painted whole, with nothing landing outside of
            // it, whatever scale it works out to. The scaled drawing underneath offsets from
            // the edge of the source pixel it is given rather than pivoting around its
            // center, which is what lets a zero offset fill the rectangle from its corner at
            // any scale. A pivot there would pull the fill half a destination pixel off the
            // corner it was asked for and leave a gap on the far side.
            pntr_image* src = pntr_gen_image_color(8, 6, PNTR_BLANK);
            NEQUALS(src, NULL);
            for (int y = 0; y < 6; y++) {
                for (int x = 0; x < 8; x++) {
                    pntr_draw_point(src, x, y, pntr_new_color(
                        (unsigned char)(x * 30 + 5),
                        (unsigned char)(y * 40 + 7),
                        (unsigned char)(x * y + 1),
                        255));
                }
            }

            pntr_rectangle srcRect = {0, 0, 8, 6};
            pntr_filter filters[2] = {PNTR_FILTER_NEARESTNEIGHBOR, PNTR_FILTER_BILINEAR};

            for (int f = 0; f < 2; f++) {
                for (int height = 1; height <= 20; height++) {
                    for (int width = 1; width <= 25; width++) {
                        pntr_image* dst = pntr_test_canvas(40, 36);
                        NEQUALS(dst, NULL);

                        pntr_rectangle dstRect = PNTR_CLITERAL(pntr_rectangle) { .x = 5, .y = 7, .width = width, .height = height };
                        pntr_draw_image_dest_rec(dst, src, srcRect, dstRect, filters[f], PNTR_WHITE);

                        // The bounds pin where the fill starts and stops, and the count pins
                        // that there are no holes left inside of them.
                        pntr_rectangle bounds = pntr_test_painted_bounds(dst, PNTR_TEST_BACKGROUND);
                        RECTEQUALS(bounds, dstRect);

                        // Exactly, in both filters and whether or not this build blends. An
                        // opaque source interpolates to an opaque destination, so there is
                        // nothing for PNTR_NO_ALPHABLEND to skip.
                        EQUALS(pntr_test_painted_count(dst), width * height);

                        pntr_unload_image(dst);
                    }
                }
            }

            pntr_unload_image(src);
        });

        IT("pntr_draw_image_dest_rec() reproduces the source region at every 1:1 size", {
            // The identity case of the scale above: a destination rectangle the size of the
            // source region copies it across untouched, for every region size, which is only
            // true while a zero offset keeps drawing from the corner of the rectangle.
            pntr_image* src = pntr_gen_image_color(12, 10, PNTR_BLANK);
            NEQUALS(src, NULL);
            for (int y = 0; y < 10; y++) {
                for (int x = 0; x < 12; x++) {
                    pntr_draw_point(src, x, y, pntr_new_color(
                        (unsigned char)(x * 20 + 11),
                        (unsigned char)(y * 25 + 3),
                        (unsigned char)(x + y * 12 + 1),
                        255));
                }
            }

            for (int height = 1; height <= 10; height++) {
                for (int width = 1; width <= 12; width++) {
                    pntr_image* dst = pntr_test_canvas(20, 18);
                    NEQUALS(dst, NULL);

                    pntr_rectangle srcRect = PNTR_CLITERAL(pntr_rectangle) { .x = 0, .y = 0, .width = width, .height = height };
                    pntr_rectangle dstRect = PNTR_CLITERAL(pntr_rectangle) { .x = 4, .y = 3, .width = width, .height = height };
                    pntr_draw_image_dest_rec(dst, src, srcRect, dstRect, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);

                    for (int y = 0; y < height; y++) {
                        for (int x = 0; x < width; x++) {
                            COLOREQUALS(pntr_image_get_color(dst, 4 + x, 3 + y), pntr_image_get_color(src, x, y));
                        }
                    }

                    pntr_rectangle bounds = pntr_test_painted_bounds(dst, PNTR_TEST_BACKGROUND);
                    RECTEQUALS(bounds, dstRect);

                    pntr_unload_image(dst);
                }
            }

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

    IT("pntr_draw_image_rec() lands the source rectangle on the position", {
        // Asserted against the coordinates the rectangle names rather than against
        // another pntr call. pntr_draw_image_rec() is the oracle the
        // pntr_draw_image_dest_rec() tests are written against, so a test of it that
        // asked a second pntr function where a pixel came from would pass just as well
        // if both of them ignored srcRect.x and srcRect.y.
        //
        // Every pixel of the source carries its own position in its red and green
        // channels, so the color found at a destination pixel says which source pixel was
        // read for it, and the expectation below is arithmetic on the rectangle.
        pntr_image* src = pntr_test_coordinate_image(20, 20);
        pntr_image* dst = pntr_test_canvas(40, 40);
        NEQUALS(src, NULL);
        NEQUALS(dst, NULL);

        pntr_rectangle srcRect = PNTR_CLITERAL(pntr_rectangle) {5, 3, 7, 4};
        int posX = 9;
        int posY = 11;
        pntr_draw_image_rec(dst, src, srcRect, posX, posY);

        for (int y = 0; y < srcRect.height; y++) {
            for (int x = 0; x < srcRect.width; x++) {
                pntr_color expected = pntr_new_color(
                    (unsigned char)(10 + (srcRect.x + x) * 9),
                    (unsigned char)(10 + (srcRect.y + y) * 9),
                    180, 255);
                pntr_color actual = pntr_image_get_color(dst, posX + x, posY + y);
                if (actual.value != expected.value) {
                    printf("\n\tpntr_draw_image_rec() at (%d, %d)\n", posX + x, posY + y);
                }
                COLOREQUALS(actual, expected);
            }
        }

        // And the rectangle is the only thing that was drawn: nothing spilled past it,
        // and no row or column of it was skipped.
        pntr_rectangle expectedBounds = PNTR_CLITERAL(pntr_rectangle) {posX, posY, srcRect.width, srcRect.height};
        pntr_rectangle actualBounds = pntr_test_painted_bounds(dst, PNTR_TEST_BACKGROUND);
        RECTEQUALS(actualBounds, expectedBounds);
        EQUALS(pntr_test_painted_count(dst), srcRect.width * srcRect.height);

        // The tinted form reads the same source pixels, then multiplies each of them by
        // the tint.
        pntr_image* tinted = pntr_test_canvas(40, 40);
        NEQUALS(tinted, NULL);
        pntr_color tint = pntr_new_color(128, 200, 64, 255);
        pntr_draw_image_tint_rec(tinted, src, srcRect, posX, posY, tint);

        for (int y = 0; y < srcRect.height; y++) {
            for (int x = 0; x < srcRect.width; x++) {
                pntr_color expected = pntr_color_tint(pntr_new_color(
                    (unsigned char)(10 + (srcRect.x + x) * 9),
                    (unsigned char)(10 + (srcRect.y + y) * 9),
                    180, 255), tint);
                pntr_color actual = pntr_image_get_color(tinted, posX + x, posY + y);
                if (actual.value != expected.value) {
                    printf("\n\tpntr_draw_image_tint_rec() at (%d, %d)\n", posX + x, posY + y);
                }
                COLOREQUALS(actual, expected);
            }
        }

        pntr_unload_image(tinted);
        pntr_unload_image(dst);
        pntr_unload_image(src);
    });

    IT("pntr_draw_image_flipped_rec()", {
        IT("pntr_draw_image_flipped_rec() reads from the source rectangle", {
            // The reproduction from https://github.com/RobLoach/pntr/issues/252. The
            // rectangle's origin used to size the sampling loops without offsetting the
            // read, so a flipped sprite came out of the top left corner of the sheet
            // instead of out of the rectangle.
            pntr_image* src = pntr_gen_image_color(4, 1, PNTR_BLANK);
            NEQUALS(src, NULL);
            pntr_draw_point(src, 0, 0, PNTR_RED);
            pntr_draw_point(src, 1, 0, PNTR_GREEN);
            pntr_draw_point(src, 2, 0, PNTR_BLUE);
            pntr_draw_point(src, 3, 0, PNTR_WHITE);

            // The blue and white pixels.
            pntr_rectangle rect = {2, 0, 2, 1};

            pntr_image* expected = pntr_gen_image_color(2, 1, PNTR_BLACK);
            NEQUALS(expected, NULL);
            pntr_draw_point(expected, 0, 0, PNTR_BLUE);
            pntr_draw_point(expected, 1, 0, PNTR_WHITE);

            pntr_image* dst = pntr_gen_image_color(2, 1, PNTR_BLACK);
            NEQUALS(dst, NULL);
            pntr_draw_image_rec(dst, src, rect, 0, 0);
            IMAGEEQUALS(dst, expected);

            // A vertical flip of a single row is the identity, so it has to agree. This
            // used to draw the red and green pixels instead.
            pntr_clear_background(dst, PNTR_BLACK);
            pntr_draw_image_flipped_rec(dst, src, rect, 0, 0, false, true, false);
            IMAGEEQUALS(dst, expected);

            // So does asking for no flips at all, which delegates to the unflipped draw.
            pntr_clear_background(dst, PNTR_BLACK);
            pntr_draw_image_flipped_rec(dst, src, rect, 0, 0, false, false, false);
            IMAGEEQUALS(dst, expected);

            pntr_unload_image(expected);
            pntr_unload_image(dst);
            pntr_unload_image(src);
        });

        IT("pntr_draw_image_flipped_rec() with no flips matches pntr_draw_image_rec()", {
            // The no-flip path delegates to the unflipped blit, so the two can only agree
            // once the flipping path reads from the same place. Every rectangle here is a
            // different origin, which is what the two used to disagree about.
            pntr_image* src = pntr_test_coordinate_image(8, 6);
            NEQUALS(src, NULL);

            pntr_rectangle rects[6] = {
                {0, 0, 8, 6},
                {0, 0, 3, 2},
                {2, 1, 4, 3},
                {5, 4, 3, 2},
                {7, 5, 1, 1},
                {6, 2, 4, 6}    // Hangs off of the right and bottom edges of the source.
            };

            for (int i = 0; i < 6; i++) {
                pntr_image* expected = pntr_test_canvas(14, 12);
                NEQUALS(expected, NULL);
                pntr_draw_image_rec(expected, src, rects[i], 2, 3);

                pntr_image* dst = pntr_test_canvas(14, 12);
                NEQUALS(dst, NULL);
                pntr_draw_image_flipped_rec(dst, src, rects[i], 2, 3, false, false, false);
                IMAGEEQUALS(dst, expected);

                pntr_unload_image(dst);
                pntr_unload_image(expected);
            }

            pntr_unload_image(src);
        });

        IT("pntr_draw_image_flipped_rec() mirrors the rectangle and not the image", {
            // The bottom right 2x2 of the source, named pixel by pixel so that a region
            // taken from the top left corner instead cannot pass.
            pntr_image* src = pntr_test_coordinate_image(4, 4);
            NEQUALS(src, NULL);
            pntr_rectangle rect = {2, 2, 2, 2};

            pntr_image* dst = pntr_test_canvas(2, 2);
            NEQUALS(dst, NULL);

            // Horizontally, the rectangle's two columns swap.
            pntr_draw_image_flipped_rec(dst, src, rect, 0, 0, true, false, false);
            COLOREQUALS(pntr_image_get_color(dst, 0, 0), pntr_image_get_color(src, 3, 2));
            COLOREQUALS(pntr_image_get_color(dst, 1, 0), pntr_image_get_color(src, 2, 2));
            COLOREQUALS(pntr_image_get_color(dst, 0, 1), pntr_image_get_color(src, 3, 3));
            COLOREQUALS(pntr_image_get_color(dst, 1, 1), pntr_image_get_color(src, 2, 3));

            // Vertically, its two rows swap.
            pntr_clear_background(dst, PNTR_TEST_BACKGROUND);
            pntr_draw_image_flipped_rec(dst, src, rect, 0, 0, false, true, false);
            COLOREQUALS(pntr_image_get_color(dst, 0, 0), pntr_image_get_color(src, 2, 3));
            COLOREQUALS(pntr_image_get_color(dst, 1, 0), pntr_image_get_color(src, 3, 3));
            COLOREQUALS(pntr_image_get_color(dst, 0, 1), pntr_image_get_color(src, 2, 2));
            COLOREQUALS(pntr_image_get_color(dst, 1, 1), pntr_image_get_color(src, 3, 2));

            // Both at once is a half turn of the rectangle.
            pntr_clear_background(dst, PNTR_TEST_BACKGROUND);
            pntr_draw_image_flipped_rec(dst, src, rect, 0, 0, true, true, false);
            COLOREQUALS(pntr_image_get_color(dst, 0, 0), pntr_image_get_color(src, 3, 3));
            COLOREQUALS(pntr_image_get_color(dst, 1, 0), pntr_image_get_color(src, 2, 3));
            COLOREQUALS(pntr_image_get_color(dst, 0, 1), pntr_image_get_color(src, 3, 2));
            COLOREQUALS(pntr_image_get_color(dst, 1, 1), pntr_image_get_color(src, 2, 2));

            // The diagonal flip transposes the rectangle, swapping its rows for its columns.
            pntr_clear_background(dst, PNTR_TEST_BACKGROUND);
            pntr_draw_image_flipped_rec(dst, src, rect, 0, 0, false, false, true);
            COLOREQUALS(pntr_image_get_color(dst, 0, 0), pntr_image_get_color(src, 2, 2));
            COLOREQUALS(pntr_image_get_color(dst, 1, 0), pntr_image_get_color(src, 2, 3));
            COLOREQUALS(pntr_image_get_color(dst, 0, 1), pntr_image_get_color(src, 3, 2));
            COLOREQUALS(pntr_image_get_color(dst, 1, 1), pntr_image_get_color(src, 3, 3));

            pntr_unload_image(dst);
            pntr_unload_image(src);
        });

        IT("pntr_draw_image_flipped_rec() flips every orientation of a sub-rectangle", {
            // Each of the eight flag combinations, against a region cropped out of the
            // source and flipped on its own, which is an answer that does not share any
            // arithmetic with the drawing path.
            pntr_image* src = pntr_test_coordinate_image(8, 6);
            NEQUALS(src, NULL);

            pntr_rectangle rects[5] = {
                {0, 0, 8, 6},
                {2, 1, 4, 3},
                {3, 0, 2, 6},
                {0, 4, 8, 2},
                {5, 3, 3, 3}
            };

            for (int i = 0; i < 5; i++) {
                for (int flips = 0; flips < 8; flips++) {
                    bool flipHorizontal = (flips & 1) != 0;
                    bool flipVertical = (flips & 2) != 0;
                    bool flipDiagonal = (flips & 4) != 0;

                    pntr_image* region = pntr_test_flipped_region(src, rects[i], flipHorizontal, flipVertical, flipDiagonal);
                    NEQUALS(region, NULL);

                    pntr_image* expected = pntr_test_canvas(20, 20);
                    NEQUALS(expected, NULL);
                    pntr_draw_image(expected, region, 3, 4);

                    pntr_image* dst = pntr_test_canvas(20, 20);
                    NEQUALS(dst, NULL);
                    pntr_draw_image_flipped_rec(dst, src, rects[i], 3, 4, flipHorizontal, flipVertical, flipDiagonal);
                    IMAGEEQUALS(dst, expected);

                    pntr_unload_image(dst);
                    pntr_unload_image(expected);
                    pntr_unload_image(region);
                }
            }

            pntr_unload_image(src);
        });

        IT("pntr_draw_image_flipped_rec() keeps a rectangle at the source corner in bounds", {
            // A rectangle sitting on the far corner is where an unoffset read used to stay
            // safe and an offset one can run past the last pixel, so this is the block
            // worth running under -fsanitize=address.
            pntr_image* src = pntr_test_coordinate_image(8, 6);
            NEQUALS(src, NULL);

            pntr_rectangle corners[4] = {
                {7, 5, 1, 1},   // The single bottom right pixel.
                {6, 4, 2, 2},   // The bottom right corner exactly.
                {6, 4, 8, 8},   // Running off of the far corner, which clamps to the above.
                {7, 0, 1, 6}    // The last column.
            };

            for (int i = 0; i < 4; i++) {
                for (int flips = 0; flips < 8; flips++) {
                    bool flipHorizontal = (flips & 1) != 0;
                    bool flipVertical = (flips & 2) != 0;
                    bool flipDiagonal = (flips & 4) != 0;

                    pntr_image* region = pntr_test_flipped_region(src, corners[i], flipHorizontal, flipVertical, flipDiagonal);
                    NEQUALS(region, NULL);

                    pntr_image* expected = pntr_test_canvas(12, 12);
                    NEQUALS(expected, NULL);
                    pntr_draw_image(expected, region, 1, 2);

                    pntr_image* dst = pntr_test_canvas(12, 12);
                    NEQUALS(dst, NULL);
                    pntr_draw_image_flipped_rec(dst, src, corners[i], 1, 2, flipHorizontal, flipVertical, flipDiagonal);
                    IMAGEEQUALS(dst, expected);

                    pntr_unload_image(dst);
                    pntr_unload_image(expected);
                    pntr_unload_image(region);
                }
            }

            pntr_unload_image(src);
        });

        IT("pntr_draw_image_flipped_rec() clips without shifting what it samples", {
            // Clipping trims the destination, and the source offset has to compose with it
            // rather than slide the region, so the pixels that survive have to be the same
            // ones a full draw would have put there.
            pntr_image* src = pntr_test_coordinate_image(8, 6);
            NEQUALS(src, NULL);
            pntr_rectangle rect = {2, 1, 4, 3};

            IT("partially off of the destination", {
                int positions[4][2] = { {-2, -1}, {6, 5}, {-3, 4}, {5, -2} };

                for (int i = 0; i < 4; i++) {
                    for (int flips = 0; flips < 8; flips++) {
                        bool flipHorizontal = (flips & 1) != 0;
                        bool flipVertical = (flips & 2) != 0;
                        bool flipDiagonal = (flips & 4) != 0;

                        pntr_image* region = pntr_test_flipped_region(src, rect, flipHorizontal, flipVertical, flipDiagonal);
                        NEQUALS(region, NULL);

                        pntr_image* expected = pntr_test_canvas(8, 8);
                        NEQUALS(expected, NULL);
                        pntr_draw_image(expected, region, positions[i][0], positions[i][1]);

                        pntr_image* dst = pntr_test_canvas(8, 8);
                        NEQUALS(dst, NULL);
                        pntr_draw_image_flipped_rec(dst, src, rect, positions[i][0], positions[i][1],
                            flipHorizontal, flipVertical, flipDiagonal);
                        IMAGEEQUALS(dst, expected);

                        pntr_unload_image(dst);
                        pntr_unload_image(expected);
                        pntr_unload_image(region);
                    }
                }
            });

            IT("against the clip rectangle of the destination", {
                pntr_rectangle clip = {3, 3, 3, 2};

                for (int flips = 0; flips < 8; flips++) {
                    bool flipHorizontal = (flips & 1) != 0;
                    bool flipVertical = (flips & 2) != 0;
                    bool flipDiagonal = (flips & 4) != 0;

                    pntr_image* region = pntr_test_flipped_region(src, rect, flipHorizontal, flipVertical, flipDiagonal);
                    NEQUALS(region, NULL);

                    pntr_image* expected = pntr_test_canvas(10, 10);
                    NEQUALS(expected, NULL);
                    pntr_image_set_clip(expected, clip.x, clip.y, clip.width, clip.height);
                    RECTEQUALS(pntr_image_get_clip(expected), clip);
                    pntr_draw_image(expected, region, 2, 2);
                    pntr_image_reset_clip(expected);

                    pntr_image* dst = pntr_test_canvas(10, 10);
                    NEQUALS(dst, NULL);
                    pntr_image_set_clip(dst, clip.x, clip.y, clip.width, clip.height);
                    pntr_draw_image_flipped_rec(dst, src, rect, 2, 2, flipHorizontal, flipVertical, flipDiagonal);
                    pntr_image_reset_clip(dst);

                    // Only the clip region was painted, and with the pixels the unclipped
                    // draw would have left there.
                    IMAGEEQUALS(dst, expected);
                    GREATER(pntr_test_painted_count(dst), 0);

                    pntr_unload_image(dst);
                    pntr_unload_image(expected);
                    pntr_unload_image(region);
                }
            });

            pntr_unload_image(src);
        });

        IT("pntr_draw_image_flipped_rec() ignores invalid arguments", {
            pntr_image* src = pntr_test_coordinate_image(8, 6);
            NEQUALS(src, NULL);
            pntr_image* dst = pntr_test_canvas(10, 10);
            NEQUALS(dst, NULL);
            pntr_image* expected = pntr_test_canvas(10, 10);
            NEQUALS(expected, NULL);

            pntr_rectangle rect = {2, 1, 4, 3};

            // NULL images do not crash, and draw nothing.
            pntr_draw_image_flipped_rec(NULL, src, rect, 0, 0, true, true, true);
            pntr_draw_image_flipped_rec(dst, NULL, rect, 0, 0, true, true, true);
            pntr_draw_image_flipped_rec(NULL, NULL, rect, 0, 0, true, true, true);
            pntr_draw_image_flipped_rec(NULL, src, rect, 0, 0, false, false, false);
            pntr_draw_image_flipped_rec(dst, NULL, rect, 0, 0, false, false, false);
            IMAGEEQUALS(dst, expected);

            // A rectangle that is entirely off of the source draws nothing.
            pntr_rectangle offSource[2] = { {20, 20, 4, 4}, {-8, -8, 4, 4} };
            for (int i = 0; i < 2; i++) {
                for (int flips = 0; flips < 8; flips++) {
                    pntr_draw_image_flipped_rec(dst, src, offSource[i], 0, 0,
                        (flips & 1) != 0, (flips & 2) != 0, (flips & 4) != 0);
                }
            }
            IMAGEEQUALS(dst, expected);

            // A flipped draw reads a width or height of zero or less as an empty region
            // and paints nothing. The unflipped path deliberately differs, reading it as
            // the full source dimension, which is what pntr_draw_image_rec() does.
            pntr_rectangle sizeless[4] = {
                {0, 0, 0, 6},
                {0, 0, 8, 0},
                {0, 0, -8, 6},
                {0, 0, 8, -6}
            };
            for (int i = 0; i < 4; i++) {
                for (int flips = 1; flips < 8; flips++) {
                    pntr_draw_image_flipped_rec(dst, src, sizeless[i], 0, 0,
                        (flips & 1) != 0, (flips & 2) != 0, (flips & 4) != 0);
                }
            }
            IMAGEEQUALS(dst, expected);

            // The documented exception: no flips means the full source is drawn instead.
            pntr_draw_image_flipped_rec(dst, src, sizeless[0], 0, 0, false, false, false);
            pntr_draw_image(expected, src, 0, 0);
            IMAGEEQUALS(dst, expected);

            pntr_unload_image(expected);
            pntr_unload_image(dst);
            pntr_unload_image(src);
        });
    });

    IT("pntr_draw_image_flipped()", {
        // The whole-image variant hands the full rectangle down, so its origin is zero and
        // the source offset cannot change what it draws.
        pntr_image* src = pntr_test_coordinate_image(8, 6);
        NEQUALS(src, NULL);
        pntr_rectangle whole = {0, 0, 8, 6};

        for (int flips = 0; flips < 8; flips++) {
            bool flipHorizontal = (flips & 1) != 0;
            bool flipVertical = (flips & 2) != 0;
            bool flipDiagonal = (flips & 4) != 0;

            pntr_image* region = pntr_test_flipped_region(src, whole, flipHorizontal, flipVertical, flipDiagonal);
            NEQUALS(region, NULL);

            pntr_image* expected = pntr_test_canvas(20, 20);
            NEQUALS(expected, NULL);
            pntr_draw_image(expected, region, 4, 5);

            pntr_image* dst = pntr_test_canvas(20, 20);
            NEQUALS(dst, NULL);
            pntr_draw_image_flipped(dst, src, 4, 5, flipHorizontal, flipVertical, flipDiagonal);
            IMAGEEQUALS(dst, expected);

            // And it agrees with handing the same full rectangle down by hand.
            pntr_image* viaRec = pntr_test_canvas(20, 20);
            NEQUALS(viaRec, NULL);
            pntr_draw_image_flipped_rec(viaRec, src, whole, 4, 5, flipHorizontal, flipVertical, flipDiagonal);
            IMAGEEQUALS(viaRec, expected);

            pntr_unload_image(viaRec);
            pntr_unload_image(dst);
            pntr_unload_image(expected);
            pntr_unload_image(region);
        }

        // No flips at all draws the image exactly as pntr_draw_image() would.
        pntr_image* plain = pntr_test_canvas(20, 20);
        NEQUALS(plain, NULL);
        pntr_draw_image_flipped(plain, src, 4, 5, false, false, false);

        pntr_image* plainExpected = pntr_test_canvas(20, 20);
        NEQUALS(plainExpected, NULL);
        pntr_draw_image(plainExpected, src, 4, 5);
        IMAGEEQUALS(plain, plainExpected);

        // NULL images do not crash, and draw nothing.
        pntr_draw_image_flipped(NULL, src, 0, 0, true, true, true);
        pntr_draw_image_flipped(plain, NULL, 0, 0, true, true, true);
        pntr_draw_image_flipped(NULL, NULL, 0, 0, false, false, false);
        IMAGEEQUALS(plain, plainExpected);

        pntr_unload_image(plainExpected);
        pntr_unload_image(plain);
        pntr_unload_image(src);
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

            IT("pntr_draw_line_aa() anti-aliases a diagonal", {
                // The axis-aligned cases above hand the line straight to
                // pntr_draw_line_horizontal() or pntr_draw_line_vertical() and return, so
                // they never reach Xiaolin Wu's algorithm at all. A diagonal does, and the
                // four below are the shallow and the steep case in both directions, which
                // is every combination of the two swaps the algorithm opens with.
                int lines[4][4] = {
                    { 5, 20, 35, 30}, // shallow, left to right
                    {35, 30,  5, 20}, // shallow, right to left
                    {20,  5, 30, 35}, // steep, top to bottom
                    {30, 35, 20,  5}  // steep, bottom to top
                };

                for (int i = 0; i < 4; i++) {
                    int startX = lines[i][0];
                    int startY = lines[i][1];
                    int endX = lines[i][2];
                    int endY = lines[i][3];

                    // Drawn onto a fully transparent canvas, so that the alpha each pixel
                    // was given survives to be read back rather than being blended into
                    // an opaque background.
                    pntr_image* image = pntr_new_image(40, 40);
                    NEQUALS(image, NULL);
                    pntr_clear_background(image, PNTR_BLANK);

                    pntr_draw_line_aa(image, startX, startY, endX, endY, PNTR_RED);

                    // Every pixel Xiaolin Wu's algorithm plots is a partial one, and
                    // pntr_blend_color() skips those outright without alpha blending, so
                    // the whole line is dropped rather than merely un-anti-aliased.
                    if (!pntr_alphablend()) {
                        EQUALS(pntr_test_blended_count(image), 0);
                        pntr_unload_image(image);
                        continue;
                    }

                    // Both endpoints are painted, the same as pntr_draw_line() paints them.
                    GREATER((int)pntr_color_a(pntr_image_get_color(image, startX, startY)), 0);
                    GREATER((int)pntr_color_a(pntr_image_get_color(image, endX, endY)), 0);

                    // And nothing is painted outside the box the two endpoints span: the
                    // partial pixels straddle the line, they do not overhang its ends.
                    pntr_rectangle expectedBounds = PNTR_CLITERAL(pntr_rectangle) {
                        startX < endX ? startX : endX,
                        startY < endY ? startY : endY,
                        (startX < endX ? endX - startX : startX - endX) + 1,
                        (startY < endY ? endY - startY : startY - endY) + 1
                    };
                    pntr_rectangle actualBounds = pntr_test_painted_bounds(image, PNTR_BLANK);
                    RECTEQUALS(actualBounds, expectedBounds);

                    // Which is the anti-aliasing: a line that fell through to plotting
                    // whole pixels leaves every pixel it touched fully opaque.
                    GREATER(pntr_test_partial_alpha_count(image), 0);

                    pntr_unload_image(image);
                }
            });

            IT("pntr_draw_line_aa() covers the same bounds as pntr_draw_line()", {
                // The same diagonals, against the line that is not anti-aliased. The two
                // do not paint the same pixels, but they run between the same endpoints,
                // so they occupy the same area.
                int lines[4][4] = {
                    { 5, 20, 35, 30},
                    {35, 30,  5, 20},
                    {20,  5, 30, 35},
                    {30, 35, 20,  5}
                };

                for (int i = 0; i < 4; i++) {
                    pntr_image* aa = pntr_test_canvas(40, 40);
                    pntr_image* plain = pntr_test_canvas(40, 40);
                    NEQUALS(aa, NULL);
                    NEQUALS(plain, NULL);

                    pntr_draw_line_aa(aa, lines[i][0], lines[i][1], lines[i][2], lines[i][3], PNTR_RED);
                    pntr_draw_line(plain, lines[i][0], lines[i][1], lines[i][2], lines[i][3], PNTR_RED);

                    // Without blending the anti-aliased line paints nothing at all, since
                    // every pixel it plots is a partial one, while the plain line is
                    // unaffected because the colour it writes is opaque.
                    if (!pntr_alphablend()) {
                        EQUALS(pntr_test_painted_count(aa), 0);
                        GREATER(pntr_test_painted_count(plain), 0);
                        pntr_unload_image(aa);
                        pntr_unload_image(plain);
                        continue;
                    }

                    pntr_rectangle aaBounds = pntr_test_painted_bounds(aa, PNTR_TEST_BACKGROUND);
                    pntr_rectangle plainBounds = pntr_test_painted_bounds(plain, PNTR_TEST_BACKGROUND);
                    RECTEQUALS(aaBounds, plainBounds);

                    // The anti-aliased line paints the pixels on both sides of the one the
                    // plain line stepped onto, so it covers strictly more of them.
                    GREATER(pntr_test_painted_count(aa), pntr_test_painted_count(plain));

                    pntr_unload_image(aa);
                    pntr_unload_image(plain);
                }
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

            // Stated first, because the loop below says nothing at all about an outline
            // that painted nothing.
            pntr_rectangle expected = PNTR_CLITERAL(pntr_rectangle) {5, 5, 21, 21};
            pntr_rectangle outlineBounds = pntr_test_painted_bounds(outline, PNTR_TEST_BACKGROUND);
            pntr_rectangle fillBounds = pntr_test_painted_bounds(fill, PNTR_TEST_BACKGROUND);
            RECTEQUALS(outlineBounds, expected);
            RECTEQUALS(fillBounds, expected);

            // An axis-aligned square has no shallow edges to round out, so here the fill
            // does cover every single pixel of the outline.
            EQUALS(pntr_test_uncovered(outline, fill), 0);
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

        IT("A thickness far larger than the canvas finishes", {
            // The brush used to walk its whole thickness box for every point of the line,
            // without clipping it first and scanning for each row's first column rather
            // than computing it, so the cost was quadratic in a thickness the caller
            // chooses. pntr_draw_line_thick() at a thickness of a million on a 20x20
            // canvas did not finish; the curve put it in the hours. These all return
            // immediately now, and a test that hangs is a test that failed.
            int thicknesses[4] = {100000, 1000000, 100000000, INT_MAX};

            for (int i = 0; i < 4; i++) {
                pntr_image* image = pntr_test_canvas(20, 20);
                NEQUALS(image, NULL);

                // A brush this wide covers every pixel of the canvas from any center.
                pntr_draw_line_thick(image, 0, 10, 19, 10, thicknesses[i], PNTR_RED);
                EQUALS(pntr_test_painted_count(image), 20 * 20);

                pntr_unload_image(image);

                pntr_image* circle = pntr_test_canvas(20, 20);
                NEQUALS(circle, NULL);

                pntr_draw_circle_thick(circle, 10, 10, 5, thicknesses[i], PNTR_RED);
                EQUALS(pntr_test_painted_count(circle), 20 * 20);

                pntr_unload_image(circle);
            }
        });

        IT("A thick brush stays inside the clip", {
            // Clamping the brush to the clip rows is what made the above fast, so the
            // clamp has to be right at every edge and corner rather than only in the
            // middle. Each center below puts part of the brush outside the clip.
            int centers[9][2] = {
                {0, 0}, {39, 0}, {0, 39}, {39, 39},
                {20, 0}, {0, 20}, {39, 20}, {20, 39}, {20, 20}
            };

            for (int i = 0; i < 9; i++) {
                for (int thickness = 2; thickness <= 12; thickness++) {
                    pntr_image* image = pntr_test_canvas(40, 40);
                    NEQUALS(image, NULL);

                    pntr_image* reference = pntr_image_copy(image);
                    NEQUALS(reference, NULL);

                    pntr_image_set_clip(image, 12, 14, 15, 13);
                    pntr_draw_line_thick(image, centers[i][0], centers[i][1], centers[i][0], centers[i][1], thickness, PNTR_RED);
                    pntr_image_reset_clip(image);

                    // Nothing outside the clip changed, and the clip is where it was.
                    int outside = 0;
                    for (int y = 0; y < 40; y++) {
                        for (int x = 0; x < 40; x++) {
                            bool inside = (x >= 12 && x < 27 && y >= 14 && y < 27);
                            if (!inside && pntr_image_get_color(image, x, y).value != pntr_image_get_color(reference, x, y).value) {
                                outside++;
                            }
                        }
                    }
                    EQUALS(outside, 0);

                    pntr_unload_image(reference);
                    pntr_unload_image(image);
                }
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

        IT("pntr_draw_ellipse_thick() grows with its thickness", {
            // The same shape as the circle: the brush straddles the outline, so each
            // extra pixel of thickness reaches one pixel further out on both axes. A
            // thickness of one is pntr_draw_ellipse() itself, which the differential
            // tests pin, so this is about what the thicknesses above one do.
            int painted[4] = {0};
            int width[4] = {0};
            int height[4] = {0};

            for (int thickness = 1; thickness <= 3; thickness++) {
                pntr_image* image = pntr_test_canvas(60, 40);
                NEQUALS(image, NULL);

                pntr_draw_ellipse_thick(image, 30, 20, 16, 9, thickness, PNTR_RED);

                pntr_rectangle bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
                painted[thickness] = pntr_test_painted_count(image);
                width[thickness] = bounds.width;
                height[thickness] = bounds.height;

                pntr_unload_image(image);
            }

            for (int thickness = 1; thickness <= 3; thickness++) {
                EQUALS(width[thickness], 16 * 2 + thickness);
                EQUALS(height[thickness], 9 * 2 + thickness);
            }

            GREATER(painted[1], 0);
            for (int thickness = 2; thickness <= 3; thickness++) {
                GREATER(painted[thickness], painted[thickness - 1]);
            }

            // A negative radius is the same ellipse as the positive one it mirrors, on
            // both axes and at every thickness.
            for (int thickness = 1; thickness <= 3; thickness++) {
                pntr_image* positive = pntr_test_canvas(60, 40);
                pntr_image* negative = pntr_test_canvas(60, 40);
                NEQUALS(positive, NULL);
                NEQUALS(negative, NULL);

                pntr_draw_ellipse_thick(positive, 30, 20, 16, 9, thickness, PNTR_RED);
                pntr_draw_ellipse_thick(negative, 30, 20, -16, -9, thickness, PNTR_RED);
                IMAGEEQUALS(negative, positive);

                pntr_unload_image(negative);
                pntr_unload_image(positive);
            }

            // And an ellipse that is nowhere near the canvas draws nothing at all.
            pntr_image* offscreen = pntr_test_canvas(60, 40);
            NEQUALS(offscreen, NULL);
            pntr_draw_ellipse_thick(offscreen, 400, 400, 16, 9, 3, PNTR_RED);
            pntr_draw_circle_thick(offscreen, 400, 400, 12, 3, PNTR_RED);
            EQUALS(pntr_test_painted_count(offscreen), 0);
            pntr_unload_image(offscreen);
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

        IT("A coordinate at the end of the int range draws nothing", {
            // pntr_draw_rectangle_fill() used to segfault here. The clip test computed
            // `x + width`, which overflows, so PNTR_MIN picked the wrapped negative sum
            // and the subtraction wrapped back to a positive width: an entirely off-image
            // rectangle was reported as visible and written through.
            pntr_image* image = pntr_test_canvas(40, 40);
            NEQUALS(image, NULL);

            int far = INT_MAX - 1;

            pntr_draw_rectangle_fill(image, far, 0, 10, 10, PNTR_RED);
            pntr_draw_rectangle(image, far, 0, 10, 10, PNTR_RED);
            pntr_draw_rectangle_fill(image, 0, far, 10, 10, PNTR_RED);
            pntr_draw_circle(image, far, 10, 5, PNTR_RED);
            pntr_draw_circle_fill(image, far, 10, 5, PNTR_RED);
            pntr_draw_circle_thick(image, far, 10, 5, 2, PNTR_RED);
            pntr_draw_ellipse(image, far, 10, 5, 5, PNTR_RED);
            pntr_draw_ellipse_fill(image, far, 10, 5, 5, PNTR_RED);
            pntr_draw_rectangle_rounded(image, far, 0, 10, 10, 2, 2, 2, 2, PNTR_RED);
            pntr_draw_rectangle_rounded_fill(image, far, 0, 10, 10, 2, PNTR_RED);
            pntr_draw_rectangle_thick(image, far, 0, 10, 10, 3, PNTR_RED);
            pntr_draw_line_horizontal(image, far, 5, 10, PNTR_RED);
            pntr_draw_line_vertical(image, 5, far, 10, PNTR_RED);
            pntr_draw_arc_fill(image, far, 10, 5.0f, 0.0f, 360.0f, 32, PNTR_RED);
            pntr_draw_point(image, far, far, PNTR_RED);

            EQUALS(pntr_test_painted_count(image), 0);

            // pntr_image_set_clip() leaves the clip alone when the rectangle it is given
            // does not reach the image, rather than clipping everything away, so the
            // following fill covers the canvas. That is the existing behavior, asserted
            // here only so the arithmetic above is not mistaken for it.
            pntr_image_set_clip(image, far, far, 10, 10);
            pntr_draw_rectangle_fill(image, 0, 0, 40, 40, PNTR_RED);
            pntr_image_reset_clip(image);

            EQUALS(pntr_test_painted_count(image), 40 * 40);

            pntr_unload_image(image);
        });

        IT("A rectangle the size of the int range covers the whole image", {
            // The other direction of the same arithmetic: the far edge overflows, so the
            // intersect has to come back as the whole clip rather than as nothing.
            pntr_image* image = pntr_test_canvas(40, 40);
            NEQUALS(image, NULL);

            pntr_draw_rectangle_fill(image, 0, 0, INT_MAX, INT_MAX, PNTR_RED);

            EQUALS(pntr_test_painted_count(image), 40 * 40);

            pntr_unload_image(image);
        });

        IT("A radius past the square root of the int range still fills", {
            // Squaring the radius overflowed an int above 46340, and the sum of two
            // squares overflowed above about 32767, so a circle wide enough to cover the
            // canvas was computed from a wrapped radius. Both are now widened. The sweep
            // crosses 32767 and 46340 rather than only sitting past them.
            int radii[5] = {32766, 32768, 46339, 46341, 1000000};

            for (int i = 0; i < 5; i++) {
                pntr_image* image = pntr_test_canvas(20, 20);
                NEQUALS(image, NULL);

                // Centered on the canvas, with a radius far larger than it, so every
                // pixel is inside the circle and the fill has to cover all of them.
                pntr_draw_circle_fill(image, 10, 10, radii[i], PNTR_RED);
                EQUALS(pntr_test_painted_count(image), 20 * 20);

                pntr_unload_image(image);
            }

            // The same for the thickness brush, which squares its thickness the same way.
            pntr_image* thick = pntr_test_canvas(20, 20);
            NEQUALS(thick, NULL);

            pntr_draw_line_thick(thick, 0, 10, 19, 10, 50000, PNTR_RED);
            EQUALS(pntr_test_painted_count(thick), 20 * 20);

            pntr_unload_image(thick);
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

            // The fill reaches every pixel of the outline of the same rectangle. Stated
            // through pntr_test_uncovered() as well as the loop, because the loop on its
            // own says nothing about an outline that painted nothing.
            EQUALS(pntr_test_uncovered(outline, rounded), 0);
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
            //
            // Without pntr_alphablend() there is no blending to count: a non-opaque source
            // is skipped rather than blended, so every one of these draws leaves its
            // transparent image exactly as it found it and the alpha stays at 0.
            pntr_color semi = pntr_new_color(230, 41, 55, 128);
            int semiAlpha = pntr_alphablend() ? 128 : 0;

            IT("pntr_draw_rectangle() one pixel tall", {
                pntr_image* image = pntr_new_image(40, 40);
                NEQUALS(image, NULL);
                pntr_clear_background(image, PNTR_BLANK);

                pntr_draw_rectangle(image, 5, 5, 20, 1, semi);
                for (int x = 5; x < 25; x++) {
                    EQUALS((int)pntr_color_a(pntr_image_get_color(image, x, 5)), semiAlpha);
                }

                pntr_unload_image(image);
            });

            IT("pntr_draw_rectangle() one pixel wide", {
                pntr_image* image = pntr_new_image(40, 40);
                NEQUALS(image, NULL);
                pntr_clear_background(image, PNTR_BLANK);

                pntr_draw_rectangle(image, 5, 5, 1, 20, semi);
                for (int y = 5; y < 25; y++) {
                    EQUALS((int)pntr_color_a(pntr_image_get_color(image, 5, y)), semiAlpha);
                }

                pntr_unload_image(image);
            });

            IT("pntr_draw_rectangle() with a hollow middle", {
                pntr_image* image = pntr_new_image(40, 40);
                NEQUALS(image, NULL);
                pntr_clear_background(image, PNTR_BLANK);

                // The corners are where the horizontal and vertical edges used to meet.
                pntr_draw_rectangle(image, 5, 5, 20, 12, semi);
                EQUALS((int)pntr_color_a(pntr_image_get_color(image, 5, 5)), semiAlpha);
                EQUALS((int)pntr_color_a(pntr_image_get_color(image, 24, 5)), semiAlpha);
                EQUALS((int)pntr_color_a(pntr_image_get_color(image, 5, 16)), semiAlpha);
                EQUALS((int)pntr_color_a(pntr_image_get_color(image, 24, 16)), semiAlpha);

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
                EQUALS((int)pntr_color_a(pntr_image_get_color(image, 20, 20)), semiAlpha);

                pntr_unload_image(image);
            });

            IT("pntr_draw_ellipse_fill()", {
                pntr_image* image = pntr_new_image(40, 40);
                NEQUALS(image, NULL);
                pntr_clear_background(image, PNTR_BLANK);

                pntr_draw_ellipse_fill(image, 20, 20, 15, 9, semi);
                int painted = 0;
                for (int y = 0; y < 40; y++) {
                    for (int x = 0; x < 40; x++) {
                        unsigned char alpha = pntr_color_a(pntr_image_get_color(image, x, y));
                        if (alpha != 0) {
                            EQUALS((int)alpha, 128);
                            painted++;
                        }
                    }
                }

                // Something was painted, which is what makes the alpha above worth
                // checking. Without pntr_alphablend() nothing is, and the fill is skipped.
                if (pntr_alphablend()) {
                    GREATER(painted, 0);
                    EQUALS((int)pntr_color_a(pntr_image_get_color(image, 20, 20)), 128);
                }
                else {
                    EQUALS(painted, 0);
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
                int painted = 0;
                for (int y = 0; y < 40; y++) {
                    for (int x = 0; x < 40; x++) {
                        unsigned char alpha = pntr_color_a(pntr_image_get_color(image, x, y));
                        if (alpha != 0) {
                            EQUALS((int)alpha, 128);
                            painted++;
                        }
                    }
                }

                // Something was painted, which is what makes the alpha above worth
                // checking. Without pntr_alphablend() nothing is, and the outline is skipped.
                if (pntr_alphablend()) {
                    GREATER(painted, 0);
                    EQUALS((int)pntr_color_a(pntr_image_get_color(image, 8, 20)), 128);
                    EQUALS((int)pntr_color_a(pntr_image_get_color(image, 32, 20)), 128);
                    EQUALS((int)pntr_color_a(pntr_image_get_color(image, 20, 8)), 128);
                    EQUALS((int)pntr_color_a(pntr_image_get_color(image, 20, 32)), 128);
                }
                else {
                    EQUALS(painted, 0);
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

                    // Which is only worth checking because something was painted, and
                    // without pntr_alphablend() nothing is.
                    int expectedOutlinePainted = pntr_alphablend() ? expectedOutline[radius] : 0;
                    int expectedFillPainted = pntr_alphablend() ? expectedFill[radius] : 0;
                    EQUALS(outlinePainted, expectedOutlinePainted);
                    EQUALS(fillPainted, expectedFillPainted);

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

                int painted = 0;
                for (int y = 0; y < 40; y++) {
                    for (int x = 0; x < 40; x++) {
                        unsigned char alpha = pntr_color_a(pntr_image_get_color(image, x, y));
                        if (alpha != 0) {
                            EQUALS((int)alpha, 128);
                            painted++;
                        }
                    }
                }

                // Something was painted, which is what makes the alpha above worth
                // checking. Without pntr_alphablend() nothing is, and the fill is skipped.
                if (pntr_alphablend()) {
                    GREATER(painted, 0);
                    EQUALS((int)pntr_color_a(pntr_image_get_color(image, 20, 20)), 128);
                }
                else {
                    EQUALS(painted, 0);
                }

                pntr_unload_image(image);
            });

            IT("pntr_draw_rectangle_rounded_fill()", {
                pntr_image* image = pntr_new_image(40, 40);
                NEQUALS(image, NULL);
                pntr_clear_background(image, PNTR_BLANK);

                pntr_draw_rectangle_rounded_fill(image, 5, 5, 24, 16, 4, semi);
                int painted = 0;
                for (int y = 0; y < 40; y++) {
                    for (int x = 0; x < 40; x++) {
                        unsigned char alpha = pntr_color_a(pntr_image_get_color(image, x, y));
                        if (alpha != 0) {
                            EQUALS((int)alpha, 128);
                            painted++;
                        }
                    }
                }

                // Something was painted, which is what makes the alpha above worth
                // checking. Without pntr_alphablend() nothing is, and the fill is skipped.
                if (pntr_alphablend()) {
                    GREATER(painted, 0);
                    EQUALS(painted, 24 * 16 - 4 * 8);
                    EQUALS((int)pntr_color_a(pntr_image_get_color(image, 17, 13)), 128);
                }
                else {
                    EQUALS(painted, 0);
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

        IT("pntr_test_uncovered() refuses an outline that painted nothing", {
            // Every caller compares this against zero, so reporting zero for a blank
            // image would be a pass for a primitive that stopped drawing entirely. The
            // sweeps of pntr_test_unfilled_circle() and pntr_test_unfilled_ellipse()
            // are the tests that rest on this.
            pntr_image* blank = pntr_test_canvas(10, 10);
            pntr_image* cover = pntr_test_canvas(10, 10);
            NEQUALS(blank, NULL);
            NEQUALS(cover, NULL);

            pntr_draw_rectangle_fill(cover, 0, 0, 10, 10, PNTR_RED);
            EQUALS(pntr_test_uncovered(blank, cover), -1);

            // A single painted pixel is enough for the answer to mean something.
            pntr_draw_point(blank, 4, 4, PNTR_RED);
            EQUALS(pntr_test_uncovered(blank, cover), 0);

            // And an uncovered pixel is still counted.
            pntr_image* empty = pntr_test_canvas(10, 10);
            NEQUALS(empty, NULL);
            EQUALS(pntr_test_uncovered(blank, empty), 1);

            // A mismatched or missing image says nothing either.
            EQUALS(pntr_test_uncovered(blank, NULL), -1);
            EQUALS(pntr_test_uncovered(NULL, cover), -1);

            pntr_unload_image(empty);
            pntr_unload_image(cover);
            pntr_unload_image(blank);
        });
    });

    /**
     * Holds each entry point against the one it is built out of.
     *
     * Nothing in here knows what a circle or a rounded corner is supposed to look like.
     * Each test says only that two ways of asking for the same drawing produce the same
     * drawing, which is a property that holds by construction and so can be asserted for
     * a whole table of calls at once. Walking the API surface this way is what reaches the
     * wrappers that no test had ever called.
     *
     * @see pntr_test_draw_variant()
     */
    IT("differential", {
        IT("every wrapper draws what the call it delegates to draws", {
            // The source for the image variants. Its pixels are all different and all
            // opaque, so a blit that read from the wrong place shows up as a mismatch
            // rather than as the same flat color in the wrong position.
            pntr_image* src = pntr_test_coordinate_image(16, 16);
            NEQUALS(src, NULL);

            struct {
                const char* name;
                pntr_test_variant actual;
                pntr_test_variant expected;
            } pairs[] = {
                // The _vec forms against the coordinates they unpack into.
                {"pntr_draw_point_vec() == pntr_draw_point()", PNTR_TEST_VARIANT_POINT_VEC, PNTR_TEST_VARIANT_POINT},
                {"pntr_draw_line_vec() == pntr_draw_line()", PNTR_TEST_VARIANT_LINE_VEC, PNTR_TEST_VARIANT_LINE},
                {"pntr_draw_line_thick_vec() == pntr_draw_line_thick()", PNTR_TEST_VARIANT_LINE_THICK_3_VEC, PNTR_TEST_VARIANT_LINE_THICK_3},
                {"pntr_draw_triangle_vec() == pntr_draw_triangle()", PNTR_TEST_VARIANT_TRIANGLE_VEC, PNTR_TEST_VARIANT_TRIANGLE},
                {"pntr_draw_triangle_thick_vec() == pntr_draw_triangle_thick()", PNTR_TEST_VARIANT_TRIANGLE_THICK_3_VEC, PNTR_TEST_VARIANT_TRIANGLE_THICK_3},
                {"pntr_draw_triangle_fill_vec() == pntr_draw_triangle_fill()", PNTR_TEST_VARIANT_TRIANGLE_FILL_VEC, PNTR_TEST_VARIANT_TRIANGLE_FILL},

                // The _rec forms against the coordinate forms they unpack into.
                {"pntr_draw_rectangle_rec() == pntr_draw_rectangle()", PNTR_TEST_VARIANT_RECTANGLE_REC, PNTR_TEST_VARIANT_RECTANGLE},
                {"pntr_draw_rectangle_fill_rec() == pntr_draw_rectangle_fill()", PNTR_TEST_VARIANT_RECTANGLE_FILL_REC, PNTR_TEST_VARIANT_RECTANGLE_FILL},
                {"pntr_draw_rectangle_thick_rec() == pntr_draw_rectangle_thick()", PNTR_TEST_VARIANT_RECTANGLE_THICK_3_REC, PNTR_TEST_VARIANT_RECTANGLE_THICK_3},
                {"pntr_draw_rectangle_gradient_rec() == pntr_draw_rectangle_gradient()", PNTR_TEST_VARIANT_RECTANGLE_GRADIENT_REC, PNTR_TEST_VARIANT_RECTANGLE_GRADIENT},

                // A thickness of one is the plain primitive.
                {"pntr_draw_line_thick(1) == pntr_draw_line()", PNTR_TEST_VARIANT_LINE_THICK_1, PNTR_TEST_VARIANT_LINE},
                {"pntr_draw_line_horizontal_thick(1) == pntr_draw_line_horizontal()", PNTR_TEST_VARIANT_LINE_HORIZONTAL_THICK_1, PNTR_TEST_VARIANT_LINE_HORIZONTAL},
                {"pntr_draw_line_vertical_thick(1) == pntr_draw_line_vertical()", PNTR_TEST_VARIANT_LINE_VERTICAL_THICK_1, PNTR_TEST_VARIANT_LINE_VERTICAL},
                {"pntr_draw_line_curve_thick(1) == pntr_draw_line_curve()", PNTR_TEST_VARIANT_LINE_CURVE_THICK_1, PNTR_TEST_VARIANT_LINE_CURVE},
                {"pntr_draw_rectangle_thick(1) == pntr_draw_rectangle()", PNTR_TEST_VARIANT_RECTANGLE_THICK_1, PNTR_TEST_VARIANT_RECTANGLE},
                {"pntr_draw_rectangle_thick_rounded(1) == pntr_draw_rectangle_rounded()", PNTR_TEST_VARIANT_RECTANGLE_THICK_ROUNDED_1, PNTR_TEST_VARIANT_RECTANGLE_ROUNDED_4},
                {"pntr_draw_triangle_thick(1) == pntr_draw_triangle()", PNTR_TEST_VARIANT_TRIANGLE_THICK_1, PNTR_TEST_VARIANT_TRIANGLE},
                {"pntr_draw_ellipse_thick(1) == pntr_draw_ellipse()", PNTR_TEST_VARIANT_ELLIPSE_THICK_1, PNTR_TEST_VARIANT_ELLIPSE},
                {"pntr_draw_circle_thick(1) == pntr_draw_circle()", PNTR_TEST_VARIANT_CIRCLE_THICK_1, PNTR_TEST_VARIANT_CIRCLE},
                {"pntr_draw_polygon_thick(1) == pntr_draw_polygon()", PNTR_TEST_VARIANT_POLYGON_THICK_1, PNTR_TEST_VARIANT_POLYGON},
                {"pntr_draw_polyline_thick(1) == pntr_draw_polyline()", PNTR_TEST_VARIANT_POLYLINE_THICK_1, PNTR_TEST_VARIANT_POLYLINE},

                // A corner radius of zero is the square-cornered rectangle.
                {"pntr_draw_rectangle_rounded(0) == pntr_draw_rectangle()", PNTR_TEST_VARIANT_RECTANGLE_ROUNDED_0, PNTR_TEST_VARIANT_RECTANGLE},
                {"pntr_draw_rectangle_rounded_fill(0) == pntr_draw_rectangle_fill()", PNTR_TEST_VARIANT_RECTANGLE_ROUNDED_FILL_0, PNTR_TEST_VARIANT_RECTANGLE_FILL},

                // An axis-aligned run is the same run that pntr_draw_line() walks, and
                // the unsafe form is the same run again without the clipping.
                {"pntr_draw_line_horizontal() == pntr_draw_line() along a row", PNTR_TEST_VARIANT_LINE_HORIZONTAL, PNTR_TEST_VARIANT_LINE_HORIZONTAL_AS_LINE},
                {"pntr_draw_line_vertical() == pntr_draw_line() down a column", PNTR_TEST_VARIANT_LINE_VERTICAL, PNTR_TEST_VARIANT_LINE_VERTICAL_AS_LINE},
                {"pntr_put_horizontal_line_unsafe() == pntr_draw_line_horizontal()", PNTR_TEST_VARIANT_LINE_HORIZONTAL_UNSAFE, PNTR_TEST_VARIANT_LINE_HORIZONTAL},
                {"pntr_draw_point_unsafe() == pntr_draw_point()", PNTR_TEST_VARIANT_POINT_UNSAFE, PNTR_TEST_VARIANT_POINT},

                // A whole-image draw is a sub-rectangle draw of the whole image, and the
                // untinted draws are the tinted draw with a tint of PNTR_WHITE.
                {"pntr_draw_image() == pntr_draw_image_rec() of the whole image", PNTR_TEST_VARIANT_IMAGE, PNTR_TEST_VARIANT_IMAGE_REC},
                {"pntr_draw_image_rec() == pntr_draw_image_tint_rec(PNTR_WHITE)", PNTR_TEST_VARIANT_IMAGE_REC, PNTR_TEST_VARIANT_IMAGE_TINT_REC_WHITE},
                {"pntr_draw_image_tint() == pntr_draw_image_tint_rec() of the whole image", PNTR_TEST_VARIANT_IMAGE_TINT, PNTR_TEST_VARIANT_IMAGE_TINT_REC},
                {"pntr_draw_image_rotated(0) == pntr_draw_image()", PNTR_TEST_VARIANT_IMAGE_ROTATED_0, PNTR_TEST_VARIANT_IMAGE},
                {"pntr_draw_image_flipped(no flips) == pntr_draw_image()", PNTR_TEST_VARIANT_IMAGE_FLIPPED_NONE, PNTR_TEST_VARIANT_IMAGE}
            };
            int pairsLen = (int)(sizeof(pairs) / sizeof(pairs[0]));

            for (int i = 0; i < pairsLen; i++) {
                pntr_image* actual = pntr_test_canvas(PNTR_TEST_VARIANT_SIZE, PNTR_TEST_VARIANT_SIZE);
                pntr_image* expected = pntr_test_canvas(PNTR_TEST_VARIANT_SIZE, PNTR_TEST_VARIANT_SIZE);
                if (actual == NULL || expected == NULL) {
                    pntr_unload_image(actual);
                    pntr_unload_image(expected);
                    EQUALS(0, 1);
                    break;
                }

                pntr_test_draw_variant(actual, src, pairs[i].actual);
                pntr_test_draw_variant(expected, src, pairs[i].expected);

                // Both of them drew something, so that matching is not two blank canvases
                // agreeing with each other.
                int painted = pntr_test_painted_count(expected);
                int difference = pntr_test_first_difference(actual, expected);
                if (painted == 0 || difference >= 0) {
                    printf("\n\tdifferential pair %d: %s\n", i, pairs[i].name);
                }

                GREATER(painted, 0);
                IMAGEEQUALS(actual, expected);

                pntr_unload_image(actual);
                pntr_unload_image(expected);
            }

            pntr_unload_image(src);
        });

        IT("every pntr_image_color_*() is pntr_color_*() on each pixel", {
            for (int op = 0; op < PNTR_TEST_COLOR_OP_LEN; op++) {
                pntr_image* source = pntr_test_color_source(24, 18);
                pntr_image* actual = pntr_image_copy(source);
                NEQUALS(source, NULL);
                NEQUALS(actual, NULL);

                pntr_test_image_color_op(actual, (pntr_test_color_op)op);

                for (int y = 0; y < 18; y++) {
                    for (int x = 0; x < 24; x++) {
                        pntr_color was = pntr_image_get_color(source, x, y);
                        pntr_color is = pntr_image_get_color(actual, x, y);
                        pntr_color should = pntr_test_color_op_pixel(was, (pntr_test_color_op)op);
                        if (is.value != should.value) {
                            printf("\n\tcolor operation %d at (%d, %d)\n", op, x, y);
                        }
                        COLOREQUALS(is, should);
                    }
                }

                pntr_unload_image(actual);
                pntr_unload_image(source);
            }
        });

        IT("every pntr_image_color_*() stays inside the clip region", {
            // The whole-image operations walk the clip region rather than the image, so
            // the pixels outside of it have to come back untouched. Only one of them had
            // ever been asked.
            pntr_rectangle clip = PNTR_CLITERAL(pntr_rectangle) {4, 3, 11, 8};

            for (int op = 0; op < PNTR_TEST_COLOR_OP_LEN; op++) {
                pntr_image* source = pntr_test_color_source(24, 18);
                pntr_image* actual = pntr_image_copy(source);
                NEQUALS(source, NULL);
                NEQUALS(actual, NULL);

                pntr_image_set_clip_rec(actual, clip);
                RECTEQUALS(pntr_image_get_clip(actual), clip);
                pntr_test_image_color_op(actual, (pntr_test_color_op)op);
                pntr_image_reset_clip(actual);

                for (int y = 0; y < 18; y++) {
                    for (int x = 0; x < 24; x++) {
                        bool inside = x >= clip.x && x < clip.x + clip.width && y >= clip.y && y < clip.y + clip.height;
                        pntr_color was = pntr_image_get_color(source, x, y);
                        pntr_color is = pntr_image_get_color(actual, x, y);
                        pntr_color should = was;
                        if (inside) {
                            should = pntr_test_color_op_pixel(was, (pntr_test_color_op)op);
                        }
                        if (is.value != should.value) {
                            printf("\n\tclipped color operation %d at (%d, %d)\n", op, x, y);
                        }
                        COLOREQUALS(is, should);
                    }
                }

                pntr_unload_image(actual);
                pntr_unload_image(source);
            }
        });

        IT("an axis-aligned line read backwards covers the same run", {
            // Both of these normalize a negative extent into a run that starts where it
            // would have ended, so the same pixels come out either way round. The
            // pntr_draw_rectangle_rounded() corner arithmetic relies on it.
            struct {
                const char* name;
                int posX;
                int posY;
                int extent;
            } runs[] = {
                {"inside the canvas", 10, 14, 20},
                {"starting at the first pixel", 0, 0, 12},
                {"overhanging the far edge", 30, 30, 20},
                {"starting outside the canvas", -6, 7, 14},
                {"spanning the whole canvas", -4, 9, 48},
                {"entirely outside the canvas", -30, 5, 10},
                {"a single pixel", 20, 20, 1}
            };
            int runsLen = (int)(sizeof(runs) / sizeof(runs[0]));

            for (int i = 0; i < runsLen; i++) {
                int posX = runs[i].posX;
                int posY = runs[i].posY;
                int extent = runs[i].extent;

                pntr_image* forwards = pntr_test_canvas(40, 40);
                pntr_image* backwards = pntr_test_canvas(40, 40);
                NEQUALS(forwards, NULL);
                NEQUALS(backwards, NULL);

                // A negative width starts at the far end of the same run.
                pntr_draw_line_horizontal(forwards, posX, posY, extent, PNTR_RED);
                pntr_draw_line_horizontal(backwards, posX + extent, posY, -extent, PNTR_RED);
                if (pntr_test_first_difference(forwards, backwards) >= 0) {
                    printf("\n\thorizontal run %d: %s\n", i, runs[i].name);
                }
                IMAGEEQUALS(backwards, forwards);

                pntr_clear_background(forwards, PNTR_TEST_BACKGROUND);
                pntr_clear_background(backwards, PNTR_TEST_BACKGROUND);
                pntr_draw_line_vertical(forwards, posY, posX, extent, PNTR_RED);
                pntr_draw_line_vertical(backwards, posY, posX + extent, -extent, PNTR_RED);
                if (pntr_test_first_difference(forwards, backwards) >= 0) {
                    printf("\n\tvertical run %d: %s\n", i, runs[i].name);
                }
                IMAGEEQUALS(backwards, forwards);

                pntr_unload_image(backwards);
                pntr_unload_image(forwards);
            }
        });

        IT("an axis-aligned line is clipped, not shifted", {
            // A run that reaches outside the clip region draws the part of itself that is
            // inside it, in the place that part belongs. Asserted against the same run
            // trimmed to the clip region by hand, which is an answer that does not go
            // through the clipping arithmetic being tested.
            pntr_rectangle clip = PNTR_CLITERAL(pntr_rectangle) {8, 6, 20, 24};

            for (int i = 0; i < 5; i++) {
                // Left of the clip, through it, past its right edge, inside it, and
                // entirely outside of it.
                int starts[5] = {0, -10, 20, 12, 32};
                int extents[5] = {40, 60, 30, 6, 10};

                pntr_image* clipped = pntr_test_canvas(40, 40);
                pntr_image* trimmed = pntr_test_canvas(40, 40);
                NEQUALS(clipped, NULL);
                NEQUALS(trimmed, NULL);

                pntr_image_set_clip_rec(clipped, clip);
                pntr_draw_line_horizontal(clipped, starts[i], 14, extents[i], PNTR_RED);
                pntr_image_reset_clip(clipped);

                for (int x = starts[i]; x < starts[i] + extents[i]; x++) {
                    if (x < clip.x || x >= clip.x + clip.width) {
                        continue;
                    }
                    pntr_draw_point(trimmed, x, 14, PNTR_RED);
                }

                if (pntr_test_first_difference(clipped, trimmed) >= 0) {
                    printf("\n\thorizontal clip case %d\n", i);
                }
                IMAGEEQUALS(clipped, trimmed);

                pntr_clear_background(clipped, PNTR_TEST_BACKGROUND);
                pntr_clear_background(trimmed, PNTR_TEST_BACKGROUND);

                pntr_image_set_clip_rec(clipped, clip);
                pntr_draw_line_vertical(clipped, 14, starts[i], extents[i], PNTR_RED);
                pntr_image_reset_clip(clipped);

                for (int y = starts[i]; y < starts[i] + extents[i]; y++) {
                    if (y < clip.y || y >= clip.y + clip.height) {
                        continue;
                    }
                    pntr_draw_point(trimmed, 14, y, PNTR_RED);
                }

                if (pntr_test_first_difference(clipped, trimmed) >= 0) {
                    printf("\n\tvertical clip case %d\n", i);
                }
                IMAGEEQUALS(clipped, trimmed);

                pntr_unload_image(trimmed);
                pntr_unload_image(clipped);
            }
        });

        IT("pntr_image_set_clip_rec() sets the clip that pntr_image_set_clip() sets", {
            pntr_image* fromRect = pntr_test_canvas(40, 30);
            pntr_image* fromCoordinates = pntr_test_canvas(40, 30);
            NEQUALS(fromRect, NULL);
            NEQUALS(fromCoordinates, NULL);

            pntr_rectangle clip = PNTR_CLITERAL(pntr_rectangle) {5, 6, 12, 9};
            pntr_image_set_clip_rec(fromRect, clip);
            pntr_image_set_clip(fromCoordinates, 5, 6, 12, 9);

            pntr_rectangle actual = pntr_image_get_clip(fromRect);
            pntr_rectangle expected = pntr_image_get_clip(fromCoordinates);
            RECTEQUALS(actual, expected);
            RECTEQUALS(actual, clip);

            // And both confine a draw the same way, rather than only reporting the same
            // numbers.
            pntr_draw_rectangle_fill(fromRect, 0, 0, 40, 30, PNTR_RED);
            pntr_draw_rectangle_fill(fromCoordinates, 0, 0, 40, 30, PNTR_RED);
            IMAGEEQUALS(fromRect, fromCoordinates);
            RECTEQUALS(pntr_test_painted_bounds(fromRect, PNTR_TEST_BACKGROUND), clip);

            pntr_unload_image(fromRect);
            pntr_unload_image(fromCoordinates);
        });

        IT("pntr_memory_copy() copies the bytes it is given", {
            unsigned char source[8] = {11, 22, 33, 44, 55, 66, 77, 88};
            unsigned char target[8] = {0, 0, 0, 0, 0, 0, 0, 0};

            void* result = pntr_memory_copy((void*)target, (void*)source, sizeof(target));
            EQUALS(result, (void*)target);
            for (int i = 0; i < 8; i++) {
                EQUALS((int)target[i], (int)source[i]);
            }

            // A size of zero leaves the destination alone.
            unsigned char untouched[2] = {7, 9};
            pntr_memory_copy((void*)untouched, (void*)source, 0);
            EQUALS((int)untouched[0], 7);
            EQUALS((int)untouched[1], 9);
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

        // Only the last path component decides the type, so an extension that appears in
        // a directory name is not the file's own.
        EQUALS(pntr_get_file_image_type("/home/me/.png-cache/photo.jpg"), PNTR_IMAGE_TYPE_JPG);
        EQUALS(pntr_get_file_image_type("png/jpeg/image.bmp"), PNTR_IMAGE_TYPE_BMP);
        EQUALS(pntr_get_file_image_type("my.png/image.jpeg"), PNTR_IMAGE_TYPE_JPG);
        EQUALS(pntr_get_file_image_type("my.png/image"), PNTR_IMAGE_TYPE_UNKNOWN);

        // Only the last extension counts, in the order the name has them rather than any
        // order of preference.
        EQUALS(pntr_get_file_image_type("screenshot.PNG.bak"), PNTR_IMAGE_TYPE_UNKNOWN);
        EQUALS(pntr_get_file_image_type("archive.png.bmp"), PNTR_IMAGE_TYPE_BMP);
        EQUALS(pntr_get_file_image_type("archive.bmp.png"), PNTR_IMAGE_TYPE_PNG);
        EQUALS(pntr_get_file_image_type("archive.jpg.png"), PNTR_IMAGE_TYPE_PNG);

        // The extension has to end the name, not just start it.
        EQUALS(pntr_get_file_image_type("myimage.pngx"), PNTR_IMAGE_TYPE_UNKNOWN);
        EQUALS(pntr_get_file_image_type("myimage.jpgs"), PNTR_IMAGE_TYPE_UNKNOWN);
        EQUALS(pntr_get_file_image_type("myimage.bmps"), PNTR_IMAGE_TYPE_UNKNOWN);

        // The case of the extension makes no difference, in any mix.
        EQUALS(pntr_get_file_image_type("photo.Png"), PNTR_IMAGE_TYPE_PNG);
        EQUALS(pntr_get_file_image_type("photo.pNG"), PNTR_IMAGE_TYPE_PNG);
        EQUALS(pntr_get_file_image_type("photo.PNG"), PNTR_IMAGE_TYPE_PNG);
        EQUALS(pntr_get_file_image_type("photo.Jpg"), PNTR_IMAGE_TYPE_JPG);
        EQUALS(pntr_get_file_image_type("image.JPg"), PNTR_IMAGE_TYPE_JPG);
        EQUALS(pntr_get_file_image_type("image.JPG"), PNTR_IMAGE_TYPE_JPG);
        EQUALS(pntr_get_file_image_type("image.Jpeg"), PNTR_IMAGE_TYPE_JPG);
        EQUALS(pntr_get_file_image_type("image.jPeG"), PNTR_IMAGE_TYPE_JPG);
        EQUALS(pntr_get_file_image_type("image.JPEG"), PNTR_IMAGE_TYPE_JPG);
        EQUALS(pntr_get_file_image_type("image.Bmp"), PNTR_IMAGE_TYPE_BMP);
        EQUALS(pntr_get_file_image_type("image.bMP"), PNTR_IMAGE_TYPE_BMP);
        EQUALS(pntr_get_file_image_type("image.BMP"), PNTR_IMAGE_TYPE_BMP);

        // Windows separates path components with a backslash.
        EQUALS(pntr_get_file_image_type("C:\\Users\\me\\photo.jpg"), PNTR_IMAGE_TYPE_JPG);
        EQUALS(pntr_get_file_image_type("C:\\png-cache\\photo.BMP"), PNTR_IMAGE_TYPE_BMP);
        EQUALS(pntr_get_file_image_type("C:\\my.png\\photo"), PNTR_IMAGE_TYPE_UNKNOWN);
        EQUALS(pntr_get_file_image_type("C:\\my.png\\"), PNTR_IMAGE_TYPE_UNKNOWN);

        // A name with no extension of its own has no type to find.
        EQUALS(pntr_get_file_image_type("myimage"), PNTR_IMAGE_TYPE_UNKNOWN);
        EQUALS(pntr_get_file_image_type("my/path/myimage"), PNTR_IMAGE_TYPE_UNKNOWN);
        EQUALS(pntr_get_file_image_type("myimage."), PNTR_IMAGE_TYPE_UNKNOWN);
        EQUALS(pntr_get_file_image_type("myimage.png."), PNTR_IMAGE_TYPE_UNKNOWN);
        EQUALS(pntr_get_file_image_type("."), PNTR_IMAGE_TYPE_UNKNOWN);
        EQUALS(pntr_get_file_image_type(".."), PNTR_IMAGE_TYPE_UNKNOWN);

        // A directory is not a file, however it is spelled.
        EQUALS(pntr_get_file_image_type("my/path/"), PNTR_IMAGE_TYPE_UNKNOWN);
        EQUALS(pntr_get_file_image_type("my/image.png/"), PNTR_IMAGE_TYPE_UNKNOWN);
        EQUALS(pntr_get_file_image_type("/"), PNTR_IMAGE_TYPE_UNKNOWN);

        // A leading dot hides a file rather than giving it an extension.
        EQUALS(pntr_get_file_image_type(".gitignore"), PNTR_IMAGE_TYPE_UNKNOWN);
        EQUALS(pntr_get_file_image_type(".png"), PNTR_IMAGE_TYPE_UNKNOWN);
        EQUALS(pntr_get_file_image_type("/home/me/.png"), PNTR_IMAGE_TYPE_UNKNOWN);
        EQUALS(pntr_get_file_image_type(".hidden.png"), PNTR_IMAGE_TYPE_PNG);
    });

    if (pntr_bmp() && pntr_image_saving() && pntr_file_io()) {
        IT("pntr_save_image() with an extension that used to be mistaken", {
            pntr_image* image = pntr_gen_image_color(8, 8, PNTR_RED);
            NEQUALS(image, NULL);

            // The ".png" in the middle of the name used to choose the encoder, which put
            // a PNG inside a file called .bmp.
            EQUALS(pntr_save_image(image, "save.png.bmp"), true);

            unsigned int bytesRead = 0;
            unsigned char* fileData = pntr_load_file("save.png.bmp", &bytesRead);
            NEQUALS(fileData, NULL);
            GREATER(bytesRead, 2u);

            // "BM" starts every bitmap, where a PNG would start with its own signature.
            EQUALS(fileData[0], 'B');
            EQUALS(fileData[1], 'M');
            NEQUALS(fileData[0], 0x89);

            pntr_unload_file(fileData);
            remove("save.png.bmp");
            pntr_unload_image(image);
        });
    }

    if (pntr_image_from_file()) {
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
    }
    else {
        IT("pntr_load_image(): no image can be loaded from a file", {
            // The refusal is pinned by "pntr_load_image(): Reports that it was compiled out",
            // so there is nothing left for the happy path to say here.
            SKIP();
        });

        IT("pntr_load_image_from_memory(): no image to read", {
            // A buffer still has to be handed over.
            EQUALS(pntr_load_image_from_memory(PNTR_IMAGE_TYPE_PNG, NULL, 16), NULL);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_INVALID_ARGS);
            pntr_set_error(PNTR_ERROR_NONE);
            EQUALS(pntr_load_image_from_memory(PNTR_IMAGE_TYPE_PNG, (const unsigned char*)"not a png", 0), NULL);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_INVALID_ARGS);
            pntr_set_error(PNTR_ERROR_NONE);

            // Bytes that reach the decoder are refused for being undecodable where there is a
            // decoder, and for there being none where there isn't.
            EQUALS(pntr_load_image_from_memory(PNTR_IMAGE_TYPE_PNG, (const unsigned char*)"not a png", 9), NULL);
            pntr_error undecodable = pntr_image_loading() ? PNTR_ERROR_INVALID_DATA : PNTR_ERROR_NOT_SUPPORTED;
            EQUALS(pntr_get_error_code(), undecodable);
            pntr_set_error(PNTR_ERROR_NONE);
        });

        IT("pntr_load_font_bmf(): refuses without its atlas", {
            EQUALS(pntr_load_font_bmf("resources/font.png", PNTR_TEST_BMF_CHARACTERS), NULL);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
            pntr_set_error(PNTR_ERROR_NONE);
        });

        IT("pntr_measure_text(), pntr_measure_text_ex(): without a font", {
            // Nothing can be measured without an atlas to measure against, but neither is
            // a missing font allowed to be read as a size.
            EQUALS(pntr_measure_text(NULL, "Hello World!"), 0);
            pntr_vector size = pntr_measure_text_ex(NULL, "Hello World!", 0);
            EQUALS(size.x, 0);
            EQUALS(size.y, 0);
            EQUALS(pntr_gen_image_text(NULL, "Hello World!", PNTR_WHITE, PNTR_BLANK), NULL);
            pntr_set_error(PNTR_ERROR_NONE);
        });

        IT("pntr_load_font_tty(): refuses without its atlas", {
            EQUALS(pntr_load_font_tty("resources/font-tty-8x8.png", 8, 8, PNTR_TEST_TTY_CHARACTERS), NULL);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
            pntr_set_error(PNTR_ERROR_NONE);
        });

        IT("pntr_load_font_bmf_from_memory(): refuses without an atlas", {
            const unsigned char* notAnAtlas = (const unsigned char*)"not a png";

            // Whatever the atlas is, it has to decode into an image first.
            EQUALS(pntr_load_font_bmf_from_memory(notAnAtlas, 9, PNTR_TEST_BMF_CHARACTERS), NULL);
            pntr_error undecodable = pntr_image_loading() ? PNTR_ERROR_INVALID_DATA : PNTR_ERROR_NOT_SUPPORTED;
            EQUALS(pntr_get_error_code(), undecodable);
            pntr_set_error(PNTR_ERROR_NONE);

            // Invalid arguments are still rejected as such.
            EQUALS(pntr_load_font_bmf_from_memory(NULL, 9, PNTR_TEST_BMF_CHARACTERS), NULL);
            EQUALS(pntr_load_font_bmf_from_memory(notAnAtlas, 0, PNTR_TEST_BMF_CHARACTERS), NULL);
            EQUALS(pntr_load_font_bmf_from_memory(notAnAtlas, 9, NULL), NULL);
            EQUALS(pntr_load_font_bmf_from_image(NULL, PNTR_TEST_BMF_CHARACTERS), NULL);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_INVALID_ARGS);
            pntr_set_error(PNTR_ERROR_NONE);
        });

        IT("pntr_load_font_tty_from_memory(): refuses without an atlas", {
            const unsigned char* notAnAtlas = (const unsigned char*)"not a png";

            EQUALS(pntr_load_font_tty_from_memory(notAnAtlas, 9, 8, 8, PNTR_TEST_TTY_CHARACTERS), NULL);
            pntr_error undecodable = pntr_image_loading() ? PNTR_ERROR_INVALID_DATA : PNTR_ERROR_NOT_SUPPORTED;
            EQUALS(pntr_get_error_code(), undecodable);
            pntr_set_error(PNTR_ERROR_NONE);

            // Invalid arguments are still rejected as such.
            EQUALS(pntr_load_font_tty_from_memory(NULL, 9, 8, 8, PNTR_TEST_TTY_CHARACTERS), NULL);
            EQUALS(pntr_load_font_tty_from_memory(notAnAtlas, 0, 8, 8, PNTR_TEST_TTY_CHARACTERS), NULL);
            EQUALS(pntr_load_font_tty_from_memory(notAnAtlas, 9, 0, 8, PNTR_TEST_TTY_CHARACTERS), NULL);
            EQUALS(pntr_load_font_tty_from_memory(notAnAtlas, 9, 8, -8, PNTR_TEST_TTY_CHARACTERS), NULL);
            EQUALS(pntr_load_font_tty_from_memory(notAnAtlas, 9, 8, 8, NULL), NULL);
            EQUALS(pntr_load_font_tty_from_image(NULL, 8, 8, PNTR_TEST_TTY_CHARACTERS), NULL);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_INVALID_ARGS);
            pntr_set_error(PNTR_ERROR_NONE);
        });
    }

    if (pntr_file_io()) {
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
    }
    else {
        IT("pntr_load_font_ttf_from_memory(): without a font file", {
            // Data that isn't a ttf must not produce a font, which needs no file system.
            const unsigned char* notAFont = (const unsigned char*)"this is not a ttf";
            EQUALS(pntr_load_font_ttf_from_memory(notAFont, 17, 20), NULL);
            NEQUALS(pntr_get_error(), NULL);
            pntr_set_error(PNTR_ERROR_NONE);

            // Invalid arguments.
            EQUALS(pntr_load_font_ttf_from_memory(NULL, 17, 20), NULL);
            EQUALS(pntr_load_font_ttf_from_memory(notAFont, 0, 20), NULL);
            EQUALS(pntr_load_font_ttf_from_memory(notAFont, 17, 0), NULL);
            pntr_set_error(PNTR_ERROR_NONE);

            // The real font can't be reached, so nothing is left to compare it against.
            EQUALS(pntr_load_font_ttf("resources/tuffy.ttf", 20), NULL);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
            pntr_set_error(PNTR_ERROR_NONE);
        });
    }

    if (pntr_image_from_file()) {
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
    }
    else {
        IT("pntr_draw_text(): without a font", {
            // Drawing needs the BMFont atlas, whose refusal is pinned by
            // "pntr_load_font_bmf(): refuses without its atlas". Drawing with no font at all
            // still has to leave the destination alone.
            pntr_image* image = pntr_gen_image_color(40, 40, PNTR_DARKBROWN);
            NEQUALS(image, NULL);
            pntr_image* expected = pntr_gen_image_color(40, 40, PNTR_DARKBROWN);
            NEQUALS(expected, NULL);

            pntr_draw_text(image, NULL, "~~~", 0, 0, PNTR_WHITE);
            IMAGEEQUALS(image, expected);

            pntr_unload_image(expected);
            pntr_unload_image(image);
            pntr_set_error(PNTR_ERROR_NONE);
        });
    }

    IT("pntr_load_font_default()", {
        pntr_font* font = pntr_load_font_default();
        NEQUALS(font, NULL);
        NEQUALS(font->atlas, NULL);
        GREATER(font->charactersLen, 10);
        pntr_unload_font(font);
    });

    IT("pntr_draw_text() paints the default font's glyphs where it says", {
        // Every other pixel-level assertion about text in this suite is a negative one:
        // that a row was left blank, or that two draws agree with each other. None of
        // them would notice a glyph landing a pixel off, or the wrong glyph being picked
        // out of the atlas, or the whole string being drawn upside down.
        //
        // The default font is font8x8, so its glyphs are exactly known. Each row below is
        // one row of a glyph, with bit 0 the leftmost pixel, copied from
        // external/font8x8_basic.h. These are the expectation; nothing in pntr is asked
        // where the pixels should be.
        struct {
            char character;
            unsigned char rows[8];
        } glyphs[] = {
            {'_', {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF}},
            {'-', {0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00}},
            {'|', {0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00}},
            {'+', {0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00}},
            {'A', {0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00}},
            {'H', {0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00}},
            {'L', {0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00}},
            {'T', {0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}},
            {'0', {0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00}},
            {' ', {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}
        };
        int glyphsLen = (int)(sizeof(glyphs) / sizeof(glyphs[0]));

        pntr_font* font = pntr_load_font_default();
        NEQUALS(font, NULL);

        // Drawn away from the origin on both axes, so that a draw that ignored the
        // position would land somewhere else entirely.
        int posX = 6;
        int posY = 5;

        for (int i = 0; i < glyphsLen; i++) {
            pntr_image* canvas = pntr_test_canvas(32, 24);
            NEQUALS(canvas, NULL);

            char text[2];
            text[0] = glyphs[i].character;
            text[1] = '\0';
            pntr_draw_text(canvas, font, text, posX, posY, PNTR_BLACK);

            int painted = 0;
            for (int y = 0; y < 8; y++) {
                for (int x = 0; x < 8; x++) {
                    bool expected = ((glyphs[i].rows[y] >> x) & 1) != 0;
                    bool actual = pntr_test_painted(canvas, posX + x, posY + y);
                    if (actual != expected) {
                        printf("\n\tglyph '%c' at (%d, %d): %d, expected %d\n",
                            glyphs[i].character, x, y, (int)actual, (int)expected);
                    }
                    EQUALS(actual, expected);
                    if (expected) {
                        painted++;
                    }
                }
            }

            // The glyph is the only thing on the canvas, so nothing was drawn outside of
            // its eight by eight cell either.
            EQUALS(pntr_test_painted_count(canvas), painted);

            pntr_unload_image(canvas);
        }

        // The tint is the color the glyph is painted in, not a filter over the atlas.
        pntr_image* tinted = pntr_test_canvas(32, 24);
        NEQUALS(tinted, NULL);
        pntr_draw_text(tinted, font, "_", posX, posY, PNTR_BLUE);
        for (int x = 0; x < 8; x++) {
            COLOREQUALS(pntr_image_get_color(tinted, posX + x, posY + 7), PNTR_BLUE);
        }
        EQUALS(pntr_test_painted_count(tinted), 8);
        pntr_unload_image(tinted);

        // A glyph in a string sits one glyph width to the right of the one before it, and
        // the space between them is left alone rather than being drawn over.
        pntr_image* word = pntr_test_canvas(40, 24);
        NEQUALS(word, NULL);
        pntr_draw_text(word, font, "_ _", posX, posY, PNTR_BLACK);
        for (int x = 0; x < 8; x++) {
            EQUALS(pntr_test_painted(word, posX + x, posY + 7), true);
            EQUALS(pntr_test_painted(word, posX + 8 + x, posY + 7), false);
            EQUALS(pntr_test_painted(word, posX + 16 + x, posY + 7), true);
        }
        EQUALS(pntr_test_painted_count(word), 16);
        pntr_unload_image(word);

        pntr_unload_font(font);
    });

    IT("pntr_draw_text_wrapped() with nothing to wrap is pntr_draw_text()", {
        // pntr_draw_text_wrapped() had one call in this suite and no assertion at all
        // against it. These are the cases where it has nothing to decide, so they say
        // only that it draws the text it was handed in the place it was handed.
        pntr_font* font = pntr_load_font_default();
        NEQUALS(font, NULL);

        int lineHeight = pntr_test_line_height(font);
        EQUALS(lineHeight, 8);

        // A width that nothing can exceed.
        pntr_image* wrapped = pntr_test_canvas(160, 40);
        pntr_image* plain = pntr_test_canvas(160, 40);
        NEQUALS(wrapped, NULL);
        NEQUALS(plain, NULL);
        pntr_draw_text_wrapped(wrapped, font, "Hello world", 5, 4, 1000, PNTR_BLACK);
        pntr_draw_text(plain, font, "Hello world", 5, 4, PNTR_BLACK);
        IMAGEEQUALS(wrapped, plain);
        GREATER(pntr_test_painted_count(plain), 0);

        // Including when the text breaks its own lines.
        pntr_image* newlines = pntr_test_canvas(160, 40);
        pntr_image* plainNewlines = pntr_test_canvas(160, 40);
        NEQUALS(newlines, NULL);
        NEQUALS(plainNewlines, NULL);
        pntr_draw_text_wrapped(newlines, font, "Hello\nworld", 5, 4, 1000, PNTR_BLACK);
        pntr_draw_text(plainNewlines, font, "Hello\nworld", 5, 4, PNTR_BLACK);
        IMAGEEQUALS(newlines, plainNewlines);

        // A single word wider than the maximum has no space to break at, so it is drawn
        // as it is rather than being dropped.
        pntr_image* unbreakable = pntr_test_canvas(160, 40);
        pntr_image* word = pntr_test_canvas(160, 40);
        NEQUALS(unbreakable, NULL);
        NEQUALS(word, NULL);
        pntr_draw_text_wrapped(unbreakable, font, "Hello", 5, 4, 10, PNTR_BLACK);
        pntr_draw_text(word, font, "Hello", 5, 4, PNTR_BLACK);
        IMAGEEQUALS(unbreakable, word);

        // Two words that do not fit on one line are the two of them on their own lines.
        pntr_image* twoWords = pntr_test_canvas(160, 40);
        pntr_image* twoLines = pntr_test_canvas(160, 40);
        NEQUALS(twoWords, NULL);
        NEQUALS(twoLines, NULL);
        pntr_draw_text_wrapped(twoWords, font, "Hello world", 5, 4, 50, PNTR_BLACK);
        pntr_draw_text(twoLines, font, "Hello\nworld", 5, 4, PNTR_BLACK);
        IMAGEEQUALS(twoWords, twoLines);

        pntr_unload_image(twoLines);
        pntr_unload_image(twoWords);
        pntr_unload_image(word);
        pntr_unload_image(unbreakable);
        pntr_unload_image(plainNewlines);
        pntr_unload_image(newlines);
        pntr_unload_image(plain);
        pntr_unload_image(wrapped);
        pntr_unload_font(font);
    });

    IT("pntr_draw_text_wrapped(): breaks at every space, not only the first", {
        // Every line after the first used to be drawn twice and unwrapped, because the
        // space the line before it was broken at was still remembered, left behind the
        // start of the new line, and read as a negative length.
        pntr_font* font = pntr_load_font_default();
        NEQUALS(font, NULL);

        int lineHeight = pntr_test_line_height(font);
        EQUALS(lineHeight, 8);

        // Two letters of the default font measure 16 pixels, so only one word at a time
        // fits inside 20 of them.
        pntr_image* three = pntr_test_canvas(160, 48);
        pntr_image* threeLines = pntr_test_canvas(160, 48);
        NEQUALS(three, NULL);
        NEQUALS(threeLines, NULL);
        pntr_draw_text_wrapped(three, font, "aa bb cc", 5, 4, 20, PNTR_BLACK);
        pntr_draw_text(threeLines, font, "aa\nbb\ncc", 5, 4, PNTR_BLACK);
        IMAGEEQUALS(three, threeLines);

        pntr_image* four = pntr_test_canvas(160, 48);
        pntr_image* fourLines = pntr_test_canvas(160, 48);
        NEQUALS(four, NULL);
        NEQUALS(fourLines, NULL);
        pntr_draw_text_wrapped(four, font, "aa bb cc dd", 5, 4, 20, PNTR_BLACK);
        pntr_draw_text(fourLines, font, "aa\nbb\ncc\ndd", 5, 4, PNTR_BLACK);
        IMAGEEQUALS(four, fourLines);

        // Every one of the four lines holds a single two letter word, inside the 20
        // pixels asked for, where the remainder of the string used to be drawn 63 pixels
        // wide on three lines in a row.
        for (int line = 0; line < 4; line++) {
            int top = 4 + line * lineHeight;
            GREATER(pntr_test_painted_width(four, top, top + lineHeight), 0);
            LESSEREQ(pntr_test_painted_width(four, top, top + lineHeight), 20);
        }
        EQUALS(pntr_test_painted_rows(four, 4 + lineHeight * 4, 48), 0);

        // Nothing is painted twice: the four words drawn one per line are all there is.
        pntr_image* words = pntr_test_canvas(160, 48);
        NEQUALS(words, NULL);
        pntr_draw_text(words, font, "aa", 5, 4, PNTR_BLACK);
        pntr_draw_text(words, font, "bb", 5, 4 + lineHeight, PNTR_BLACK);
        pntr_draw_text(words, font, "cc", 5, 4 + lineHeight * 2, PNTR_BLACK);
        pntr_draw_text(words, font, "dd", 5, 4 + lineHeight * 3, PNTR_BLACK);
        IMAGEEQUALS(four, words);
        EQUALS(pntr_test_painted_count(four), pntr_test_painted_count(words));

        // As many words as fit go on a line, so twice the width is two words to a line.
        pntr_image* pairs = pntr_test_canvas(160, 48);
        pntr_image* pairLines = pntr_test_canvas(160, 48);
        NEQUALS(pairs, NULL);
        NEQUALS(pairLines, NULL);
        pntr_draw_text_wrapped(pairs, font, "aa bb cc dd", 5, 4, 40, PNTR_BLACK);
        pntr_draw_text(pairLines, font, "aa bb\ncc dd", 5, 4, PNTR_BLACK);
        IMAGEEQUALS(pairs, pairLines);

        pntr_unload_image(pairLines);
        pntr_unload_image(pairs);
        pntr_unload_image(words);
        pntr_unload_image(fourLines);
        pntr_unload_image(four);
        pntr_unload_image(threeLines);
        pntr_unload_image(three);
        pntr_unload_font(font);
    });

    IT("pntr_draw_text_wrapped(): no line is wider than the maximum", {
        // The invariant, rather than any particular arrangement of the words: no line is
        // drawn wider than it was told it could be, for every width a caller might ask
        // for. A single word that cannot be broken is the only exception, and the widest
        // word here is "jumped" at 48 pixels.
        pntr_font* font = pntr_load_font_default();
        NEQUALS(font, NULL);

        int lineHeight = pntr_test_line_height(font);
        const char* text = "The quick brown fox jumped over the lazy dog.";

        // The whole of it on one line, which is every glyph of it painted exactly once.
        pntr_image* oneLine = pntr_test_canvas(400, 128);
        NEQUALS(oneLine, NULL);
        pntr_draw_text(oneLine, font, text, 2, 0, PNTR_BLACK);
        GREATER(pntr_test_painted_count(oneLine), 0);

        for (int maxWidth = 48; maxWidth <= 200; maxWidth += 8) {
            pntr_image* image = pntr_test_canvas(400, 128);
            NEQUALS(image, NULL);
            pntr_draw_text_wrapped(image, font, text, 2, 0, maxWidth, PNTR_BLACK);

            for (int top = 0; top + lineHeight <= image->height; top += lineHeight) {
                LESSEREQ(pntr_test_painted_width(image, top, top + lineHeight), maxWidth);
            }

            // Spaces paint nothing and a glyph paints the same wherever it lands, so the
            // wrapped text is the same number of pixels as the unwrapped text. Fewer
            // would mean a word was dropped, more that one was drawn twice.
            EQUALS(pntr_test_painted_count(image), pntr_test_painted_count(oneLine));

            pntr_unload_image(image);
        }

        pntr_unload_image(oneLine);
        pntr_unload_font(font);
    });

    IT("pntr_draw_text_wrapped(): the spaces and newlines around a break", {
        pntr_font* font = pntr_load_font_default();
        NEQUALS(font, NULL);

        int lineHeight = pntr_test_line_height(font);

        pntr_image* broken = pntr_test_canvas(160, 48);
        NEQUALS(broken, NULL);
        pntr_draw_text(broken, font, "aa\nbb", 5, 4, PNTR_BLACK);

        // The spaces a line is broken at are not carried onto the next line, however many
        // of them there are.
        pntr_image* twoSpaces = pntr_test_canvas(160, 48);
        NEQUALS(twoSpaces, NULL);
        pntr_draw_text_wrapped(twoSpaces, font, "aa  bb", 5, 4, 20, PNTR_BLACK);
        IMAGEEQUALS(twoSpaces, broken);

        // A space at the end of the text paints nothing and opens no line of its own.
        pntr_image* trailing = pntr_test_canvas(160, 48);
        NEQUALS(trailing, NULL);
        pntr_draw_text_wrapped(trailing, font, "aa bb ", 5, 4, 20, PNTR_BLACK);
        IMAGEEQUALS(trailing, broken);

        // A space in front of the text is drawn, the way pntr_draw_text() draws it.
        pntr_image* leading = pntr_test_canvas(160, 48);
        pntr_image* leadingExpected = pntr_test_canvas(160, 48);
        NEQUALS(leading, NULL);
        NEQUALS(leadingExpected, NULL);
        pntr_draw_text_wrapped(leading, font, " aa bb", 5, 4, 20, PNTR_BLACK);
        pntr_draw_text(leadingExpected, font, " aa\nbb", 5, 4, PNTR_BLACK);
        IMAGEEQUALS(leading, leadingExpected);

        // Nothing but spaces is nothing to draw.
        pntr_image* spaces = pntr_test_canvas(160, 48);
        NEQUALS(spaces, NULL);
        pntr_draw_text_wrapped(spaces, font, "   ", 5, 4, 20, PNTR_BLACK);
        EQUALS(pntr_test_painted_count(spaces), 0);

        // And so is nothing at all.
        pntr_image* empty = pntr_test_canvas(160, 48);
        NEQUALS(empty, NULL);
        pntr_draw_text_wrapped(empty, font, "", 5, 4, 20, PNTR_BLACK);
        EQUALS(pntr_test_painted_count(empty), 0);

        // A newline the text asks for breaks a line, and the wrapping carries on below it.
        pntr_image* mixed = pntr_test_canvas(160, 48);
        pntr_image* mixedExpected = pntr_test_canvas(160, 48);
        NEQUALS(mixed, NULL);
        NEQUALS(mixedExpected, NULL);
        pntr_draw_text_wrapped(mixed, font, "aa\nbb cc", 5, 4, 20, PNTR_BLACK);
        pntr_draw_text(mixedExpected, font, "aa\nbb\ncc", 5, 4, PNTR_BLACK);
        IMAGEEQUALS(mixed, mixedExpected);

        // A newline at the end of the text opens a line with nothing on it.
        pntr_image* trailingNewline = pntr_test_canvas(160, 48);
        NEQUALS(trailingNewline, NULL);
        pntr_draw_text_wrapped(trailingNewline, font, "aa bb\n", 5, 4, 20, PNTR_BLACK);
        IMAGEEQUALS(trailingNewline, broken);

        // An empty line is as tall as a full one, so what wraps below it starts two lines
        // down rather than one.
        pntr_image* blank = pntr_test_canvas(160, 48);
        pntr_image* blankExpected = pntr_test_canvas(160, 48);
        NEQUALS(blank, NULL);
        NEQUALS(blankExpected, NULL);
        pntr_draw_text_wrapped(blank, font, "aa\n\nbb cc", 5, 4, 20, PNTR_BLACK);
        pntr_draw_text(blankExpected, font, "aa\n\nbb\ncc", 5, 4, PNTR_BLACK);
        IMAGEEQUALS(blank, blankExpected);
        EQUALS(pntr_test_painted_rows(blank, 4 + lineHeight, 4 + lineHeight * 2), 0);

        pntr_unload_image(blankExpected);
        pntr_unload_image(blank);
        pntr_unload_image(trailingNewline);
        pntr_unload_image(mixedExpected);
        pntr_unload_image(mixed);
        pntr_unload_image(empty);
        pntr_unload_image(spaces);
        pntr_unload_image(leadingExpected);
        pntr_unload_image(leading);
        pntr_unload_image(trailing);
        pntr_unload_image(twoSpaces);
        pntr_unload_image(broken);
        pntr_unload_font(font);
    });

    if (pntr_utf8()) {
        IT("pntr_draw_text_wrapped(): multi-byte characters", {
            // A line is broken on the characters of the text rather than on the bytes
            // they take up, so the length handed to pntr_draw_text_len() means the same
            // thing as the length handed to pntr_measure_text_ex().
            pntr_font* font = pntr_load_font_default();
            NEQUALS(font, NULL);

            // "héllo" is six bytes of five characters.
            const char* text = "h\xC3\xA9llo world";

            pntr_image* wrapped = pntr_test_canvas(200, 48);
            pntr_image* expected = pntr_test_canvas(200, 48);
            NEQUALS(wrapped, NULL);
            NEQUALS(expected, NULL);
            pntr_draw_text_wrapped(wrapped, font, text, 5, 4, 48, PNTR_BLACK);
            pntr_draw_text(expected, font, "h\xC3\xA9llo\nworld", 5, 4, PNTR_BLACK);
            IMAGEEQUALS(wrapped, expected);

            // Counted in bytes, the break would be a character into "world", so the "w"
            // would be drawn on the first line as well as the second.
            pntr_vector size = pntr_measure_text_ex(font, "h\xC3\xA9llo", 0);
            LESSEREQ(pntr_test_painted_width(wrapped, 4, 4 + pntr_test_line_height(font)), size.x);

            // Both words together when there is room for the two of them.
            pntr_image* together = pntr_test_canvas(200, 48);
            pntr_image* oneLine = pntr_test_canvas(200, 48);
            NEQUALS(together, NULL);
            NEQUALS(oneLine, NULL);
            pntr_draw_text_wrapped(together, font, text, 5, 4, 200, PNTR_BLACK);
            pntr_draw_text(oneLine, font, text, 5, 4, PNTR_BLACK);
            IMAGEEQUALS(together, oneLine);

            pntr_unload_image(oneLine);
            pntr_unload_image(together);
            pntr_unload_image(expected);
            pntr_unload_image(wrapped);
            pntr_unload_font(font);
        });
    }
    else {
        IT("pntr_draw_text_wrapped(): multi-byte characters", {
            // Without PNTR_ENABLE_UTF8 a character is a byte, so a line broken on bytes
            // and a line broken on characters are the same line.
            SKIP();
        });
    }

    if (pntr_vargs()) {
        IT("pntr_draw_text_ex() draws what pntr_draw_text() draws of the same string", {
            // pntr_draw_text_ex() formats its arguments and hands the result to
            // pntr_draw_text(), so the only thing it can get wrong is the formatting.
            pntr_font* font = pntr_load_font_default();
            NEQUALS(font, NULL);

            pntr_image* formatted = pntr_test_canvas(120, 24);
            pntr_image* expected = pntr_test_canvas(120, 24);
            NEQUALS(formatted, NULL);
            NEQUALS(expected, NULL);

            pntr_test_draw_text_ex(formatted, font, 4, 6, PNTR_BLACK, 0, 42);
            pntr_draw_text(expected, font, "n=42", 4, 6, PNTR_BLACK);
            IMAGEEQUALS(formatted, expected);
            GREATER(pntr_test_painted_count(expected), 0);

            // A maximum length truncates the formatted string, counting its terminator.
            pntr_image* truncated = pntr_test_canvas(120, 24);
            pntr_image* shorter = pntr_test_canvas(120, 24);
            NEQUALS(truncated, NULL);
            NEQUALS(shorter, NULL);

            pntr_test_draw_text_ex(truncated, font, 4, 6, PNTR_BLACK, 4, 42);
            pntr_draw_text(shorter, font, "n=4", 4, 6, PNTR_BLACK);
            IMAGEEQUALS(truncated, shorter);
            NEQUALS(pntr_test_painted_count(truncated), pntr_test_painted_count(formatted));

            pntr_unload_image(shorter);
            pntr_unload_image(truncated);
            pntr_unload_image(expected);
            pntr_unload_image(formatted);
            pntr_unload_font(font);
        });
    }
    else {
        IT("pntr_draw_text_ex() draws what pntr_draw_text() draws of the same string", {
            // pntr_draw_text_ex() needs variadic arguments, so without PNTR_ENABLE_VARGS
            // there is no formatted draw to compare.
            SKIP();
        });
    }

    IT("pntr_draw_text_len(): with a length", {
        // A length counts characters of the text, where zero means all of them.
        pntr_font* font = pntr_load_font_default();
        NEQUALS(font, NULL);

        pntr_image* whole = pntr_test_canvas(60, 10);
        pntr_image* counted = pntr_test_canvas(60, 10);
        pntr_draw_text_len(whole, font, "Hello", 0, 2, 1, PNTR_BLACK);
        pntr_draw_text_len(counted, font, "Hello", 5, 2, 1, PNTR_BLACK);
        IMAGEEQUALS(counted, whole);

        // A length that reaches past the end of the string is the whole string.
        pntr_image* tooLong = pntr_test_canvas(60, 10);
        pntr_draw_text_len(tooLong, font, "Hello", 50, 2, 1, PNTR_BLACK);
        IMAGEEQUALS(tooLong, whole);

        // A shorter length stops where it says it does.
        pntr_image* partial = pntr_test_canvas(60, 10);
        pntr_image* expected = pntr_test_canvas(60, 10);
        pntr_draw_text_len(partial, font, "Hello", 2, 2, 1, PNTR_BLACK);
        pntr_draw_text_len(expected, font, "He", 0, 2, 1, PNTR_BLACK);
        IMAGEEQUALS(partial, expected);
        NEQUALS(pntr_test_painted_count(partial), pntr_test_painted_count(whole));

        // A newline is one of the characters counted, so this stops before the "B".
        pntr_image* newline = pntr_test_canvas(60, 20);
        pntr_image* onlyA = pntr_test_canvas(60, 20);
        pntr_draw_text_len(newline, font, "A\nB", 2, 2, 1, PNTR_BLACK);
        pntr_draw_text_len(onlyA, font, "A", 0, 2, 1, PNTR_BLACK);
        IMAGEEQUALS(newline, onlyA);

        pntr_unload_image(onlyA);
        pntr_unload_image(newline);
        pntr_unload_image(expected);
        pntr_unload_image(partial);
        pntr_unload_image(tooLong);
        pntr_unload_image(counted);
        pntr_unload_image(whole);
        pntr_unload_font(font);
    });

    IT("pntr_draw_text_aligned(): against the measured width", {
        // Each alignment places the text exactly where pntr_measure_text_ex() says it
        // will land, which is what lets a caller reserve room for it.
        pntr_font* font = pntr_load_font_default();
        NEQUALS(font, NULL);

        const char* text = "Bye";
        int width = pntr_measure_text(font, text);
        EQUALS(width, 24);

        pntr_image* left = pntr_test_canvas(100, 16);
        pntr_draw_text_aligned(left, font, text, 40, 4, PNTR_TEXT_ALIGN_LEFT, PNTR_BLACK);
        pntr_image* leftExpected = pntr_test_text_lines(font, text, 100, 16, 40, 4, PNTR_TEXT_ALIGN_LEFT);
        IMAGEEQUALS(left, leftExpected);

        pntr_image* center = pntr_test_canvas(100, 16);
        pntr_draw_text_aligned(center, font, text, 40, 4, PNTR_TEXT_ALIGN_CENTER, PNTR_BLACK);
        pntr_image* centerExpected = pntr_test_text_lines(font, text, 100, 16, 40, 4, PNTR_TEXT_ALIGN_CENTER);
        IMAGEEQUALS(center, centerExpected);

        pntr_image* right = pntr_test_canvas(100, 16);
        pntr_draw_text_aligned(right, font, text, 40, 4, PNTR_TEXT_ALIGN_RIGHT, PNTR_BLACK);
        pntr_image* rightExpected = pntr_test_text_lines(font, text, 100, 16, 40, 4, PNTR_TEXT_ALIGN_RIGHT);
        IMAGEEQUALS(right, rightExpected);

        // The "B" paints from the first column of its glyph, so the left edge of what is
        // painted is the position the line was started at.
        pntr_rectangle leftBounds = pntr_test_painted_bounds(left, PNTR_TEST_BACKGROUND);
        pntr_rectangle centerBounds = pntr_test_painted_bounds(center, PNTR_TEST_BACKGROUND);
        pntr_rectangle rightBounds = pntr_test_painted_bounds(right, PNTR_TEST_BACKGROUND);
        EQUALS(leftBounds.x, 40);
        EQUALS(centerBounds.x, 40 - width / 2);
        EQUALS(rightBounds.x, 40 - width);
        EQUALS(leftBounds.y, 4);
        EQUALS(centerBounds.y, 4);
        EQUALS(rightBounds.y, 4);
        EQUALS(centerBounds.width, leftBounds.width);
        EQUALS(rightBounds.width, leftBounds.width);
        EQUALS(centerBounds.height, leftBounds.height);
        EQUALS(rightBounds.height, leftBounds.height);

        pntr_unload_image(rightExpected);
        pntr_unload_image(right);
        pntr_unload_image(centerExpected);
        pntr_unload_image(center);
        pntr_unload_image(leftExpected);
        pntr_unload_image(left);
        pntr_unload_font(font);
    });

    IT("pntr_draw_text_aligned(): an empty line", {
        // An empty line leaves a blank line behind, and the line after it is drawn once.
        pntr_font* font = pntr_load_font_default();
        NEQUALS(font, NULL);

        int lineHeight = pntr_test_line_height(font);
        EQUALS(lineHeight, 8);

        pntr_image* image = pntr_test_canvas(100, 48);
        pntr_draw_text_aligned(image, font, "Hi\n\nBye", 40, 4, PNTR_TEXT_ALIGN_CENTER, PNTR_BLACK);
        pntr_image* expected = pntr_test_text_lines(font, "Hi\n\nBye", 100, 48, 40, 4, PNTR_TEXT_ALIGN_CENTER);
        IMAGEEQUALS(image, expected);

        // Three lines of text, from the top of the first to the bottom of the third.
        pntr_rectangle bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
        pntr_rectangle wanted = PNTR_CLITERAL(pntr_rectangle) { .x = 28, .y = 4, .width = 22, .height = 24 };
        RECTEQUALS(bounds, wanted);

        // The middle line has nothing on it.
        EQUALS(pntr_test_painted_rows(image, 4 + lineHeight, 4 + lineHeight * 2), 0);

        // Nothing is painted twice, so the two words together are all there is.
        pntr_image* words = pntr_test_canvas(100, 48);
        pntr_draw_text_aligned(words, font, "Hi", 40, 4, PNTR_TEXT_ALIGN_CENTER, PNTR_BLACK);
        pntr_draw_text_aligned(words, font, "Bye", 40, 4 + lineHeight * 2, PNTR_TEXT_ALIGN_CENTER, PNTR_BLACK);
        EQUALS(pntr_test_painted_count(image), pntr_test_painted_count(words));
        IMAGEEQUALS(image, words);

        pntr_unload_image(words);
        pntr_unload_image(expected);
        pntr_unload_image(image);
        pntr_unload_font(font);
    });

    IT("pntr_draw_text_aligned(): a leading newline", {
        // A newline before any text has been drawn still moves down a line.
        pntr_font* font = pntr_load_font_default();
        NEQUALS(font, NULL);

        int lineHeight = pntr_test_line_height(font);

        pntr_image* center = pntr_test_canvas(100, 32);
        pntr_draw_text_aligned(center, font, "\nAB", 40, 0, PNTR_TEXT_ALIGN_CENTER, PNTR_BLACK);
        pntr_image* centerExpected = pntr_test_text_lines(font, "\nAB", 100, 32, 40, 0, PNTR_TEXT_ALIGN_CENTER);
        IMAGEEQUALS(center, centerExpected);

        pntr_rectangle centerBounds = pntr_test_painted_bounds(center, PNTR_TEST_BACKGROUND);
        EQUALS(centerBounds.y, lineHeight);
        EQUALS(pntr_test_painted_rows(center, 0, lineHeight), 0);

        // The left alignment hands the whole string to pntr_draw_text(), which has to
        // agree about where the second line is.
        pntr_image* left = pntr_test_canvas(100, 32);
        pntr_draw_text_aligned(left, font, "\nAB", 40, 0, PNTR_TEXT_ALIGN_LEFT, PNTR_BLACK);
        pntr_image* leftExpected = pntr_test_text_lines(font, "\nAB", 100, 32, 40, 0, PNTR_TEXT_ALIGN_LEFT);
        IMAGEEQUALS(left, leftExpected);

        pntr_rectangle leftBounds = pntr_test_painted_bounds(left, PNTR_TEST_BACKGROUND);
        EQUALS(leftBounds.y, lineHeight);

        pntr_unload_image(leftExpected);
        pntr_unload_image(left);
        pntr_unload_image(centerExpected);
        pntr_unload_image(center);
        pntr_unload_font(font);
    });

    IT("pntr_draw_text_aligned(): a trailing newline", {
        // The line a trailing newline opens is empty, so it paints nothing.
        pntr_font* font = pntr_load_font_default();
        NEQUALS(font, NULL);

        pntr_image* center = pntr_test_canvas(100, 32);
        pntr_image* centerExpected = pntr_test_canvas(100, 32);
        pntr_draw_text_aligned(center, font, "A\n", 40, 4, PNTR_TEXT_ALIGN_CENTER, PNTR_BLACK);
        pntr_draw_text_aligned(centerExpected, font, "A", 40, 4, PNTR_TEXT_ALIGN_CENTER, PNTR_BLACK);
        IMAGEEQUALS(center, centerExpected);

        pntr_image* right = pntr_test_canvas(100, 32);
        pntr_image* rightExpected = pntr_test_canvas(100, 32);
        pntr_draw_text_aligned(right, font, "A\n", 40, 4, PNTR_TEXT_ALIGN_RIGHT, PNTR_BLACK);
        pntr_draw_text_aligned(rightExpected, font, "A", 40, 4, PNTR_TEXT_ALIGN_RIGHT, PNTR_BLACK);
        IMAGEEQUALS(right, rightExpected);

        pntr_unload_image(rightExpected);
        pntr_unload_image(right);
        pntr_unload_image(centerExpected);
        pntr_unload_image(center);
        pntr_unload_font(font);
    });

    IT("pntr_draw_text_aligned(): consecutive newlines", {
        // Every newline moves down one line, however many of them there are in a row.
        pntr_font* font = pntr_load_font_default();
        NEQUALS(font, NULL);

        int lineHeight = pntr_test_line_height(font);

        pntr_image* image = pntr_test_canvas(100, 48);
        pntr_draw_text_aligned(image, font, "A\n\n\nB", 40, 0, PNTR_TEXT_ALIGN_CENTER, PNTR_BLACK);
        pntr_image* expected = pntr_test_text_lines(font, "A\n\n\nB", 100, 48, 40, 0, PNTR_TEXT_ALIGN_CENTER);
        IMAGEEQUALS(image, expected);

        // The "B" is on the fourth line, three line heights down.
        pntr_image* words = pntr_test_canvas(100, 48);
        pntr_draw_text_aligned(words, font, "A", 40, 0, PNTR_TEXT_ALIGN_CENTER, PNTR_BLACK);
        pntr_draw_text_aligned(words, font, "B", 40, lineHeight * 3, PNTR_TEXT_ALIGN_CENTER, PNTR_BLACK);
        IMAGEEQUALS(image, words);
        EQUALS(pntr_test_painted_rows(image, lineHeight, lineHeight * 3), 0);

        pntr_rectangle bounds = pntr_test_painted_bounds(image, PNTR_TEST_BACKGROUND);
        EQUALS(bounds.y, 0);
        EQUALS(bounds.height, lineHeight * 3 + 7);

        pntr_unload_image(words);
        pntr_unload_image(expected);
        pntr_unload_image(image);
        pntr_unload_font(font);
    });

    if (pntr_utf8()) {
        IT("pntr_draw_text_aligned(): multi-byte characters", {
            // A line is as long as the characters on it, not as the bytes they take up,
            // which is what keeps the newline that ends the line out of the measurement.
            pntr_font* font = pntr_load_font_default();
            NEQUALS(font, NULL);

            int lineHeight = pntr_test_line_height(font);

            // "héllo" is six bytes of five characters.
            const char* text = "h\xC3\xA9llo\nworld";
            pntr_image* image = pntr_test_canvas(120, 48);
            pntr_draw_text_aligned(image, font, text, 60, 0, PNTR_TEXT_ALIGN_CENTER, PNTR_BLACK);
            pntr_image* expected = pntr_test_text_lines(font, text, 120, 48, 60, 0, PNTR_TEXT_ALIGN_CENTER);
            IMAGEEQUALS(image, expected);

            // "world" is one line below "héllo", not two.
            pntr_image* lines = pntr_test_canvas(120, 48);
            pntr_draw_text_aligned(lines, font, "h\xC3\xA9llo", 60, 0, PNTR_TEXT_ALIGN_CENTER, PNTR_BLACK);
            pntr_draw_text_aligned(lines, font, "world", 60, lineHeight, PNTR_TEXT_ALIGN_CENTER, PNTR_BLACK);
            IMAGEEQUALS(image, lines);

            // Two multi-byte characters would reach two bytes past the newline, and draw
            // the start of the next line twice.
            const char* twice = "h\xC3\xA9ll\xC3\xB3\nworld";
            pntr_image* spill = pntr_test_canvas(120, 48);
            pntr_draw_text_aligned(spill, font, twice, 60, 0, PNTR_TEXT_ALIGN_CENTER, PNTR_BLACK);
            pntr_image* spillExpected = pntr_test_text_lines(font, twice, 120, 48, 60, 0, PNTR_TEXT_ALIGN_CENTER);
            IMAGEEQUALS(spill, spillExpected);
            EQUALS(pntr_test_painted_rows(spill, lineHeight * 2, 48), 0);

            // A length is counted in characters too, so five of them stops at the newline.
            pntr_vector size = pntr_measure_text_ex(font, text, 5);
            pntr_vector line = pntr_measure_text_ex(font, "h\xC3\xA9llo", 0);
            EQUALS(size.x, line.x);
            EQUALS(size.y, line.y);
            EQUALS(size.y, lineHeight);

            pntr_unload_image(spillExpected);
            pntr_unload_image(spill);
            pntr_unload_image(lines);
            pntr_unload_image(expected);
            pntr_unload_image(image);
            pntr_unload_font(font);
        });
    }
    else {
        IT("pntr_draw_text_aligned(): multi-byte characters", {
            // Without PNTR_ENABLE_UTF8 a character is a byte, so there is no multi-byte
            // text for the line length to be counted wrong from.
            SKIP();
        });
    }

    IT("pntr_measure_text_ex(), pntr_gen_image_text(): newlines", {
        // The height of the text is a line for each newline, plus the last line.
        pntr_font* font = pntr_load_font_default();
        NEQUALS(font, NULL);

        int lineHeight = pntr_test_line_height(font);

        pntr_vector single = pntr_measure_text_ex(font, "Hello", 0);
        EQUALS(single.x, 40);
        EQUALS(single.y, lineHeight);

        // A newline before any glyph still opens a line of its own.
        pntr_vector leading = pntr_measure_text_ex(font, "\nHello", 0);
        EQUALS(leading.x, single.x);
        EQUALS(leading.y, lineHeight * 2);

        // An empty line between two words is as tall as a full one.
        pntr_vector blank = pntr_measure_text_ex(font, "Hi\n\nBye", 0);
        EQUALS(blank.x, 24);
        EQUALS(blank.y, lineHeight * 3);

        // A measurement of nothing is nothing, which is what pntr_gen_image_text()
        // refuses on.
        pntr_vector empty = pntr_measure_text_ex(font, "", 0);
        EQUALS(empty.x, 0);
        EQUALS(empty.y, 0);

        // The image generated for the text is sized by that measurement, and the text is
        // drawn on the line the measurement left room for.
        pntr_image* image = pntr_gen_image_text(font, "\nHello", PNTR_BLACK, PNTR_TEST_BACKGROUND);
        NEQUALS(image, NULL);
        EQUALS(image->width, leading.x);
        EQUALS(image->height, leading.y);

        pntr_image* expected = pntr_test_canvas(leading.x, leading.y);
        pntr_draw_text(expected, font, "Hello", 0, lineHeight, PNTR_BLACK);
        IMAGEEQUALS(image, expected);

        pntr_unload_image(expected);
        pntr_unload_image(image);
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
        pntr_color halfBlue = pntr_color_fade(PNTR_BLUE, -0.5f);
        pntr_color blended = pntr_color_alpha_blend(halfBlue, half);
        if (pntr_alphablend()) {
            GREATER(blended.rgba.r, PNTR_BLUE.rgba.r);
            GREATER(blended.rgba.a, 127);
            LESSER(blended.rgba.b, PNTR_BLUE.rgba.b);
        }
        else {
            // A non-opaque source is skipped rather than blended, so the destination
            // comes back exactly as it went in.
            COLOREQUALS(blended, halfBlue);
        }
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

    if (pntr_file_io()) {
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
    }
    else {
        IT("pntr_load_file(): nothing can be read", {
            // A file that is really there is refused just the same, and the read size is still
            // reported back as the nothing that was read.
            unsigned int bytesRead = 1234;
            EQUALS(pntr_load_file("resources/text.txt", &bytesRead), NULL);
            EQUALS(bytesRead, 0);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
            pntr_set_error(PNTR_ERROR_NONE);

            // Reporting the size back stays optional.
            EQUALS(pntr_load_file("resources/text.txt", NULL), NULL);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
            pntr_set_error(PNTR_ERROR_NONE);

            // Unloading nothing is still safe.
            pntr_unload_file(NULL);
        });

        IT("pntr_load_file(): Empty file is refused too", {
            // The refusal happens before anything about the file is looked at, so a file that
            // exists and holds no data is refused for the same reason as any other.
            const char* fileName = "tempFileEmpty.txt";
            FILE* emptyFile = fopen(fileName, "wb");
            NEQUALS(emptyFile, NULL);
            fclose(emptyFile);

            unsigned int bytesRead = 1234;
            EQUALS(pntr_load_file(fileName, &bytesRead), NULL);
            EQUALS(bytesRead, 0);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
            pntr_set_error(PNTR_ERROR_NONE);

            remove(fileName);
        });

        IT("pntr_load_file(): Directory is refused too", {
            // Pinned by "pntr_load_file(): Reports that it was compiled out", which is where a
            // missing file and a directory are shown to be refused alike.
            SKIP();
        });

        IT("pntr_load_file_text(): nothing can be read", {
            // The one thing a caller must be able to rely on is that a refusal hands back
            // nothing rather than a string that can't be read.
            EQUALS(pntr_load_file_text("resources/text.txt"), NULL);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
            pntr_set_error(PNTR_ERROR_NONE);

            EQUALS(pntr_load_file_text(NULL), NULL);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_INVALID_ARGS);
            pntr_set_error(PNTR_ERROR_NONE);

            // Unloading nothing is still safe.
            pntr_unload_file_text(NULL);
        });

        IT("pntr_load_file_text(): Empty file is refused too", {
            const char* fileName = "tempFileEmptyText.txt";
            FILE* emptyFile = fopen(fileName, "wb");
            NEQUALS(emptyFile, NULL);
            fclose(emptyFile);

            EQUALS(pntr_load_file_text(fileName), NULL);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
            pntr_set_error(PNTR_ERROR_NONE);

            remove(fileName);
        });
    }

    if (pntr_file_io()) {
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
    }
    else {
        IT("pntr_load_font_ttf(): refuses without a file", {
            EQUALS(pntr_load_font_ttf("resources/tuffy.ttf", 20), NULL);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
            pntr_set_error(PNTR_ERROR_NONE);

            // Invalid arguments are still rejected as such.
            EQUALS(pntr_load_font_ttf(NULL, 20), NULL);
            EQUALS(pntr_load_font_ttf("resources/tuffy.ttf", 0), NULL);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_INVALID_ARGS);
            pntr_set_error(PNTR_ERROR_NONE);

            IT("pntr_measure_text_ex(): without a font", {
                pntr_vector size = pntr_measure_text_ex(NULL, "Hello!!", 0);
                EQUALS(size.x, 0);
                EQUALS(size.y, 0);
            });
        });
    }

    if (pntr_file_io()) {
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
    }
    else {
        IT("pntr_save_file(): nothing can be written", {
            const char* fileName = "tempFile.txt";
            const char* fileData = "Hello World!";

            EQUALS(pntr_save_file(fileName, (unsigned char*)fileData, 12), false);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
            pntr_set_error(PNTR_ERROR_NONE);

            // Nothing was created, so there is nothing to read back either.
            EQUALS(pntr_load_file(fileName, NULL), NULL);
            pntr_set_error(PNTR_ERROR_NONE);

            // Invalid arguments are still rejected as such.
            EQUALS(pntr_save_file(NULL, (unsigned char*)fileData, 12), false);
            EQUALS(pntr_save_file(fileName, NULL, 12), false);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_INVALID_ARGS);
            pntr_set_error(PNTR_ERROR_NONE);
        });

        IT("pntr_save_file(): No data is refused too", {
            // Saving zero bytes is a successful way of emptying a file where there is a file
            // system, and nothing at all where there isn't.
            EQUALS(pntr_save_file("tempFileNoData.txt", (unsigned char*)"Hello World!", 0), false);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
            pntr_set_error(PNTR_ERROR_NONE);
        });

        IT("pntr_save_file(): Failures", {
            // Every save already fails, so there is no failure of the file system left to
            // tell apart from it.
            SKIP();
        });

        IT("pntr_save_file(), pntr_load_file(): Round trip", {
            // Neither half of the round trip is there to make.
            SKIP();
        });
    }

    if (pntr_image_saving() && pntr_image_from_file()) {
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
    }
    else {
        IT("pntr_save_image(): without a round trip", {
            pntr_image* saveImage = pntr_gen_image_color(400, 300, PNTR_RED);
            NEQUALS(saveImage, NULL);
            pntr_draw_rectangle_fill(saveImage, 10, 10, 20, 20, PNTR_GREEN);

            if (pntr_image_saving() && pntr_file_io()) {
                // The file is still written. It is only reading one back that was compiled out.
                EQUALS(pntr_save_image(saveImage, "saveImage.png"), true);
                EQUALS(pntr_get_error_code(), PNTR_ERROR_NONE);

                EQUALS(pntr_load_image("saveImage.png"), NULL);
                EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
                pntr_set_error(PNTR_ERROR_NONE);
                remove("saveImage.png");
            }
            else {
                // Without an encoder, or without a file system to write to, nothing is saved.
                EQUALS(pntr_save_image(saveImage, "saveImage.png"), false);
                EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
                pntr_set_error(PNTR_ERROR_NONE);
            }

            pntr_unload_image(saveImage);
        });
    }

    if (pntr_bmp_loading() && pntr_image_saving() && pntr_file_io()) {
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
        IT("pntr_save_image(): .bmp: without a round trip", {
            pntr_image* saveImage = pntr_gen_image_color(64, 64, PNTR_RED);
            NEQUALS(saveImage, NULL);
            pntr_draw_rectangle_fill(saveImage, 8, 8, 16, 16, PNTR_GREEN);

            // A 64x64 RGBA bitmap is a 14 byte file header, a 108 byte V4 info
            // header, then 32 bits per pixel.
            unsigned int expectedSize = 14 + 108 + (64 * 64 * 4);

            unsigned int memorySize = 100;
            unsigned char* memoryData = pntr_save_image_to_memory(saveImage, PNTR_IMAGE_TYPE_BMP, &memorySize);

            if (!pntr_bmp() || !pntr_image_saving()) {
                // A PNG-only backend, and a build with no encoder at all, both have to
                // refuse the bitmap rather than write one.
                EQUALS(memoryData, NULL);
                EQUALS(memorySize, 0);
                EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
                pntr_set_error(PNTR_ERROR_NONE);

                EQUALS(pntr_save_image(saveImage, "saveImage.bmp"), false);
                EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
                pntr_set_error(PNTR_ERROR_NONE);
            }
            else {
                // The bitmap is still written, header and all.
                NEQUALS(memoryData, NULL);
                EQUALS(memorySize, expectedSize);
                EQUALS(memoryData[0], 'B');
                EQUALS(memoryData[1], 'M');
                unsigned int declaredSize = (unsigned int)memoryData[2] | ((unsigned int)memoryData[3] << 8) |
                    ((unsigned int)memoryData[4] << 16) | ((unsigned int)memoryData[5] << 24);
                EQUALS(declaredSize, expectedSize);

                if (pntr_file_io()) {
                    // The file has to hold exactly what was saved to memory.
                    EQUALS(pntr_save_image(saveImage, "saveImage.bmp"), true);

                    unsigned int bytesRead = 0;
                    unsigned char* fileData = pntr_load_file("saveImage.bmp", &bytesRead);
                    NEQUALS(fileData, NULL);
                    EQUALS(bytesRead, memorySize);
                    EQUALS(memcmp(fileData, memoryData, (size_t)memorySize), 0);
                    pntr_unload_file(fileData);
                    remove("saveImage.bmp");
                }
                else {
                    // There is nowhere to put it.
                    EQUALS(pntr_save_image(saveImage, "saveImage.bmp"), false);
                    EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
                    pntr_set_error(PNTR_ERROR_NONE);
                }

                if (pntr_bmp_loading()) {
                    // The bitmap still decodes, pixel for pixel, out of memory.
                    pntr_image* loadedImage = pntr_load_image_from_memory(PNTR_IMAGE_TYPE_BMP, memoryData, memorySize);
                    NEQUALS(loadedImage, NULL);
                    IMAGEEQUALS(loadedImage, saveImage);
                    pntr_unload_image(loadedImage);
                }
                else {
                    // What was written can't be read back, but never for a lack of memory.
                    EQUALS(pntr_load_image_from_memory(PNTR_IMAGE_TYPE_BMP, memoryData, memorySize), NULL);
                    NEQUALS(pntr_get_error_code(), PNTR_ERROR_NONE);
                    NEQUALS(pntr_get_error_code(), PNTR_ERROR_NO_MEMORY);
                    pntr_set_error(PNTR_ERROR_NONE);
                }
            }

            pntr_unload_memory(memoryData);
            pntr_unload_image(saveImage);
        });
    }

    if (pntr_image_saving()) {
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
            if (pntr_image_loading()) {
                NEQUALS(loadedPng, NULL);
                IMAGEEQUALS(loadedPng, image);
                pntr_unload_image(loadedPng);
            }
            else {
                // Decoding was compiled out, so what was just encoded can't be read back.
                EQUALS(loadedPng, NULL);
                EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
                pntr_set_error(PNTR_ERROR_NONE);
            }
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
    }
    else {
        IT("pntr_save_image_to_memory(): refuses without an encoder", {
            pntr_image* image = pntr_gen_image_color(64, 64, PNTR_BLUE);
            NEQUALS(image, NULL);

            // Nothing is written, and the reported size has to say so rather than keep
            // whatever the caller happened to pass in.
            unsigned int pngSize = 100;
            EQUALS(pntr_save_image_to_memory(image, PNTR_IMAGE_TYPE_PNG, &pngSize), NULL);
            EQUALS(pngSize, 0);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
            pntr_set_error(PNTR_ERROR_NONE);

            // Every type is refused the same way, including the unknown one that otherwise
            // falls back to PNG.
            EQUALS(pntr_save_image_to_memory(image, PNTR_IMAGE_TYPE_UNKNOWN, NULL), NULL);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
            pntr_set_error(PNTR_ERROR_NONE);

            // A NULL image is still invalid rather than unsupported.
            EQUALS(pntr_save_image_to_memory(NULL, PNTR_IMAGE_TYPE_PNG, NULL), NULL);
            EQUALS(pntr_get_error_code(), PNTR_ERROR_INVALID_ARGS);
            pntr_set_error(PNTR_ERROR_NONE);

            pntr_unload_image(image);
        });
    }

    if (pntr_jpeg() && pntr_jpeg_saving() && pntr_file_io()) {
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
        IT("pntr_save_image(): .jpg: without a round trip", {
            pntr_image* image = pntr_gen_image_color(64, 64, PNTR_BLUE);
            NEQUALS(image, NULL);
            pntr_draw_rectangle_fill(image, 4, 4, 10, 10, PNTR_WHITE);

            unsigned int jpgSize = 100;
            unsigned char* jpg = pntr_save_image_to_memory(image, PNTR_IMAGE_TYPE_JPG, &jpgSize);

            if (!pntr_jpeg_saving()) {
                // A PNG-only backend, and a build with no encoder at all, both have to
                // refuse the JPEG rather than write one.
                EQUALS(jpg, NULL);
                EQUALS(jpgSize, 0);
                EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
                pntr_set_error(PNTR_ERROR_NONE);

                EQUALS(pntr_save_image(image, "saveImage.jpg"), false);
                EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
                pntr_set_error(PNTR_ERROR_NONE);
            }
            else {
                // Saving a JPEG is always available. It is only loading one back that
                // needs PNTR_ENABLE_JPEG.
                NEQUALS(jpg, NULL);
                GREATER(jpgSize, 600);
                EQUALS(jpg[0], 0xFF);
                EQUALS(jpg[1], 0xD8);
                EQUALS(jpg[jpgSize - 2], 0xFF);
                EQUALS(jpg[jpgSize - 1], 0xD9);

                if (pntr_file_io()) {
                    // The file has to hold exactly what was saved to memory.
                    EQUALS(pntr_save_image(image, "saveImage.jpg"), true);

                    unsigned int bytesRead = 0;
                    unsigned char* fileData = pntr_load_file("saveImage.jpg", &bytesRead);
                    NEQUALS(fileData, NULL);
                    EQUALS(bytesRead, jpgSize);
                    EQUALS(memcmp(fileData, jpg, (size_t)jpgSize), 0);
                    pntr_unload_file(fileData);
                    remove("saveImage.jpg");
                }
                else {
                    EQUALS(pntr_save_image(image, "saveImage.jpg"), false);
                    EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
                    pntr_set_error(PNTR_ERROR_NONE);
                }

                if (pntr_jpeg()) {
                    pntr_image* loadedJpg = pntr_load_image_from_memory(PNTR_IMAGE_TYPE_JPG, jpg, jpgSize);
                    NEQUALS(loadedJpg, NULL);
                    EQUALS(loadedJpg->width, 64);
                    EQUALS(loadedJpg->height, 64);
                    pntr_unload_image(loadedJpg);
                }
                else {
                    // What was written can't be read back, but never for a lack of memory.
                    EQUALS(pntr_load_image_from_memory(PNTR_IMAGE_TYPE_JPG, jpg, jpgSize), NULL);
                    NEQUALS(pntr_get_error_code(), PNTR_ERROR_NONE);
                    NEQUALS(pntr_get_error_code(), PNTR_ERROR_NO_MEMORY);
                    pntr_set_error(PNTR_ERROR_NONE);
                }
            }

            pntr_unload_memory(jpg);
            pntr_unload_image(image);
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

    IT("pntr_image_resize_canvas() moves pixels rather than blending them", {
        // It used to draw the original onto the filled canvas, which blends: a
        // semi-transparent pixel came out mixed with the fill in the default build, and
        // under PNTR_NO_ALPHABLEND it was skipped entirely and the fill was left in its
        // place. Resizing a canvas moves pixels, so every one has to arrive unchanged
        // whatever the build does about blending. All three existing tests here use opaque
        // colors over an opaque fill, which is why this was invisible.
        pntr_image* image = pntr_new_image(6, 5);
        NEQUALS(image, NULL);

        // No two pixels alike, and a different alpha in each, so a blend cannot coincide
        // with the right answer.
        for (int y = 0; y < 5; y++) {
            for (int x = 0; x < 6; x++) {
                pntr_draw_point(image, x, y, pntr_new_color(
                    (unsigned char)(x * 20 + 1),
                    (unsigned char)(y * 30 + 2),
                    (unsigned char)(x * y + 3),
                    (unsigned char)(x * 7 + y * 11)));
            }
        }

        pntr_image* expected = pntr_image_copy(image);
        NEQUALS(expected, NULL);

        // An opaque fill, which is what makes a blend visible on every channel.
        pntr_color fill = pntr_new_color(9, 9, 9, 200);
        EQUALS(pntr_image_resize_canvas(image, 20, 16, 7, 6, fill), true);

        int moved = 0;
        for (int y = 0; y < 5; y++) {
            for (int x = 0; x < 6; x++) {
                if (pntr_image_get_color(image, 7 + x, 6 + y).value == pntr_image_get_color(expected, x, y).value) {
                    moved++;
                }
            }
        }
        EQUALS(moved, 6 * 5);

        // And the margin is the fill, not a blend of it with anything.
        COLOREQUALS(pntr_image_get_color(image, 0, 0), fill);
        COLOREQUALS(pntr_image_get_color(image, 19, 15), fill);

        pntr_unload_image(expected);
        pntr_unload_image(image);
    });

    IT("pntr_image_resize_canvas() with a negative offset and a smaller canvas", {
        // The offset can push the image off the top left, and the new canvas can be
        // smaller than the old one, so only part of the original survives. What survives
        // still has to be exact rather than blended.
        pntr_image* image = pntr_new_image(10, 10);
        NEQUALS(image, NULL);

        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 10; x++) {
                pntr_draw_point(image, x, y, pntr_new_color(
                    (unsigned char)(x * 25),
                    (unsigned char)(y * 25),
                    7,
                    (unsigned char)(x + y * 10)));
            }
        }

        pntr_image* expected = pntr_image_copy(image);
        NEQUALS(expected, NULL);

        EQUALS(pntr_image_resize_canvas(image, 6, 6, -3, -2, PNTR_BLANK), true);

        int exact = 0;
        for (int y = 0; y < 6; y++) {
            for (int x = 0; x < 6; x++) {
                // The pixel of the original that landed here, or the fill where the
                // original did not reach.
                pntr_color want = (x + 3 < 10 && y + 2 < 10)
                    ? pntr_image_get_color(expected, x + 3, y + 2)
                    : PNTR_BLANK;

                if (pntr_image_get_color(image, x, y).value == want.value) {
                    exact++;
                }
            }
        }
        EQUALS(exact, 6 * 6);

        pntr_unload_image(expected);
        pntr_unload_image(image);
    });

    IT("pntr_image_resize_canvas() of a subimage", {
        // A subimage has a pitch wider than its width, so copying it row by row has to
        // step by the pitch rather than by the width.
        pntr_image* parent = pntr_gen_image_color(16, 16, PNTR_RED);
        NEQUALS(parent, NULL);

        pntr_image* image = pntr_image_subimage(parent, 4, 4, 5, 5);
        NEQUALS(image, NULL);

        for (int y = 0; y < 5; y++) {
            for (int x = 0; x < 5; x++) {
                pntr_draw_point(image, x, y, pntr_new_color(
                    (unsigned char)(x + 1), (unsigned char)(y + 1), 2, (unsigned char)(x * 5 + y + 1)));
            }
        }

        pntr_image* expected = pntr_image_copy(image);
        NEQUALS(expected, NULL);

        EQUALS(pntr_image_resize_canvas(image, 9, 9, 2, 2, PNTR_BLANK), true);

        int exact = 0;
        for (int y = 0; y < 5; y++) {
            for (int x = 0; x < 5; x++) {
                if (pntr_image_get_color(image, 2 + x, 2 + y).value == pntr_image_get_color(expected, x, y).value) {
                    exact++;
                }
            }
        }
        EQUALS(exact, 5 * 5);

        pntr_unload_image(expected);
        pntr_unload_image(image);
        pntr_unload_image(parent);
    });

    IT("pntr_image_resize_canvas() pushed entirely off the new canvas", {
        pntr_image* image = pntr_gen_image_color(4, 4, pntr_new_color(10, 20, 30, 128));
        NEQUALS(image, NULL);

        EQUALS(pntr_image_resize_canvas(image, 4, 4, 100, 100, PNTR_WHITE), true);

        // Nothing of the original reaches the new canvas, so all of it is the fill.
        int fill = 0;
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (pntr_image_get_color(image, x, y).value == PNTR_WHITE.value) {
                    fill++;
                }
            }
        }
        EQUALS(fill, 4 * 4);

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

                // The origin is a pivot, so the center of source pixel (0, 0) is what lands
                // on (2, 2). At a scale of two that pixel covers two destination pixels, and
                // its center falls on the second of them, which puts the 8x8 draw at
                // (1, 1) through (8, 8) rather than (2, 2) through (9, 9).

                IT("with the bilinear filter", {
                    pntr_image* dst = pntr_gen_image_color(20, 20, PNTR_GREEN);
                    NEQUALS(dst, NULL);
                    pntr_draw_image_rotozoom(dst, src, srcRect, 2, 2, 0.0f, 2.0f, 2.0f, 0.0f, 0.0f, PNTR_FILTER_BILINEAR, PNTR_WHITE);
                    COLOREQUALS(pntr_image_get_color(dst, 2, 2), PNTR_RED);
                    COLOREQUALS(pntr_image_get_color(dst, 5, 5), PNTR_RED);
                    COLOREQUALS(pntr_image_get_color(dst, 1, 1), PNTR_RED);
                    COLOREQUALS(pntr_image_get_color(dst, 8, 8), PNTR_RED);
                    COLOREQUALS(pntr_image_get_color(dst, 0, 0), PNTR_GREEN);
                    COLOREQUALS(pntr_image_get_color(dst, 9, 9), PNTR_GREEN);
                    COLOREQUALS(pntr_image_get_color(dst, 10, 10), PNTR_GREEN);
                    pntr_unload_image(dst);
                });

                IT("with the nearest neighbor filter", {
                    pntr_image* dst = pntr_gen_image_color(20, 20, PNTR_GREEN);
                    NEQUALS(dst, NULL);
                    pntr_draw_image_rotozoom(dst, src, srcRect, 2, 2, 0.0f, 2.0f, 2.0f, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);
                    COLOREQUALS(pntr_image_get_color(dst, 2, 2), PNTR_RED);
                    COLOREQUALS(pntr_image_get_color(dst, 1, 1), PNTR_RED);
                    COLOREQUALS(pntr_image_get_color(dst, 8, 8), PNTR_RED);
                    COLOREQUALS(pntr_image_get_color(dst, 0, 0), PNTR_GREEN);
                    COLOREQUALS(pntr_image_get_color(dst, 9, 9), PNTR_GREEN);
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

            IT("pntr_draw_image_rotozoom() moves continuously through no rotation at any scale", {
                // No rotation is the one angle that is handed to the scaled drawing path
                // rather than to a rotation, so it is the angle most able to disagree with
                // the general rotation a thousandth of a degree either side of it. The two
                // only agree because both place the center of the origin pixel on the
                // position: the scaled path offsets from the edge of that pixel, so
                // pntr_draw_image_rotozoom() adds the half source pixel between them.
                //
                // A scale of one, whole number scales and scales that are neither are all
                // covered, because the drawn size is a whole multiple of the source at the
                // first two and a truncation of it at the third. The half pixel between the
                // two conventions is worth half a destination pixel, so the step this used
                // to leave grew with the scale rather than staying at a pixel: 2px at a
                // scale of four and 4px at a scale of eight.
                //
                // A source small enough that eight times it still fits inside the
                // destination, so the far edges report the draw rather than the clip.
                pntr_image* zoomSource = pntr_test_pivot_source(16, 10, 2, 5);
                NEQUALS(zoomSource, NULL);

                float zoomScales[8] = {1.0f, 1.5f, 2.0f, 2.5f, 3.0f, 4.0f, 6.0f, 8.0f};

                for (int i = 0; i < 8; i++) {
                    pntr_rectangle before = pntr_test_rotozoom_bounds(zoomSource, -0.001f, 2.0f, 5.0f, zoomScales[i]);
                    pntr_rectangle exact = pntr_test_rotozoom_bounds(zoomSource, 0.0f, 2.0f, 5.0f, zoomScales[i]);
                    pntr_rectangle after = pntr_test_rotozoom_bounds(zoomSource, 0.001f, 2.0f, 5.0f, zoomScales[i]);

                    // Something was actually drawn, so an empty rectangle can't pass as still.
                    GREATER(exact.width, 0);
                    GREATER(exact.height, 0);

                    // The scaled path truncates the drawn size while the rotation takes the
                    // ceiling of it, so the two boxes can differ by a pixel in size however
                    // well their pivots agree. They may not differ by more than that.
                    LESSER(pntr_test_rectangle_distance(before, exact), 2);
                    LESSER(pntr_test_rectangle_distance(exact, after), 2);
                }

                pntr_unload_image(zoomSource);
            });

            IT("pntr_draw_image_rotozoom() lands the origin pixel on the position when scaled", {
                // A source where every pixel carries a color of its own, so the pixel that
                // lands on the position names itself. At a whole number scale every source
                // pixel covers a whole block of destination pixels, and the center of the
                // origin pixel is inside its own block, so the position has to report the
                // origin pixel exactly rather than within a pixel of it.
                pntr_image* unique = pntr_gen_image_color(8, 6, PNTR_BLANK);
                NEQUALS(unique, NULL);
                for (int y = 0; y < 6; y++) {
                    for (int x = 0; x < 8; x++) {
                        pntr_draw_point(unique, x, y, pntr_new_color(
                            (unsigned char)(x * 30 + 5),
                            (unsigned char)(y * 40 + 7),
                            (unsigned char)(x * y + 1),
                            255));
                    }
                }

                pntr_rectangle uniqueRect = {0, 0, 8, 6};
                for (int scale = 1; scale <= 5; scale++) {
                    for (int originY = 0; originY < 6; originY++) {
                        for (int originX = 0; originX < 8; originX++) {
                            pntr_image* dst = pntr_test_canvas(100, 100);
                            NEQUALS(dst, NULL);

                            pntr_draw_image_rotozoom(dst, unique, uniqueRect, 50, 50, 0.0f,
                                (float)scale, (float)scale, (float)originX, (float)originY,
                                PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);

                            COLOREQUALS(pntr_image_get_color(dst, 50, 50), pntr_image_get_color(unique, originX, originY));

                            pntr_unload_image(dst);
                        }
                    }
                }

                pntr_unload_image(unique);
            });

            IT("pntr_draw_image_rotozoom() keeps its pivot on the position at a fractional scale", {
                // A scale that truncates the drawn size steps the position onto one of the
                // neighbours of the origin pixel instead, because the block the origin pixel
                // was given is narrower than the half pixel its center sits at. Following
                // the marker is what holds that to the pixel it has always been held to.
                float fractionalScales[6] = {0.5f, 0.75f, 1.25f, 1.75f, 2.5f, 3.5f};

                for (int i = 0; i < 6; i++) {
                    pntr_image* dst = pntr_gen_image_color(300, 300, PNTR_GREEN);
                    NEQUALS(dst, NULL);

                    pntr_draw_image_rotozoom(dst, pivotSource, pivotRect, 150, 150, 0.0f,
                        fractionalScales[i], fractionalScales[i], 2.0f, 12.0f,
                        PNTR_FILTER_NEARESTNEIGHBOR, PNTR_WHITE);

                    LESSER(pntr_test_pivot_offset(dst, 150, 150), 2);

                    pntr_unload_image(dst);
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

            IT("the rotated draws land in the same place either side of a quarter turn", {
                // The pivot of a rotated draw is rounded down onto one of the bounding box
                // pixels, and at a quarter turn the rotated pivot lands exactly on the
                // boundary between two of them, because the sine and cosine there are zero
                // and one. A rotation a thousandth of a degree to either side moves it by
                // about a thousandth of a pixel, which without a tolerance is enough for the
                // rounding to pick the pixel before the boundary on one side and the pixel
                // after it on the other. The two sides of a quarter turn are mirror images,
                // so they have to land in the same place, and holding them to that is what
                // stops the rounding from being decided by the last few digits of whichever
                // sine and cosine the build was given.
                //
                // This is the half of the two pixel step that PNTR_ENABLE_MATH used to leave
                // behind. The other half is the bounding box honestly growing by a pixel the
                // moment the rotation stops being an exact quarter turn, and the two of them
                // compounded on the far edge of the draw.
                //
                // Whether the rounding lands on the boundary at all depends on the size of
                // the source and on where in it the pivot sits, which is why these shapes are
                // not the 40x24 the tests above use: that one happened to be a shape the
                // rounding never fell foul of, and it is what let this go unnoticed. Each of
                // these steps on at least one of the turns without the tolerance, in both the
                // built in trigonometry and that of math.h.
                int shapes[5][4] = {
                    {7, 5, 3, 1},
                    {15, 9, 1, 7},
                    {21, 13, 19, 11},
                    {33, 17, 30, 2},
                    {40, 24, 2, 12}
                };
                float turns[4] = {0.0f, 90.0f, 180.0f, 270.0f};
                float nudges[3] = {0.0001f, 0.001f, 0.01f};

                for (int s = 0; s < 5; s++) {
                    pntr_image* turnSource = pntr_test_pivot_source(shapes[s][0], shapes[s][1], shapes[s][2], shapes[s][3]);
                    NEQUALS(turnSource, NULL);

                    for (int t = 0; t < 4; t++) {
                        for (int n = 0; n < 3; n++) {
                            pntr_rectangle before = pntr_test_rotated_bounds(turnSource, turns[t] - nudges[n], (float)shapes[s][2], (float)shapes[s][3]);
                            pntr_rectangle after = pntr_test_rotated_bounds(turnSource, turns[t] + nudges[n], (float)shapes[s][2], (float)shapes[s][3]);

                            // Something was actually drawn, so two empty rectangles can't match.
                            GREATER(before.width, 0);
                            GREATER(before.height, 0);

                            RECTEQUALS(before, after);
                        }
                    }

                    pntr_unload_image(turnSource);
                }
            });

            IT("pntr_draw_image_rotozoom() moves continuously past every quarter turn at any scale", {
                // The whole sweep the continuity guarantee is made over, rather than the one
                // angle and the one scale each of the tests above happens to cover. Every
                // quarter turn hands the exact angle to a path of its own and the angles
                // either side of it to the general rotation, at every scale, so each of them
                // is a place the drawn area could step.
                //
                // A source small enough that eight times it still fits inside the
                // destination, so the edges report the draw rather than the clip.
                pntr_image* sweepSource = pntr_test_pivot_source(16, 10, 2, 5);
                NEQUALS(sweepSource, NULL);

                float turns[4] = {0.0f, 90.0f, 180.0f, 270.0f};
                float sweepScales[5] = {1.0f, 1.5f, 2.0f, 4.0f, 8.0f};

                for (int t = 0; t < 4; t++) {
                    for (int s = 0; s < 5; s++) {
                        pntr_rectangle before = pntr_test_rotozoom_bounds(sweepSource, turns[t] - 0.001f, 2.0f, 5.0f, sweepScales[s]);
                        pntr_rectangle exact = pntr_test_rotozoom_bounds(sweepSource, turns[t], 2.0f, 5.0f, sweepScales[s]);
                        pntr_rectangle after = pntr_test_rotozoom_bounds(sweepSource, turns[t] + 0.001f, 2.0f, 5.0f, sweepScales[s]);

                        GREATER(exact.width, 0);
                        GREATER(exact.height, 0);

                        // The bounding box of a rotation is rounded up while the exact angle
                        // measures its own size, so the two can differ by a pixel in size
                        // however well their pivots agree. They may not differ by more.
                        LESSER(pntr_test_rectangle_distance(before, exact), 2);
                        LESSER(pntr_test_rectangle_distance(exact, after), 2);
                        LESSER(pntr_test_rectangle_distance(before, after), 2);
                    }
                }

                pntr_unload_image(sweepSource);
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

    if (pntr_utf8() && pntr_file_io()) {
        IT("PNTR_ENABLE_UTF8", {
            pntr_font* font = pntr_load_font_ttf("resources/tuffy.ttf", 38);
            NEQUALS(font, NULL);

            // Generate the image displaying UTF-8 text.
            const char* text = "Добрий день!";
            pntr_image* image = pntr_gen_image_text(font, text, PNTR_BLACK, PNTR_WHITE);
            NEQUALS(image, NULL);

            if (pntr_image_saving()) {
                EQUALS(pntr_save_image(image, "pntr_test_utf8.png"), true);
            }
            else {
                EQUALS(pntr_save_image(image, "pntr_test_utf8.png"), false);
                EQUALS(pntr_get_error_code(), PNTR_ERROR_NOT_SUPPORTED);
                pntr_set_error(PNTR_ERROR_NONE);
            }

            EQUALS(image->width, 190);
            EQUALS(image->height, 37);

            pntr_unload_font(font);
            pntr_unload_image(image);
        });
    }
    else {
        IT("PNTR_ENABLE_UTF8: without a font to render it with", {
            if (!pntr_utf8()) {
                // There is no multi-byte text to render without UTF-8 support.
                SKIP();
            }

            // UTF-8 is there, but the TrueType font it is rendered with can't be
            // reached, whose refusal is pinned by
            // "pntr_load_font_ttf(): refuses without a file".
            SKIP();
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
