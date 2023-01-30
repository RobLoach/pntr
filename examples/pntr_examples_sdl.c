#include <stdbool.h>
#include <SDL.h>

// Load pntr and the examples
#include "examples/examples.h"

int main() {
    // Initialize
    examples_init();

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_Window* window = SDL_CreateWindow("pntr: Examples - SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, examples_width(), examples_height(), SDL_WINDOW_SHOWN);
    SDL_Surface* screen = SDL_GetWindowSurface(window);
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(examples_data(), examples_width(), examples_height(), 8, examples_pitch(), SDL_PIXELFORMAT_ARGB8888);
    SDL_Event event;

    bool shouldClose = false;
    while (!shouldClose) {

        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                case SDL_QUIT:
                    shouldClose = true;
                    break;
                case SDL_MOUSEBUTTONUP:
                    examples_next();
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            shouldClose = true;
                            break;
                        case SDLK_LEFT:
                            examples_previous();
                            break;
                        case SDLK_RIGHT:
                        case SDLK_SPACE:
                            examples_next();
                            break;
                    }
                    break;
            }
        }

        // Update
        examples_update();

        SDL_BlitSurface(surface, NULL, screen, NULL);
        SDL_UpdateWindowSurface(window);
    }

    SDL_FreeSurface(surface);

    // Unload
    examples_unload();

    SDL_DestroyWindow(window);
    SDL_Quit();
}
