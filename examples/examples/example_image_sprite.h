#include "../../pntr.h"

pntr_image* scarfy;
pntr_rectangle frameRec;
int framesSpeed = 8;
int framesCounter = 0;
int currentFrame = 0;

void example_image_sprite_init() {
    // Load the image
    scarfy = pntr_load_image("resources/scarfy.png");
    if (scarfy == NULL) {
        return;
    }
    frameRec.x = 0;
    frameRec.y = 0;
    frameRec.width = scarfy->width / 6;
    frameRec.height = scarfy->height;
}

const char* example_image_sprite_update(pntr_image* canvas) {
    if (scarfy == NULL) {
        return pntr_get_error();
    }

    framesCounter++;

    if (framesCounter >= (60 / framesSpeed)) {
        framesCounter = 0;
        currentFrame++;

        if (currentFrame > 5) {
            currentFrame = 0;
        }

        frameRec.x = (float)currentFrame*(float)scarfy->width / 6;
    }

    pntr_draw_image_rec(canvas, scarfy, frameRec, canvas->width / 2 - frameRec.width / 2, canvas->height / 2 - frameRec.height / 2);

    return "Image Sprite";
}

void example_image_sprite_unload() {
    pntr_unload_image(scarfy);
}
