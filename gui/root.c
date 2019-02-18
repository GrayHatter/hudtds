#include "root.h"

#include "notifier.h"
#include "music.h"
#include "nav.h"
// #include "onscreenkeys.h"

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

    text_draw_string("HudTds!", 2, 2);
}


struct ui_panel main_frame = {
    .name = "main frame",
    .draw = draw_root_frame,
    .focused = true,
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
        &notifier,
        &main_frame,
        &nav_frame,
        NULL,
    }
};

struct ui_panel *gui_build_root(void)
{
    return &root_panel;
}
