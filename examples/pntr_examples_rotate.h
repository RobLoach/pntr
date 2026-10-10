void pntr_examples_rotate() {
    pntr_image* canvas = pntr_gen_image_color(400, 225, PNTR_RAYWHITE);

    pntr_image* imageToRotate = pntr_load_image("resources/logo-128x128.png");

    // The offset is a pivot, in unrotated source pixels: the source pixel there is placed on
    // the position given. Naming the center of the image spins it in place, so nothing but
    // the angle has to change from one of these draws to the next.
    float originX = imageToRotate->width / 2.0f;
    float originY = imageToRotate->height / 2.0f;

    // Draw the image rotated on screen using a nearest neighbor filter.
    pntr_draw_image_rotated(canvas, imageToRotate, 68, canvas->height / 2, 0.0f, originX, originY, PNTR_FILTER_NEARESTNEIGHBOR);

    // Draw the rotated image on the screen with a smooth filter.
    pntr_draw_image_rotated(canvas, imageToRotate, 200, canvas->height / 2, 32.0f, originX, originY, PNTR_FILTER_BILINEAR);

    // Draw the rotated image on the screen with a smooth filter.
    pntr_draw_image_rotated(canvas, imageToRotate, 332, canvas->height / 2, 180.0f, originX, originY, PNTR_FILTER_BILINEAR);

    pntr_save_image(canvas, "pntr_examples_rotate.png");

    pntr_unload_image(imageToRotate);
    pntr_unload_image(canvas);
}
