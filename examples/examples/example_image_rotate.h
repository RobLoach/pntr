#include "../../pntr.h"

pntr_image* imageToRotate;
float rotation = 0.0f;

pntr_image* something;

void example_image_rotate_init() {
    // Load an image
    imageToRotate = pntr_load_image("resources/logo-128x128.png");
    something = pntr_load_image("resources/scarfy.png");
}

const char* example_image_rotate_update(pntr_image* canvas) {
    if (imageToRotate == NULL) {
        return "Failed to load resources/logo-128x128.png";
    }

    rotation += 0.001f;

    // Resize the image
    pntr_image* notSmooth = pntr_image_rotate(imageToRotate, rotation, PNTR_FILTER_NEARESTNEIGHBOR);
    pntr_image* rotatedImage = pntr_image_rotate(imageToRotate, rotation, PNTR_FILTER_BILINEAR);
    if (rotatedImage != NULL) {

        // Draw an image on the canvas
        pntr_draw_image(canvas, notSmooth, canvas->width / 4 - notSmooth->width / 2, canvas->height / 2 - notSmooth->height / 2);
        pntr_draw_image(canvas, rotatedImage, canvas->width / 4 - rotatedImage->width / 2 + canvas->width / 2, canvas->height / 2 - rotatedImage->height / 2);

        // Unload the resized images
        pntr_unload_image(notSmooth);
        pntr_unload_image(rotatedImage);
    }

    // Simply draw on the screen
    pntr_draw_image_rotated(canvas, imageToRotate, canvas->width / 2, canvas->height / 2, rotation * 2, imageToRotate->width / 2.0f, imageToRotate->height / 2.0f, PNTR_FILTER_BILINEAR);


    float scalex = 2.0f;
    float scaley = 1.0f;
    pntr_rectangle rect;
    rect.y = 0;
    rect.width = something->width / 6;
    rect.height = something->height;
    rect.x = rect.width;
    pntr_draw_image_rec_ex(canvas, something, rect,
        canvas->width / 2, canvas->height / 2,
        0.4f,
        scalex, scaley,
        rect.width / 2, rect.height / 2,
        false, false,
        PNTR_FILTER_NEARESTNEIGHBOR);

    return "Image Rotate";
}

void example_image_rotate_unload() {
    pntr_unload_image(imageToRotate);
    pntr_unload_image(something);
}
