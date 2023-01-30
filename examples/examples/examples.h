#include "../../pntr.h"

#include "example_shapes.h"
#include "example_fonts.h"
#include "example_images.h"

int examples_count() {
    return 3;
}

void examples_init() {
    example_shapes_init();
    example_fonts_init();
    example_images_init();
}

const char* examples_update(int i, pntr_image* canvas) {
    switch (i) {
        case 0:
            return example_shapes_update(canvas);
        case 1:
            return example_images_update(canvas);
        case 2:
            return example_fonts_update(canvas);
    }
}

void examples_unload() {
    example_shapes_unload();
    example_fonts_unload();
    example_images_unload();
}
