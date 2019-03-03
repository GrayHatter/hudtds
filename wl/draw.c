#include "draw.h"

#include "../hud.h"
#include "../wayland.h"
#include "../log.h"

#include <stdbool.h>
#include <math.h>

#define CONSTRAIN_SIZE(x, y, pad) (x < pad || y < pad || x >= (WIDTH - pad) || y >= (HEIGHT) )
#define P draw_pixel


static uint32_t *draw_buffer = NULL;

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


void draw_swap_buffer(uint32_t *buffer)
{
    draw_buffer = buffer;
}


static inline uint32_t *get_pixel(int32_t x, int32_t y)
{
    return draw_buffer + x + y * WIDTH;
}


static uint8_t calculate_mix_8(uint8_t old, uint8_t new, double mix)
{
    if (old > new) {
        return old - ((old - new) * (1.0 - mix));
    } else {
        return old + ((new - old) * (1.0 - mix));
    }
}


static uint32_t calculate_mix_32(uint32_t old, uint32_t new, double mix)
{
    return 0xff000000
          | calculate_mix_8(old >> 16, new >> 16, mix) << 16
          | calculate_mix_8(old >> 8,  new >> 8,  mix) << 8
          | calculate_mix_8(old >> 0,  new >> 0,  mix);
}


bool draw_pixel(int32_t x, int32_t y, uint32_t c)
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


bool draw_pixel_mix(int32_t x, int32_t y, double mix, uint32_t c)
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

    if (mix <= 0.0) {
        *p = c;
    } else if (mix >= 1.0) {
        return true;
    } else {
        *p = calculate_mix_32(*p, c, mix);
    }
    return true;
}


void draw_dot_c(int32_t x, int32_t y, uint32_t c)
{
    if (x < 2 || x >= WIDTH - 2 || y < 2 || y >= HEIGHT - 2) {
        LOG_E("draw_dot no\n");
        return;
    }

    uint32_t *p = draw_buffer + (x - 2) + (y - 2) * WIDTH;
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
}


void draw_dot(int32_t x, int32_t y)
{
    draw_dot_c(x, y, 0xff000000);
}


void draw_vline_c(int32_t x, int32_t y, int32_t h, uint32_t c)
{
    FIX_Y();

    for (int32_t rem = h - y; rem > 0; rem--) {
        P(x, y + rem, c);
    }
}


void draw_hline_c(int32_t x, int32_t y, int32_t w, uint32_t c)
{
    FIX_X();

    for (int rem = w - x; rem > 0; rem--) {
        P(x + rem, y, c);
    }
}


void draw_square_c(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t c)
{
    for (int32_t draw_y = h; draw_y > y; draw_y--) {
        for (int32_t draw_x = w; draw_x > x; draw_x--) {
            P(draw_x, draw_y, c);
        }
    }
}


void draw_square(int32_t x, int32_t y, int32_t w, int32_t h)
{
    return draw_square_c(x, y, w, h, 0xff000000);
}


void draw_box_depth_c(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t d, uint32_t c)
{
    LOG_D("draw box %i %i %i %i \n", x, y, w, h);
    for (uint32_t i = 0; i < d; i++) {
        draw_hline_c(x, y + i, w , c);

        draw_vline_c(x + i, y, h, c);
        draw_vline_c(w - i, y, h, c);

        draw_hline_c(x, h - i, w, c);
    }
}


void draw_box_depth(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t d)
{
    return draw_box_depth_c(x, y, w, h, d, 0xff000000);
}



void draw_box_c(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t c)
{
    LOG_D("draw box %i %i %i %i \n", x, y, w, h);
    draw_box_depth_c(x, y, w, h, 2, c);
}


void draw_box(int32_t x, int32_t y, int32_t w, int32_t h)
{
    return draw_box_c(x, y, w, h, 0xff000000);
}


void draw_circle_c(int32_t x, int32_t y, int32_t w, uint32_t c)
{
    LOG_D("draw circle %i %i %i \n", x, y, w);

    double hw = (w / 2.0) - 0.5;
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < w; j++) {
            double di = (i - hw);
            double dj = (j - hw);
            double mix = sqrtf(di * di + dj * dj) - hw + 0.5;
            draw_pixel_mix(i + x, j + y, mix, c);
        }
    }
}


void draw_circle(int32_t x, int32_t y, int32_t w)
{
    return draw_circle_c(x, y, w, 0xff000000);
}


void draw_circle_radius_c(int32_t x, int32_t y, int32_t radius, uint32_t c)
{
    LOG_D("draw circle %i %i %i \n", x, y, radius);
    double r = radius - 0.5;
    int32_t ax = x - r;
    int32_t ay = y - r;
    for (int i = 0; i < r * 2; i++) {
        for (int j = 0; j < r * 2; j++) {
            double di = i - r;
            double dj = j - r;
            double mix = sqrtf(di * di + dj * dj) - r + 0.5;
            draw_pixel_mix(i + ax, j + ay, mix, c);
        }
    }
}


void draw_circle_radius(int32_t x, int32_t y, int32_t w)
{
    return draw_circle_radius_c(x, y, w, 0xff000000);
}

