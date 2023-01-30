#include <stdbool.h>
#include <SDL.h>

#define PNTR_SUPPORT_DEFAULT_FONT
#define PNTR_IMPLEMENTATION
#include "../pntr.h"

#include "examples/examples.h"

int main() {
    // pntr: Create an image to display
    pntr_image* canvas = pntr_new_image(400, 225);
    pntr_font* font = pntr_load_default_font();
    int currentExample = 0;
    examples_init();

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
                case SDL_MOUSEBUTTONUP:
                    if (++currentExample >= examples_count()) {
                        currentExample = 0;
                    }
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            shouldClose = true;
                            break;
                        case SDLK_LEFT:
                            if (--currentExample < 0) {
                                currentExample = examples_count() - 1;
                            }
                            break;
                        case SDLK_RIGHT:
                        case SDLK_SPACE:
                            if (++currentExample >= examples_count()) {
                                currentExample = 0;
                            }
                            break;
                    }
                    break;
            }
        }

        pntr_clear_background(canvas, PNTR_RAYWHITE);
        const char* exampleTitle = examples_update(currentExample, canvas);
        pntr_draw_text(canvas, font, "Example:", 10, 10);
        pntr_draw_text(canvas, font, exampleTitle, 80, 10);
        pntr_draw_text(canvas, font, "Press left and right or click to switch", 10, canvas->height - 18);

        // SDL: Push to the screen
        SDL_BlitSurface(surface, NULL, screen, NULL);
        SDL_UpdateWindowSurface(window);
    }

    SDL_FreeSurface(surface);

    // Unload
    pntr_unload_image(canvas);

    SDL_DestroyWindow(window);
    SDL_Quit();

    SDL_Log("Error: %s", pntr_get_error());
}
