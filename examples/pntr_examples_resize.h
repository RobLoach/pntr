#include "../pntr.h"

const char* pntr_examples_resize() {
    float size = 1.0f;
    float speed = 0.02f;
    pntr_image* originalImage = pntr_load_image("resources/logo-128x128.png");
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

    pntr_image* canvas = pntr_gen_image_color(400, 225, PNTR_RAYWHITE);

    pntr_draw_image_scaled(canvas, originalImage,
        canvas->width / 4 - originalImage->width * size / 2.0f,
        canvas->height / 2 - originalImage->height * size / 2.0f,
        size, size, 0.0f, 0.0f, PNTR_FILTER_NEARESTNEIGHBOR);

    pntr_draw_image_scaled(canvas, originalImage,
        canvas->width / 2 + canvas->width / 4 - originalImage->width * size / 2.0f,
        canvas->height / 2 - originalImage->height * size / 2.0f,
        size, size, 0.0f, 0.0f, PNTR_FILTER_BILINEAR);

    pntr_save_image(canvas, "pntr_examples_resize.png");

    pntr_unload_image(originalImage);
    pntr_unload_image(canvas);

    return true;
}
