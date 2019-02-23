#include "gps.h"

#include "../wl/ui.h"
#include "../wl/text.h"
#include "../wl/draw.h"
#include "../log.h"

#include "../hud.h"
#include "../gps.h"


static void draw_gps_frame(struct ui_panel *p, int32_t x, int32_t y, int32_t w, int32_t h)
{
    (void) p;
    static char str[1024] = {0};

    x = p->pos_x < 0 ? w + p->pos_x : x + p->pos_x;
    y = p->pos_y < 0 ? h + p->pos_y : y + p->pos_y;
    w = p->width <= 0 ? w + p->width : x + p->width;
    h = p->height <= 0 ? h + p->height : y + p->height;

    LOG_T("starting draw\n");

    draw_square_c(x, y, w, h, 0xff303040);
    text_draw_string("GPS!", x + 2, y + 2);

    struct hud_gps_data *data = gps_get_data();
    if (!data) {
        text_draw_string("ERROR, unable to get gps data!", x + 2, y + 20);
        return;
    }

    snprintf(str, sizeof str, "LAT: %lf", data->lat);
    text_draw_string(str, x + 2, y + 20);

    snprintf(str, sizeof str, "LON: %lf", data->lon);
    text_draw_string(str, x + 2, y + 40);

    snprintf(str, sizeof str, "ALT: %lf", data->alt);
    text_draw_string(str, x + 2, y + 60);

    snprintf(str, sizeof str, "Ground Speed: %lf", data->gnd_speed);
    text_draw_string(str, x + 2, y + 90);

    snprintf(str, sizeof str, "Climb Speed: %lf", data->vert_speed);
    text_draw_string(str, x + 2, y + 110);

    snprintf(str, sizeof str, "Satellites: %i (%i)", data->num_sats_visible, data->num_sats_connected);
    text_draw_string(str, x + 2, y + 140);


    LOG_T("ending draw \n");

    free(data);
}


struct ui_panel gps_frame = {
    .name = "gps_frame",
    .draw = draw_gps_frame,
    .focused = false,
    .disabled = true,
    .pos_x = 0,
    .pos_y = 40,

    .children = NULL,
};
