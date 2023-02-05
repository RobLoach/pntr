#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include "libretro.h"

// libretro uses ARGB8888
#define PNTR_PIXELFORMAT_ARGB
#define PNTR_SUPPORT_TTF
#define PNTR_SUPPORT_DEFAULT_FONT
#define PNTR_IMPLEMENTATION
#include "pntr.h"

// Bring in the example code
#include "examples/examples.h"

static struct retro_log_callback logging;
static retro_log_printf_t log_cb;
static int switchedExampleCooldown = 0;

static void fallback_log(enum retro_log_level level, const char *fmt, ...) {
   (void)level;
   va_list va;
   va_start(va, fmt);
   vfprintf(stderr, fmt, va);
   va_end(va);
}

void retro_init(void) {
   examples_init();
}

void retro_deinit(void) {
   examples_unload();
}

unsigned retro_api_version(void) {
   return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device) {
   log_cb(RETRO_LOG_INFO, "Plugging device %u into port %u.\n", device, port);
}

void retro_get_system_info(struct retro_system_info *info) {
   memset(info, 0, sizeof(*info));
   info->library_name     = "pntr_examples_libretro";
   info->library_version  = "0.0.1";
   info->need_fullpath    = false;
   info->valid_extensions = NULL; // Anything is fine, we don't care.
}

static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environ_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;

void retro_get_system_av_info(struct retro_system_av_info *info) {
   info->timing = (struct retro_system_timing) {
      .fps = 60.0,
      .sample_rate = 0.0,
   };

   info->geometry = (struct retro_game_geometry) {
      .base_width   = examples_width(),
      .base_height  = examples_height(),
      .max_width    = examples_width(),
      .max_height   = examples_height(),
      .aspect_ratio = (float)examples_width() / (float)examples_height(),
   };
}

void retro_set_environment(retro_environment_t cb) {
   environ_cb = cb;

   bool no_content = true;
   cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &no_content);

   if (cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging)) {
      log_cb = logging.log;
   }
   else {
      log_cb = fallback_log;
   }
}

void retro_set_audio_sample(retro_audio_sample_t cb) {
   audio_cb = cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) {
   audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb) {
   input_poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb) {
   input_state_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb) {
   video_cb = cb;
}

void retro_reset(void) {
   // Nothing
}

static void update_input(void) {
   input_poll_cb();

    if (switchedExampleCooldown-- > 0) {
        return;
    }

    if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT)) {
        examples_previous();
        switchedExampleCooldown = 20;
    }

    if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT)) {
        examples_next();
        switchedExampleCooldown = 20;
    }

    if (input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT)) {
        examples_next();
        switchedExampleCooldown = 20;
    }
}

static void render(void) {
    pntr_color* buffer = examples_data();
    size_t pitch = examples_pitch();
    unsigned width = examples_width();
    unsigned height = examples_height();

    video_cb((void*)buffer, width, height, pitch);
}

static void check_variables(void) {
    // Nothing.
}

static void audio_callback(void) {
   audio_cb(0, 0);
}

void retro_run(void) {
    update_input();

    // Update the example
    examples_update();

    // Render the example
    render();

    audio_callback();

    bool updated = false;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated) {
        check_variables();
    }
}

bool retro_load_game(const struct retro_game_info *info) {
    enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
    if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt)) {
        log_cb(RETRO_LOG_INFO, "XRGB8888 is not supported.\n");
        return false;
    }

    check_variables();

    (void)info;
    return true;
}

void retro_unload_game(void) {
    // Nothing.
}

unsigned retro_get_region(void) {
   return RETRO_REGION_NTSC;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num) {
    (void)type;
    (void)info;
    (void)num;
    return retro_load_game(NULL);
}

size_t retro_serialize_size(void) {
    return 0;
}

bool retro_serialize(void *data, size_t size) {
    (void)data;
    (void)size;
    return true;
}

bool retro_unserialize(const void *data, size_t size) {
    (void)data;
    (void)size;
    return true;
}

void *retro_get_memory_data(unsigned id) {
    (void)id;
    return NULL;
}

size_t retro_get_memory_size(unsigned id) {
    (void)id;
    return 0;
}

void retro_cheat_reset(void) {
    // Nothing.
}

void retro_cheat_set(unsigned index, bool enabled, const char *code) {
    (void)index;
    (void)enabled;
    (void)code;
}

