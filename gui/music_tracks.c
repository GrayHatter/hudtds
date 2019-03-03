#include "music.h"

#include "../wl/ui.h"
#include "../wl/draw.h"
#include "../wl/text.h"
#include "../wl/keyboard.h"

#include "../log.h"
#include "../audio.h"
#include "../audio_search.h"

#include <stdlib.h>


static uint32_t cur_db_loc = 0;


static const struct track_data *track_get_pos(uint32_t pos)
{
    pos += cur_db_loc;
    return find_track(pos, NULL);
}


static const char *entry_text(const struct track_data *t)
{
    static char str[2048] = {0}; // because fuck memory safety!
    if (!t) {
        snprintf(str, 2048, "%s", "No Track Found");
        return str;
    }

    int pos = 0;
    if (t->file_read) {
        if (t->album_artist_id >= 0) {
            pos += snprintf(str, 2048, "%s - ", track_artist_name(t->album_artist_id));
        } else if (t->artist_id >= 0) {
            pos += snprintf(str, 2048, "%s - ", track_artist_name(t->artist_id));
        }

        if (t->md_title) {
            pos += snprintf(str + pos, 2048 - pos, "%s", t->md_title);
        } else {
            pos += snprintf(str + pos, 2048 - pos, "%s", t->filename);
        }
    }

    if (pos == 0) {
        if (t->filename) {
            snprintf(str, 2048, "file: %s", t->filename);
            return str;
        }

        snprintf(str, 2048, "%s", "Unable to generate name");
    }

    return str;
}


static void draw_music_track(struct ui_panel *p, int32_t x, int32_t y, int32_t w, int32_t h)
{
    x = p->pos_x < 0 ? w + p->pos_x : x + p->pos_x; \
    y = p->pos_y < 0 ? h + p->pos_y : y + p->pos_y; \
    w = p->width <= 0 ? w + p->width : x + p->width; \
    h = p->height <= 0 ? h + p->height : y + p->height; \
    draw_square_c(x, y, w, y + p->height, 0xff000000);

    // LOG_E("Draw music entry (track: %s), %i %i %i %i (%i)\n", music->track_title, x, y, w, h, y + p->height);
    if (music_tracks_frame.focused && p->focused) {
        draw_box_depth_c(x, y, w, y + p->height, 8, 0xff881010);
        draw_box_depth_c(x, y, w, y + p->height, 4, 0xff660505);
    }

    draw_box_depth_c(x, y, w, y + p->height, 2, 0xff440000);

    const struct track_data *track = track_get_pos(p->order);
    text_draw_string_width(entry_text(track), x + 8, y + 8, w - 8);
}


struct ui_panel music_track_0 = {
    .type = PANEL_LIST_ENTRY,
    .name = "music entry_0",
    .draw = draw_music_track,
    .pos_x = 0,
    .pos_y = 0,
    .height = 40,
    .color = 0xffff0000,
    .children = NULL,
    .order = 0,
};

struct ui_panel music_track_1 = {
    .type = PANEL_LIST_ENTRY,
    .name = "music entry_1",
    .draw = draw_music_track,
    .pos_x = 0,
    .pos_y = 40,
    .height = 40,
    .color = 0xffff0000,
    .children = NULL,
    .order = 1,
};

struct ui_panel music_track_2 = {
    .type = PANEL_LIST_ENTRY,
    .name = "music entry_2",
    .draw = draw_music_track,
    .pos_x = 0,
    .pos_y = 80,
    .height = 40,
    .color = 0xffff0000,
    .children = NULL,
    .order = 2,
};

struct ui_panel music_track_3 = {
    .type = PANEL_LIST_ENTRY,
    .name = "music entry_3",
    .draw = draw_music_track,
    .pos_x = 0,
    .pos_y = 120,
    .height = 40,
    .color = 0xffff0000,
    .children = NULL,
    .order = 3,
};

struct ui_panel music_track_4 = {
    .type = PANEL_LIST_ENTRY,
    .name = "music entry_4",
    .draw = draw_music_track,
    .pos_x = 0,
    .pos_y = 160,
    .height = 40,
    .color = 0xffff0000,
    .children = NULL,
    .order = 4,
};

struct ui_panel music_track_5 = {
    .type = PANEL_LIST_ENTRY,
    .name = "music entry_5",
    .draw = draw_music_track,
    .pos_x = 0,
    .pos_y = 200,
    .height = 40,
    .color = 0xffff0000,
    .children = NULL,
    .order = 5,
};

struct ui_panel music_track_6 = {
    .type = PANEL_LIST_ENTRY,
    .name = "music entry_6",
    .draw = draw_music_track,
    .pos_x = 0,
    .pos_y = 240,
    .height = 40,
    .color = 0xffff0000,
    .children = NULL,
    .order = 6,
};



static bool frame_key_down(struct ui_panel *p, const uint32_t key, const uint32_t s)
{
    (void) s;
    struct ui_panel **children = p->children;

    if (!children) {
        return false;
    }
    struct ui_panel *first = *children, *entry, *prev;

    while ((entry = *children++)) {
        if (entry->focused) {
            switch (key) {
                case MZD_KEYMAP_ROTATE_LEFT: {
                    if (entry != first) {
                        LOG_T("move to prev\n");
                        prev->focused = true;
                        entry->focused = false;
                    } else {
                        LOG_D("already at beginning\n");
                        if (cur_db_loc) {
                            cur_db_loc--;
                        }
                    }
                    break;
                }
                case MZD_KEYMAP_ROTATE_RIGHT: {
                    if (*children) {
                        LOG_T("move to next %s\n", (*children)->name);
                        (*children)->focused = true;
                        entry->focused = false;
                    } else {
                        LOG_D("already at end\n");
                        cur_db_loc++;
                    }
                    break;
                }
                case MZD_KEYMAP_DPAD_CENTER: {
                    LOG_D("play this one %s\n", entry->name);
                    if (track_get_pos(entry->order)) {
                        postmsg_audio(AMSG_PLAY, (void *)track_get_pos(entry->order));
                    } else {
                        LOG_E("No track found here %s\n", entry->name);
                    }
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
    first->focused = true;
    return false;
}


struct ui_panel music_tracks_frame = {
    .type = PANEL_LIST,
    .name = "music entry frame",
    .pos_x = 0,
    .pos_y = 60,
    .width = -80,
    .height = -80,
    .k_dn = frame_key_down,
    .focused = true,
    .children = (struct ui_panel*[]) {
        (struct ui_panel*)&music_track_0,
        (struct ui_panel*)&music_track_1,
        (struct ui_panel*)&music_track_2,
        (struct ui_panel*)&music_track_3,
        (struct ui_panel*)&music_track_4,
        (struct ui_panel*)&music_track_5,
        (struct ui_panel*)&music_track_6,
        NULL
    }
};
