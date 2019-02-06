#include "root.h"

#include "nav.h"
#include "music.h"

#include "../wl/ui.h"
#include "../wl/text.h"
#include "../log.h"

#include "../hud.h"

static void draw_root_frame(struct ui_panel *p, int32_t x, int32_t y, int32_t w, int32_t h)
{
    (void) p;
    (void) x;
    (void) y;
    (void) w;
    (void) h;

    text_draw_string("HudTds!", 30, 30);
}


struct ui_panel main_frame = {
    .name = "main frame",
    .draw = draw_root_frame,
    .children = (struct ui_panel*[]) {
        &music_frame,
        NULL,
    }
};

struct ui_panel root_panel = {
    .width = WIDTH - 1,
    .height = HEIGHT - 1,

    .name = "root frame",

    .children = (struct ui_panel*[]){
        &main_frame,
        &nav_frame,
        NULL,
    }
};

struct ui_panel *gui_build_root(void)
{
    LOG_W("init root children");

    LOG_W(" -- done\n");
    return &root_panel;
}
