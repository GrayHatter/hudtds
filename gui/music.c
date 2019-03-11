#include "music.h"

#include "../ui.h"

#include "../wl/draw.h"
#include "../wl/text.h"
#include "../wl/keyboard.h"

#include "../log.h"
#include "../audio.h"
#include "../audio_search.h"


#include <stdlib.h>


static void draw_music_playing(struct ui_panel *p, int32_t x, int32_t y, int32_t w, int32_t h)
{
    x = p->pos_x < 0 ? w + p->pos_x : x + p->pos_x;
    y = p->pos_y < 0 ? h + p->pos_y : y + p->pos_y;
    w = p->width <= 0 ? w + p->width : x + p->width;
    h = p->height <= 0 ? h + p->height : y + p->height;

    // LOG_E("Draw music entry (track: %s), %i %i %i %i (%i)\n", music->track_title, x, y, w, h, y + p->height);
    if (p->focused) {
        draw_square_c(x, y, w, y + p->height, 0xff666666);
    } else {
        draw_square_c(x, y, w, y + p->height, 0xff111111);
    }

    struct track_data *track = audio_track_get_current();
    if (track) {
        text_string(track->filename, x + 3, y + 3);
    } else {
        text_string("No Song Playing", x + 3, y + 3);
    }
}


static bool music_playing_kdn(struct ui_panel *p, const uint32_t key, const uint32_t s)
{
    p->draw_needed = true;
    (void) s;

    switch (key) {
        case MZD_KEYMAP_DPAD_CENTER: {
            postmsg_audio(AMSG_PAUSE, NULL);
            return true;
        }
    }
    return false;
}



struct ui_panel music_track_playing = {
    .type = PANEL_LIST_ENTRY,
    .name = "music entry playing",
    .draw = draw_music_playing,
    .k_dn = music_playing_kdn,
    .enabled = true,
    .pos_x = 0,
    .pos_y = 0,
    .height = 60,
    .touch_focus = true,
    .order = 0,
};


static bool frame_key_down_main(struct ui_panel *panel, const uint32_t key, const uint32_t s)
{
    panel->draw_needed = true;
    (void) s;

    switch (key) {
        case MZD_KEYMAP_DPAD_UP: {
            LOG_T("music_frame_keydown dpad up\n");
            if (music_track_playing.focused == true) {
                return false;
            } else {
                music_track_playing.focused = true;
                music_buttons_frame.focused = false;
                music_tracks_frame.focused = false;
                return true;
            }
        }
        case MZD_KEYMAP_DPAD_DOWN: {
            LOG_T("music_frame_keydown dpad dn\n");
            if (music_tracks_frame.focused == true) {
                return false;
            } else {
                music_tracks_frame.focused = true;
                music_track_playing.focused = false;
                music_buttons_frame.focused = false;
                return true;
            }
        }
        case MZD_KEYMAP_DPAD_LEFT: {
            LOG_T("music_frame_keydown dpad left\n");
            if (music_tracks_frame.focused == true) {
                return false;
            } else {
                music_tracks_frame.focused = true;
                music_track_playing.focused = false;
                music_buttons_frame.focused = false;
                return true;
            }
        }
        case MZD_KEYMAP_DPAD_RIGHT: {
            LOG_T("music_frame_keydown dpad right\n");
            if (music_buttons_frame.focused == true) {
                return false;
            } else {
                music_buttons_frame.focused = true;
                music_track_playing.focused = false;
                music_tracks_frame.focused = false;
                return true;
            }
        }
    }
    return false;
}


struct ui_panel music_frame = {
    .type = PANEL_LIST,
    .name = "music frame",
    .pos_x = 0,
    .pos_y = 0,
    .height = -80,
    .k_dn = frame_key_down_main,
    .focused = true,
    .enabled = true,

    .children = (struct ui_panel*[]) {
        (struct ui_panel*)&music_track_playing,
        (struct ui_panel*)&music_tracks_frame,
        (struct ui_panel*)&music_buttons_frame,
        (struct ui_panel*)&music_artists_frame,
        (struct ui_panel*)&music_albums_frame,
        NULL
    }
};
