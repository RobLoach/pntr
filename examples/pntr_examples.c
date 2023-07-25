#define PNTR_ENABLE_DEFAULT_FONT
#define PNTR_ENABLE_FILTER_SMOOTH
#define PNTR_ENABLE_TTF
#define PNTR_ENABLE_MATH

#define PNTR_APP_IMPLEMENTATION
#include "pntr_app.h"

#include "examples/examples.h"

bool Init(void* userData) {
    examples_init();

    return true;
}

bool Update(pntr_image* screen, void* userData) {
    examples_update(screen);

    return true;
}

void Event(pntr_app_event* event, void* userData) {
    switch (event->type) {
        case PNTR_APP_EVENTTYPE_KEY_DOWN: {
            if (event->key == PNTR_APP_KEY_RIGHT) {
                examples_next();
            }
            if (event->key == PNTR_APP_KEY_LEFT) {
                examples_previous();
            }
            if (event->key == PNTR_APP_KEY_F1) {
                examples_screenshot();
            }
        }
        break;
        case PNTR_APP_EVENTTYPE_MOUSE_BUTTON_DOWN: {
            if (event->mouseButton == PNTR_APP_MOUSE_BUTTON_LEFT) {
                examples_next();
            }
            if (event->mouseButton == PNTR_APP_MOUSE_BUTTON_RIGHT) {
                examples_previous();
            }
        }
        break;
        case PNTR_APP_EVENTTYPE_GAMEPAD_BUTTON_DOWN: {
            if (event->gamepadButton == PNTR_APP_GAMEPAD_BUTTON_RIGHT_FACE_DOWN) {
                examples_next();
            }
            if (event->gamepadButton == PNTR_APP_GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) {
                examples_previous();
            }
        }
        break;
    }
}

void Close(void* userData) {
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
