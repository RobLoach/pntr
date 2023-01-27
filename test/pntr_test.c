#include <assert.h>

#define PNTR_IMPLEMENTATION
#include "../pntr.h"

int main() {
    pntr_image* image = pntr_new_image(640, 480, PNTR_SKYBLUE);
    pntr_unload_image(image);

    uint32_t red = PNTR_RED;
    unsigned char red_r, red_g, red_b, red_a;
    assert(pntr_color_r(red) == 230);
    pntr_color_rgba(red, &red_r, &red_g, &red_b, &red_a);
    assert(red_r == 230);
    assert(red_g == 41);
    assert(red_b == 55);
    assert(red_a == 255);

    return 0;
}
