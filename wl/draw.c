#include "draw.h"

#include "../hud.h"
#include "../wayland.h"
#include "../log.h"


#define CONSTRAIN_SIZE(x, y, pad) (x < pad || y < pad || x >= (WIDTH - pad) || y >= (HEIGHT) )


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
    if CONSTRAIN_SIZE(x, y, 2) {
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
    if (CONSTRAIN_SIZE(x, y, 0) && CONSTRAIN_SIZE(x, h, 1) ) {
        LOG_E("draw_vline no\n");
        LOG_E("vl %i %i %i\n", x, y, h);
        return;
    }

    // LOG_E("vl %i %i %i\n", x, y, h);
    uint32_t *p = root_pool_data->memory + x + y * WIDTH;
    for (int32_t rem = h - y; rem > 0; rem--) {
        p[0] = c;
        p[1] = c;
        p += WIDTH;
    }
}


void draw_hline_c(int32_t x, int32_t y, int32_t w, uint32_t c)
{
    // LOG_E("hl %i %i %i\n", x, y, w);
    if (CONSTRAIN_SIZE(x, y, 1) && CONSTRAIN_SIZE(x, w, 1) ) {
        LOG_E("draw_hline no\n");
        LOG_E("hl %i %i %i\n", x, y, w);
        return;
    }

    uint32_t *p = root_pool_data->memory + x + y * WIDTH;
    for (int rem = w - x; rem > 0; rem--) {
        *p++ = c;
    }
    p = root_pool_data->memory + x + y + 1 * WIDTH;
    for (int rem = w - x; rem > 0; rem--) {
        *p++ = c;
    }
    // p = root_pool_data->memory + (x - 1) + (y + 1) * WIDTH;
    // for (int rem = (w + 2); rem > 0; rem--) {
    //     *p++ = c;
    // }
}


void draw_square_c(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t c)
{
    if (w < x) {
        int32_t ww = w;
        w = x;
        x = ww;
    }

    if (h < y) {
        int32_t hh = h;
        h = y;
        y = hh;
    }

    if (CONSTRAIN_SIZE(x, y, 2) && CONSTRAIN_SIZE(w, h, 2) ) {
        LOG_E("draw_vline no\n");
        return;
    }

    uint32_t *p = root_pool_data->memory + x + y * WIDTH;
    for (int32_t draw_y = h - y; draw_y > 0; draw_y--) {
        for (int32_t draw_x = w - x; draw_x > 0; draw_x--) {
            *p++ = c;
        }
        p += WIDTH;
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
