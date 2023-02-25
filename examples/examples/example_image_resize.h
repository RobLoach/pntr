#include "../../pntr.h"

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
    if (size >= 2.0f) {
        speed = -0.02f;
    }
    else if (size <= 0.2f) {
        speed = 0.02f;
    }

    // Resize the image
    pntr_image* nearestNeighbor = pntr_image_resize(originalImage, originalImage->width * size, originalImage->height * size, PNTR_FILTER_NEARESTNEIGHBOR);
    pntr_image* smooth = pntr_image_resize(originalImage, originalImage->width * size, originalImage->height * size, PNTR_FILTER_DEFAULT);

    // Draw an image on the canvas
    pntr_draw_image(canvas, nearestNeighbor, canvas->width / 4 - nearestNeighbor->width / 2, canvas->height / 2 - nearestNeighbor->height / 2);
    pntr_draw_image(canvas, smooth, canvas->width / 4 - smooth->width / 2 + canvas->width / 2, canvas->height / 2 - smooth->height / 2);

    // Unload the resized image
    pntr_unload_image(nearestNeighbor);
    pntr_unload_image(smooth);

    return "Image Resize";
}

void example_image_resize_unload() {
    pntr_unload_image(originalImage);
}
