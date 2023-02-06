#include "../../pntr.h"

pntr_image* image;
pntr_image* resized;

void example_images_init() {
    // Load an image
    image = pntr_load_image("resources/image.png");

    if (image == NULL) {
        printf("resources/image.png not loaded!\n");
        return;
    }

    // Resize the image
    resized = pntr_image_resize(image, image->width / 2, image->height / 2, PNTR_FILTER_NEARESTNEIGHBOR);
}

const char* example_images_update(pntr_image* canvas) {
    // Draw an image on the canvas
    pntr_draw_image(canvas, image, 50, 50);

    // Draw a section of an image
    pntr_rectangle rect;
    rect.x = 25;
    rect.y = 16;
    rect.width = 65;
    rect.height = 58;
    pntr_draw_image_rec(canvas, image, rect, 200, 50);

    // Get a pixel color from an image
    pntr_color faceColor = pntr_image_get_color(canvas, 150, 80);
    pntr_draw_rectangle(canvas, 290, 50, 65, 58, faceColor);

    // Draw the resized image
    pntr_draw_image(canvas, image, 200, 130);

    return "Images";
}

void example_images_unload() {
    pntr_unload_image(image);
    pntr_unload_image(resized);
}
