#include "../../pntr.h"

pntr_font* defaultFont;
pntr_font* bmFont;
pntr_font* ttyFont;
pntr_font* bdfFont;

#include <stdio.h>

void example_fonts_init() {
    // Default Font
    defaultFont = pntr_load_default_font();

    // BM Font
    bmFont = pntr_load_bmfont("resources/bmfont.png", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/");

    // TTY Font
    ttyFont = pntr_load_ttyfont("resources/ttyfont-16x16.png", 16, 16,
        "\x7f !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");

    // BDF Font
    bdfFont = pntr_load_bdffont("resources/bdfont.bdf", PNTR_BLACK);
    printf("%s\n", pntr_get_error());
}

const char* example_fonts_update(pntr_image* canvas) {
    // Only display the fonts if they loaded correctly.
    if (defaultFont == NULL || bmFont == NULL || ttyFont == NULL || bdfFont == NULL) {
        return "Fonts - Failed to load fonts";
    }

    // Font Drawing
    pntr_draw_text(canvas, defaultFont, "Default Font Example", 20, 50);

    // BMFont
    pntr_draw_rectangle(canvas, 0, 80, 200, bmFont->atlas->height, PNTR_BLACK);
    pntr_draw_text(canvas, bmFont, "BM Font Example", 20, 80);

    // TTY Font styling
    pntr_color background = pntr_get_color(0x0000caff);
    pntr_color border = pntr_get_color(0x8a8affff);
    pntr_draw_rectangle(canvas, 0, 120, canvas->width, 60, border);
    pntr_draw_rectangle(canvas, 0, 130, canvas->width, 40, background);

    const char* text = "*** TTY Font Example ***";
    int textWidth = pntr_measure_text(ttyFont, text);
    pntr_draw_text(canvas, ttyFont, text, canvas->width / 2 - textWidth / 2, 140);

    pntr_draw_text(canvas, bdfFont, "Hello World!", 50, 10);

    return "Fonts";
}

void example_fonts_unload() {
    pntr_unload_font(defaultFont);
    pntr_unload_font(ttyFont);
    pntr_unload_font(bmFont);
}
