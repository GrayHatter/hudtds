#include "keyboard.h"

#include "ui.h"

#include "../audio.h"
#include "../log.h"

#include <stdint.h>
#include <stdio.h>
#include <wayland-client.h>


struct wl_keyboard *keyboard;

static void keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard, uint32_t format, int fd, uint32_t size)
{
    (void) data;
    (void) keyboard;
    (void) fd;
    (void) size;
    LOG_I("keymap format %u\n", format);
}

static void keyboard_handle_enter(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface,
                                  struct wl_array *keys)
{
    (void) data;
    (void) keyboard;
    (void) serial;
    (void) surface;
    (void) keys;
    LOG_I("Keyboard gained focus\n");
}

static void keyboard_handle_leave(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface)
{
    (void) data;
    (void) keyboard;
    (void) serial;
    (void) surface;
    LOG_I("Keyboard lost focus\n");
}


static bool known_key(uint32_t key, uint32_t state)
{
    if (!state) {
        return false;
    }

    switch (key) {
        case MZD_KEYMAP_ROTATE_LEFT: {
            // LOG_E("rotate left\n");
            return true;
        }

        case MZD_KEYMAP_ROTATE_RIGHT: {
            // LOG_E("rotate right\n");
            return true;
        }

        case MZD_KEYMAP_DPAD_UP: {
            LOG_E("DPAD up\n");
            return true;
        }

        case MZD_KEYMAP_DPAD_DOWN: {
            LOG_E("DPAD down\n");
            return true;
        }

        case MZD_KEYMAP_DPAD_LEFT: {
            LOG_E("DPAD left\n");
            return true;
        }

        case MZD_KEYMAP_DPAD_RIGHT: {
            LOG_E("DPAD right\n");
            return true;
        }

        case MZD_KEYMAP_DPAD_CENTER: {
            LOG_E("DPAD center\n");
            return true;
        }

        case MZD_KEYMAP_EXT_MUSIC: {
            LOG_E("Flavor music\n");
            return true;
        }

        case MZD_KEYMAP_EXT_HOME: {
            LOG_E("Flavor home\n");
            return true;
        }

        case MZD_KEYMAP_EXT_NAV: {
            LOG_E("Flavor nav\n");
            return true;
        }

        case MZD_KEYMAP_EXT_BACK: {
            LOG_E("Flavor back\n");
            return true;
        }

        case MZD_KEYMAP_EXT_STAR: {
            LOG_E("Flavor star\n");
            return true;
        }

        case MZD_KEYMAP_WHEEL_PREV: {
            LOG_E("WHEEL previous\n");
            return true;
        }

        case MZD_KEYMAP_WHEEL_NEXT: {
            LOG_E("WHEEL next\n");
            return true;
        }

        case MZD_KEYMAP_WHEEL_SEND: {
            LOG_E("WHEEL send\n");
            return true;
        }

        case MZD_KEYMAP_WHEEL_END: {
            LOG_E("WHEEL end\n");
            return true;
        }

        case MZD_KEYMAP_WHEEL_VOICE: {
            LOG_E("WHEEL voice\n");
            return true;
        }
        default: {
            LOG_E("other key %d %d\n", key, state);
            return false;
        }
    }
    return false;
}

static void keyboard_handle_key(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key,
                                uint32_t state)
{
    (void) data;
    (void) keyboard;
    (void) serial;
    (void) time;

    // TODO this is wrong
    if (state) {
        if (!known_key(key, state)) {
            LOG_E("Key is %d state is %d\n", key, state);
        }
        ui_key_down_root(key, serial);
    } else {
        ui_key_up_root(key, serial);
    }
}

static void keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t mods_depressed,
                                      uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
{
    (void) data;
    (void) keyboard;
    (void) serial;
    LOG_N("Mods depressed %d, latched %d, locked %d, group %d\n", mods_depressed, mods_latched, mods_locked, group);
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
        LOG_I("setting keys\n");
        keyboard = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(keyboard, &keyboard_listener, NULL);
        return true;
    }
    return false;
}

bool hud_kb_raze(void)
{
    if (keyboard) {
        LOG_I("killing keys\n");
        wl_keyboard_destroy(keyboard);
        keyboard = NULL;
        return true;
    }
    return false;
}
