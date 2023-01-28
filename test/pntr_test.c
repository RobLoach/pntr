#include <assert.h>

#define PNTR_IMPLEMENTATION
#include "../pntr.h"

int main() {
    {
        pntr_color color = PNTR_RED;
        unsigned char color_r, color_g, color_b, color_a;
        assert(pntr_color_r(color) == 230);
        assert(pntr_color_g(color) == 41);
        assert(pntr_color_b(color) == 55);
        assert(pntr_color_a(color) == 255);
        pntr_color_rgba(color, &color_r, &color_g, &color_b, &color_a);
        assert(color_r == 230);
        assert(color_g == 41);
        assert(color_b == 55);
        assert(color_a == 255);
    }

    {
        pntr_image* image = pntr_gen_image_color(640, 480, PNTR_SKYBLUE);
        assert(image->width == 640);
        assert(image->height == 480);

        pntr_color color = pntr_image_get_color(image, 10, 10);
        assert(color.data == PNTR_SKYBLUE.data);
        pntr_draw_pixel(image, 10, 10, PNTR_PURPLE);
        color = pntr_image_get_color(image, 10, 10);
        assert(color.data == PNTR_PURPLE.data);

        pntr_unload_image(image);
    }

    return 0;
}
