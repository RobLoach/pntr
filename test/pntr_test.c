#define UNIT_TEST_PREFIX ""
#define UNIT_STATIC
#include "unit.h"

#define PNTR_SUPPORT_DEFAULT_FONT
#define PNTR_SUPPORT_TTF
#define PNTR_SUPPORT_FILTER_SMOOTH
#define PNTR_IMPLEMENTATION
#include "../pntr.h"


#define COLOREQUALS(color1, color2) { \
    pntr_color firstColor = (color1); \
    pntr_color secondColor = (color2); \
    EQUALS(firstColor.r, secondColor.r); \
    EQUALS(firstColor.g, secondColor.g); \
    EQUALS(firstColor.b, secondColor.b); \
    EQUALS(firstColor.a, secondColor.a); \
}

MODULE(pntr, {
    IT("pntr_set_error(), pntr_get_error()", {
        pntr_set_error(NULL);
        EQUALS(pntr_get_error(), NULL);
        pntr_image* image = pntr_new_image(-500, -500);
        EQUALS(image, NULL);
        NEQUALS(pntr_get_error(), NULL);
        pntr_unload_image(image);
        pntr_set_error(NULL);
    });

    // pntr_color, pntr_color_get_r(), pntr_color_get_g(), pntr_color_get_b(), pntr_color_get_a()
    IT("pntr_color", {
        pntr_color color = PNTR_RED;
        EQUALS(pntr_color_get_r(color), 230);
        EQUALS(pntr_color_get_g(color), 41);
        EQUALS(pntr_color_get_b(color), 55);
        EQUALS(pntr_color_get_a(color), 255);
        EQUALS(color.r, 230);
        EQUALS(color.g, 41);
        EQUALS(color.b, 55);
        EQUALS(color.a, 255);
    });

    IT("pntr_color_set_*", {
        pntr_color blank = PNTR_BLANK;
        pntr_color_set_r(&blank, 10);
        pntr_color_set_g(&blank, 20);
        pntr_color_set_b(&blank, 30);
        pntr_color_set_a(&blank, 40);
        EQUALS(blank.r, 10);
        EQUALS(blank.g, 20);
        EQUALS(blank.b, 30);
        EQUALS(blank.a, 40);
    });

    IT("pntr_color_get_rgba()", {
        unsigned char color_r, color_g, color_b, color_a;
        pntr_color_get_rgba(PNTR_RED, &color_r, &color_g, &color_b, &color_a);
        EQUALS(color_r, 230);
        EQUALS(color_g, 41);
        EQUALS(color_b, 55);
        EQUALS(color_a, 255);
    });

    IT("pntr_get_color()", {
        pntr_color color = pntr_get_color(0x052c46ff);
        EQUALS(color.r, 5);
        EQUALS(color.g, 44);
        EQUALS(color.b, 70);
        EQUALS(color.a, 255);
    });

    IT("pntr_gen_image_color(), pntr_image_get_color()", {
        pntr_image* image = pntr_gen_image_color(640, 480, PNTR_SKYBLUE);
        NEQUALS(image, NULL);
        EQUALS(image->width, 640);
        EQUALS(image->height, 480);

        pntr_color color = pntr_image_get_color(image, 10, 10);
        COLOREQUALS(color, PNTR_SKYBLUE);
        pntr_unload_image(image);
    });

    IT("pntr_image_get_color_pointer(), pntr_draw_pixel()", {
        pntr_image* image = pntr_gen_image_color(100, 100, PNTR_SKYBLUE);
        NEQUALS(image, NULL);
        EQUALS(image->width, 100);
        EQUALS(image->height, 100);

        pntr_draw_pixel(image, 10, 10, PNTR_PURPLE);
        pntr_color color = pntr_image_get_color(image, 10, 10);
        COLOREQUALS(color, PNTR_PURPLE);

        pntr_color* colorPointer = pntr_image_get_color_pointer(image, 50, 50);
        COLOREQUALS(*colorPointer, PNTR_SKYBLUE);

        pntr_unload_image(image);
    });

    IT("pntr_clear_background(), pntr_draw_rectangle()", {
        pntr_image* image = pntr_new_image(100, 100);
        NEQUALS(image, NULL);
        pntr_clear_background(image, PNTR_RED);

        pntr_color color = pntr_image_get_color(image, 10, 10);
        COLOREQUALS(color, PNTR_RED);

        pntr_clear_background(image, PNTR_BLANK);
        color = pntr_image_get_color(image, 10, 10);
        COLOREQUALS(color, PNTR_BLANK);

        pntr_draw_rectangle(image, 9, 9, 3, 3, PNTR_BLUE);
        color = pntr_image_get_color(image, 10, 10);
        COLOREQUALS(color, PNTR_BLUE);
        pntr_unload_image(image);
    });

    IT("pntr_load_image()", {
        pntr_image* image = pntr_load_image("NotFoundImage.png");
        EQUALS(image, NULL);
        pntr_set_error(NULL);

        image = pntr_load_image("resources/image.png");
        NEQUALS(image, NULL);
        EQUALS(image->width, 128);
        EQUALS(image->height, 128);
        NEQUALS(image->data, NULL);
        pntr_unload_image(image);
    });

    IT("pntr_load_bmfont(), pntr_unload_font(), pntr_draw_text()", {
        pntr_font* font = pntr_load_bmfont("resources/font.png", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/");
        NEQUALS(font, NULL);
        GREATER(font->charactersLen, 10);

        pntr_image* image = pntr_gen_image_color(200, 200, PNTR_DARKBROWN);
        NEQUALS(image, NULL);
        pntr_draw_text(image, font, "Hello World!", 10, 10);
        NEQUALS(image, NULL);

        pntr_unload_image(image);
        pntr_unload_font(font);
    });

    IT("pntr_measure_text(), pntr_measure_text_ex(), pntr_gen_image_text()", {
        pntr_font* font = pntr_load_bmfont("resources/font.png", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/");
        GREATER(pntr_measure_text(font, "Hello World!"), 50);
        pntr_vector size = pntr_measure_text_ex(font, "Hello World!");
        GREATER(size.x, 50);
        EQUALS(size.y, font->atlas->height);

        pntr_image* textImage = pntr_gen_image_text(font, "Hello World!");
        NEQUALS(textImage, NULL);
        EQUALS(textImage->width, size.x);
        EQUALS(textImage->height, size.y);
        pntr_unload_image(textImage);

        size = pntr_measure_text_ex(font, "On\nNew\nLines");
        EQUALS(size.y, font->atlas->height * 3);

        pntr_unload_font(font);
    });

    IT("pntr_load_ttyfont()", {
        pntr_font* font = pntr_load_ttyfont("resources/font-tty-8x8.png", 8, 8, "\x7f !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
        NEQUALS(font, NULL);
        GREATER(font->charactersLen, 20);
        pntr_unload_font(font);
    });

    IT("pntr_load_default_font()", {
        pntr_font* font = pntr_load_default_font();
        NEQUALS(font, NULL);
        GREATER(font->charactersLen, 10);
        pntr_unload_font(font);
    });

    IT("pntr_image_resize()", {
        pntr_image* image = pntr_new_image(300, 100);
        NEQUALS(image, NULL);

        IT("pntr_image_resize() same size", {
            pntr_image* resized = pntr_image_resize(image, 300, 100, PNTR_FILTER_DEFAULT);
            NEQUALS(resized, NULL);
            EQUALS(resized->width, 300);
            EQUALS(resized->height, 100);
            pntr_unload_image(resized);
        });

        IT("pntr_image_resize() nearest neighbor", {
            pntr_image* resized = pntr_image_resize(image, 100, 100, PNTR_FILTER_NEARESTNEIGHBOR);
            NEQUALS(resized, NULL);
            EQUALS(resized->width, 100);
            EQUALS(resized->height, 100);
            pntr_unload_image(resized);
        });

        #ifdef PNTR_SUPPORT_FILTER_SMOOOTH
            IT("pntr_image_resize() smooth", {
                pntr_image* resized = pntr_image_resize(image, 800, 600, PNTR_FILTER_SMOOTH);
                NEQUALS(resized, NULL);
                EQUALS(resized->width, 800);
                EQUALS(resized->height, 600);
                pntr_unload_image(resized);
            });
        #endif  // PNTR_SUPPORT_FILTER_SMOOOTH

        IT("pntr_image_resize() bilinear", {
            pntr_image* resized = pntr_image_resize(image, 400, 300, PNTR_FILTER_BILINEAR);
            NEQUALS(resized, NULL);
            EQUALS(resized->width, 400);
            EQUALS(resized->height, 300);
            pntr_unload_image(resized);
        });

        pntr_unload_image(image);
    });

    IT("pntr_image_color_replace()", {
        pntr_image* image = pntr_gen_image_color(100, 100, PNTR_BLUE);
        NEQUALS(image, NULL);
        pntr_color color = pntr_image_get_color(image, 10, 10);
        COLOREQUALS(color, PNTR_BLUE);
        pntr_image_color_replace(image, PNTR_BLUE, PNTR_RED);
        color = pntr_image_get_color(image, 10, 10);
        COLOREQUALS(color, PNTR_RED);
        pntr_unload_image(image);
    });

    IT("pntr_color_fade()", {
        pntr_color color = PNTR_RED;
        EQUALS(color.a, 255);
        EQUALS(color.r, 230);
        pntr_color faded = pntr_color_fade(color, -0.5f);
        EQUALS(faded.a, 127);
        EQUALS(faded.r, 230);
    });

    IT("pntr_load_file(), pntr_unload_file()", {
        unsigned int bytesRead;
        unsigned char* fileData = pntr_load_file("resources/text.txt", &bytesRead);

        GREATER(bytesRead, 5);
        STRCEQUALS((const char*)fileData, "Hello", 5);
        pntr_unload_file(fileData);
    });

    #ifdef PNTR_SUPPORT_TTF
        IT("pntr_load_ttffont()", {
            pntr_font* font = pntr_load_ttffont("resources/tuffy.ttf", 20, PNTR_BLACK);
            NEQUALS(font, NULL);
            GREATER(font->charactersLen, 20);

            pntr_image* canvas = pntr_gen_image_text(font, "Hello World!");
            NEQUALS(canvas, NULL);
            GREATER(canvas->width, 10);
            GREATER(canvas->height, 10);

            IT("pntr_load_ttffont() - pntr_measure_text_ex()", {
                pntr_vector size = pntr_measure_text_ex(font, "Hello!!");
                GREATER(size.x, 20);
                GREATER(size.y, 5);
            });

            pntr_unload_image(canvas);
            pntr_unload_font(font);
        });
    #else
        IT("pntr_load_ttffont(): PNTR_SUPPORT_TTF is disabled, unable to test.", {
            // Nothing
        });
    #endif  // PNTR_SUPPORT_TTF

    IT("pntr_save_file()", {
        const char* fileName = "tempFile.txt";
        const char* fileData = "Hello World!";
        unsigned int bytes = 12;
        bool result = pntr_save_file(fileName, (unsigned char*)fileData, bytes);
        EQUALS(result, true);

        unsigned char* fileDataResult = pntr_load_file(fileName, &bytes);
        GREATER(bytes, 5);
        STRCEQUALS((const char*)fileDataResult, "Hello", 5);
        pntr_unload_file(fileDataResult);
    });

    IT("pntr_save_image()", {
        int width = 400;
        int height = 300;
        pntr_image* saveImage = pntr_gen_image_color(width, height, PNTR_RED);
        NEQUALS(saveImage, NULL);
        pntr_draw_circle(saveImage, 200, 150, 80, PNTR_BLUE);
        pntr_draw_rectangle(saveImage, 10, 10, 20, 20, PNTR_GREEN);
        bool result = pntr_save_image(saveImage, "saveImage.png");
        EQUALS(result, true);
        pntr_unload_image(saveImage);

        pntr_image* loadedImage = pntr_load_image("saveImage.png");
        NEQUALS(loadedImage, NULL);
        EQUALS(loadedImage->width, 400);
        EQUALS(loadedImage->height, height);
        COLOREQUALS(pntr_image_get_color(loadedImage, 15, 15), PNTR_GREEN)
        pntr_unload_image(loadedImage);
    });

    IT("pntr_get_pixel_data_size()", {
        EQUALS(pntr_get_pixel_data_size(1, 1, PNTR_PIXELFORMAT_RGBA8888), 4);
        EQUALS(pntr_get_pixel_data_size(2, 3, PNTR_PIXELFORMAT_RGBA8888), 24);
        EQUALS(pntr_get_pixel_data_size(3, 2, PNTR_PIXELFORMAT_ARGB8888), 24);
    });

    IT("pntr_image_alpha_border(), pntr_image_alpha_crop()", {
        pntr_image* image = pntr_gen_image_color(400, 400, PNTR_BLANK);
        NEQUALS(image, NULL);
        EQUALS(image->width, 400);
        EQUALS(image->height, 400);

        pntr_draw_rectangle(image, 100, 100, 200, 200, PNTR_BLUE);

        pntr_rectangle crop = pntr_image_alpha_border(image, 0);
        EQUALS(crop.x, 100);
        EQUALS(crop.y, 100);
        EQUALS(crop.width, 200);
        EQUALS(crop.height, 200);

        pntr_image_alpha_crop(image, 0);
        NEQUALS(image, NULL);
        EQUALS(image->width, 200);
        EQUALS(image->height, 200);

        pntr_color color = pntr_image_get_color(image, 50, 50);
        COLOREQUALS(color, PNTR_BLUE);

        pntr_unload_image(image);
    });

    IT("pntr_image_crop()", {
        pntr_image* image = pntr_gen_image_color(200, 200, PNTR_RED);
        NEQUALS(image, NULL);
        pntr_image_crop(image, 10, 30, 20, 50);
        NEQUALS(image, NULL);
        EQUALS(image->width, 20);
        EQUALS(image->height, 50);
        pntr_color color = pntr_image_get_color(image, 10, 20);
        COLOREQUALS(color, PNTR_RED);
        pntr_unload_image(image);
    });

    IT("pntr_image_resize_canvas()", {
        pntr_image* image = pntr_gen_image_color(200, 200, PNTR_BLUE);
        NEQUALS(image, NULL);
        EQUALS(image->width, 200);
        EQUALS(image->height, 200);
        pntr_image_resize_canvas(image, 400, 400, 100, 100, PNTR_RED);
        NEQUALS(image, NULL);
        EQUALS(image->width, 400);
        EQUALS(image->height, 400);
        pntr_color color = pntr_image_get_color(image, 50, 50);
        COLOREQUALS(color, PNTR_RED);
        color = pntr_image_get_color(image, 150, 150);
        COLOREQUALS(color, PNTR_BLUE);
        pntr_unload_image(image);
    });

    IT("pntr_image_rotate()", {
        pntr_image* image = pntr_gen_image_color(40, 30, PNTR_BLUE);
        NEQUALS(image, NULL);
        pntr_draw_rectangle(image, 9, 9, 3, 3, PNTR_RED);

        IT("pntr_image_rotate(image, 0.0f)", {
            pntr_image* rotated = pntr_image_rotate(image, 0.0f);
            NEQUALS(rotated, NULL);
            EQUALS(image->width, 40);
            EQUALS(image->height, 30);
            pntr_unload_image(rotated);
        });

        IT("pntr_image_rotate(image, 0.25f)", {
            pntr_image* rotated = pntr_image_rotate(image, 0.25f);
            NEQUALS(rotated, NULL);
            EQUALS(rotated->width, image->height);
            EQUALS(rotated->height, image->width);
            pntr_color color = pntr_image_get_color(rotated, 10, 10);
            COLOREQUALS(color, PNTR_BLUE);
            color = pntr_image_get_color(rotated, 10, 30);
            pntr_save_image(rotated, "rotate2.png");
            COLOREQUALS(color, PNTR_RED);
            pntr_unload_image(rotated);
        })

        IT("pntr_image_rotate(image, 0.5f)", {
            pntr_image* rotated = pntr_image_rotate(image, 0.5f);
            NEQUALS(rotated, NULL);
            EQUALS(rotated->width, image->width);
            EQUALS(rotated->height, image->height);
            pntr_color color = pntr_image_get_color(rotated, 10, 10);
            COLOREQUALS(color, PNTR_BLUE);
            color = pntr_image_get_color(rotated, 30, 20);
            COLOREQUALS(color, PNTR_RED);
            pntr_unload_image(rotated);
        });

        IT("pntr_image_rotate(image, 0.75f)", {
            pntr_image* rotated = pntr_image_rotate(image, 0.75f);
            NEQUALS(rotated, NULL);
            EQUALS(rotated->width, image->height);
            EQUALS(rotated->height, image->width);
            pntr_color color = pntr_image_get_color(rotated, 10, 10);
            COLOREQUALS(color, PNTR_BLUE);
            color = pntr_image_get_color(rotated, 20, 10);
            COLOREQUALS(color, PNTR_RED);
            pntr_unload_image(rotated);
        });

        #ifndef PNTR_NO_MATH
            IT("pntr_image_rotate(image, 0.33f)", {
                pntr_image* rotated = pntr_image_rotate(image, 0.33f);
                NEQUALS(rotated, NULL);
                NEQUALS(rotated->width, image->height);
                NEQUALS(rotated->height, image->width);
                pntr_color color = pntr_image_get_color(rotated, 5, 5);
                COLOREQUALS(color, PNTR_BLANK);
                color = pntr_image_get_color(rotated, rotated->width / 2, rotated->height / 2);
                COLOREQUALS(color, PNTR_BLUE);
                pntr_unload_image(rotated);
            });
        #else
            IT("pntr_image_rotate(image, 0.33f): PNTR_NO_MATH is defined, unable to test.", {
                // Nothing
            });
        #endif  // PNTR_NO_MATH

        pntr_unload_image(image);
    });

    IT("No reported errors", {
        const char* err = "";
        if (pntr_get_error() != NULL) {
            err = pntr_get_error();
        }

        STREQUALS(err, "");
    });
});

int main() {
    UNIT_CREATE("pntr");
    UNIT_MODULE(pntr);
    return UNIT_RUN();
}
