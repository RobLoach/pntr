#include "../../pntr.h"

void example_shapes_init() {
    // Nothing
}

const char* example_shapes_update(pntr_image* canvas) {
    // Rectangle
    pntr_draw_rectangle(canvas, 60, 60, 50, 50, PNTR_RED);

    pntr_color lightGreen = PNTR_GREEN;
    lightGreen.a = 180;
    pntr_draw_rectangle(canvas, 70, 70, 50, 50, lightGreen);

    pntr_color lightBlue = PNTR_BLUE;
    lightBlue.a = 180;
    pntr_draw_rectangle(canvas, 80, 80, 50, 50, lightBlue);

    // Circle
    pntr_draw_circle(canvas, 220, 150, 50, PNTR_BLUE);

    // Pixel
    pntr_draw_pixel(canvas, 50, 50, PNTR_PURPLE);

    // Line
    pntr_draw_line(canvas, 200, 50, 250, 80, PNTR_DARKGREEN);

    return "Shapes";
}

void example_shapes_unload() {
    // Nothing
}
