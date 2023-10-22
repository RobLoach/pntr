#include <stdlib.h> // rand()
#include <stdio.h>  // sprintf()

#define MAX_BUNNIES        50000    // 50K bunnies limit

pntr_image* bunnyImage;
typedef struct Bunny {
    float positionX;
    float positionY;
    float speedX;
    float speedY;
    pntr_color color;
} Bunny;

Bunny *bunnies;
int bunniesCount;
int frameCount;
char title[80];

void example_bunnymark_init() {
    // Load an image
    bunnyImage = pntr_load_image("resources/bunny.png");

    bunnies = (Bunny *)pntr_load_memory(MAX_BUNNIES * sizeof(Bunny));
    bunniesCount = 0;
    frameCount = 0;
}

const char* example_bunnymark_update(pntr_image* canvas) {
    if (bunnyImage == NULL) {
        return "resources/bunny.png failed to load";
    }

    // Add new bunnies
    if (++frameCount > pntr_app_random(5, 10) && bunniesCount < MAX_BUNNIES) {
        frameCount = 0;
        bunnies[bunniesCount].positionX = pntr_app_random(0, canvas->width - bunnyImage->width);
        bunnies[bunniesCount].positionY = pntr_app_random(0, canvas->height - bunnyImage->height);
        bunnies[bunniesCount].speedX = (float)pntr_app_random(-250, 250) / 60.0f;
        bunnies[bunniesCount].speedY = (float)pntr_app_random(-250, 250) / 60.0f;
        bunnies[bunniesCount].color = pntr_new_color(pntr_app_random(50, 240), pntr_app_random(80, 240), pntr_app_random(100, 240), 255);
        bunniesCount++;
    }

    // Update bunnies
    for (int i = 0; i < bunniesCount; i++) {
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
    }

    // Draw bunnies
    for (int i = 0; i < bunniesCount; i++) {
        //pntr_draw_image(canvas, bunnyImage, bunnies[i].positionX, bunnies[i].positionY);
        pntr_draw_image_tint(canvas, bunnyImage, bunnies[i].positionX, bunnies[i].positionY, bunnies[i].color);
    }

    sprintf(title, "Bunnies %i", bunniesCount);
    return title;
}

void example_bunnymark_unload() {
    pntr_unload_memory(bunnies);
    pntr_unload_image(bunnyImage);
}
