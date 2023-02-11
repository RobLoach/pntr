#include <stdio.h>

#define PNTR_SUPPORT_DEFAULT_FONT
#define PNTR_SUPPORT_TTF
#define PNTR_IMPLEMENTATION
#include "../pntr.h"

#ifdef assert
#undef assert
#endif
#define assert(condition) if (!(bool)(condition)) { printf("Fail:      %s\nCondition: %s\n%s:%d\n", pntr_get_error() == NULL ? "" : pntr_get_error(), #condition, __FILE__, __LINE__); return 1; }

int main() {
    // pntr_set_error(), pntr_get_error()
    {
        pntr_set_error(NULL);
        assert(pntr_get_error() == NULL);
        pntr_image* image = pntr_new_image(-500, -500);
        assert(image == NULL);
        assert(pntr_get_error() != NULL);
        pntr_unload_image(image);
        pntr_set_error(NULL);
    }

    // pntr_color, pntr_color_get_r(), pntr_color_get_g(), pntr_color_get_b(), pntr_color_get_a()
    {
        pntr_color color = PNTR_RED;
        assert(pntr_color_get_r(color) == 230);
        assert(pntr_color_get_g(color) == 41);
        assert(pntr_color_get_b(color) == 55);
        assert(pntr_color_get_a(color) == 255);
        assert(color.r == 230);
        assert(color.g == 41);
        assert(color.b == 55);
        assert(color.a == 255);
    }

    // pntr_color_set_r(), pntr_color_set_g(), pntr_color_set_b(), pntr_color_set_a()
    {
        pntr_color color = PNTR_BLANK;
        pntr_color_set_r(&color, 10);
        pntr_color_set_g(&color, 20);
        pntr_color_set_b(&color, 30);
        pntr_color_set_a(&color, 40);
        assert(color.r == 10);
        assert(color.g == 20);
        assert(color.b == 30);
        assert(color.a == 40);
    }

    // pntr_color_get_rgba()
    {
        unsigned char color_r, color_g, color_b, color_a;
        pntr_color_get_rgba(PNTR_RED, &color_r, &color_g, &color_b, &color_a);
        assert(color_r == 230);
        assert(color_g == 41);
        assert(color_b == 55);
        assert(color_a == 255);
    }

    // pntr_get_color()
    {
        pntr_color color = pntr_get_color(0x052c46ff);
        assert(color.r == 5);
        assert(color.g == 44);
        assert(color.b == 70);
        assert(color.a == 255);
    }

    // pntr_gen_image_color(), pntr_image_get_color(), pntr_image_get_color_pointer(), pntr_draw_pixel()
    {
        pntr_image* image = pntr_gen_image_color(640, 480, PNTR_SKYBLUE);
        assert(image->width == 640);
        assert(image->height == 480);

        pntr_color color = pntr_image_get_color(image, 10, 10);
        assert(color.data == PNTR_SKYBLUE.data);

        pntr_draw_pixel(image, 10, 10, PNTR_PURPLE);
        color = pntr_image_get_color(image, 10, 10);
        assert(color.data == PNTR_PURPLE.data);

        pntr_color* colorPointer = pntr_image_get_color_pointer(image, 50, 50);
        assert(colorPointer->data == PNTR_SKYBLUE.data);

        pntr_unload_image(image);
    }

    // pntr_clear_background(), pntr_draw_rectangle()
    {
        pntr_image* image = pntr_new_image(100, 100);
        pntr_clear_background(image, PNTR_RED);
        pntr_color color = pntr_image_get_color(image, 10, 10);
        assert(color.data == PNTR_RED.data);

        pntr_draw_rectangle(image, 9, 9, 3, 3, PNTR_BLUE);
        color = pntr_image_get_color(image, 10, 10);
        assert(color.data == PNTR_BLUE.data);
        pntr_unload_image(image);
    }

    // pntr_load_image()
    {
        pntr_image* image = pntr_load_image("NotFoundImage.png");
        assert(image == NULL);
        pntr_set_error(NULL);

        image = pntr_load_image("resources/image.png");
        assert(image != NULL);
        assert(image->width == 128);
        assert(image->height == 108);
        assert(image->data != NULL);
        pntr_unload_image(image);
    }

    // pntr_load_bmfont(), pntr_unload_font(), pntr_draw_text()
    {
        pntr_font* font = pntr_load_bmfont("resources/font.png", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/");
        assert(font != NULL);
        assert(font->charactersFound > 10);

        pntr_image* image = pntr_gen_image_color(200, 200, PNTR_DARKBROWN);
        pntr_draw_text(image, font, "Hello World!", 10, 10);
        assert(image != NULL);

        pntr_unload_image(image);
        pntr_unload_font(font);
    }

    // pntr_measure_text(), pntr_measure_text_ex(), pntr_gen_image_text()
    {
        pntr_font* font = pntr_load_bmfont("resources/font.png", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/");
        assert(pntr_measure_text(font, "Hello World!") > 50);
        pntr_vector size = pntr_measure_text_ex(font, "Hello World!");
        assert(size.x > 50);
        assert(size.y == font->atlas->height);

        pntr_image* textImage = pntr_gen_image_text(font, "Hello World!");
        assert(textImage != NULL);
        assert(textImage->width == size.x);
        assert(textImage->height == size.y);
        pntr_unload_image(textImage);

        size = pntr_measure_text_ex(font, "On\nNew\nLines");
        assert(size.y == font->atlas->height * 3);

        pntr_unload_font(font);
    }

    // pntr_load_ttyfont()
    {
        pntr_font* font = pntr_load_ttyfont("resources/font-tty-8x8.png", 8, 8, "\x7f !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
        assert(font != NULL);
        assert(font->charactersFound > 20);
        pntr_unload_font(font);
    }

    // pntr_load_default_font()
    {
        pntr_font* font = pntr_load_default_font();
        assert(font != NULL);
        assert(font->charactersFound > 10);
        pntr_unload_font(font);
    }

    // pntr_image_resize()
    {
        pntr_image* image = pntr_new_image(300, 100);
        assert(image != NULL);

        pntr_image* resized = pntr_image_resize(image, 640, 480, PNTR_FILTER_NEARESTNEIGHBOR);
        assert(resized != NULL);
        assert(resized->width == 640);
        assert(resized->height == 480);
        pntr_unload_image(resized);

        resized = pntr_image_resize(image, 100, 100, PNTR_FILTER_NEARESTNEIGHBOR);
        assert(resized != NULL);
        assert(resized->width == 100);
        assert(resized->height == 100);
        pntr_unload_image(resized);

        pntr_unload_image(image);
    }

    // pntr_image_color_replace()
    {
        pntr_image* image = pntr_gen_image_color(100, 100, PNTR_BLUE);
        pntr_color color = pntr_image_get_color(image, 10, 10);
        assert(color.data == PNTR_BLUE.data);
        pntr_image_color_replace(image, PNTR_BLUE, PNTR_RED);
        color = pntr_image_get_color(image, 10, 10);
        assert(color.data == PNTR_RED.data);
        pntr_unload_image(image);
    }

    // pntr_color_fade()
    {
        pntr_color color = PNTR_RED;
        assert(color.a == 255);
        assert(color.r == 230);
        pntr_color faded = pntr_color_fade(color, -0.5f);
        assert(faded.a == 127);
        assert(faded.r == 230);
    }

    // pntr_load_file(), pntr_unload_file()
    {
        unsigned int bytesRead;
        unsigned char* fileData = pntr_load_file("resources/text.txt", &bytesRead);

        assert(bytesRead > 5);
        assert(fileData[0] == 'H');
        assert(fileData[1] == 'e');
        assert(fileData[2] == 'l');
        assert(fileData[3] == 'l');
        assert(fileData[4] == 'o');
        pntr_unload_file(fileData);
    }

    // pntr_load_ttffont()
    {
        pntr_font* font = pntr_load_ttffont("resources/tuffy.ttf", 20, PNTR_BLACK);
        assert(font != NULL);
        assert(font->charactersFound > 20);
        pntr_image* canvas = pntr_gen_image_text(font, "Hello World!");
        assert(canvas != NULL);
        assert(canvas->width > 10);
        assert(canvas->height > 10);
        pntr_unload_image(canvas);
        pntr_unload_font(font);
    }

    // pntr_save_file()
    {
        const char* fileName = "tempFile.txt";
        const char* fileData = "Hello World!";
        unsigned int bytes = 12;
        bool result = pntr_save_file(fileName, (unsigned char*)fileData, bytes);
        assert(result);

        unsigned char* fileDataResult = pntr_load_file(fileName, &bytes);
        assert(bytes > 5);
        assert(fileDataResult[0] == 'H');
        assert(fileDataResult[1] == 'e');
        assert(fileDataResult[2] == 'l');
        assert(fileDataResult[3] == 'l');
        assert(fileDataResult[4] == 'o');
        pntr_unload_file(fileDataResult);
    }

    // pntr_save_image()
    {
        int width = 400;
        int height = 300;
        pntr_image* saveImage = pntr_gen_image_color(width, height, PNTR_RED);
        assert(saveImage != NULL);
        pntr_draw_circle(saveImage, 200, 150, 80, PNTR_BLUE);
        pntr_draw_rectangle(saveImage, 10, 10, 20, 20, PNTR_GREEN);
        bool result = pntr_save_image(saveImage, "saveImage.png");
        assert(result);
        pntr_unload_image(saveImage);

        // Re-load the same image to verify it worked.
        pntr_image* loadedImage = pntr_load_image("saveImage.png");
        assert(loadedImage != NULL);
        assert(loadedImage->width == 400);
        assert(loadedImage->height == height);
        assert(pntr_image_get_color(loadedImage, 15, 15).data == PNTR_GREEN.data)
        pntr_unload_image(loadedImage);
    }

    // pntr_get_pixel_data_size()
    {
        assert(pntr_get_pixel_data_size(1, 1, PNTR_PIXELFORMAT_RGBA8888) == 4);
        assert(pntr_get_pixel_data_size(2, 3, PNTR_PIXELFORMAT_RGBA8888) == 24);
        assert(pntr_get_pixel_data_size(3, 2, PNTR_PIXELFORMAT_ARGB8888) == 24);
    }

    // pntr_image_alpha_border(), pntr_image_alpha_crop()
    {
        pntr_image* image = pntr_gen_image_color(400, 400, PNTR_BLANK);
        assert(image != NULL);
        assert(image->width == 400);
        assert(image->height == 400);

        pntr_draw_rectangle(image, 100, 100, 200, 200, PNTR_BLUE);

        pntr_rectangle crop = pntr_image_alpha_border(image, 0);
        assert(crop.x == 100);
        assert(crop.y == 100);
        assert(crop.width == 200);
        assert(crop.height == 200);

        pntr_image_alpha_crop(image, 0);
        assert(image != NULL);
        assert(image->width == 200);
        assert(image->height == 200);

        pntr_color color = pntr_image_get_color(image, 50, 50);
        assert(color.data == PNTR_BLUE.data);

        pntr_unload_image(image);
    }

    // pntr_image_crop()
    {
        pntr_image* image = pntr_gen_image_color(200, 200, PNTR_RED);
        assert(image != NULL);
        pntr_image_crop(image, 10, 30, 20, 50);
        assert(image != NULL);
        assert(image->width == 20);
        assert(image->height == 50);
        pntr_color color = pntr_image_get_color(image, 10, 20);
        assert(color.data == PNTR_RED.data);
        pntr_unload_image(image);
    }

    // Ensure there were no errors.
    if (pntr_get_error() != NULL) {
        printf("Error: %s\n", pntr_get_error());
        return 1;
    }

    assert(pntr_get_error() == NULL);

    return 0;
}
