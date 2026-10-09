/**
 * pntr_assert: Assertion library for pntr.
 *
 * Configuration:
 * - PNTR_ASSERT: Callback used to assert a condition. By default, will use assert() from assert.h.
 * - PNTR_ASSERTF: Callback used to assert a condition, reporting a formatted message when it fails.
 * - PNTR_ASSERT_PRINTF: Callback used to report assertion failures. By default, will use fprintf() to stderr from stdio.h.
 * - PNTR_NO_ASSERT: When enabled, will compile every assertion in this file down to a no-op.
 * - PNTR_NO_STDIO: When enabled, will not use stdio.h, so failures are reported without their formatted message.
 * - PNTR_ASSERT_VARARGS: Define as 1 or 0 to state whether or not variadic macros are available. Detected automatically.
 *
 * @file pntr_assert.h
 *
 * @copyright 2026 Rob Loach (@RobLoach, https://robloach.net)
 * @license Zlib
 *
 * Copyright (c) 2026 Rob Loach (@RobLoach, https://robloach.net)
 *
 * This software is provided "as-is", without any express or implied warranty. In no event
 * will the authors be held liable for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose, including commercial
 * applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented; you must not claim that you
 *   wrote the original software. If you use this software in a product, an acknowledgment
 *   in the product documentation would be appreciated but is not required.
 *
 *   2. Altered source versions must be plainly marked as such, and must not be misrepresented
 *   as being the original software.
 *
 *   3. This notice may not be removed or altered from any source distribution.
 */
#ifndef PNTR_ASSERT_H__
#define PNTR_ASSERT_H__

/**
 * @defgroup pntr_assert pntr_assert
 * @{
 *
 * @brief Assertion library for pntr.
 */

/**
 * When defined, every assertion in pntr_assert.h is compiled down to a no-op.
 *
 * @details This is enabled by PNTR_NO_ASSERT, or by NDEBUG when you have not provided
 * your own PNTR_ASSERT(). Providing your own PNTR_ASSERT() keeps the assertions active
 * under NDEBUG, as your callback decides what a failure means.
 *
 * @see PNTR_NO_ASSERT
 * @see PNTR_ASSERT
 */
#if !defined(PNTR_ASSERT_DISABLED) && (defined(PNTR_NO_ASSERT) || (defined(NDEBUG) && !defined(PNTR_ASSERT)))
#define PNTR_ASSERT_DISABLED
#endif  // PNTR_ASSERT_DISABLED

#ifndef PNTR_ASSERT_VOID_CAST
/**
 * Casts the given expression to void.
 *
 * @param expression The expression to discard.
 */
#ifdef __cplusplus
#define PNTR_ASSERT_VOID_CAST(expression) static_cast<void>(expression)
#else
#define PNTR_ASSERT_VOID_CAST(expression) (void)(expression)
#endif
#endif  // PNTR_ASSERT_VOID_CAST

#ifndef PNTR_ASSERT_VARARGS
/**
 * Whether or not variadic macros, like PNTR_ASSERTF(), are available.
 *
 * @details Defined as 1 when variadic macros can be used, 0 otherwise. Define it
 * yourself to override the detection.
 *
 * @see PNTR_ASSERTF()
 */
#if defined(__cplusplus) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (defined(_MSC_VER) && _MSC_VER >= 1400)
#define PNTR_ASSERT_VARARGS 1
#else
#define PNTR_ASSERT_VARARGS 0
#endif
#endif  // PNTR_ASSERT_VARARGS

#ifndef PNTR_ASSERT_UNUSED
/**
 * Consumes the given expression without evaluating it.
 *
 * @details Used by the disabled assertions so that values that are only referenced by
 * an assertion don't report as unused.
 *
 * @param expression The expression to consume.
 */
#define PNTR_ASSERT_UNUSED(expression) PNTR_ASSERT_VOID_CAST(sizeof((expression)))
#endif  // PNTR_ASSERT_UNUSED

