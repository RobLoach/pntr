#include "../../pntr.h"

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

    rotation += 0.001f;

    // Resize the image
    pntr_image* notSmooth = pntr_image_rotate(imageToRotate, rotation, PNTR_FILTER_NEARESTNEIGHBOR);
    pntr_image* rotatedImage = pntr_image_rotate(imageToRotate, rotation, PNTR_FILTER_DEFAULT);
    if (rotatedImage != NULL) {

        // Draw an image on the canvas
        pntr_draw_image(canvas, notSmooth, canvas->width / 4 - notSmooth->width / 2, canvas->height / 2 - notSmooth->height / 2);
        pntr_draw_image(canvas, rotatedImage, canvas->width / 4 - rotatedImage->width / 2 + canvas->width / 2, canvas->height / 2 - rotatedImage->height / 2);

        // Unload the resized images
        pntr_unload_image(notSmooth);
        pntr_unload_image(rotatedImage);
    }

    // Simply draw on the screen
    pntr_draw_image_rotate(canvas, imageToRotate, canvas->width / 2, canvas->height / 2, rotation, imageToRotate->width / 2.0f, imageToRotate->height / 2.0f, PNTR_FILTER_BILINEAR);

    return "Image Rotate";
}

void example_image_rotate_unload() {
    pntr_unload_image(imageToRotate);
}
