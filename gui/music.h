#ifndef _HUDTDS_GUI_MUSIC_
#define _HUDTDS_GUI_MUSIC_

#include "../wl/ui.h"

extern struct ui_panel music_frame;
extern struct ui_panel music_buttons_frame;
extern struct ui_panel music_tracks_frame;
extern struct ui_panel music_artists_frame;
extern struct ui_panel music_albums_frame;

struct music_track_panel {
    struct ui_panel panel;

    uint32_t position;
};


#endif // _HUDTDS_GUI_MUSIC_
