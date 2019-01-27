#include "draw.h"

#include "../hud.h"
#include "../wayland.h"


#define CONSTRAIN_XY(x, y, pad) (x < pad || y < pad || x >= (WIDTH - pad) || y >= (HEIGHT - 2) )


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

void draw_line_c(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t c)
{
    if CONSTRAIN_XY(x, y, 2) {
        LOG_E("no\n");
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

    hud_surface_damage(x - 2, y - 2, w - x + 2, h - y + 2);
    hud_surface_commit();
}

void draw_line(int32_t x, int32_t y, int32_t w, int32_t h)
{
    return draw_line_c(x, y, w, h, 0xff000000);
}


void draw_vline_c(int32_t x, int32_t y, int32_t h, uint32_t c)
{
    if (h < y) {
        int32_t hh = h;
        h = y;
        y = hh;
    }

    if (CONSTRAIN_XY(x, y, 2) && CONSTRAIN_XY(x, h, 2) ) {
        LOG_E("draw_vline no\n");
        return;
    }

    uint32_t *p = root_pool_data->memory + (x - 1) + (y - 1) * WIDTH;
    p[2] = c;
    p += WIDTH;
    p[1] = c;
    p[2] = c;
    p[3] = c;
    p += WIDTH;
    for (int32_t rem = h; rem > 0; rem--) {
        p[0] = c;
        p[1] = c;
        p[2] = c;
        p[3] = c;
        p[4] = c;
        p += WIDTH;
    }
    p[1] = c;
    p[2] = c;
    p[3] = c;
    p += WIDTH;
    p[2] = c;

    hud_surface_damage(x - 2, y - 2, x + 2, h - y + 2);
    hud_surface_commit();
}


void draw_hline_c(int32_t x, int32_t y, int32_t w, uint32_t c)
{
    if (w < x) {
        int32_t ww = w;
        w = x;
        x = ww;
    }

    if (CONSTRAIN_XY(x, y, 2) && CONSTRAIN_XY(x, w, 2) ) {
        LOG_E("draw_vline no\n");
        return;
    }

    uint32_t *p = root_pool_data->memory + (x - 1) + (y - 1) * WIDTH;
    p[2] = c;
    p += WIDTH;
    p[1] = c;
    p[2] = c;
    p[3] = c;
    p += WIDTH;
    for (int i = 0; i < 3; i++) {
        if (i == 1) {
            p--;
        }

        for (int32_t len = w; len > 0; len--) {
            *p++ = c;
        }

        if (i == 1) {
            *p++ = c;
            *p++ = c;
        }

        p += WIDTH;
    }
}


void draw_square(void) {}


void draw_box_c(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t c)
{
    draw_vline_c(x, y, h, c);
    draw_vline_c(w, y, h, c);

    draw_hline_c(w, y, h, c);

}

void draw_box(int32_t x, int32_t y, int32_t w, int32_t h) {
    return draw_box_c(x, y, w, h, 0xff000000);
}

void draw_circle(void) {}