#if PNTR_ASSERT_VARARGS
#ifndef PNTR_ASSERT_UNUSED_ARGS
int pntr_assert_unused_args_(const char* format, ...); /** Never called, nor defined. @private */
/**
 * Consumes a printf-style argument list without evaluating it.
 *
 * @details The arguments only ever appear as the operand of `sizeof`, which is why
 * pntr_assert_unused_args_() never needs to exist.
 *
 * @param ... The format string, along with any of its arguments, to consume.
 */
#define PNTR_ASSERT_UNUSED_ARGS(...) PNTR_ASSERT_VOID_CAST(sizeof(pntr_assert_unused_args_(__VA_ARGS__)))
#endif  // PNTR_ASSERT_UNUSED_ARGS
#endif  // PNTR_ASSERT_VARARGS

#ifndef PNTR_ASSERT_PRINTF
#if PNTR_ASSERT_VARARGS && !defined(PNTR_ASSERT_DISABLED) && !defined(PNTR_NO_STDIO)
#include <stdio.h> // fprintf, stderr
/**
 * Reports an assertion failure message.
 *
 * @details By default, will write to `stderr` with `fprintf()`. When PNTR_NO_STDIO is
 * enabled, this is left undefined and the assertions report their failure without the
 * formatted message.
 *
 * @param ... The format string, along with any of its arguments.
 *
 * @see PNTR_NO_STDIO
 */
#define PNTR_ASSERT_PRINTF(...) fprintf(stderr, __VA_ARGS__)
#endif
#endif  // PNTR_ASSERT_PRINTF

#ifndef PNTR_ASSERT
#ifdef PNTR_ASSERT_DISABLED
#define PNTR_ASSERT(condition) PNTR_ASSERT_UNUSED(condition)
#else
#include <assert.h>
/**
 * Assert whether the given condition is true or not.
 *
 * @details By default, will use `assert()`, but you can override this by defining it to your own assertion call.
 *
 * @param condition Expression of scalar type.
 */
#define PNTR_ASSERT(condition) assert(condition)
#endif
#endif  // PNTR_ASSERT

#if PNTR_ASSERT_VARARGS
#ifndef PNTR_ASSERTF
#if defined(PNTR_ASSERT_DISABLED)
#define PNTR_ASSERTF(condition, ...) do { \
    PNTR_ASSERT_UNUSED(condition); \
    PNTR_ASSERT_UNUSED_ARGS(__VA_ARGS__); \
} while (0)
#elif !defined(PNTR_ASSERT_PRINTF)
#define PNTR_ASSERTF(condition, ...) do { \
    PNTR_ASSERT_UNUSED_ARGS(__VA_ARGS__); \
    PNTR_ASSERT(condition); \
} while (0)
#else
/**
 * Assert whether the given condition is true or not, reporting a formatted message when it is not.
 *
 * @details When the condition fails, the message is reported along with the file and line
 * that it failed on, and then the failure is handed off to PNTR_ASSERT(). Requires variadic
 * macros. When PNTR_NO_STDIO is enabled, the message is skipped and only the condition is
 * asserted.
 *
 * @param condition Expression of scalar type.
 * @param ... The printf-style format string describing the failure, along with any of its arguments.
 *
 * @see PNTR_ASSERT()
 * @see PNTR_ASSERT_PRINTF()
 * @see PNTR_ASSERT_VARARGS
 */
#define PNTR_ASSERTF(condition, ...) do { \
    if (!(condition)) { \
        PNTR_ASSERT_PRINTF("pntr_assert: %s:%d: ", __FILE__, __LINE__); \
        PNTR_ASSERT_PRINTF(__VA_ARGS__); \
        PNTR_ASSERT_PRINTF("\n"); \
    } \
    PNTR_ASSERT(condition); \
} while (0)
#endif
#endif  // PNTR_ASSERTF
#endif  // PNTR_ASSERT_VARARGS

#ifndef PNTR_ASSERT_EQUALS
#ifdef PNTR_ASSERT_DISABLED
#define PNTR_ASSERT_EQUALS(actual, expected) do { \
    PNTR_ASSERT_UNUSED(actual); \
    PNTR_ASSERT_UNUSED(expected); \
} while (0)
#else
/**
 * Evaluates whether or not the given parameters are equal.
 *
 * @param actual The evaluated value to check against.
 * @param expected The expected value.
 */
