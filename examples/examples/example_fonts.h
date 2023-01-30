#include "../../pntr.h"

pntr_font* defaultFont;
pntr_font* ttyFont;
pntr_font* bmFont;

void example_fonts_init() {
    // Default Font
    defaultFont = pntr_load_default_font();

    // TTY Font
    ttyFont = pntr_load_ttyfont("resources/font-c64-16x16.png", 16, 16,
        "\x7f !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");

    // BMFont
    bmFont = pntr_load_bmfont("resources/font.png", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/");
}

const char* example_fonts_update(pntr_image* canvas) {
    // Font Drawing
    pntr_draw_text(canvas, defaultFont, "Congrats! You created a pntr app.", 80, 50);

    // TTY Font styling
    pntr_color background = pntr_get_color(0x0000caff);
    pntr_color border = pntr_get_color(0x8a8affff);
    pntr_draw_rectangle(canvas, 0, 110, canvas->width, 60, border);
    pntr_draw_rectangle(canvas, 0, 120, canvas->width, 40, background);

    const char* text = "*** Welcome to pntr ***";
    int textWidth = pntr_measure_text(ttyFont, text);
    pntr_draw_text(canvas, ttyFont, text, canvas->width / 2 - textWidth / 2, 130);

    // BMFont
    pntr_draw_rectangle(canvas, 0, 180, 200, bmFont->atlas->height, PNTR_BLACK);
    pntr_draw_text(canvas, bmFont, "Thank you for coming", 20, 180);

    return "Fonts";
}

void example_fonts_unload() {
    pntr_unload_font(defaultFont);
    pntr_unload_font(ttyFont);
    pntr_unload_font(bmFont);
}
