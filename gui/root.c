#include "root.h"

#include "../wl/ui.h"
#include "../log.h"

#include "../hud.h"

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


struct ui_panel nav_btn_0 = {
    .color = 0xff888888,
    .t_dn = touch_test_1,
    .pos_x = 0,
    .pos_y = 0,
    .width = 80,
    .height = 80
};

struct ui_panel nav_btn_1 = {
    .color = 0xffff0000,
    .t_dn = touch_test_1,
    .pos_x = 80,
    .pos_y = 0,
    .width = 80,
    .height = 80
};

struct ui_panel nav_btn_2 = {
    .color = 0xffffffff,
    .t_dn = touch_test_1,
    .pos_x = 160,
    .pos_y = 0,
    .width = 80,
    .height = 80
};

struct ui_panel nav_btn_3 = {
    .color = 0xff00ff00,
    .t_dn = touch_test_1,
    .pos_x = 240,
    .pos_y = 0,
    .width = 80,
    .height = 80
};

struct ui_panel nav_btn_4 = {
    .color = 0xff00ffff,
    .t_dn = touch_test_1,
    .pos_x = 320,
    .pos_y = 0,
    .width = 80,
    .height = 80
};

struct ui_panel nav_btn_5 = {
    .color = 0xffff00ff,
    .t_dn = touch_test_1,
    .pos_x = 400,
    .pos_y = 0,
    .width = 80,
    .height = 80
};

struct ui_panel nav_btn_6 = {
    .color = 0xff88ffff,
    .t_dn = touch_test_1,
    .pos_x = 480,
    .pos_y = 0,
    .width = 80,
    .height = 80
};

struct ui_panel nav_btn_7 = {
    .color = 0xffff8888,
    .t_dn = touch_test_1,
    .pos_x = 560,
    .pos_y = 0,
    .width = 80,
    .height = 80
};

struct ui_panel nav_btn_8 = {
    .color = 0xff8888ff,
    .t_dn = touch_test_1,
    .pos_x = 640,
    .pos_y = 0,
    .width = 80,
    .height = 80
};

struct ui_panel nav_btn_9 = {
    .color = 0xff000000,
    .t_dn = touch_test_1,
    .pos_x = 720,
    .pos_y = 0,
    .width = 80,
    .height = 80
};

struct ui_panel main_panel = {0};




struct ui_panel nav_panel = {
    .pos_x = 0,
    .pos_y = -80,
    .height = 80,
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


struct ui_panel root_panel = {
    .width = WIDTH - 1,
    .height = HEIGHT - 1,

    .children = (struct ui_panel*[]){
        &main_panel,
        &nav_panel,
        NULL,
    }
};

struct ui_panel *gui_build_root(void)
{
    LOG_W("init root children");

    LOG_W(" -- done\n");
    return &root_panel;
}
