#include "raylib.h"

// raylib can display RGBA, so tell pntr to reverse the pixels
#define PNTR_PIXELFORMAT_RGBA

// Support the default font
#define PNTR_ENABLE_DEFAULT_FONT
#define PNTR_ENABLE_TTF
#define PNTR_ENABLE_FILTER_SMOOTH
#define PTNR_NO_STB_IMAGE_RESIZE_IMPLEMENTATION // raylib embeds stb_image_resize, so tell pntr to skip its implementation

#define PNTR_IMPLEMENTATION
#include "../pntr.h"

// Load pntr and the examples
#include "examples/examples.h"

int main() {
    // Initialize
    examples_init();

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(examples_width() * 2, examples_height() * 2, "pntr: Examples - raylib");
    SetTargetFPS(60);

    Image image;
    image.data = examples_data();
    image.width = examples_width();
    image.height = examples_height();
    image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    image.mipmaps = 1;
    Texture texture = LoadTextureFromImage(image);

    while (!WindowShouldClose()) {
        // Input
        if (IsKeyPressed(KEY_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            examples_previous();
        }
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_ENTER)) {
            examples_next();
        }

        // Screenshot
        if (IsKeyPressed(KEY_F2)) {
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
            //TakeScreenshot(fileName);
            examples_screenshot(fileName);
        }

        // Update
        examples_update();

        // Get the pixels on the screen
        UpdateTexture(texture, examples_data());

        BeginDrawing();
            ClearBackground(RAYWHITE);

            // Find the aspect ratio.
            float aspect = GetScreenHeight() / GetScreenWidth() ;
            if (aspect <= 0) {
                aspect = (float)image.width / (float)image.height;
            }

            // Calculate the optimal width/height to display in the screen size.
            int height = GetScreenHeight();
            int width = height * aspect;
            if (width > GetScreenWidth()) {
                height = (float)GetScreenWidth() / aspect;
                width = GetScreenWidth();
            }

            // Draw the texture in the middle of the screen.
            int x = (GetScreenWidth() - width) / 2;
            int y = (GetScreenHeight() - height) / 2;
            Rectangle destRec = {x, y, width, height};

            Rectangle source = {0, 0, image.width, image.height};
            Vector2 origin = {0, 0};
            DrawTexturePro(texture, source, destRec, origin, 0, WHITE);

        EndDrawing();
    }

    CloseWindow();

    // Unload
    examples_unload();

    return 0;
}
