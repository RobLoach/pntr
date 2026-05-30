/**
 * @page Examples Examples
 *
 * @brief These are a few examples of pntr in use.
 *
 * ## Shapes
 * @image html pntr_examples_shapes.png
 * @include pntr_examples_shapes.h
 *
 * ## Image
 * @image html pntr_examples_image.png
 * @include pntr_examples_image.h
 *
 * ## Resize
 * @image html pntr_examples_resize.png
 * @include pntr_examples_resize.h
 *
 * ## Fonts
 * @image html pntr_examples_fonts.png
 * @include pntr_examples_fonts.h
 *
 * ## Alpha Mask
 * @image html pntr_examples_alphamask.png
 * @include pntr_examples_alphamask.h
 *
 * ## Rotate
 * @image html pntr_examples_rotate.png
 * @include pntr_examples_rotate.h
 *
 * ## Sprite
 * @image html pntr_examples_sprite.png
 * @include pntr_examples_sprite.h
 *
 * ## Thick
 * @image html pntr_examples_thick.png
 * @include pntr_examples_thick.h
 */

#ifndef _DOXYGEN_

#define PNTR_ENABLE_DEFAULT_FONT
#define PNTR_ENABLE_TTF
#define PNTR_ENABLE_UTF8
#define PNTR_ENABLE_VARGS
//#define PNTR_CUTE_PNG

// Math: Link the m library to use math.h
//#define PNTR_ENABLE_MATH

#define PNTR_IMPLEMENTATION
#include "../pntr.h"

#include "pntr_examples_alphamask.h"
#include "pntr_examples_fonts.h"
#include "pntr_examples_image.h"
#include "pntr_examples_resize.h"
#include "pntr_examples_rotate.h"
#include "pntr_examples_shapes.h"
#include "pntr_examples_sprite.h"
#include "pntr_examples_thick.h"

int main(int argc, char* argv[]) {
    pntr_examples_alphamask();
    pntr_examples_fonts();
    pntr_examples_image();
    pntr_examples_resize();
    pntr_examples_rotate();
    pntr_examples_shapes();
    pntr_examples_sprite();
    pntr_examples_thick();

    return 0;
}

#endif
