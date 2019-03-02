#ifndef _HUDTDS_AUDIO_SEARCH_H_
#define _HUDTDS_AUDIO_SEARCH_H_

#include <stdint.h>
#include <stdbool.h>

struct track_data {
    uint32_t length;
    char *filename;
    char *md_title;
    char *md_genre;

    int dir_id;
    int artist_id;
    int album_artist_id;
    int album_id;
    bool file_read;
};

struct artist_data {
    uint32_t length;
    char *name;
};

struct album_data {
    uint32_t length;
    char *name;
};

struct directory_data {
    uint32_t length;
    char *dirname;

    uint32_t track_count;
    struct track_data **tracks;

    // Includes all tracks in subdirectories.
    uint32_t track_count_total;
};

struct track_db {
    int count;
    int capacity;
    struct track_data *data;
};

struct artist_db {
    int count;
    int capacity;
    struct artist_data *data;
};

struct album_db {
    int count;
    int capacity;
    struct album_data *data;
};

struct directory_db {
    int count;
    int capacity;
    struct directory_data *data;
};

struct music_db {
    bool search_done;

    struct track_db     track_db;
    struct artist_db    artist_db;
    struct album_db     album_db;
    struct directory_db dir_db;
};


void *find_files_thread(void *db);


struct music_db *audio_db_get(void);

const struct track_data *find_track(int skip, const char *search);
const struct artist_data *find_artist(int skip, const char *search);
const struct album_data *find_album(int skip, const char *search);
const struct directory_data *find_dir(int skip, const char *search);

const struct track_data *audio_db_search_track(int skip, const char *search);
const struct track_data *audio_db_search_artist(int skip, const char *search);
const struct track_data *audio_db_search_album(int skip, const char *search);

const char *track_artist_name(const int id);
const char *track_album_name(const int id);

bool track_delete(int track_id);

#endif // _HUDTDS_AUDIO_SEARCH_H_
