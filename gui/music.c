#include "music.h"

#include "../wl/ui.h"
#include "../wl/draw.h"
#include "../wl/text.h"
#include "../wl/keyboard.h"

#include "../log.h"
#include "../audio.h"

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
    // LOG_E("Draw music frame, %i %i %i %i\n", x, y, w, h);
}


static void draw_music_entry(struct ui_panel *p, int32_t x, int32_t y, int32_t w, int32_t h)
{
    x = p->pos_x < 0 ? w + p->pos_x : x + p->pos_x; \
    y = p->pos_y < 0 ? h + p->pos_y : y + p->pos_y; \
    w = p->width <= 0 ? w + p->width : x + p->width; \
    h = p->height <= 0 ? h + p->height : y + p->height; \


    struct music_entry *music = (struct music_entry *)p;
    // LOG_E("Draw music entry (track: %s), %i %i %i %i (%i)\n", music->track_title, x, y, w, h, y + p->height);
    if (p->focused) {
        draw_box_c(x, y, w, y + p->height, p->color);
    } else {
        draw_box_c(x, y, w, y + p->height, 0xff000000);
    }

    text_draw_string(music->track_title, x + 3, y + 3);
}


struct music_entry music_entry_0 = {
    .panel = {
        .type = PANEL_LIST_ENTRY,
        .name = "music entry_0",
        .draw = draw_music_entry,
        .pos_x = 0,
        .pos_y = 0,
        .height = 40,
        .color = 0xffff00ff,
        .children = NULL,
    },

    .track_title = "test track 00",
    .track_loc = "./black.mp3"

};

struct music_entry music_entry_1 = {
    .panel = {
        .type = PANEL_LIST_ENTRY,
        .name = "music entry_1",
        .draw = draw_music_entry,
        .pos_x = 0,
        .pos_y = 40,
        .height = 40,
        .color = 0xffff00ff,
        .children = NULL,
    },

    .track_title = "test track 01",
    .track_loc = "./dream.mp3"

};

struct music_entry music_entry_2 = {
    .panel = {
        .type = PANEL_LIST_ENTRY,
        .name = "music entry_2",
        .draw = draw_music_entry,
        .pos_x = 0,
        .pos_y = 80,
        .height = 40,
        .color = 0xffff00ff,
        .children = NULL,
    },

    .track_title = "test track 02",
    .track_loc = "./test.ogg"

};

struct music_entry music_entry_3 = {
    .panel = {
        .type = PANEL_LIST_ENTRY,
        .name = "music entry_3",
        .draw = draw_music_entry,
        .pos_x = 0,
        .pos_y = 120,
        .height = 40,
        .color = 0xffff00ff,
        .children = NULL,
    },

    .track_title = "test track 03",
    .track_loc = "./test2.ogg"

};

struct music_entry music_entry_4 = {
    .panel = {
        .type = PANEL_LIST_ENTRY,
        .name = "music entry_4",
        .draw = draw_music_entry,
        .pos_x = 0,
        .pos_y = 160,
        .height = 40,
        .color = 0xffff00ff,
        .children = NULL,
    },

    .track_title = "test track 04",
    .track_loc = "./black.mp3"

};

struct music_entry music_entry_5 = {
    .panel = {
        .type = PANEL_LIST_ENTRY,
        .name = "music entry_5",
        .draw = draw_music_entry,
        .pos_x = 0,
        .pos_y = 200,
        .height = 40,
        .color = 0xffff00ff,
        .children = NULL,
    },

    .track_title = "test track 05",
    .track_loc = "./dream.mp3"

};

struct music_entry music_entry_6 = {
    .panel = {
        .type = PANEL_LIST_ENTRY,
        .name = "music entry_6",
        .draw = draw_music_entry,
        .pos_x = 0,
        .pos_y = 240,
        .height = 40,
        .color = 0xffff00ff,
        .children = NULL,
    },

    .track_title = "test track 06",
    .track_loc = "./test.ogg"

};

struct music_entry music_entry_7 = {
    .panel = {
        .type = PANEL_LIST_ENTRY,
        .name = "music entry_7",
        .draw = draw_music_entry,
        .pos_x = 0,
        .pos_y = 280,
        .height = 40,
        .color = 0xffff00ff,
        .children = NULL,
    },

    .track_title = "test track 07",
    .track_loc = "./test2.ogg"

};

static bool frame_key_down(struct ui_panel *p, const uint32_t key, const uint32_t s)
{
    (void) s;
    struct music_entry **children = (struct music_entry**)p->children;
    if (key == MZD_KEYMAP_ROTATE_LEFT
            || key == MZD_KEYMAP_ROTATE_RIGHT
            || key == MZD_KEYMAP_DPAD_DOWN
            || key == MZD_KEYMAP_DPAD_UP
            || key == MZD_KEYMAP_DPAD_CENTER) {
        if (children) {
            bool found_focus = false;
            struct music_entry *first = *children, *entry, *prev;
            while ((entry = *children++)) {
                if (entry->panel.focused) {
                    found_focus = true;
                    switch (key) {
                        case MZD_KEYMAP_ROTATE_LEFT:
                        case MZD_KEYMAP_DPAD_UP: {
                            if (entry != first) {
                                prev->panel.focused = true;
                                entry->panel.focused = false;
                                LOG_T("move to prev\n");
                            } else {
                                LOG_D("already at beginning\n");
                                return false;
                            }
                            break;
                        }
                        case MZD_KEYMAP_ROTATE_RIGHT:
                        case MZD_KEYMAP_DPAD_DOWN: {
                            if (*children) {
                                LOG_T("move to next %s\n", (*children)->panel.name);
                                (*children)->panel.focused = true;
                                entry->panel.focused = false;
                            } else {
                                LOG_D("already at end\n");
                                return false;
                            }
                            break;
                        }
                        case MZD_KEYMAP_DPAD_CENTER: {
                            LOG_D("play this one %s (%s)\n", entry->track_title, entry->track_loc);
                            if (entry->track_loc) {
                                postmsg_audio(AMSG_PLAY, entry->track_loc);
                            }
                            break;
                        }
                        default: {
                            LOG_T("frame_key_down default case, can't happen!\n");
                            return false;
                        }
                    }
                    return true;
                }

                prev = entry;
            }
            if (!found_focus) {
                LOG_D("focus not found\n");
                first->panel.focused = true;
            }
        }
    }
    return false;
}

struct ui_panel music_frame = {
    .type = PANEL_LIST,
    .name = "music frame",
    .draw = draw_music_frame,
    .pos_x = 10,
    .pos_y = 50,
    .height = -80,
    .k_dn = frame_key_down,
    .focused = true,
    .children = (struct ui_panel*[]) {
        (struct ui_panel*)&music_entry_0,
        (struct ui_panel*)&music_entry_1,
        (struct ui_panel*)&music_entry_2,
        (struct ui_panel*)&music_entry_3,
        (struct ui_panel*)&music_entry_4,
        (struct ui_panel*)&music_entry_5,
        (struct ui_panel*)&music_entry_6,
        (struct ui_panel*)&music_entry_7,
        NULL
    }
};
