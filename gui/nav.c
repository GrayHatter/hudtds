#include "nav.h"

#include "../wl/ui.h"
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


static bool touch_play_1(struct ui_panel *p, const int mx, const int my, const int x, const int y, const uint32_t w, uint32_t h,
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
    char *name = malloc(sizeof "./black.mp3");
    memcpy(name, "./black.mp3", sizeof "./black.mp3");
    postmsg_audio(AMSG_PLAY, name);

    return true;
}

static bool touch_play_2(struct ui_panel *p, const int mx, const int my, const int x, const int y, const uint32_t w, uint32_t h,
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

    char *name = malloc(sizeof "./dream.mp3");
    memcpy(name, "./dream.mp3", sizeof "./dream.mp3");
    postmsg_audio(AMSG_PLAY, name);

    return true;
}

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


struct ui_panel nav_btn_0 = {
    .color = 0xff888888,
    .name = "nav_btn_0",
    .t_dn = touch_test_1,
    .pos_x = 0,
    .pos_y = 0,
    .width = 80,
    .height = 80
};

struct ui_panel nav_btn_1 = {
    .color = 0xffff0000,
    .name = "nav_btn_1",
    .t_dn = touch_test_1,
    .pos_x = 80,
    .pos_y = 0,
    .width = 80,
    .height = 80
};

struct ui_panel nav_btn_2 = {
    .color = 0xffffffff,
    .name = "nav_btn_2",
    .t_dn = touch_test_1,
    .pos_x = 160,
    .pos_y = 0,
    .width = 80,
    .height = 80
};

struct ui_panel nav_btn_3 = {
    .color = 0xff00ff00,
    .name = "nav_btn_3",
    .t_dn = touch_test_1,
    .pos_x = 240,
    .pos_y = 0,
    .width = 80,
    .height = 80
};

struct ui_panel nav_btn_4 = {
    .color = 0xff00ffff,
    .name = "nav_btn_4",
    .t_dn = touch_test_1,
    .pos_x = 320,
    .pos_y = 0,
    .width = 80,
    .height = 80
};

struct ui_panel nav_btn_5 = {
    .color = 0xffff00ff,
    .name = "nav_btn_5",
    .t_dn = touch_test_1,
    .pos_x = 400,
    .pos_y = 0,
    .width = 80,
    .height = 80
};

struct ui_panel nav_btn_6 = {
    .color = 0xff88ffff,
    .name = "nav_btn_6",
    .t_dn = touch_test_1,
    .pos_x = 480,
    .pos_y = 0,
    .width = 80,
    .height = 80
};

struct ui_panel nav_btn_7 = {
    .color = 0xffff8888,
    .name = "nav_btn_7",
    .t_dn = touch_play_1,
    .pos_x = 560,
    .pos_y = 0,
    .width = 80,
    .height = 80
};

struct ui_panel nav_btn_8 = {
    .color = 0xff8888ff,
    .name = "nav_btn_8",
    .t_dn = touch_play_2,
    .pos_x = 640,
    .pos_y = 0,
    .width = 80,
    .height = 80
};

struct ui_panel nav_btn_9 = {
    .color = 0xff000000,
    .name = "nav_btn_9",
    .t_dn = touch_exit,
    .pos_x = 720,
    .pos_y = 0,
    .width = 80,
    .height = 80
};


struct ui_panel nav_frame = {
    .type = PANEL_FRAME,
    .name = "nav frame",
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
