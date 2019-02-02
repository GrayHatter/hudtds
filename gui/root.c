#include "root.h"

#include "nav.h"

#include "../wl/ui.h"
#include "../log.h"

#include "../hud.h"


struct ui_panel main_frame = {0};


struct ui_panel root_panel = {
    .width = WIDTH - 1,
    .height = HEIGHT - 1,

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