#define PNTR_ASSERT_EQUALS(actual, expected) PNTR_ASSERT((actual) == (expected))
#endif
#endif  // PNTR_ASSERT_EQUALS

#ifndef PNTR_ASSERT_NEQUALS
#ifdef PNTR_ASSERT_DISABLED
#define PNTR_ASSERT_NEQUALS(actual, expected) do { \
    PNTR_ASSERT_UNUSED(actual); \
    PNTR_ASSERT_UNUSED(expected); \
} while (0)
#else
/**
 * Evaluates whether or not the given parameters are not equal.
 *
 * @param actual The evaluated value to check against.
 * @param expected The expected value that should not be equal.
 */
#define PNTR_ASSERT_NEQUALS(actual, expected) PNTR_ASSERT((actual) != (expected))
#endif
#endif  // PNTR_ASSERT_NEQUALS

#ifndef PNTR_ASSERT_FIELD_EQUALS
#if PNTR_ASSERT_VARARGS
/**
 * Evaluates whether or not the two given integer fields are equal, reporting which field
 * differed, and both of its values, when they are not.
 *
 * @details The values are evaluated more than once, so give it plain values rather than
 * expressions that have side effects.
 *
 * @param name The name of the field that is being compared, as a string.
 * @param actual The evaluated value to check against.
 * @param expected The expected value.
 *
 * @see PNTR_ASSERTF()
 */
#define PNTR_ASSERT_FIELD_EQUALS(name, actual, expected) \
    PNTR_ASSERTF((actual) == (expected), "%s: actual %d, expected %d", (name), (int)(actual), (int)(expected))
#else
#define PNTR_ASSERT_FIELD_EQUALS(name, actual, expected) do { \
    PNTR_ASSERT_UNUSED(name); \
    PNTR_ASSERT_EQUALS(actual, expected); \
} while (0)
#endif
#endif  // PNTR_ASSERT_FIELD_EQUALS

#ifndef PNTR_ASSERT_COLOR_EQUALS
#ifdef PNTR_ASSERT_DISABLED
#define PNTR_ASSERT_COLOR_EQUALS(actual, expected) do { \
    PNTR_ASSERT_UNUSED(actual); \
    PNTR_ASSERT_UNUSED(expected); \
} while (0)
#else
/**
 * Check whether or not the given colors are the same.
 *
 * @details Reports the first channel that differs, along with both of its values.
 *
 * @param actual The actual color to check.
 * @param expected The color that is expected.
 *
 * @see PNTR_ASSERT_FIELD_EQUALS()
 */
#define PNTR_ASSERT_COLOR_EQUALS(actual, expected) do { \
    pntr_color pntrAssertColorActual = (actual); \
    pntr_color pntrAssertColorExpected = (expected); \
    if (pntrAssertColorActual.rgba.r != pntrAssertColorExpected.rgba.r) { \
        PNTR_ASSERT_FIELD_EQUALS("color red channel", pntrAssertColorActual.rgba.r, pntrAssertColorExpected.rgba.r); \
        break; \
    } \
    if (pntrAssertColorActual.rgba.g != pntrAssertColorExpected.rgba.g) { \
        PNTR_ASSERT_FIELD_EQUALS("color green channel", pntrAssertColorActual.rgba.g, pntrAssertColorExpected.rgba.g); \
        break; \
    } \
    if (pntrAssertColorActual.rgba.b != pntrAssertColorExpected.rgba.b) { \
        PNTR_ASSERT_FIELD_EQUALS("color blue channel", pntrAssertColorActual.rgba.b, pntrAssertColorExpected.rgba.b); \
        break; \
    } \
    PNTR_ASSERT_FIELD_EQUALS("color alpha channel", pntrAssertColorActual.rgba.a, pntrAssertColorExpected.rgba.a); \
} while (0)
#endif
#endif  // PNTR_ASSERT_COLOR_EQUALS

