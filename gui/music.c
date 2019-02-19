#include "music.h"

#include "../wl/ui.h"
#include "../wl/draw.h"
#include "../wl/text.h"
#include "../wl/keyboard.h"

#include "../log.h"
#include "../audio.h"
#include "../audio_search.h"

#include <stdlib.h>
// #include <string.h>

static uint32_t cur_db_loc = 0;


static struct audio_track *find_track(uint32_t pos, struct music_dir *dir)
{
    if (pos > dir->total_track_count) {
        return NULL;
    }

    if (pos < dir->track_count) {
        return &dir->tracks[pos];
    }

    pos -= dir->track_count;

    for (uint32_t i = 0; i < dir->dir_count; i++) {
        struct audio_track *track = find_track(pos, &dir->subdirs[i]);
        if (track) {
            return track;
        }
        pos -= dir[i].total_track_count;
    }

    LOG_E("Music track search can't happen\n");
    return NULL;
}


static struct audio_track *track_pos_get(uint32_t pos)
{
    pos += cur_db_loc;
    struct music_db *db = audio_db_get();
    if (!db) {
        return NULL;
    }

    struct music_dir *dir = db->dirs;
    if (pos > dir->total_track_count) {
        return NULL;
    }

    return find_track(pos, dir);
}


struct music_entry {
    struct ui_panel panel;

    uint32_t position;
};


static void draw_music_frame(struct ui_panel *p, int32_t x, int32_t y, int32_t w, int32_t h)
{
    (void) p;
    (void) x;
    (void) y;
    (void) w;
    (void) h;
    // LOG_E("Draw music frame, %i %i %i %i\n", x, y, w, h);
}


static void draw_music_playing(struct ui_panel *p, int32_t x, int32_t y, int32_t w, int32_t h)
{
    x = p->pos_x < 0 ? w + p->pos_x : x + p->pos_x; \
    y = p->pos_y < 0 ? h + p->pos_y : y + p->pos_y; \
    w = p->width <= 0 ? w + p->width : x + p->width; \
    h = p->height <= 0 ? h + p->height : y + p->height; \
    draw_square_c(x, y, w, y + p->height, 0xff333333);

    // LOG_E("Draw music entry (track: %s), %i %i %i %i (%i)\n", music->track_title, x, y, w, h, y + p->height);
    if (p->focused) {
        draw_box_c(x, y, w, y + p->height, p->color);
        draw_box_c(x + 2, y + 2, w - 2, y + p->height - 2, p->color);
    } else {
        draw_box_c(x, y, w, y + p->height, 0xff000000);
    }

    struct audio_track *track = audio_track_get_current();
    if (track) {
        text_draw_string(track->filename, x + 3, y + 3);
    } else {
        text_draw_string("No Song Playing", x + 3, y + 3);
    }
}


struct music_entry music_entry_playing = {
    .panel = {
        .type = PANEL_LIST_ENTRY,
        .name = "music entry playing",
        .draw = draw_music_playing,
        .pos_x = 80,
        .pos_y = 0,
        .height = 60,
        .color = 0xffff0000,
        .touch_focus = true,
    },
};


static void draw_music_entry(struct ui_panel *p, int32_t x, int32_t y, int32_t w, int32_t h)
{
    x = p->pos_x < 0 ? w + p->pos_x : x + p->pos_x; \
    y = p->pos_y < 0 ? h + p->pos_y : y + p->pos_y; \
    w = p->width <= 0 ? w + p->width : x + p->width; \
    h = p->height <= 0 ? h + p->height : y + p->height; \
    draw_square_c(x, y, w, y + p->height, 0xff000000);

    char *name = NULL;
    struct music_entry *music = (struct music_entry *)p;
    struct audio_track *track = track_pos_get(music->position);
    if (track) {
        name = track->filename;
    } else {
        name = "no track found";
    }

    // LOG_E("Draw music entry (track: %s), %i %i %i %i (%i)\n", music->track_title, x, y, w, h, y + p->height);
    if (p->focused) {
        draw_box_c(x, y, w, y + p->height, p->color);
    } else {
        draw_box_c(x, y, w, y + p->height, 0xff000000);
    }

    text_draw_string(name, x + 3, y + 3);
}


