#include "../../pntr.h"

pntr_font* defaultFont;
pntr_font* bmFont;
pntr_font* ttyFont;
pntr_font* ttfFont;

#include <stdio.h>

void example_fonts_init() {
    // Default Font
    defaultFont = pntr_load_default_font();

    // BM Font
    bmFont = pntr_load_bmfont("resources/bmfont.png", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/");
    if (bmFont == NULL) {
        printf("resources/bmfont.png not loaded!\n");
    }

    // TTY Font
    ttyFont = pntr_load_ttyfont("resources/ttyfont-16x16.png", 16, 16,
        "\x7f !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
    if (ttyFont == NULL) {
        printf("resources/ttyfonFailed to load ttyFontt-16x16.png not loaded");
    }

    // TTF Font
    ttfFont = pntr_load_ttffont("resources/tuffy.ttf", 25, PNTR_DARKPURPLE);
    if (ttyFont == NULL) {
        printf("resources/ttyfont-16x16.png not loaded!\n");
    }
}

const char* example_fonts_update(pntr_image* canvas) {
    // Only display the fonts if they loaded correctly.
    if (defaultFont == NULL || bmFont == NULL || ttyFont == NULL) {
        return "Fonts - Failed to load fonts";
    }

    // Font Drawing
    pntr_draw_text(canvas, defaultFont, "Default Font Example", 20, 50);

    // BMFont
    pntr_draw_rectangle(canvas, 0, 80, 200, 20, PNTR_BLACK);
    pntr_draw_text(canvas, bmFont, "BM Font Example", 20, 80);

    // TTY Font
    pntr_color background = pntr_get_color(0x0000caff);
    pntr_color border = pntr_get_color(0x8a8affff);
    pntr_draw_rectangle(canvas, 0, 120, canvas->width, 60, border);
    pntr_draw_rectangle(canvas, 0, 130, canvas->width, 40, background);

    const char* text = "*** TTY Font Example ***";
    int textWidth = pntr_measure_text(ttyFont, text);
    pntr_draw_text(canvas, ttyFont, text, canvas->width / 2 - textWidth / 2, 140);

    // TTF Font
    const char* ttfText = "TTF Font Example";
    pntr_vector textSize = pntr_measure_text_ex(ttfFont, ttfText);
    pntr_draw_rectangle(canvas, 230, 50, textSize.x, textSize.y, PNTR_LIGHTGRAY);
    pntr_draw_text(canvas, ttfFont, ttfText, 230, 50);

    // pntr_draw_rectangle(canvas, 10, 80, ttfFont->atlas->width, ttfFont->atlas->height, PNTR_WHITE);
    // pntr_draw_image(canvas, ttfFont->atlas, 10, 80);

    return "Fonts";
}

void example_fonts_unload() {
    pntr_unload_font(defaultFont);
    pntr_unload_font(ttyFont);
    pntr_unload_font(bmFont);
    pntr_unload_font(ttfFont);
}
