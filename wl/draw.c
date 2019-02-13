#include "draw.h"

#include "../hud.h"
#include "../wayland.h"
#include "../log.h"

#include <stdbool.h>

#define CONSTRAIN_SIZE(x, y, pad) (x < pad || y < pad || x >= (WIDTH - pad) || y >= (HEIGHT) )
#define P draw_pixel

#define FIX_X() do {        \
    if (x > w) {            \
        int32_t ww = w;     \
        w = x;              \
        x = ww;             \
    }                       \
}  while(0)

#define FIX_Y() do {        \
    if (y > h) {            \
        int32_t hh = h;     \
        h = y;              \
        y = hh;             \
    }                       \
}  while(0)


#define FIX_XY() do {   \
    FIX_X();            \
    FIX_Y();            \
}  while(0)


struct clipping_box_t {
    bool set;
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
};

static struct clipping_box_t clip_box = {0};

void reset_cliping_box()
{
    if (clip_box.set) {
        // do cliping box memcpy
    }

    clip_box.set = false;
    clip_box.x = 0;
    clip_box.y = 0;
    clip_box.w = 0;
    clip_box.h = 0;
}


void set_cliping_box(int32_t x, int32_t y, int32_t w, int32_t h)
{
    if (x == w || y == h) {
        reset_cliping_box();
        return;
    }

    FIX_XY();

    clip_box.x = x;
    clip_box.y = y;
    clip_box.w = w;
    clip_box.h = h;
    clip_box.set = true;
}


static inline uint32_t *get_pixel(int32_t x, int32_t y)
{
    return root_pool_data->memory + x + y * WIDTH;
}

static bool draw_pixel(int32_t x, int32_t y, uint32_t c)
{
    if ((clip_box.set
            && (x < clip_box.x
                || x > clip_box.w
                || y < clip_box.y
                || y > clip_box.h))
        || y >= HEIGHT || y < 0
        || x >= WIDTH  || x < 0
    ) {
        return false;
    }

    uint32_t *p = get_pixel(x, y);
    *p = c;
    return true;
}


void draw_dot_c(int32_t x, int32_t y, uint32_t c)
{
    if (x < 2 || x >= WIDTH - 2 || y < 2 || y >= HEIGHT - 2) {
        LOG_E("draw_dot no\n");
        return;
    }

    uint32_t *p = root_pool_data->memory + (x - 2) + (y - 2) * WIDTH;
    p[2] = c;
    p += WIDTH;
    p[1] = c;
    p[2] = c;
    p[3] = c;
    p += WIDTH;
    p[0] = c;
    p[1] = c;
    p[2] = c;
    p[3] = c;
    p[4] = c;
    p += WIDTH;
    p[1] = c;
    p[2] = c;
    p[3] = c;
    p += WIDTH;
    p[2] = c;

    hud_surface_damage(x - 2, y - 2, 5, 5);
    hud_surface_commit();
}

void draw_dot(int32_t x, int32_t y)
{
    draw_dot_c(x, y, 0xff000000);
}


void draw_vline_c(int32_t x, int32_t y, int32_t h, uint32_t c)
{
    if (CONSTRAIN_SIZE(x, y, 0) && CONSTRAIN_SIZE(x, h, 1) ) {
        LOG_E("draw_vline no\n");
        LOG_E("vl %i %i %i\n", x, y, h);
        return;
    }
    FIX_Y();

    // LOG_E("vl %i %i %i\n", x, y, h);
    for (int32_t rem = h - y; rem > 0; rem--) {
        P(x, y + rem, c);
        P(x + 1, y + rem, c);
    }
}


void draw_hline_c(int32_t x, int32_t y, int32_t w, uint32_t c)
{
    // LOG_E("hl %i %i %i\n", x, y, w);
    if (CONSTRAIN_SIZE(x, y, 0) && CONSTRAIN_SIZE(w, y + 1, 0) ) {
        LOG_E("draw_hline no\n");
        LOG_E("hl %i %i %i\n", x, y, w);
        return;
    }

    for (int rem = w - x; rem > 0; rem--) {
        P(x + rem, y, c);
        P(x + rem, y + 1, c);
    }
}


void draw_square_c(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t c)
{
    if (CONSTRAIN_SIZE(x, y, 2) && CONSTRAIN_SIZE(w, h, 2) ) {
        LOG_E("draw_vline no\n");
        return;
    }
    for (int32_t draw_y = h; draw_y > y; draw_y--) {
        for (int32_t draw_x = w; draw_x > x; draw_x--) {
            P(draw_x, draw_y, c);
        }
    }
}


void draw_square(int32_t x, int32_t y, int32_t w, int32_t h) {
    return draw_square_c(x, y, w, h, 0xff000000);
}


void draw_box_c(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t c)
{
    LOG_D("draw box %i %i %i %i \n", x, y, w, h);
    draw_vline_c(x, y, h, c);
    draw_vline_c(w - 1, y, h, c);

    draw_hline_c(x, y, w, c);
    draw_hline_c(x, h - 1, w, c);

    hud_surface_damage(x, y, w, h);
    hud_surface_commit();
}


void draw_box(int32_t x, int32_t y, int32_t w, int32_t h) {
    return draw_box_c(x, y, w, h, 0xff000000);
}


void draw_circle(void) {}
