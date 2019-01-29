#include "touch.h"

#include "draw.h"
#include "ui.h"

#include "../hud.h"
#include "../wayland.h"

#include <stdio.h>

static struct wl_touch *touch;

#define N_TOUCHES 20  // 20 aught to be enough for anyone!

static void touch_paint(int32_t x, int32_t y, int32_t id, bool mdown)
{
    if (x < 2 || x >= WIDTH - 2 || y < 2 || y >= HEIGHT - 2) {
        LOG_E("no\n");
        return;
    }

    static const uint32_t colors[] = {
        0xffff0000,
        0xff00ff00,
        0xff0000ff,
        0xff00ffff,
        0xffff00ff,
        0xffffff00,
    };
    static uint32_t map[N_TOUCHES] = {0};
    static uint8_t t = 0;

    if (mdown) {
        map[id] = colors[t++ % 6];
    }

    draw_dot_c(x, y, map[id % N_TOUCHES]);

    hud_surface_damage(x - 2, y - 2, 5, 5);
    hud_surface_commit();
}

/***************************************************************************************
 ***  Touch Interface                                                                ***
 ***************************************************************************************/
static struct touch {
    wl_fixed_t x;
    wl_fixed_t y;
} touches[N_TOUCHES] = {0};


static void touch_down(void *data, struct wl_touch *wl_touch, uint32_t serial, uint32_t time,
                       struct wl_surface *surface, int32_t id, wl_fixed_t f_x, wl_fixed_t f_y)
{
    (void) data;
    (void) wl_touch;
    (void) serial;
    (void) time;
    (void) surface;

    float x = wl_fixed_to_double(f_x);
    float y = wl_fixed_to_double(f_y);
    LOG_E("touch down %f %f %u\n", x, y, id);

    touch_paint(x, y, id, true);
}

static void touch_up_cb(void *data, struct wl_touch *wl_touch, uint32_t serial, uint32_t time, int32_t id)
{
    (void) data;
    (void) wl_touch;
    (void) serial;
    (void) time;
    (void) id;
    LOG_E("touch up\n");

    ui_touch_up(wl_fixed_to_int(touches[id % N_TOUCHES].x), wl_fixed_to_int(touches[id % N_TOUCHES].y));
}

static void touch_motion(void *data, struct wl_touch *wl_touch, uint32_t time, int32_t id, wl_fixed_t f_x,
                         wl_fixed_t f_y)
{
    (void) data;
    (void) wl_touch;
    (void) time;

    float x = wl_fixed_to_double(f_x);
    float y = wl_fixed_to_double(f_y);
    LOG_E("touch move %f %f %i\n", x, y, id);

    touches[id % N_TOUCHES].x = f_x;
    touches[id % N_TOUCHES].y = f_y;

    touch_paint(x, y, id, false);
}

static void touch_frame(void *data, struct wl_touch *wl_touch)
{
    (void) data;
    (void) wl_touch;
    LOG_E("touch frame\n");
}

static void touch_cancel(void *data, struct wl_touch *wl_touch)
{
    (void) data;
    (void) wl_touch;
    LOG_E("touch canceled\n");
}

static const struct wl_touch_listener touch_listener = {
    .down   = touch_down,
    .up     = touch_up_cb,
    .motion = touch_motion,
    .frame  = touch_frame,
    .cancel = touch_cancel
};

bool hud_touch_init(struct wl_seat *seat)
{
    if (!touch){
        LOG_E("setting keys\n");
        touch = wl_seat_get_touch(seat);
        wl_touch_add_listener(touch, &touch_listener, NULL);
        return true;
    }
    return false;
}

bool hud_touch_raze(void)
{
    if (touch) {
        LOG_E("killing keys\n");
        wl_touch_destroy(touch);
        touch = NULL;
        return true;
    }
    return false;
}
