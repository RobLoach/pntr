#include <stdbool.h>
#include <SDL2/SDL.h>

#define PNTR_IMPLEMENTATION
#include "../pntr.h"

int main() {
    // Create an image to display
    pntr_image* canvas = pntr_gen_image_color(400, 225, PNTR_RAYWHITE);
    pntr_draw_rectangle(canvas, 10, 10, 80, 200, PNTR_RED);
    pntr_draw_rectangle(canvas, 100, 10, 80, 200, PNTR_GREEN);
    pntr_draw_rectangle(canvas, 200, 10, 80, 200, PNTR_BLUE);
    pntr_draw_pixel(canvas, 300, 80, PNTR_BLUE);

    pntr_image* image = pntr_load_image("resources/image.png");
    pntr_draw_image(canvas, image, 200, 50);
    pntr_unload_image(image);

    // Render the image to an SDL screen.
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	SDL_Window* window = SDL_CreateWindow("pntr: SDL Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, canvas->width, canvas->height, SDL_WINDOW_SHOWN);
    SDL_Event event;
    SDL_Surface* screen = SDL_GetWindowSurface(window);
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(canvas->data, canvas->width, canvas->height, 8, canvas->pitch, SDL_PIXELFORMAT_ARGB8888);

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
    pntr_unload_image(canvas);

	SDL_DestroyWindow(window);
	SDL_Quit();
}
