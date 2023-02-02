#include "raylib.h"

// raylib can display RGBA, so tell pntr to reverse the pixels
#define PNTR_PIXELFORMAT_RGBA

// Support the default font
#define PNTR_SUPPORT_DEFAULT_FONT

#define PNTR_SUPPORT_BDF

// raylib already uses stb_image, so we skip its implementation
#define PNTR_NO_STB_IMAGE_IMPLEMENTATION

#define PNTR_IMPLEMENTATION
#include "../pntr.h"

// Load pntr and the examples
#include "examples/examples.h"

int main() {
    // Initialize
    examples_init();

    InitWindow(examples_width(), examples_height(), "pntr: Examples - raylib");

    Image image;
    image.data = examples_data();
    image.width = examples_width();
    image.height = examples_height();
    image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    image.mipmaps = 1;
    Texture texture = LoadTextureFromImage(image);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Input
        if (IsKeyPressed(KEY_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            examples_previous();
        }
        if (IsKeyPressed(KEY_RIGHT) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            examples_next();
        }

        // Update
        examples_update();

        // Get the pixels on the screen
        UpdateTexture(texture, examples_data());

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawTexture(texture, 0, 0, WHITE);
        EndDrawing();
    }

    CloseWindow();

    // Unload
    examples_unload();

    return 0;
}
