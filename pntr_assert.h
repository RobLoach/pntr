/**
 * pntr_assert: Assertion library for pntr.
 *
 * @file pntr_assert.h
 *
 * @copyright 2023 Rob Loach (@RobLoach, https://robloach.net)
 * @license Zlib
 *
 * Copyright (c) 2023 Rob Loach (@RobLoach, https://robloach.net)
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

#ifndef PNTR_ASSERT
#ifdef NDEBUG
#if defined __cplusplus && __GNUC_PREREQ (2,95)
#define PNTR_ASSERT_VOID_CAST static_cast<void>
#else
#define PNTR_ASSERT_VOID_CAST (void)
#endif
#define PNTR_ASSERT(condition) (PNTR_ASSERT_VOID_CAST 0)
#else
#include <assert.h>
#define PNTR_ASSERT(condition) assert(condition)
#endif
#endif

#ifndef PNTR_ASSERT_EQUALS
/**
 * Evaluates whether or not the given parameters are equal.
 *
 * @param actual The evaluated value to check against.
 * @param expected The expected value.
 */
#define PNTR_ASSERT_EQUALS(actual, expected) PNTR_ASSERT((actual) == (expected))
#endif

#ifndef PNTR_ASSERT_NEQUALS
/**
 * Evaluates whether or not the given parameters are not equal.
 *
 * @param actual The evaluated value to check against.
 * @param expected The expected value that should not be equal.
 */
#define PNTR_ASSERT_NEQUALS(actual, expected) PNTR_ASSERT((actual) != (expected))
#endif

#ifndef PNTR_ASSERT_COLOR_EQUALS
/**
 * Check whether or not the given colors are the same.
 *
 * @param actual The actual color to check.
 * @param expected The color that is expected.
 */
#define PNTR_ASSERT_COLOR_EQUALS(actual, expected) do { \
    pntr_color actualColor = (actual); \
    pntr_color expectedColor = (expected); \
    PNTR_ASSERT_EQUALS(actualColor.r, expectedColor.r); \
    PNTR_ASSERT_EQUALS(actualColor.g, expectedColor.g); \
    PNTR_ASSERT_EQUALS(actualColor.b, expectedColor.b); \
    PNTR_ASSERT_EQUALS(actualColor.a, expectedColor.a); \
} while (0)
#endif

#ifndef PNTR_ASSERT_IMAGE_EQUALS
/**
 * Check whether or not the given images are the same.
 *
 * @param actual The image to check.
 * @param expected The image that is expected.
 */
#define PNTR_ASSERT_IMAGE_EQUALS(actual, expected) do { \
    PNTR_ASSERT_NEQUALS(actual, NULL); \
    PNTR_ASSERT_NEQUALS(expected, NULL); \
    PNTR_ASSERT_EQUALS(actual->width, expected->width); \
    PNTR_ASSERT_EQUALS(actual->height, expected->height); \
    for (int x = 0; x < actual->width; x++) { \
        for (int y = 0; y < actual->width; y++) { \
            PNTR_ASSERT_COLOR_EQUALS(pntr_image_get_color(actual, x, y), pntr_image_get_color(expected, x, y)); \
        } \
    } \
} while (0)
#endif

#endif
