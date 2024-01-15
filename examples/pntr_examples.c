#define PNTR_ENABLE_DEFAULT_FONT
#define PNTR_ENABLE_TTF
#define PNTR_ENABLE_VARGS
#define PNTR_ENABLE_MATH
#define PNTR_IMPLEMENTATION
#include "../pntr.h"

#include "pntr_examples_alphamask.h"
#include "pntr_examples_fonts.h"
#include "pntr_examples_image.h"
#include "pntr_examples_resize.h"
#include "pntr_examples_rotate.h"
#include "pntr_examples_shapes.h"
#include "pntr_examples_sprite.h"

int main(int argc, char* argv[]) {

    pntr_examples_alphamask();
    pntr_examples_fonts();
    pntr_examples_image();
    pntr_examples_resize();
    pntr_examples_rotate();
    pntr_examples_shapes();
    pntr_examples_sprite();

    return 0;
}
