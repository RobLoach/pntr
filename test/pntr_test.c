#include <assert.h>
#include <stdio.h>

#define PNTR_SUPPORT_DEFAULT_FONT
#define PNTR_IMPLEMENTATION
#include "../pntr.h"

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

        pntr_unload_font(font);
        pntr_unload_image(image);
    }

    // pntr_measure_text(), pntr_measure_text_ex(), pntr_gen_image_text()
    {
        pntr_font* font = pntr_load_bmfont("resources/font.png", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/");
        assert(pntr_measure_text(font, "Hello World!") > 50);
        pntr_vector size = pntr_measure_text_ex(font, "pntr_measure_text_ex()");
        assert(size.x > 50);
        assert(size.y == font->atlas->height);

        size = pntr_measure_text_ex(font, "On\nNew\nLines");
        assert(size.y == font->atlas->height * 3);

        pntr_image* textImage = pntr_gen_image_text(font, "Hello World!");
        assert(textImage != NULL);
        pntr_unload_image(textImage);

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
        pntr_color faded = pntr_color_fade(color, 0.5f);
        assert(faded.a == 127);
        assert(faded.r == 230);
    }

    // pntr_load_file(), pntr_unload_file()
    {
        unsigned int bytesRead;
        unsigned char* fileData = pntr_load_file("resources/text.txt", &bytesRead);

        assert(bytesRead == 14);
        assert(fileData[0] == 'H');
        assert(fileData[1] == 'e');
        assert(fileData[2] == 'l');
        assert(fileData[3] == 'l');
        assert(fileData[4] == 'o');
        pntr_unload_file(fileData);
    }

    // Ensure there were no errors.
    assert(pntr_get_error() == NULL);

    return 0;
}
