#include <stdio.h> // sprintf

#include "../../pntr.h"

#define EXAMPLES_COUNT 8
#include "example_welcome.h"
#include "example_shapes.h"
#include "example_fonts.h"
#include "example_image.h"
#include "example_image_resize.h"
#include "example_image_rotate.h"
#include "example_image_sprite.h"
#include "example_bunnymark.h"

int currentExample = 0;
pntr_image* canvas;
pntr_font* font;

void examples_init() {
    canvas = pntr_new_image(400, 225);
    font = pntr_load_default_font();
    example_welcome_init();
    example_shapes_init();
    example_fonts_init();
    example_image_init();
    example_image_resize_init();
    example_image_rotate_init();
    example_image_sprite_init();
    example_bunnymark_init();
}

const char* examples_update() {
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
            exampleTitle = example_image_sprite_update(canvas);
            break;
        case 7:
            exampleTitle = example_bunnymark_update(canvas);
            break;
    }

    char title[50];
    if (currentExample == 0) {
        sprintf(title, "%s", exampleTitle);
    }
    else {
        sprintf(title, "%d/%d %s", currentExample, EXAMPLES_COUNT - 1, exampleTitle);
    }

    pntr_draw_text(canvas, font, title, 10, 10);
    pntr_draw_text(canvas, font, "CLICK or press LEFT/RIGHT to switch examples", 10, canvas->height - 18);

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
    example_bunnymark_unload();
    pntr_unload_image(canvas);
    pntr_unload_font(font);
}

void* examples_data() {
    return (void*)canvas->data;
}

int examples_width() {
    return canvas->width;
}

int examples_pitch() {
    return canvas->pitch;
}

int examples_height() {
    return canvas->height;
}

int examples_size() {
    return canvas->pitch * canvas->height;
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

void examples_screenshot(const char* fileName) {
    pntr_save_image(canvas, fileName);
}
