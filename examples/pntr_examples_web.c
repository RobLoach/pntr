#include <stdbool.h>
#include <emscripten.h>
#include <emscripten/html5.h>

// Use RGBA pixel format
#define PNTR_PIXELFORMAT_RGBA
#define PNTR_SUPPORT_TTF

// Support the default font
#define PNTR_SUPPORT_DEFAULT_FONT

// Tell Emscripten to export the pntr API
#define PNTR_API EMSCRIPTEN_KEEPALIVE

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
    examples_next();
    return 0;
}

/**
 * Key down event
 */
EM_BOOL key_callback(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData) {
    examples_next();
    return 0;
}

int main() {
    // Initialize
    examples_init();

    // Set up input
    emscripten_set_click_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, 0, 1, click_callback);
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, key_callback);

    // Start the main loop
    emscripten_set_main_loop(update, 60, true);
    examples_unload();
}
