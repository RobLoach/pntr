#define PNTR_ENABLE_DEFAULT_FONT
#define PNTR_ENABLE_TTF
#define PNTR_ENABLE_VARGS
#define PNTR_ENABLE_MATH
#define PNTR_APP_IMPLEMENTATION
#include "pntr_app.h"

#include "examples/examples.h"

bool Init(pntr_app* app) {
    examples_init();

    return true;
}

bool Update(pntr_app* app, pntr_image* screen) {
    examples_update(screen);

    return true;
}

void Event(pntr_app* app, pntr_app_event* event) {
    switch (event->type) {
        case PNTR_APP_EVENTTYPE_KEY_DOWN: {
            switch (event->key) {
                case PNTR_APP_KEY_RIGHT:
                    examples_next();
                    break;
                case PNTR_APP_KEY_LEFT:
                    examples_previous();
                    break;
                case PNTR_APP_KEY_F1:
                    examples_screenshot();
                    break;
            }
        }
        break;
        case PNTR_APP_EVENTTYPE_MOUSE_BUTTON_DOWN: {
            switch (event->mouseButton) {
                case PNTR_APP_MOUSE_BUTTON_LEFT:
                    examples_next();
                    break;
                case PNTR_APP_MOUSE_BUTTON_RIGHT:
                    examples_previous();
                    break;
            }
        }
        break;
        case PNTR_APP_EVENTTYPE_GAMEPAD_BUTTON_DOWN: {
            switch (event->gamepadButton) {
                case PNTR_APP_GAMEPAD_BUTTON_A:
                case PNTR_APP_GAMEPAD_BUTTON_RIGHT:
                    examples_next();
                    break;
                case PNTR_APP_GAMEPAD_BUTTON_B:
                case PNTR_APP_GAMEPAD_BUTTON_LEFT:
                    examples_previous();
                    break;
            }
        }
        break;
    }
}

void Close(pntr_app* app) {
    examples_unload();
}

pntr_app Main(int argc, char* argv[]) {
    return (pntr_app) {
        .width = 400,
        .height = 225,
        .title = "pntr_app: Examples",
        .init = Init,
        .update = Update,
        .close = Close,
        .event = Event,
        .fps = 60,
        //.userData = PNTR_MALLOC(sizeof(AppData)),
    };
}
