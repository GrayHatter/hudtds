#include "gps.h"

#include "../wl/ui.h"
#include "../wl/text.h"
#include "../wl/draw.h"
#include "../log.h"

#include "../hud.h"
#include "../gps.h"

#include <math.h>


static void draw_gps_frame(struct ui_panel *p, int32_t x, int32_t y, int32_t w, int32_t h)
{
    x = p->pos_x < 0 ? w + p->pos_x : x + p->pos_x;
    y = p->pos_y < 0 ? h + p->pos_y : y + p->pos_y;
    w = p->width <= 0 ? w + p->width : x + p->width;
    h = p->height <= 0 ? h + p->height : y + p->height;

    LOG_T("starting draw\n");
    draw_square_c(x, y, w, h, 0xff303040);
}


static void draw_gps_data(struct ui_panel *p, int32_t x, int32_t y, int32_t w, int32_t h)
{
    x = p->pos_x < 0 ? w + p->pos_x : x + p->pos_x;
    y = p->pos_y < 0 ? h + p->pos_y : y + p->pos_y;
    w = p->width <= 0 ? w + p->width : x + p->width;
    h = p->height <= 0 ? h + p->height : y + p->height;

    struct hud_gps_data *data = gps_get_data();
    if (!data) {
        text_draw_string("ERROR, unable to get gps data!", x + 2, y + 20);
        return;
    }

    static char str[1024] = {0};

    snprintf(str, sizeof str, "LAT: %lf", data->lat);
    text_draw_string(str, x + 2, y + 10);

    snprintf(str, sizeof str, "LON: %lf", data->lon);
    text_draw_string(str, x + 2, y + 30);

    snprintf(str, sizeof str, "ALT: %lf", data->alt);
    text_draw_string(str, x + 2, y + 40);

    snprintf(str, sizeof str, "Ground Speed: %lf", data->gnd_speed);
    text_draw_string(str, x + 2, y + 80);

    snprintf(str, sizeof str, "Climb Speed: %lf", data->vert_speed);
    text_draw_string(str, x + 2, y + 100);

    snprintf(str, sizeof str, "Satellites: %i (%i)", data->satellites_visible, data->satellites_used);
    text_draw_string(str, x + 2, y + 130);

    free(data);
}


static void draw_gps_globe(struct ui_panel *p, int32_t x, int32_t y, int32_t w, int32_t h)
{
    x = p->pos_x < 0 ? w + p->pos_x : x + p->pos_x;
    y = p->pos_y < 0 ? h + p->pos_y : y + p->pos_y;
    w = p->width <= 0 ? w + p->width : x + p->width;
    h = p->height <= 0 ? h + p->height : y + p->height;

    int32_t hx = x + p->width  / 2;
    int32_t hy = y + h / 2;

    draw_circle_radius_c(hx, hy, 125, 0xffffffff);
    draw_circle_radius_c(hx, hy, 120, 0xff303040);

    draw_circle_radius_c(hx, hy, 60, 0xffffffff);
    draw_circle_radius_c(hx, hy, 58, 0xff303040);

    struct hud_gps_data *data = gps_get_data();
    for (int i = 0; i < data->satellites_visible && i < GPSCOUNT; i++) {
        draw_circle_radius_c(
            hx + (int)(sin(data->satellites[i].azimuth) * 100) + (90 - data->satellites[i].elevation),
            hy + (int)(cos(data->satellites[i].azimuth) * 100) + (90 - data->satellites[i].elevation),
            8,
            data->satellites[i].used ? 0xff00ff00 : 0xffff0000);
    }
}


struct ui_panel gps_data = {
    .name = "gps_frame",
    .draw = draw_gps_data,
    .focused = false,
    .disabled = false,
    .pos_x = 0,
    .pos_y = 0,
    .width = 530,
};


struct ui_panel gps_globe = {
    .name = "gps_frame",
    .draw = draw_gps_globe,
    .focused = false,
    .disabled = false,
    .pos_x = 530,
    .pos_y = 0,
    .width = 270,
};


struct ui_panel gps_frame = {
    .name = "gps_frame",
    .draw = draw_gps_frame,
    .focused = false,
    .disabled = true,
    .pos_x = 0,
    .pos_y = 0,
    .children = (struct ui_panel*[]) {
        (struct ui_panel*)&gps_data,
        (struct ui_panel*)&gps_globe,
        NULL
    }
};
