#include "keyboard.h"

#include "../sound.h"

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>

struct wl_keyboard *keyboard;

static void keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard, uint32_t format, int fd, uint32_t size)
{
    (void) data;
    (void) keyboard;
    (void) fd;
    (void) size;
    printf("keymap format %u\n", format);
}

static void keyboard_handle_enter(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface,
                                  struct wl_array *keys)
{
    (void) data;
    (void) keyboard;
    (void) serial;
    (void) surface;
    (void) keys;
    printf("Keyboard gained focus\n");
}

static void keyboard_handle_leave(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface)
{
    (void) data;
    (void) keyboard;
    (void) serial;
    (void) surface;
    printf("Keyboard lost focus\n");
}

typedef enum {
    MZD_KEYMAP_ROTATE_LEFT  =  49,
    MZD_KEYMAP_ROTATE_RIGHT =  50,

    MZD_KEYMAP_DPAD_UP      = 103,
    MZD_KEYMAP_DPAD_DOWN    = 108,
    MZD_KEYMAP_DPAD_LEFT    = 105,
    MZD_KEYMAP_DPAD_RIGHT   = 106,
    MZD_KEYMAP_DPAD_CENTER  =  28,


    MZD_KEYMAP_EXT_BACK     =  14,
    MZD_KEYMAP_EXT_STAR     =  20,
    MZD_KEYMAP_EXT_MUSIC    =  18,
    MZD_KEYMAP_EXT_HOME     = 102,
    MZD_KEYMAP_EXT_NAV      =  19,

    MZD_KEYMAP_WHEEL_PREV   =  26,
    MZD_KEYMAP_WHEEL_NEXT   =  27,
    MZD_KEYMAP_WHEEL_SEND   =  44,
    MZD_KEYMAP_WHEEL_END    =  45,
    MZD_KEYMAP_WHEEL_VOICE  =  34,

} MZD_KEYMAP;

static bool known_key(uint32_t key, uint32_t state)
{
    if (!state) {
        return false;
    }

    switch (key) {
        case MZD_KEYMAP_ROTATE_LEFT: {
            printf("rotate left\n");
            return true;
        }

        case MZD_KEYMAP_ROTATE_RIGHT: {
            printf("rotate right\n");
            return true;
        }

        case MZD_KEYMAP_DPAD_UP: {
            printf("DPAD up\n");
            return true;
        }

        case MZD_KEYMAP_DPAD_DOWN: {
            printf("DPAD down\n");
            return true;
        }

        case MZD_KEYMAP_DPAD_LEFT: {
            printf("DPAD left\n");
            return true;
        }

        case MZD_KEYMAP_DPAD_RIGHT: {
            printf("DPAD right\n");
            return true;
        }

        case MZD_KEYMAP_DPAD_CENTER: {
            printf("DPAD center\n");
            return true;
        }

        case MZD_KEYMAP_EXT_MUSIC: {
            printf("Flavor music\n");
            pthread_t t;
            pthread_create(&t, NULL, hud_snd_play, (void *)NULL);;
            return true;
        }

        case MZD_KEYMAP_EXT_HOME: {
            printf("Flavor home\n");
            return true;
        }

        case MZD_KEYMAP_EXT_NAV: {
            printf("Flavor nav\n");
            return true;
        }

        case MZD_KEYMAP_EXT_BACK: {
            printf("Flavor back\n");
            return true;
        }

        case MZD_KEYMAP_EXT_STAR: {
            printf("Flavor star\n");
            return true;
        }

        case MZD_KEYMAP_WHEEL_PREV: {
            printf("WHEEL previous\n");
            return true;
        }

        case MZD_KEYMAP_WHEEL_NEXT: {
            printf("WHEEL next\n");
            return true;
        }

        case MZD_KEYMAP_WHEEL_SEND: {
            printf("WHEEL send\n");
            return true;
        }

        case MZD_KEYMAP_WHEEL_END: {
            printf("WHEEL end\n");
            return true;
        }

        case MZD_KEYMAP_WHEEL_VOICE: {
            printf("WHEEL voice\n");
            return true;
        }
        default: {
            // printf("other key %d %d\n", key, state);
            return false;
        }
    }
}

static void keyboard_handle_key(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key,
                                uint32_t state)
{
    (void) data;
    (void) keyboard;
    (void) serial;
    (void) time;

    if (!known_key(key, state) && state) {
        printf("Key is %d state is %d\n", key, state);
    }
}

static void keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t mods_depressed,
                                      uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
{
    (void) data;
    (void) keyboard;
    (void) serial;
    printf("Mods depressed %d, latched %d, locked %d, group %d\n", mods_depressed, mods_latched, mods_locked, group);
}

static const struct wl_keyboard_listener keyboard_listener = {
    .keymap     = keyboard_handle_keymap,
    .enter      = keyboard_handle_enter,
    .leave      = keyboard_handle_leave,
    .key        = keyboard_handle_key,
    .modifiers  = keyboard_handle_modifiers,
};

bool hud_kb_init(struct wl_seat *seat)
{
    if (!keyboard){
        printf("setting keys\n");
        keyboard = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(keyboard, &keyboard_listener, NULL);
        return true;
    }
    return false;
}

bool hud_kb_raze(void)
{
    if (keyboard) {
        printf("killing keys\n");
        wl_keyboard_destroy(keyboard);
        keyboard = NULL;
        return true;
    }
    return false;
}
