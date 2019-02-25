#include "music.h"

#include "../log.h"

#include "../wl/ui.h"
#include "../wl/draw.h"
#include "../wl/keyboard.h"



static bool music_btn_tracks(struct ui_panel *p, const int mx, const int my, const int x, const int y, const uint32_t w, uint32_t h,
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
    LOG_E("Music show tracks\n");
    return true;
}


static bool music_btn_artists(struct ui_panel *p, const int mx, const int my, const int x, const int y, const uint32_t w, uint32_t h,
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
    LOG_E("Music Show artists\n");
    return true;
}


static bool music_btn_albums(struct ui_panel *p, const int mx, const int my, const int x, const int y, const uint32_t w, uint32_t h,
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
    LOG_E("Music Show Albums\n");
    return true;
}


static void draw_button(struct ui_panel *p, int32_t x, int32_t y, int32_t w, int32_t h)
{
    x = p->pos_x < 0 ? w + p->pos_x : x + p->pos_x;
    y = p->pos_y < 0 ? h + p->pos_y : y + p->pos_y;
    w = p->width <= 0 ? w + p->width : x + p->width;
    h = p->height <= 0 ? h + p->height : y + p->height;

    if (p->focused && music_buttons_frame.focused) {
        draw_square_c(x, y, w, h, p->color);
    } else {
        draw_box_c(x, y, w, h, p->color);
    }
}


struct ui_panel music_btn_0 = {
    .name = "music_btn_0",
    .color = 0xff0000ff,
    .draw = draw_button,
    .t_dn = music_btn_tracks,
    .pos_x = 0,
    .pos_y = 0,
    .width = 80,
    .height = 80
};

struct ui_panel music_btn_1 = {
    .color = 0xff00ff00,
    .name = "music_btn_1",
    .draw = draw_button,
    .t_dn = music_btn_artists,
    .pos_x = 0,
    .pos_y = 80 * 1,
    .width = 80,
    .height = 80
};

struct ui_panel music_btn_2 = {
    .name = "music_btn_2",
    .color = 0xffff0000,
    .draw = draw_button,
    .t_dn = music_btn_albums,
    .pos_x = 0,
    .pos_y = 80 * 2,
    .width = 80,
    .height = 80
};

struct ui_panel music_btn_3 = {
    .name = "music_btn_3",
    .color = 0xffff00ff,
    .draw = draw_button,
    .t_dn = music_btn_tracks,
    .pos_x = 0,
    .pos_y = 80 * 3,
    .width = 80,
    .height = 80
};



static bool frame_key_down(struct ui_panel *p, const uint32_t key, const uint32_t s)
{
    (void) s;
    (void) key;
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
                        // if (cur_db_loc) {
                        //     cur_db_loc--;
                        // }
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
                        // cur_db_loc++;
                    }
                    break;
                }
                case MZD_KEYMAP_DPAD_CENTER: {
                    LOG_D("play this one %s\n", entry->panel.name);
                    // if (track_pos_get(entry->position)) {
                    //     postmsg_audio(AMSG_PLAY, track_pos_get(entry->position));
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


struct ui_panel music_buttons_frame = {
    .type = PANEL_LIST,
    .name = "music entry frame",
    .pos_x = 0,
    .pos_y = 60,
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