struct music_entry music_entry_0 = {
    .panel = {
        .type = PANEL_LIST_ENTRY,
        .name = "music entry_0",
        .draw = draw_music_entry,
        .pos_x = 0,
        .pos_y = 0,
        .height = 40,
        .color = 0xffff0000,
        .children = NULL,
    },
    .position = 0
};

struct music_entry music_entry_1 = {
    .panel = {
        .type = PANEL_LIST_ENTRY,
        .name = "music entry_1",
        .draw = draw_music_entry,
        .pos_x = 0,
        .pos_y = 40,
        .height = 40,
        .color = 0xffff0000,
        .children = NULL,
    },
    .position = 1
};

struct music_entry music_entry_2 = {
    .panel = {
        .type = PANEL_LIST_ENTRY,
        .name = "music entry_2",
        .draw = draw_music_entry,
        .pos_x = 0,
        .pos_y = 80,
        .height = 40,
        .color = 0xffff0000,
        .children = NULL,
    },
    .position = 2
};

struct music_entry music_entry_3 = {
    .panel = {
        .type = PANEL_LIST_ENTRY,
        .name = "music entry_3",
        .draw = draw_music_entry,
        .pos_x = 0,
        .pos_y = 120,
        .height = 40,
        .color = 0xffff0000,
        .children = NULL,
    },
    .position = 3
};

struct music_entry music_entry_4 = {
    .panel = {
        .type = PANEL_LIST_ENTRY,
        .name = "music entry_4",
        .draw = draw_music_entry,
        .pos_x = 0,
        .pos_y = 160,
        .height = 40,
        .color = 0xffff0000,
        .children = NULL,
    },
    .position = 4
};

struct music_entry music_entry_5 = {
    .panel = {
        .type = PANEL_LIST_ENTRY,
        .name = "music entry_5",
        .draw = draw_music_entry,
        .pos_x = 0,
        .pos_y = 200,
        .height = 40,
        .color = 0xffff0000,
        .children = NULL,
    },
    .position = 5
};

struct music_entry music_entry_6 = {
    .panel = {
        .type = PANEL_LIST_ENTRY,
        .name = "music entry_6",
        .draw = draw_music_entry,
        .pos_x = 0,
        .pos_y = 240,
        .height = 40,
        .color = 0xffff0000,
        .children = NULL,
    },
    .position = 6
};


struct ui_panel music_entry_frame;


static bool frame_key_down_main(struct ui_panel *p, const uint32_t key, const uint32_t s)
{
    (void) p;
    (void) s;

    switch (key) {
        case MZD_KEYMAP_DPAD_UP: {
            music_entry_playing.panel.focused = true;
            music_entry_frame.focused = false;
            return true;
        }
        case MZD_KEYMAP_DPAD_DOWN: {
            music_entry_playing.panel.focused = false;
            music_entry_frame.focused = true;
            return true;
        }
    }
    return false;
}


