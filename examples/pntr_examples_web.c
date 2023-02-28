#include <stdbool.h>
#include <emscripten.h>
#include <emscripten/html5.h>

// Use RGBA pixel format
#define PNTR_PIXELFORMAT_RGBA

// Enable all pntr features
#define PNTR_SUPPORT_TTF
#define PNTR_SUPPORT_DEFAULT_FONT
#define PNTR_SUPPORT_FILTER_SMOOTH

#define PNTR_IMPLEMENTATION
#include "../pntr.h"

// Load pntr and the examples
#include "examples/examples.h"

/**
 * Render the pixel data onto the canvas.
 */
EM_JS(void, render_canvas, (void* data, int size, int width, int height), {
    // Make sure the canvas is set up.
    const canvas = document.getElementById('canvas');
    const context = canvas.getContext('2d');
    canvas.setAttribute('width', width);
    canvas.setAttribute('height', height);

    // Set the canvas' image data to the pntr pixel data
    const image = context.getImageData(0, 0, width, height);
    image.data.set(HEAPU8.subarray(data, data + size));

    // Display the new image data on the canvas
    context.putImageData(image, 0, 0);
});

/**
 * Updates the game loop
 */
void update() {
    examples_update();

    void* data = examples_data();
    int size = examples_size();

    render_canvas(data, size, examples_width(), examples_height());
}

/**
 * Mouse click event
 */
EM_BOOL click_callback(int eventType, const EmscriptenMouseEvent *e, void *userData) {
    if (e->button == 0) {
        examples_next();
        return true;
    }
    else if (e->button == 2) {
        examples_previous();
        return true;
    }

    return false;
}

/**
 * Key down event
 */
EM_BOOL key_callback(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData) {
    if (keyEvent->repeat == false) {
        examples_next();
        return true;
    }

    return false;
}

int main() {
    // Initialize
    examples_init();

    // Set up input
    emscripten_set_click_callback("#canvas", NULL, false, click_callback);
    emscripten_set_keydown_callback("#canvas", NULL, false, key_callback);

    // Start the main loop
    emscripten_set_main_loop(update, 60, true);
    examples_unload();

    return 0;
}
