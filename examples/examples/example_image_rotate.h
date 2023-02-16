#include "../../pntr.h"

pntr_image* imageToRotate;
float rotation = 0.0f;

void example_image_rotate_init() {
    // Load an image
    imageToRotate = pntr_load_image("resources/image.png");
}

const char* example_image_rotate_update(pntr_image* canvas) {
    if (imageToRotate == NULL) {
        return pntr_get_error();
    }

    rotation += 0.001f;

    // Resize the image
    pntr_image* rotatedImage = pntr_image_rotate(imageToRotate, rotation);
    if (rotatedImage != NULL) {

        // Draw an image on the canvas
        pntr_draw_rectangle(canvas, canvas->width / 2 - rotatedImage->width / 2 - 1, canvas->height / 2 - rotatedImage->height / 2 - 1, rotatedImage->width + 1, rotatedImage->height + 1, PNTR_BLACK);
        pntr_draw_image(canvas, rotatedImage, canvas->width / 2 - rotatedImage->width / 2, canvas->height / 2 - rotatedImage->height / 2);

        // Unload the resized image
        pntr_unload_image(rotatedImage);
    }

    return "Image Rotate";
}

void example_image_rotate_unload() {
    pntr_unload_image(imageToRotate);
}
