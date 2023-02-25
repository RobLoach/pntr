#include "../../pntr.h"

#include <stdlib.h> // malloc(), free(), rand()
#include <stdio.h>  // sprintf()

#define MAX_BUNNIES        50000    // 50K bunnies limit

pntr_image* bunnyImage;
typedef struct Bunny {
    float positionX;
    float positionY;
    float speedX;
    float speedY;
} Bunny;

Bunny *bunnies;
int bunniesCount;
int frameCount;
char title[80];

int randNum(int lower, int upper) {
    return (rand() % (upper - lower + 1)) + lower;
}

void example_bunnymark_init() {
    // Load an image
    bunnyImage = pntr_load_image("resources/bunny.png");

    bunnies = (Bunny *)malloc(MAX_BUNNIES * sizeof(Bunny));
    bunniesCount = 0;
    frameCount = 0;
}

const char* example_bunnymark_update(pntr_image* canvas) {
    if (bunnyImage == NULL) {
        return "resources/bunny.png failed to load";
    }

    if (++frameCount > randNum(5, 10)) {
        frameCount = 0;
        bunnies[bunniesCount].positionX = randNum(0, canvas->width - bunnyImage->width);
        bunnies[bunniesCount].positionY = randNum(0, canvas->height - bunnyImage->height);
        bunnies[bunniesCount].speedX = (float)randNum(-250, 250) / 60.0f;
        bunnies[bunniesCount].speedY = (float)randNum(-250, 250) / 60.0f;
        bunniesCount++;
    }

    // Update bunnies
    for (int i = 0; i < bunniesCount; i++)
    {
        bunnies[i].positionX += bunnies[i].speedX;
        bunnies[i].positionY += bunnies[i].speedY;

        if (((bunnies[i].positionX + bunnyImage->width / 2) > canvas->width) ||
            ((bunnies[i].positionX + bunnyImage->width / 2) < 0)) {
                bunnies[i].speedX *= -1;
            }
        if (((bunnies[i].positionY + bunnyImage->width / 2) > canvas->height) ||
            ((bunnies[i].positionY + bunnyImage->width / 2) < 0)) {
                bunnies[i].speedY *= -1;
            }

        pntr_draw_image(canvas, bunnyImage, bunnies[i].positionX, bunnies[i].positionY);
    }

    sprintf(title, "Bunnies %i", bunniesCount);
    return title;
}

void example_bunnymark_unload() {
    free(bunnies);
    pntr_unload_image(bunnyImage);
}
