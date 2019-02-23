#include "notifier.h"


#include "../log.h"
#include "../wl/ui.h"
#include "../wl/draw.h"
#include "../wl/text.h"

#include <string.h>


static void draw_notifier(struct ui_panel *p, int32_t x, int32_t y, int32_t w, int32_t h)
{
    x = p->pos_x < 0 ? w + p->pos_x : x + p->pos_x;
    y = p->pos_y < 0 ? h + p->pos_y : y + p->pos_y;
    w = p->width <= 0 ? w + p->width : x + p->width;
    h = p->height <= 0 ? h + p->height : y + p->height;

    // draw_box_c(x, y, w, h, p->color);
}


static void draw_notifications(struct ui_panel *p, int32_t x, int32_t y, int32_t w, int32_t h)
{
    x = p->pos_x < 0 ? w + p->pos_x : x + p->pos_x;
    y = p->pos_y < 0 ? h + p->pos_y : y + p->pos_y;
    w = p->width <= 0 ? w + p->width : x + p->width;
    h = p->height <= 0 ? h + p->height : y + p->height;

    // draw_box_c(x, y, w, h, p->color);
    // text_draw_string("20:48", x + 3, y + 3);
}

static void draw_timedate(struct ui_panel *p, int32_t x, int32_t y, int32_t w, int32_t h)
{
    x = p->pos_x < 0 ? w + p->pos_x : x + p->pos_x;
    y = p->pos_y < 0 ? h + p->pos_y : y + p->pos_y;
    w = p->width <= 0 ? w + p->width : x + p->width;
    h = p->height <= 0 ? h + p->height : y + p->height;

    // draw_box_c(x, y, w, h, p->color);
    text_draw_string("20:48", x + 3, y + 3);
}


struct ui_panel notifications = {
    .type = PANEL_FRAME,
    .name = "notifications",
    .draw = draw_notifications,
    .pos_x = -150,
    .pos_y = 0,
};


struct ui_panel time_date = {
    .type = PANEL_FRAME,
    .name = "time_date",
    .draw = draw_timedate,
    .pos_x = -150,
    .pos_y = 0,
};


struct ui_panel notifier = {
    .type = PANEL_FRAME,
    .name = "notifier",
    .draw = draw_notifier,
    .pos_x = 0,
    .pos_y = 0,
    .children = (struct ui_panel*[]){
        &notifications,
        &time_date,
        NULL,
    }
};
