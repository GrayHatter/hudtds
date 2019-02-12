#ifndef _HUD_AUDIO_SEARCH_H_
#define _HUD_AUDIO_SEARCH_H_

#include <stdint.h>
#include <stdbool.h>


extern const char *supported_ext[];

struct music_track {
    bool file_read;

    char *filename;
};

struct music_dir {
    char *dirname;

    uint32_t dir_count;
    struct music_dir *subdirs;

    uint32_t track_count;
    struct music_track *tracks;
};

struct music_db {
    char *dir_name;

    uint32_t dir_count;
    struct music_dir *dirs;

    uint32_t track_count;
};



void *find_files_thread(void *db);


#endif // _HUD_AUDIO_SEARCH_H_