static bool frame_key_down(struct ui_panel *p, const uint32_t key, const uint32_t s)
{
    (void) s;
    struct music_entry **children = (struct music_entry**)p->children;

    if (!children) {
        return false;
    }
    struct music_entry *first = *children, *entry, *prev;

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
                        if (cur_db_loc) {
                            cur_db_loc--;
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
                        cur_db_loc++;
                    }
                    break;
                }
                case MZD_KEYMAP_DPAD_CENTER: {
                    LOG_D("play this one %s\n", entry->panel.name);
                    if (track_pos_get(entry->position)) {
                        postmsg_audio(AMSG_PLAY, track_pos_get(entry->position));
                    } else {
                        LOG_E("No track found here %s\n", entry->panel.name);
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
    first->panel.focused = true;
    return false;
}



static bool update_music_list(struct ui_panel *p, const int mx, const int my, const int x, const int y, const uint32_t w, uint32_t h,
    const uint32_t id, const uint32_t serial)
{
    (void) p;
    (void) mx;
    (void) my;
    (void) x;
    (void) y;
    (void) w;
    (void) h;
    (void) id;
    (void) serial;
    LOG_E("Update Music List\n");
    return true;
}


static void draw_button(struct ui_panel *p, int32_t x, int32_t y, int32_t w, int32_t h)
{
    x = p->pos_x < 0 ? w + p->pos_x : x + p->pos_x;
    y = p->pos_y < 0 ? h + p->pos_y : y + p->pos_y;
    w = p->width <= 0 ? w + p->width : x + p->width;
    h = p->height <= 0 ? h + p->height : y + p->height;

    draw_square_c(x, y, w, h, p->color);
}


struct ui_panel music_btn_0 = {
    .draw = draw_button,
    .name = "music_btn_0",
    .color = 0xff0000ff,
    .t_dn = update_music_list,
    .pos_x = 0,
    .pos_y = 0,
    .width = 80,
    .height = 80
};

struct ui_panel music_btn_1 = {
    .draw = draw_button,
    .color = 0xff00ff00,
    .name = "music_btn_1",
    .t_dn = update_music_list,
    .pos_x = 0,
    .pos_y = 80 * 1,
    .width = 80,
    .height = 80
};

struct ui_panel music_btn_2 = {
    .draw = draw_button,
    .name = "music_btn_2",
    .color = 0xffff0000,
    .t_dn = update_music_list,
    .pos_x = 0,
    .pos_y = 80 * 2,
    .width = 80,
    .height = 80
};

struct ui_panel music_btn_3 = {
    .draw = draw_button,
    .name = "music_btn_3",
    .color = 0xffff00ff,
    .t_dn = update_music_list,
    .pos_x = 0,
    .pos_y = 80 * 3,
    .width = 80,
    .height = 80
};

struct ui_panel music_button_frame = {
    .type = PANEL_LIST,
    .name = "music entry frame",
    .draw = draw_music_frame,
    .pos_x = 0,
    .pos_y = 0,
    .height = -80,
    .k_dn = frame_key_down,
    .focused = false,
    .touch_focus = false,
    .children = (struct ui_panel*[]) {
        (struct ui_panel*)&music_btn_0,
        (struct ui_panel*)&music_btn_1,
        (struct ui_panel*)&music_btn_2,
        (struct ui_panel*)&music_btn_3,
        NULL
    }
};

struct ui_panel music_entry_frame = {
    .type = PANEL_LIST,
    .name = "music entry frame",
    .draw = draw_music_frame,
    .pos_x = 80,
    .pos_y = 60,
    .height = -80,
    .k_dn = frame_key_down,
    .focused = false,
    .children = (struct ui_panel*[]) {
        (struct ui_panel*)&music_entry_0,
        (struct ui_panel*)&music_entry_1,
        (struct ui_panel*)&music_entry_2,
        (struct ui_panel*)&music_entry_3,
        (struct ui_panel*)&music_entry_4,
        (struct ui_panel*)&music_entry_5,
        (struct ui_panel*)&music_entry_6,
        NULL
    }
};

struct ui_panel music_frame = {
    .type = PANEL_LIST,
    .name = "music frame",
    .draw = draw_music_frame,
    .pos_x = 0,
    .pos_y = 40,
    .height = -80,
    .k_dn = frame_key_down_main,
    .focused = true,
    .children = (struct ui_panel*[]) {
        (struct ui_panel*)&music_button_frame,
        (struct ui_panel*)&music_entry_playing,
        (struct ui_panel*)&music_entry_frame,
        NULL
    }
};
