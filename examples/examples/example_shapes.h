#include "../../pntr.h"

void example_shapes_init() {
    // Nothing
}

const char* example_shapes_update(pntr_image* canvas) {
    // Define some colors
    pntr_color lightGreen = PNTR_GREEN;
    lightGreen.a = 180;
    pntr_color lightBlue = PNTR_BLUE;
    lightBlue.a = 180;

    // Rectangles
    pntr_draw_rectangle_fill(canvas, 10, 30, 50, 50, PNTR_RED);
    pntr_draw_rectangle_fill(canvas, 20, 40, 50, 50, lightGreen);
    pntr_draw_rectangle_fill(canvas, 30, 50, 50, 50, lightBlue);

    pntr_draw_rectangle(canvas, 10, 120, 50, 50, 5, PNTR_RED);
    pntr_draw_rectangle(canvas, 20, 130, 50, 50, 5, PNTR_GREEN);
    pntr_draw_rectangle(canvas, 30, 140, 50, 50, 5, PNTR_BLUE);

    // Circle
    pntr_draw_circle(canvas, 110, 60, 20, PNTR_RED);
    pntr_draw_circle_fill(canvas, 160, 60, 20, PNTR_BLUE);

    // Line
    pntr_draw_line(canvas, 200, 50, 250, 80, PNTR_DARKGREEN);

    return "Shapes";
}

void example_shapes_unload() {
    // Nothing
}
