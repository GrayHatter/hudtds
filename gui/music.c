#include "music.h"

#include "../wl/ui.h"
#include "../wl/draw.h"
#include "../wl/text.h"
#include "../log.h"
// #include "../audio.h"

#include <stdlib.h>
// #include <string.h>

struct music_entry {
    struct ui_panel panel;

    char *track_title;
    char *track_artist;
    char *track_album;
    char *track_loc;
};


static void draw_music_frame(struct ui_panel *p, int32_t x, int32_t y, int32_t w, int32_t h)
{
    (void) p;
    (void) x;
    (void) y;
    (void) w;
    (void) h;
    LOG_E("Draw music frame, %i %i %i %i\n", x, y, w, h);

}


static void draw_music_entry(struct ui_panel *p, int32_t x, int32_t y, int32_t w, int32_t h)
{
    (void) p;
    (void) x;
    (void) y;
    (void) w;
    (void) h;
    LOG_E("Draw music entry, %i %i %i %i (%i)\n", x, y, w, h, y + p->height);

    draw_box_c(x, y, w, y + p->height, p->color);

    struct music_entry *music = (struct music_entry *)p;
    LOG_E("name %s\n", music->track_title);
    text_draw_string(music->track_title, x + 3, y + 3);
}


struct music_entry music_entry_0 = {
    .panel = {
        .type = PANEL_FRAME,
        .name = "music entry",
        .draw = draw_music_entry,
        .pos_x = 0,
        .pos_y = 0,
        .height = 60,
        .color = 0xffff00ff,
        .children = NULL,
    },

    .track_title = "test track",

};

struct ui_panel music_frame = {
    .type = PANEL_FRAME,
    .name = "music frame",
    .draw = draw_music_frame,
    .pos_x = 0,
    .pos_y = 50,
    .height = -80,
    .children = (struct ui_panel*[]) {
        (struct ui_panel*)&music_entry_0,
        NULL
    }
};
