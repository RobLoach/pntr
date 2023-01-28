#include <stdbool.h>
#include <SDL2/SDL.h>

#define PNTR_IMPLEMENTATION
#include "../pntr.h"

int main() {
    // Create an image to display
    pntr_image* image = pntr_gen_image_color(400, 225, PNTR_RAYWHITE);
    pntr_draw_rectangle(image, 10, 10, 80, 200, PNTR_RED);
    pntr_draw_rectangle(image, 100, 10, 80, 200, PNTR_GREEN);
    pntr_draw_rectangle(image, 200, 10, 80, 200, PNTR_BLUE);
    pntr_draw_pixel(image, 300, 80, PNTR_BLUE);

    pntr_color color;
    color.r = 200;

    // Render the image to an SDL screen.
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	SDL_Window* window = SDL_CreateWindow("pntr: SDL Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, image->width, image->height, SDL_WINDOW_SHOWN);
    SDL_Event event;
    SDL_Surface* screen = SDL_GetWindowSurface(window);
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(image->data, image->width, image->height, 8, image->pitch, SDL_PIXELFORMAT_ARGB8888);

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

        // Render to the screen.
        SDL_BlitSurface(surface, NULL, screen, NULL);
        SDL_UpdateWindowSurface(window);
    }

    SDL_FreeSurface(surface);

    // Unload
    pntr_unload_image(image);

	SDL_DestroyWindow(window);
	SDL_Quit();
}
