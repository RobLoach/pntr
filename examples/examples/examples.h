#include <stdio.h> // sprintf

int currentExample = 0;
pntr_image* canvas;
pntr_font* font;

#define EXAMPLES_COUNT 9
#include "example_welcome.h"
#include "example_shapes.h"
#include "example_fonts.h"
#include "example_image.h"
#include "example_image_resize.h"
#include "example_image_rotate.h"
#include "example_image_sprite.h"
#include "example_image_alphamask.h"
#include "example_bunnymark.h"

void examples_init() {
    font = pntr_load_font_default();
    example_welcome_init();
    example_shapes_init();
    example_fonts_init();
    example_image_init();
    example_image_resize_init();
    example_image_rotate_init();
    example_image_sprite_init();
    example_bunnymark_init();
    example_image_alphamask_init();
}

const char* examples_update(pntr_image* canvas) {
    pntr_clear_background(canvas, PNTR_RAYWHITE);
    const char* exampleTitle;
    switch (currentExample) {
        case 0:
            exampleTitle = example_welcome_update(canvas);
            break;
        case 1:
            exampleTitle = example_shapes_update(canvas);
            break;
        case 2:
            exampleTitle = example_fonts_update(canvas);
            break;
        case 3:
            exampleTitle = example_image_update(canvas);
            break;
        case 4:
            exampleTitle = example_image_resize_update(canvas);
            break;
        case 5:
            exampleTitle = example_image_rotate_update(canvas);
            break;
        case 6:
            exampleTitle = example_image_alphamask_update(canvas);
            break;
        case 7:
            exampleTitle = example_image_sprite_update(canvas);
            break;
        case 8:
            exampleTitle = example_bunnymark_update(canvas);
            break;
    }

    pntr_draw_text_ex(canvas, font, 10, 10, PNTR_BLACK, "%d/%d %s", currentExample, EXAMPLES_COUNT - 1, exampleTitle);
    pntr_draw_text(canvas, font, "CLICK or press LEFT/RIGHT to switch examples", 10, canvas->height - 18, PNTR_BLACK);

    return exampleTitle;
}

void examples_unload() {
    example_welcome_unload();
    example_shapes_unload();
    example_fonts_unload();
    example_image_unload();
    example_image_resize_unload();
    example_image_sprite_unload();
    example_image_rotate_unload();
    example_image_alphamask_unload();
    example_bunnymark_unload();
    pntr_unload_font(font);
}

void examples_previous() {
    if (--currentExample < 0) {
        currentExample = EXAMPLES_COUNT - 1;
    }
}

void examples_next() {
    if (++currentExample >= EXAMPLES_COUNT) {
        currentExample = 0;
    }
}

void examples_screenshot() {
    pntr_save_image(canvas, "screenshot.png");
}
