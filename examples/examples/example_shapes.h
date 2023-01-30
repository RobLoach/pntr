#include "../../pntr.h"

void example_shapes_init() {
    // Nothing
}

const char* example_shapes_update(pntr_image* canvas) {

    pntr_draw_rectangle(canvas, 60, 60, 50, 50, PNTR_RED);
    pntr_draw_circle(canvas, 220, 150, 50, PNTR_BLUE);
    pntr_draw_pixel(canvas, 50, 50, PNTR_PURPLE);

    return "Shapes";
}

void example_shapes_unload() {
    // Nothing
}
