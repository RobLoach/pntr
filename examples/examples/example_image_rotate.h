pntr_image* imageToRotate;
float rotation = 0.0f;

void example_image_rotate_init() {
    // Load an image
    imageToRotate = pntr_load_image("resources/logo-128x128.png");
}

const char* example_image_rotate_update(pntr_image* canvas) {
    if (imageToRotate == NULL) {
        return "Failed to load resources/logo-128x128.png";
    }

    rotation += 0.5f;

    // Draw the rotated image on the screen.
    pntr_draw_image_rotated(canvas, imageToRotate, canvas->width / 2, canvas->height / 2, rotation, imageToRotate->width / 2.0f, imageToRotate->height / 2.0f, PNTR_FILTER_BILINEAR);

    // Draw the image rotated on screen using a nearest neighbor filter.
    pntr_draw_image_rotated(canvas, imageToRotate, canvas->width / 4, canvas->height / 2, rotation * 4, imageToRotate->width / 2.0f, imageToRotate->height / 2.0f, PNTR_FILTER_NEARESTNEIGHBOR);

    // Draw the rotated image on the screen with a smooth filter.
    pntr_draw_image_rotated(canvas, imageToRotate, canvas->width / 2 + canvas->width / 4, canvas->height / 2, rotation * 8, imageToRotate->width / 2.0f, imageToRotate->height / 2.0f, PNTR_FILTER_SMOOTH);

    return "Image Rotate";
}

void example_image_rotate_unload() {
    pntr_unload_image(imageToRotate);
}
