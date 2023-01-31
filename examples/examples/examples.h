#define PNTR_SUPPORT_DEFAULT_FONT
#define PNTR_IMPLEMENTATION
#include "../../pntr.h"

#include "example_shapes.h"
#include "example_fonts.h"
#include "example_images.h"
#include "example_bunnymark.h"
#define EXAMPLES_COUNT 4

int currentExample;
pntr_image* canvas;
pntr_font* font;

void examples_init() {
    canvas = pntr_new_image(400, 225);
    font = pntr_load_default_font();
    currentExample = 0;
    example_shapes_init();
    example_fonts_init();
    example_images_init();
    example_bunnymark_init();
}

void examples_update() {
    pntr_clear_background(canvas, PNTR_RAYWHITE);
    const char* exampleTitle;
    switch (currentExample) {
        case 0:
            exampleTitle = example_shapes_update(canvas);
            break;
        case 1:
            exampleTitle = example_images_update(canvas);
            break;
        case 2:
            exampleTitle = example_fonts_update(canvas);
            break;
        case 3:
            exampleTitle = example_bunnymark_update(canvas);
            break;
    }

    pntr_draw_text(canvas, font, "Example:", 10, 10);
    pntr_draw_text(canvas, font, exampleTitle, 80, 10);
    pntr_draw_text(canvas, font, "Press left and right or click to switch", 10, canvas->height - 18);
}

void examples_unload() {
    example_shapes_unload();
    example_fonts_unload();
    example_images_unload();
    example_bunnymark_unload();
    pntr_unload_image(canvas);
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
