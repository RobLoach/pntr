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
    pntr_draw_circle(canvas, 110, 60, 21, PNTR_RED);
    pntr_draw_circle_fill(canvas, 160, 60, 21, PNTR_BLUE);

    pntr_draw_ellipse(canvas, 110, 100, 20, 10, PNTR_GREEN);
    pntr_draw_ellipse_fill(canvas, 160, 100, 20, 10, PNTR_ORANGE);

    // Line
    pntr_draw_line(canvas, 200, 50, 250, 80, PNTR_DARKGREEN);

    // Triangle
    pntr_draw_triangle_fill(canvas,
        250, 50,
        300, 80,
        350, 20, PNTR_PURPLE);

    // Rectangle Gradient
    pntr_draw_rectangle_gradient(canvas, 100, 120, 80, 80, PNTR_RED, PNTR_GREEN, PNTR_BLUE, PNTR_BLACK);

    // Polygon
    pntr_vector points[10];
    points[0] = PNTR_CLITERAL(pntr_vector) {210, 110};
    points[1] = PNTR_CLITERAL(pntr_vector) {215, 130};
    points[2] = PNTR_CLITERAL(pntr_vector) {240, 140};
    points[3] = PNTR_CLITERAL(pntr_vector) {200, 160};
    pntr_draw_polygon_fill(canvas, points, 4, PNTR_BLUE);
    pntr_draw_polygon(canvas, points, 4, PNTR_BLACK);

    // Arc
    int radius = 40;
    pntr_draw_arc_fill(canvas, 300, 120, radius, 90.0f, 180.0f, radius * 1.5f, PNTR_ORANGE);
    pntr_draw_arc(canvas, 300, 120, radius, 90.0f, 180.0f, radius *1.5f, PNTR_RED);

    // Polyline
    points[0] = PNTR_CLITERAL(pntr_vector) {240, 110};
    points[1] = PNTR_CLITERAL(pntr_vector) {260, 130};
    points[2] = PNTR_CLITERAL(pntr_vector) {220, 140};
    points[3] = PNTR_CLITERAL(pntr_vector) {240, 160};
    pntr_draw_polyline(canvas, points, 4, PNTR_PURPLE);

    return "Shapes";
}

void example_shapes_unload() {
    // Nothing
}
