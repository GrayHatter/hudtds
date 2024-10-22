#include "music.h"

#include "../log.h"

#include "../ui.h"
#include "../wl/draw.h"
#include "../wl/keyboard.h"


static void _disable_all(void)
{
    music_tracks_frame.enabled = false;
    music_tracks_frame.focused = false;
    music_artists_frame.enabled = false;
    music_artists_frame.focused = false;

    music_buttons_frame.focused = false;
}


static bool music_btn_tracks(struct ui_panel *p, const int mx, const int my, const int x, const int y, const uint32_t w, uint32_t h,
    const uint32_t id, const uint32_t serial)
{
    (void) mx;
    (void) my;
    (void) x;
    (void) y;
    (void) w;
    (void) h;
    (void) id;
    (void) serial;
    LOG_N("Music show tracks\n");

    p->draw_needed = true;
    _disable_all();
    music_tracks_frame.enabled = true;
    music_tracks_frame.focused = true;

    return true;
}


static bool music_btn_artists(struct ui_panel *p, const int mx, const int my, const int x, const int y, const uint32_t w, uint32_t h,
    const uint32_t id, const uint32_t serial)
{
    (void) mx;
    (void) my;
    (void) x;
    (void) y;
    (void) w;
    (void) h;
    (void) id;
    (void) serial;
    LOG_N("Music Show artists\n");

    p->draw_needed = true;
    _disable_all();
    music_artists_frame.enabled = true;
    music_artists_frame.focused = true;

    return true;
}


static bool music_btn_albums(struct ui_panel *p, const int mx, const int my, const int x, const int y, const uint32_t w, uint32_t h,
    const uint32_t id, const uint32_t serial)
{
    (void) mx;
    (void) my;
    (void) x;
    (void) y;
    (void) w;
    (void) h;
    (void) id;
    (void) serial;
    LOG_N("Music Show Albums\n");

    p->draw_needed = true;
    _disable_all();
    music_albums_frame.enabled = true;
    music_albums_frame.focused = true;

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


static void d_artist(struct ui_panel *p, int32_t x, int32_t y, int32_t w, int32_t h)
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

    draw_set_clipping_box(x, y, w, h);

    draw_circle_radius_c(x + (w - x) / 2, h, 20, 0xffffffff);
    draw_circle_radius_c(x + (w - x) / 2, h - (w - x * 0.75), (w - x) * 0.33, 0xffffffff);

    draw_reset_clipping_box();
}


static void d_album(struct ui_panel *p, int32_t x, int32_t y, int32_t w, int32_t h)
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

    draw_circle_radius_c(x + (w - x) / 2, y + (h - y) / 2, 20, 0xffffffff);
    draw_circle_radius_c(x + (w - x) / 2, y + (h - y) / 2, 5,  0xff000000);
}


struct ui_panel music_btn_0 = {
    .name = "music_btn_0",
    .color = 0xff0000ff,
    .enabled = true,
    .draw = draw_button,
    .t_dn = music_btn_tracks,
    .pos_x = 0,
    .pos_y = 0,
    .width = 80,
    .height = 80
};

struct ui_panel music_btn_1 = {
    .name = "Music Button Artist",
    .color = 0xff00ff00,
    .enabled = true,
    .draw = d_artist,
    .t_dn = music_btn_artists,
    .pos_x = 0,
    .pos_y = 80 * 1,
    .width = 80,
    .height = 80
};

struct ui_panel music_btn_2 = {
    .name = "Music Button Album",
    .color = 0xffff0000,
    .enabled = true,
    .draw = d_album,
    .t_dn = music_btn_albums,
    .pos_x = 0,
    .pos_y = 80 * 2,
    .width = 80,
    .height = 80
};

struct ui_panel music_btn_3 = {
    .name = "music_btn_3",
    .color = 0xffff00ff,
    .enabled = true,
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
                    }
                    break;
                }
                case MZD_KEYMAP_DPAD_CENTER: {
                    LOG_D("play this one %s\n", entry->name);
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


struct ui_panel music_buttons_frame = {
    .type = PANEL_LIST,
    .name = "music entry frame",
    .enabled = true,
    .k_dn = frame_key_down,
    .pos_x = -80,
    .pos_y = 60,
    .height = -80,
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
