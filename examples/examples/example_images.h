#include "../../pntr.h"

pntr_image* image;

void example_images_init() {
    image = pntr_load_image("resources/image.png");
}

const char* example_images_update(pntr_image* canvas) {

    pntr_draw_image(canvas, image, 100, 50);

    pntr_rectangle rect;
    rect.x = 25;
    rect.y = 16;
    rect.width = 65;
    rect.height = 58;
    pntr_draw_image_rec(canvas, image, rect, 250, 50);

    return "Images";
}

void example_images_unload() {
    pntr_unload_image(image);
}
