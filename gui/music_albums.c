#include "music.h"

#include "../wl/ui.h"
#include "../wl/draw.h"
#include "../wl/text.h"
#include "../wl/keyboard.h"

#include "../log.h"
#include "../audio.h"
#include "../audio_search.h"

#include <stdlib.h>


static uint32_t albums_skip = 0;

static const struct album_data *album_pos(uint32_t pos)
{
    pos += albums_skip;
    return find_album(pos, NULL);
}



static void draw_music_album(struct ui_panel *p, int32_t x, int32_t y, int32_t w, int32_t h)
{
    x = p->pos_x < 0 ? w + p->pos_x : x + p->pos_x; \
    y = p->pos_y < 0 ? h + p->pos_y : y + p->pos_y; \
    w = p->width <= 0 ? w + p->width : x + p->width; \
    h = p->height <= 0 ? h + p->height : y + p->height; \
    draw_square_c(x, y, w, y + p->height, 0xff000000);

    struct music_track *music = (struct music_track *)p;
    const struct album_data *album = album_pos(music->position);

    // LOG_E("Draw music entry (track: %s), %i %i %i %i (%i)\n", music->track_title, x, y, w, h, y + p->height);
    if (music_albums_frame.focused && p->focused) {
        draw_box_c(x, y, w, y + p->height, p->color);
    }

    text_draw_string(album->name, x + 3, y + 3);
}


struct music_track album_0 = {
    .panel = {
        .type = PANEL_LIST_ENTRY,
        .name = "music album_0",
        .draw = draw_music_album,
        .pos_x = 0,
        .pos_y = 0,
        .height = 40,
        .color = 0xffff0000,
        .children = NULL,
    },
    .position = 0
};

struct music_track album_1 = {
    .panel = {
        .type = PANEL_LIST_ENTRY,
        .name = "music album_1",
        .draw = draw_music_album,
        .pos_x = 0,
        .pos_y = 40,
        .height = 40,
        .color = 0xffff0000,
        .children = NULL,
    },
    .position = 1
};

struct music_track album_2 = {
    .panel = {
        .type = PANEL_LIST_ENTRY,
        .name = "music album_2",
        .draw = draw_music_album,
        .pos_x = 0,
        .pos_y = 80,
        .height = 40,
        .color = 0xffff0000,
        .children = NULL,
    },
    .position = 2
};

struct music_track album_3 = {
    .panel = {
        .type = PANEL_LIST_ENTRY,
        .name = "music album_3",
        .draw = draw_music_album,
        .pos_x = 0,
        .pos_y = 120,
        .height = 40,
        .color = 0xffff0000,
        .children = NULL,
    },
    .position = 3
};

struct music_track album_4 = {
    .panel = {
        .type = PANEL_LIST_ENTRY,
        .name = "music album_4",
        .draw = draw_music_album,
        .pos_x = 0,
        .pos_y = 160,
        .height = 40,
        .color = 0xffff0000,
        .children = NULL,
    },
    .position = 4
};

struct music_track album_5 = {
    .panel = {
        .type = PANEL_LIST_ENTRY,
        .name = "music album_5",
        .draw = draw_music_album,
        .pos_x = 0,
        .pos_y = 200,
        .height = 40,
        .color = 0xffff0000,
        .children = NULL,
    },
    .position = 5
};

struct music_track album_6 = {
    .panel = {
        .type = PANEL_LIST_ENTRY,
        .name = "music album_6",
        .draw = draw_music_album,
        .pos_x = 0,
        .pos_y = 240,
        .height = 40,
        .color = 0xffff0000,
        .children = NULL,
    },
    .position = 6
};



static bool frame_key_down(struct ui_panel *p, const uint32_t key, const uint32_t s)
{
    (void) s;
    struct music_track **children = (struct music_track**)p->children;

    if (!children) {
        return false;
    }
    struct music_track *first = *children, *entry, *prev;

    while ((entry = *children++)) {
        if (entry->panel.focused) {
            switch (key) {
                case MZD_KEYMAP_ROTATE_LEFT: {
                    if (entry != first) {
                        LOG_T("move to prev\n");
                        prev->panel.focused = true;
                        entry->panel.focused = false;
                    } else {
                        LOG_D("already at beginning\n");
                        if (albums_skip) {
                            albums_skip--;
                        }
                    }
                    break;
                }
                case MZD_KEYMAP_ROTATE_RIGHT: {
                    if (*children) {
                        LOG_T("move to next %s\n", (*children)->panel.name);
                        (*children)->panel.focused = true;
                        entry->panel.focused = false;
                    } else {
                        LOG_D("already at end\n");
                        albums_skip++;
                    }
                    break;
                }
                case MZD_KEYMAP_DPAD_CENTER: {
                    LOG_D("play this one %s\n", entry->panel.name);
                    // if (track_get_pos(entry->position)) {
                    //     postmsg_audio(AMSG_PLAY, track_get_pos(entry->position));
                    // } else {
                    //     LOG_E("No track found here %s\n", entry->panel.name);
                    // }
                    break;
                }
                default: {
                    LOG_D("frame_key_down default case\n");
                    return false;
                }
            }
            p->draw_needed = true;
            return true;
        }

        prev = entry;
    }

    LOG_D("focus not found\n");
    first->panel.focused = true;
    return false;
}


struct ui_panel music_albums_frame = {
    .type = PANEL_LIST,
    .name = "music artist frame",
    .pos_x = 80,
    .pos_y = 60,
    .height = -80,
    .k_dn = frame_key_down,
    .focused = false,
    .disabled = true,
    .children = (struct ui_panel*[]) {
        (struct ui_panel*)&album_0,
        (struct ui_panel*)&album_1,
        (struct ui_panel*)&album_2,
        (struct ui_panel*)&album_3,
        (struct ui_panel*)&album_4,
        (struct ui_panel*)&album_5,
        (struct ui_panel*)&album_6,
        NULL
    }
};
