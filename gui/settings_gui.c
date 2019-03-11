#include "settings_gui.h"

#include "../ui.h"

#include <stddef.h>

struct ui_panel settings_frame = {
    .type = PANEL_FRAME,
    .name = "settings frame",
    .pos_x = 0,
    .pos_y = 0,
    .height = -80,
    .focused = false,
    .enabled = false,

    .children = (struct ui_panel*[]) {
        NULL
    }
};

