pntr_image* output;

void example_image_alphamask_init() {
    // Load the Tuffy font, and find the size of the text output.
    pntr_font* font = pntr_load_font_ttf("resources/tuffy.ttf", 80);
    pntr_vector textSize = pntr_measure_text_ex(font, "Alpha Mask!", 0);

    // Create a background to use as the alphamask.
    output = pntr_gen_image_gradient(textSize.x, textSize.y, PNTR_RED, PNTR_DARKGREEN, PNTR_BLUE, PNTR_PURPLE);

    // Create an alpha mask of text
    pntr_image* imageText = pntr_gen_image_text(font, "Alpha Mask!", PNTR_WHITE);

    // Apply the alpha mask to the image.
    pntr_image_alpha_mask(output, imageText, 0, 0);

    // Clean up
    pntr_unload_font(font);
    pntr_unload_image(imageText);
}

const char* example_image_alphamask_update(pntr_image* canvas) {
    pntr_draw_image(canvas, output, 10, 50);

    return "Image Alpha Mask";
}

void example_image_alphamask_unload() {
    pntr_unload_image(output);
}
