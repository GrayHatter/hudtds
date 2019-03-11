#include "nav.h"

#include "onscreenkeys.h"

#include "../ui.h"
#include "../wl/draw.h"

#include "../log.h"
#include "../audio.h"

#include <stdlib.h>
#include <string.h>

static bool touch_test_1(struct ui_panel *p, const int mx, const int my, const int x, const int y, const uint32_t w, uint32_t h,
    const uint32_t id, const uint32_t serial)
{
    (void) p;
    (void) mx;
    (void) my;
    (void) x;
    (void) y;
    (void) w;
    (void) h;
    (void) id;
    (void) serial;
    LOG_W("touch test 1\n");

    return true;
}


static void draw_button(struct ui_panel *p, int32_t x, int32_t y, int32_t w, int32_t h)
{
    x = p->pos_x < 0 ? w + p->pos_x : x + p->pos_x;
    y = p->pos_y < 0 ? h + p->pos_y : y + p->pos_y;
    w = p->width <= 0 ? w + p->width : x + p->width;
    h = p->height <= 0 ? h + p->height : y + p->height;

    draw_box_c(x, y, w, h, p->color);
}

#define NAV_BTW_W 80
#define NAV_BTW_H 65

struct ui_panel nav_btn_0 = {
    .name = "nav_btn_0",
    .enabled = true,
    .color = 0xff888888,
    .draw = draw_button,
    .t_dn = touch_test_1,
    .pos_x = NAV_BTW_W * 0,
    .pos_y = 0,
    .width = NAV_BTW_W,
    .height = NAV_BTW_H
};

struct ui_panel nav_btn_1 = {
    .name = "nav_btn_1",
    .enabled = true,
    .color = 0xffff0000,
    .draw = draw_button,
    .t_dn = touch_test_1,
    .pos_x =NAV_BTW_W * 1,
    .pos_y = 0,
    .width = NAV_BTW_W,
    .height = NAV_BTW_H
};

struct ui_panel nav_btn_2 = {
    .name = "nav_btn_2",
    .enabled = true,
    .color = 0xffffffff,
    .draw = draw_button,
    .t_dn = touch_test_1,
    .pos_x = NAV_BTW_W * 2,
    .pos_y = 0,
    .width = NAV_BTW_W,
    .height = NAV_BTW_H
};

struct ui_panel nav_btn_3 = {
    .name = "nav_btn_3",
    .enabled = true,
    .color = 0xff00ff00,
    .draw = draw_button,
    .t_dn = touch_test_1,
    .pos_x = NAV_BTW_W * 3,
    .pos_y = 0,
    .width = NAV_BTW_W,
    .height = NAV_BTW_H
};

struct ui_panel nav_btn_4 = {
    .name = "nav_btn_4",
    .enabled = true,
    .color = 0xff00ffff,
    .draw = draw_button,
    .t_dn = touch_test_1,
    .pos_x = NAV_BTW_W * 4,
    .pos_y = 0,
    .width = NAV_BTW_W,
    .height = NAV_BTW_H
};

struct ui_panel nav_btn_5 = {
    .name = "nav_btn_5",
    .enabled = true,
    .color = 0xffff00ff,
    .draw = draw_button,
    .t_dn = touch_test_1,
    .pos_x = NAV_BTW_W * 5,
    .pos_y = 0,
    .width = NAV_BTW_W,
    .height = NAV_BTW_H
};

struct ui_panel nav_btn_6 = {
    .draw = draw_button,
    .color = 0xff88ffff,
    .name = "nav_btn_6",
    .t_dn = touch_test_1,
    .pos_x = NAV_BTW_W * 6,
    .pos_y = 0,
    .width = NAV_BTW_W,
    .height = NAV_BTW_H
};

struct ui_panel nav_btn_7 = {
    .draw = draw_button,
    .color = 0xffff8888,
    .name = "nav_btn_7",
    .pos_x = NAV_BTW_W * 7,
    .pos_y = 0,
    .width = NAV_BTW_W,
    .height = NAV_BTW_H
};


static bool tgl_keys(struct ui_panel *p, const int mx, const int my, const int x, const int y, const uint32_t w, uint32_t h,
    const uint32_t id, const uint32_t serial)
{
    (void) p;
    (void) mx;
    (void) my;
    (void) x;
    (void) y;
    (void) w;
    (void) h;
    (void) id;
    (void) serial;

    onscreenkey_frame.enabled = !onscreenkey_frame.enabled;

    return true;
}


static struct ui_panel nav_btn_8 = {
    .name = "nav_btn_8",
    .color = 0xff8888ff,
    .enabled = true,
    .draw = draw_button,
    .t_dn = tgl_keys,
    .pos_x = NAV_BTW_W * 8,
    .pos_y = 0,
    .width = NAV_BTW_W,
    .height = NAV_BTW_H
};


static bool touch_exit(struct ui_panel *p, const int mx, const int my, const int x, const int y, const uint32_t w, uint32_t h,
    const uint32_t id, const uint32_t serial)
{
    (void) p;
    (void) mx;
    (void) my;
    (void) x;
    (void) y;
    (void) w;
    (void) h;
    (void) id;
    (void) serial;
    LOG_W("touch exit\n");
    exit(0);

    return true;
}


struct ui_panel nav_btn_9 = {
    .name = "nav_btn_9",
    .color = 0xff333333,
    .enabled = true,
    .draw = draw_button,
    .t_dn = touch_exit,
    .pos_x = NAV_BTW_W * 9,
    .pos_y = 0,
    .width = NAV_BTW_W,
    .height = NAV_BTW_H
};


struct ui_panel nav_frame = {
    .type = PANEL_FRAME,
    .name = "nav frame",
    .enabled = true,
    .draw = draw_button,
    .pos_x = 0,
    .pos_y = -NAV_BTW_H,
    .height = NAV_BTW_H,
    .children = (struct ui_panel*[]) {
        &nav_btn_0,
        &nav_btn_1,
        &nav_btn_2,
        &nav_btn_3,
        &nav_btn_4,
        &nav_btn_5,
        &nav_btn_6,
        &nav_btn_7,
        &nav_btn_8,
        &nav_btn_9,
        NULL
    }
};
