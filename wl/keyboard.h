#ifndef _HUD_KB_H_
#define _HUD_KB_H_

#include <stdbool.h>
#include <wayland-client-protocol.h>

bool hud_kb_init(struct wl_seat *seat);

bool hud_kb_raze(void);


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



#endif // _HUD_KB_H_
