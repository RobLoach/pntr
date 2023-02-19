#include "raylib.h"

// raylib can display RGBA, so tell pntr to reverse the pixels
#define PNTR_PIXELFORMAT_RGBA

// Support the default font
#define PNTR_SUPPORT_DEFAULT_FONT
#define PNTR_SUPPORT_TTF
#define PNTR_SUPPORT_FILTER_SMOOTH
#define PTNR_NO_STB_IMAGE_RESIZE_IMPLEMENTATION // raylib embeds stb_image_resize, so tell pntr to skip its implementation

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
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_ENTER)) {
            examples_next();
        }

        // Screenshot
        if (IsKeyPressed(KEY_F12)) {
            // Grab the example name, and clean it up for a screenshot.
            const char* fileName = examples_update();

            // Clean up the Bunnymark name.
            if (TextFindIndex(fileName, "Bunn") != -1) {
                fileName = "bunnymark";
            }

            fileName = TextReplace((char*)fileName, " ", "_");
            fileName = TextToLower(fileName);

            // Find where to save the screenshot.
            if (DirectoryExists("../../examples/examples/screenshots")) {
                fileName = TextFormat("../../examples/examples/screenshots/%s.png", fileName);
            }
            else {
                fileName = TextFormat("%s.png", fileName);
            }
            TakeScreenshot(fileName);
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
