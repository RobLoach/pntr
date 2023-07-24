pntr_image* originalImage;
float size = 1.0f;
float speed = 0.02f;

void example_image_resize_init() {
    // Load an image
    originalImage = pntr_load_image("resources/logo-128x128.png");
}

const char* example_image_resize_update(pntr_image* canvas) {
    if (originalImage == NULL) {
        return "Failed to load resources/logo-128x128.png";
    }

    size += speed;
    if (size >= 1.5f) {
        speed = -0.015f;
    }
    else if (size <= 0.2f) {
        speed = 0.015f;
    }

    pntr_draw_image_scaled(canvas, originalImage,
        canvas->width / 4 - originalImage->width * size / 2.0f,
        canvas->height / 2 - originalImage->height * size / 2.0f,
        size, size, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR);

    pntr_draw_image_scaled(canvas, originalImage,
        canvas->width / 2 + canvas->width / 4 - originalImage->width * size / 2.0f,
        canvas->height / 2 - originalImage->height * size / 2.0f,
        size, size, 0.0f, 0.0f, PNTR_FILTER_SMOOTH);

    return "Image Resize";
}

void example_image_resize_unload() {
    pntr_unload_image(originalImage);
}
