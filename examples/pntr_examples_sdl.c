#include <stdbool.h>
#include <SDL.h>

// Use the ARGB pixel format
#define PNTR_PIXELFORMAT_ARGB

// Support the default font
#define PNTR_SUPPORT_DEFAULT_FONT
#define PNTR_SUPPORT_TTF

#define PNTR_IMPLEMENTATION
#include "../pntr.h"

// Load pntr and the examples
#include "examples/examples.h"

// this is just for web demo
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

SDL_Window *window;
SDL_Surface *screen;
SDL_Surface *surface;

bool shouldClose = false;

void mainloop() {
    SDL_Event event;
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

    if (shouldClose) {
        SDL_FreeSurface(surface);

        // Unload
        examples_unload();

        SDL_DestroyWindow(window);
        SDL_Quit();
    } else {
        // Update
        examples_update();

        SDL_BlitSurface(surface, NULL, screen, NULL);
        SDL_UpdateWindowSurface(window);
    }
}

int main() {
    // Initialize
    examples_init();

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    window = SDL_CreateWindow("pntr: Examples - SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, examples_width(), examples_height(), SDL_WINDOW_SHOWN);
    screen = SDL_GetWindowSurface(window);
    surface = SDL_CreateRGBSurfaceWithFormatFrom(examples_data(), examples_width(), examples_height(), 8, examples_pitch(), SDL_PIXELFORMAT_ARGB8888);

    #ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(mainloop, 0, 1);
    #else
        while(!shouldClose) {
            mainloop();
            SDL_Delay(16);
        }
    #endif
}