#ifndef PNTR_ASSERT_PIXEL_EQUALS
#if PNTR_ASSERT_VARARGS
/**
 * Check whether or not the given pixel colors are the same, reporting the position that
 * they were found at, along with both of the colors, when they are not.
 *
 * @details The colors are evaluated more than once, so give it plain values rather than
 * expressions that have side effects.
 *
 * @param x The x position that the colors were taken from.
 * @param y The y position that the colors were taken from.
 * @param actual The actual color to check.
 * @param expected The color that is expected.
 *
 * @see PNTR_ASSERTF()
 */
#define PNTR_ASSERT_PIXEL_EQUALS(x, y, actual, expected) \
    PNTR_ASSERTF((actual).value == (expected).value, \
        "images differ at (%d, %d): actual rgba(%d, %d, %d, %d), expected rgba(%d, %d, %d, %d)", \
        (int)(x), (int)(y), \
        (int)(actual).rgba.r, (int)(actual).rgba.g, (int)(actual).rgba.b, (int)(actual).rgba.a, \
        (int)(expected).rgba.r, (int)(expected).rgba.g, (int)(expected).rgba.b, (int)(expected).rgba.a)
#else
#define PNTR_ASSERT_PIXEL_EQUALS(x, y, actual, expected) do { \
    PNTR_ASSERT_UNUSED(x); \
    PNTR_ASSERT_UNUSED(y); \
    PNTR_ASSERT_COLOR_EQUALS(actual, expected); \
} while (0)
#endif
#endif  // PNTR_ASSERT_PIXEL_EQUALS

#ifndef PNTR_ASSERT_IMAGE_EQUALS
#ifdef PNTR_ASSERT_DISABLED
#define PNTR_ASSERT_IMAGE_EQUALS(actual, expected) do { \
    PNTR_ASSERT_UNUSED(actual); \
    PNTR_ASSERT_UNUSED(expected); \
} while (0)
#else
/**
 * Check whether or not the given images are the same.
 *
 * @details Stops at the first pixel that differs, reporting its coordinates along with
 * both of the colors found there.
 *
 * @param actual The image to check.
 * @param expected The image that is expected.
 *
 * @see PNTR_ASSERT_FIELD_EQUALS()
 * @see PNTR_ASSERT_PIXEL_EQUALS()
 */
#define PNTR_ASSERT_IMAGE_EQUALS(actual, expected) do { \
    pntr_image* pntrAssertImageActual = (actual); \
    pntr_image* pntrAssertImageExpected = (expected); \
    int pntrAssertImageX = 0; \
    int pntrAssertImageY = 0; \
    int pntrAssertImageDiffers = 0; \
    PNTR_ASSERT_NEQUALS(pntrAssertImageActual, NULL); \
    PNTR_ASSERT_NEQUALS(pntrAssertImageExpected, NULL); \
    if (pntrAssertImageActual == NULL || pntrAssertImageExpected == NULL) { \
        break; \
    } \
    if (pntrAssertImageActual->width != pntrAssertImageExpected->width) { \
        PNTR_ASSERT_FIELD_EQUALS("image width", pntrAssertImageActual->width, pntrAssertImageExpected->width); \
        break; \
    } \
    if (pntrAssertImageActual->height != pntrAssertImageExpected->height) { \
        PNTR_ASSERT_FIELD_EQUALS("image height", pntrAssertImageActual->height, pntrAssertImageExpected->height); \
        break; \
    } \
    for (pntrAssertImageY = 0; pntrAssertImageY < pntrAssertImageActual->height && !pntrAssertImageDiffers; pntrAssertImageY++) { \
        for (pntrAssertImageX = 0; pntrAssertImageX < pntrAssertImageActual->width; pntrAssertImageX++) { \
            pntr_color pntrAssertPixelActual = pntr_image_get_color(pntrAssertImageActual, pntrAssertImageX, pntrAssertImageY); \
            pntr_color pntrAssertPixelExpected = pntr_image_get_color(pntrAssertImageExpected, pntrAssertImageX, pntrAssertImageY); \
            if (pntrAssertPixelActual.value != pntrAssertPixelExpected.value) { \
                pntrAssertImageDiffers = 1; \
                PNTR_ASSERT_PIXEL_EQUALS(pntrAssertImageX, pntrAssertImageY, pntrAssertPixelActual, pntrAssertPixelExpected); \
                break; \
            } \
        } \
    } \
} while (0)
#endif
#endif  // PNTR_ASSERT_IMAGE_EQUALS

