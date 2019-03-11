#include "root.h"

#include "notifier.h"
#include "music.h"
#include "nav.h"
#include "gps_gui.h"
#include "settings_gui.h"
// #include "onscreenkeys.h"

#include "../wl/keyboard.h"
#include "../ui.h"
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
    text_string("HudTds!", 2, 2);
}



static void _disable_all(struct ui_panel *main)
{
    struct ui_panel **children = main->children;
    struct ui_panel *p;

    while ((p = *children++)) {
        p->enabled = false;
        p->focused = false;
    }

}

static bool main_kdn(struct ui_panel *p, const uint32_t key, const uint32_t s)
{
    (void) s;
    p->draw_needed = true;

    switch (key) {
        case MZD_KEYMAP_EXT_MUSIC: {
            LOG_E("main frame music btn\n");
            _disable_all(p);
            music_frame.enabled = true;
            music_frame.focused = true;
            return true;
        }
        case MZD_KEYMAP_EXT_NAV: {
            LOG_E("main frame nav btn\n");
            _disable_all(p);
            gps_frame.enabled = true;
            gps_frame.focused = true;
            return true;
        }
        case MZD_KEYMAP_EXT_HOME: {
            LOG_E("main frame nav btn\n");
            _disable_all(p);
            settings_frame.enabled = true;
            settings_frame.focused = true;
            return true;
        }
    }
    return false;
}


static struct ui_panel main_frame = {
    .name = "main frame",
    .draw = draw_root_frame,
    .focused = true,
    .enabled = true,
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
    .enabled = true,

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
