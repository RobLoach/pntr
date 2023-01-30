#include <stdbool.h>
#include <SDL2/SDL.h>

#define PNTR_IMPLEMENTATION
#include "../pntr.h"

int main() {
    // pntr: Create an image to display
    pntr_image* canvas = pntr_new_image(400, 225);
    pntr_image* image = pntr_load_image("resources/image.png");
    pntr_image* resized = pntr_image_resize(image, image->width * 1.2f, image->height / 2, 0);

    pntr_font* font = pntr_load_bmfont("resources/font.png", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/");

    // SDL
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_Window* window = SDL_CreateWindow("pntr: SDL Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, canvas->width, canvas->height, SDL_WINDOW_SHOWN);
    SDL_Surface* screen = SDL_GetWindowSurface(window);
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(canvas->data, canvas->width, canvas->height, 8, canvas->pitch, SDL_PIXELFORMAT_ARGB8888);
    SDL_Event event;

    bool shouldClose = false;
    while (!shouldClose) {

        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                case SDL_QUIT:
                    shouldClose = true;
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            shouldClose = true;
                            break;
                    }
                    break;
            }
        }

        // pntr: Render to the canvas
        pntr_clear_background(canvas, PNTR_DARKBROWN);
        pntr_draw_rectangle(canvas, 10, 50, 80, 200, PNTR_RED);
        pntr_draw_rectangle(canvas, 100, 50, 80, 200, PNTR_GREEN);
        pntr_draw_rectangle(canvas, 200, 50, 80, 200, PNTR_BLUE);
        pntr_draw_pixel(canvas, 300, 80, PNTR_BLUE);
        pntr_draw_image(canvas, image, 200, 50);
        pntr_draw_image(canvas, resized, 200, 10);

        pntr_draw_text(canvas, font, "Hello World!", 10, 10);

        // SDL: Push to the screen
        SDL_BlitSurface(surface, NULL, screen, NULL);
        SDL_UpdateWindowSurface(window);
    }

    SDL_FreeSurface(surface);

    // Unload
    pntr_unload_font(font);
    pntr_unload_image(resized);
    pntr_unload_image(canvas);
    pntr_unload_image(image);

    SDL_DestroyWindow(window);
    SDL_Quit();
}