#ifndef PNTR_ASSERT_RECT_EQUALS
#ifdef PNTR_ASSERT_DISABLED
#define PNTR_ASSERT_RECT_EQUALS(actual, expected) do { \
    PNTR_ASSERT_UNUSED(actual); \
    PNTR_ASSERT_UNUSED(expected); \
} while (0)
#else
/**
 * Validate whether or not the given rectangles are equal.
 *
 * @details Reports the first field that differs, along with both of its values.
 *
 * @param actual The rectangle to check.
 * @param expected The rectangle that is expected.
 *
 * @see PNTR_ASSERT_FIELD_EQUALS()
 */
#define PNTR_ASSERT_RECT_EQUALS(actual, expected) do { \
    pntr_rectangle pntrAssertRectActual = (actual); \
    pntr_rectangle pntrAssertRectExpected = (expected); \
    if (pntrAssertRectActual.x != pntrAssertRectExpected.x) { \
        PNTR_ASSERT_FIELD_EQUALS("rectangle x", pntrAssertRectActual.x, pntrAssertRectExpected.x); \
        break; \
    } \
    if (pntrAssertRectActual.y != pntrAssertRectExpected.y) { \
        PNTR_ASSERT_FIELD_EQUALS("rectangle y", pntrAssertRectActual.y, pntrAssertRectExpected.y); \
        break; \
    } \
    if (pntrAssertRectActual.width != pntrAssertRectExpected.width) { \
        PNTR_ASSERT_FIELD_EQUALS("rectangle width", pntrAssertRectActual.width, pntrAssertRectExpected.width); \
        break; \
    } \
    PNTR_ASSERT_FIELD_EQUALS("rectangle height", pntrAssertRectActual.height, pntrAssertRectExpected.height); \
} while (0)
#endif
#endif  // PNTR_ASSERT_RECT_EQUALS

#ifndef PNTR_ASSERT_VECTOR_EQUALS
#ifdef PNTR_ASSERT_DISABLED
#define PNTR_ASSERT_VECTOR_EQUALS(actual, expected) do { \
    PNTR_ASSERT_UNUSED(actual); \
    PNTR_ASSERT_UNUSED(expected); \
} while (0)
#else
/**
 * Validate whether or not the given vectors are equal.
 *
 * @details Reports the first field that differs, along with both of its values.
 *
 * @param actual The vector to check.
 * @param expected The vector that is expected.
 *
 * @see PNTR_ASSERT_FIELD_EQUALS()
 */
#define PNTR_ASSERT_VECTOR_EQUALS(actual, expected) do { \
    pntr_vector pntrAssertVectorActual = (actual); \
    pntr_vector pntrAssertVectorExpected = (expected); \
    if (pntrAssertVectorActual.x != pntrAssertVectorExpected.x) { \
        PNTR_ASSERT_FIELD_EQUALS("vector x", pntrAssertVectorActual.x, pntrAssertVectorExpected.x); \
        break; \
    } \
    PNTR_ASSERT_FIELD_EQUALS("vector y", pntrAssertVectorActual.y, pntrAssertVectorExpected.y); \
} while (0)
#endif
#endif  // PNTR_ASSERT_VECTOR_EQUALS

#ifndef pntr_assert
    /**
     * Assert whether the given condition is true or not.
     *
     * @param condition Expression of scalar type.
     *
     * @see PNTR_ASSERT()
     */
    #define pntr_assert(condition) PNTR_ASSERT(condition)
#endif  // pntr_assert

#if PNTR_ASSERT_VARARGS
#ifndef pntr_assertf
    /**
     * Assert whether the given condition is true or not, reporting a formatted message when it is not.
     *
     * @param condition Expression of scalar type.
     * @param ... The printf-style format string describing the failure, along with any of its arguments.
     *
     * @see PNTR_ASSERTF()
     */
    #define pntr_assertf(condition, ...) PNTR_ASSERTF(condition, __VA_ARGS__)
#endif  // pntr_assertf
#endif  // PNTR_ASSERT_VARARGS

/**
 * @}
 */

#endif  // PNTR_ASSERT_H__
