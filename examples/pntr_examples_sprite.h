#include "../pntr.h"

bool pntr_examples_sprite() {
    // Load the image
    pntr_image* scarfy = pntr_load_image("resources/scarfy.png");
    if (scarfy == NULL) {
        return false;
    }

pntr_rectangle frameRec;
    frameRec.x = 0;
    frameRec.y = 0;
    frameRec.width = scarfy->width / 6;
    frameRec.height = scarfy->height;
int framesSpeed = 8;
int framesCounter = 0;
int currentFrame = 0;

    if (scarfy == NULL) {
        return false;
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

    pntr_save_image(canvas, "pntr_examples_sprite.png");

    pntr_unload_image(scarfy);
    pntr_unload_image(canvas);

    return true;
}
