pntr_image* welcomeLogo;
pntr_font* welcomeFont;

void example_welcome_init() {
    // Load an image
    welcomeLogo = pntr_load_image("resources/logo-128x128.png");

    // Load the default font
    welcomeFont = pntr_load_font_default();
}

const char* example_welcome_update(pntr_app* app, pntr_image* canvas) {
    if (welcomeLogo == NULL) {
        return "Failed to load resources/logo-128x128.png";
    }

    // Rectangle
    pntr_draw_rectangle_fill(canvas, 10, 22, canvas->width - 20, 2, PNTR_DARKBROWN);

    // Welcome text
    pntr_draw_text(canvas, welcomeFont, "Image manipulation library for C", 10, 38, PNTR_BLACK);
    pntr_draw_text(canvas, welcomeFont, "https://github.com/robloach/pntr", 10, 56, PNTR_BLACK);

    const char* buttonText = "See examples";
    int width = pntr_measure_text(welcomeFont, buttonText);
    int x = 10;
    int y = 130;
    int padding = 2;
    int height = 8;
    pntr_draw_rectangle_fill(canvas, x, y, width + padding * 4, height + padding * 4, PNTR_BLACK);
    pntr_draw_rectangle_fill(canvas, x + padding, y + padding, width + padding * 2, height + padding * 2, PNTR_GREEN);
    pntr_draw_text(canvas, welcomeFont, buttonText, x + padding * 2, y + padding * 2, PNTR_BLACK);

    // Display the logo
    pntr_draw_image(canvas, welcomeLogo, canvas->width - welcomeLogo->width, 10);

    return "pntr";
}

void example_welcome_unload() {
    pntr_unload_image(welcomeLogo);
    pntr_unload_font(welcomeFont);
}
