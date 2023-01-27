#define PNTR_IMPLEMENTATION
#include "../pntr.h"

int main() {
    pntr_image* image = pntr_new_image(640, 480, PNTR_SKYBLUE);

    pntr_unload_image(image);

    return 0;
}
