#include "root.h"

#include "notifier.h"
#include "music.h"
#include "nav.h"
#include "gps.h"
// #include "onscreenkeys.h"

#include "../wl/keyboard.h"
#include "../wl/ui.h"
#include "../wl/text.h"
#include "../wl/draw.h"
#include "../log.h"

#include "../hud.h"

static void draw_root_frame(struct ui_panel *p, int32_t x, int32_t y, int32_t w, int32_t h)
{
    (void) p;
    (void) x;
    (void) y;
    (void) w;
    (void) h;

    draw_square(x, y, w, h);
    text_draw_string("HudTds!", 2, 2);
}


static bool main_kdn(struct ui_panel *p, const uint32_t key, const uint32_t s)
{
    (void) s;
    p->draw_needed = true;

    switch (key) {
        case MZD_KEYMAP_EXT_MUSIC: {
            LOG_E("main frame music btn\n");
            music_frame.disabled = false;
            music_frame.focused = true;

            gps_frame.disabled = true;
            gps_frame.focused = false;

            return true;
        }
        case MZD_KEYMAP_EXT_NAV: {
            LOG_E("main frame nav btn\n");
            gps_frame.disabled = false;
            gps_frame.focused = true;

            music_frame.disabled = true;
            music_frame.focused = false;
            return true;
        }
    }
    return false;
}


struct ui_panel main_frame = {
    .name = "main frame",
    .draw = draw_root_frame,
    .focused = true,
    .k_dn = main_kdn,
    .pos_x = 0,
    .pos_y = 35,
    .width = 0,
    .height = -65,
    .children = (struct ui_panel*[]) {
        &music_frame,
        &gps_frame,
        NULL,
    }
};



struct ui_panel root_panel = {
    .name = "root frame",
    .width = WIDTH - 1,
    .height = HEIGHT - 1,
    .focused = true,

    .children = (struct ui_panel*[]){
        &notifier,
        &nav_frame,
        &main_frame,
        NULL,
    }
};

struct ui_panel *gui_build_root(void)
{
    return &root_panel;
}
