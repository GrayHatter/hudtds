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

    int artist_id;
    int album_artist_id;
    char *md_title;
    char *md_album;
    char *md_genre;
};


struct artist_data {
    int length;
    char *name;
};

struct artist_db {
    int count;
    int capacity;
    struct artist_data *data;
};


void postmsg_audio(AUDIO_MSG msg, void *data);

void audio_thread_start(void);

struct audio_track *audio_track_get_current(void);
struct music_db *audio_db_get(void);

const char *track_artist_get(const int id);


int audio_track_add_metadata(struct audio_track *track);
void audio_track_free_metadata(struct audio_track *track);

#endif // _HUD_AUDIO_H_
