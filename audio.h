#ifndef _HUD_AUDIO_H_
#define _HUD_AUDIO_H_

#include <stdbool.h>

typedef enum {
    AMSG_THREAD_EXIT = -1,
    AMSG_NONE = 0,
    AMSG_PLAY = 1,
    AMSG_PAUSE,
    AMSG_PLAY_PAUSE,
    AMSG_STOP,
    AMSG_CHANGE_TRACK,
    AMSG_SEEK_TO,
    AMSG_PREV,
    AMSG_RESTART,
    AMSG_NEXT,
    AMSG_TRACK_SCAN_DONE,
} AUDIO_MSG;


struct audio_track {
    bool file_read;

    char *filename;
    char *dirname;

    char *md_album_artist;
    char *md_artist;
    char *md_title;
    char *md_album;
    char *md_genre;
};


void postmsg_audio(AUDIO_MSG msg, void *data);

void audio_thread_start(void);

struct audio_track *audio_track_get_current(void);
struct music_db *audio_db_get(void);

int audio_track_add_metadata(struct audio_track *track);
void audio_track_free_metadata(struct audio_track *track);

#endif // _HUD_AUDIO_H_
