#include "../../pntr.h"

pntr_image* imageToAlphaMask;
pntr_image* alphaMaskOriginalImage;

void example_image_alphamask_init() {
    // Load an image
    imageToAlphaMask = pntr_load_image("resources/logo-128x128.png");
    alphaMaskOriginalImage = pntr_image_copy(imageToAlphaMask);

    // Create an alpha mask of text
    pntr_font* font = pntr_load_ttffont("resources/tuffy.ttf", 23, PNTR_BLACK);
    pntr_image* alphaMask = pntr_gen_image_text(font, "Alpha Mask!");

    // Apply the alpha mask to the image.
    pntr_image_alpha_mask(imageToAlphaMask, alphaMask, 10, 50);

    // Crop a section of the image that we arn't using
    pntr_image_crop(imageToAlphaMask, 10, 50, alphaMask->width, alphaMask->height);

    // Clean up
    pntr_unload_font(font);
    pntr_unload_image(alphaMask);

}

const char* example_image_alphamask_update(pntr_image* canvas) {
    pntr_draw_image(canvas, imageToAlphaMask, 100, 50);
    pntr_draw_image(canvas, alphaMaskOriginalImage, 200, 50);

    return "Image Alpha Mask";
}

void example_image_alphamask_unload() {
    pntr_unload_image(imageToAlphaMask);
    pntr_unload_image(alphaMaskOriginalImage);
}
