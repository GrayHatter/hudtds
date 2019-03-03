#define _POSIX_C_SOURCE 200809L
#include "audio_search.h"

#include "audio.h"
#include "hud.h"
#include "log.h"

#define _POSIX_C_SOURCE 200809L

#include <time.h>

#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <libavformat/avformat.h>


#define _10_mSECS 1000 * 1000 * 10


static const char *supported_ext[] = {
    "mp3",
    "ogg",
    "flac",
};


static struct music_db *m_db = NULL;


static struct track_data *track_get(int id)
{
    return &m_db->track_db.data[id];
}


static struct directory_data *directory_get(int id)
{
    return &m_db->dir_db.data[id];
}


static inline bool probably_music(char *filename)
{
    for (unsigned int i = 0; i < 3; i++) {
        int ext_l = strlen(supported_ext[i]);
        int name_l = strlen(filename);
        if (ext_l + 1 >= name_l) {
            return false;
        }
        if (memcmp(filename + name_l - ext_l, supported_ext[i], ext_l) == 0) {
            return true;
        }
    }
    return false;
}


static int push_track(struct music_db *m_db, const char *name)
{
    // TODO check for unused locations so array can shrink with deletions
    LOG_T("push_track called for %s\n", name);
    if (!name) {
        return -2;
    }

    uint32_t name_length = strlen(name);
    if (name_length == 0) {
        LOG_E("track push Name length shouldn't be 0\n");
        return -2;
    }

    if (m_db->track_db.count >= m_db->track_db.capacity) {
        struct track_data *next = realloc(m_db->track_db.data, sizeof(struct track_data) * (m_db->track_db.capacity + 10));
        if (!next) {
            LOG_F("Realloc failed for push track %s", name);
            exit(2);
        }

        m_db->track_db.data = next;
        m_db->track_db.capacity += 10;
        memset(&m_db->track_db.data[m_db->track_db.count], 0, sizeof (struct track_data) * 10);
    }

    m_db->track_db.data[m_db->track_db.count].length = name_length;
    m_db->track_db.data[m_db->track_db.count].filename = strdup(name);
    if (!m_db->track_db.data[m_db->track_db.count].filename) {
        LOG_E("Strdup failed in push_track\n");
        exit(2);
    }

    m_db->track_db.count++;
    return m_db->track_db.count - 1;
}


static int push_artist(struct music_db *m_db, const char *name)
{
    // TODO check for unused locations so array can shrink with deletions
    LOG_T("push_artist called for %s\n", name);
    if (!name) {
        return -2;
    }

    uint32_t name_length = strlen(name);
    if (name_length == 0) {
        LOG_E("artist push Name length shouldn't be 0\n");
        return -2;
    }

    for (int i = 0; i < m_db->artist_db.count; i++) {
        if (m_db->artist_db.data[i].length != name_length) {
            continue;
        }

        if (strcmp(name, m_db->artist_db.data[i].name) == 0) {
            return i;
        }
    }

    if (m_db->artist_db.count >= m_db->artist_db.capacity) {
        struct artist_data *next = realloc(m_db->artist_db.data, sizeof(struct artist_data) * (m_db->artist_db.capacity + 10));
        if (!next) {
            LOG_F("Realloc failed for push artist %s", name);
            exit(2);
        }

        m_db->artist_db.data = next;
        m_db->artist_db.capacity += 10;
        memset(&m_db->artist_db.data[m_db->artist_db.count], 0, sizeof (struct artist_data) * 10);
    }

    m_db->artist_db.data[m_db->artist_db.count].length = name_length;
    m_db->artist_db.data[m_db->artist_db.count].name = strdup(name);
    if (!m_db->artist_db.data[m_db->artist_db.count].name) {
        LOG_E("Strdup failed in push_artist\n");
        exit(2);
    }

    m_db->artist_db.count++;
    return m_db->artist_db.count - 1;

    return -1;
}


static int push_album(struct music_db *m_db, const char *name)
{
    // TODO check for unused locations so array can shrink with deletions
    LOG_T("push_album called for %s\n", name);
    if (!name) {
        return -2;
    }

    uint32_t name_length = strlen(name);
    if (name_length == 0) {
        LOG_E("album push Name length shouldn't be 0\n");
        return -2;
    }

    for (int i = 0; i < m_db->album_db.count; i++) {
        if (m_db->album_db.data[i].length != name_length) {
            continue;
        }

        if (strcmp(name, m_db->album_db.data[i].name) == 0) {
            return i;
        }
    }

    if (m_db->album_db.count >= m_db->album_db.capacity) {
        struct album_data *next = realloc(m_db->album_db.data, sizeof(struct album_data) * (m_db->album_db.capacity + 10));
        if (!next) {
            LOG_F("Realloc failed for push album %s", name);
            exit(2);
        }

        m_db->album_db.data = next;
        m_db->album_db.capacity += 10;
        memset(&m_db->album_db.data[m_db->album_db.count], 0, sizeof (struct album_data) * 10);
    }

    m_db->album_db.data[m_db->album_db.count].length = name_length;
    m_db->album_db.data[m_db->album_db.count].name = strdup(name);
    if (!m_db->album_db.data[m_db->album_db.count].name) {
        LOG_E("Strdup failed in push_album\n");
        exit(2);
    }

    m_db->album_db.count++;
    return m_db->album_db.count - 1;

    return -1;
}


static int push_dir(struct music_db *m_db, const char *name)
{
    // TODO check for unused locations so array can shrink with deletions
    LOG_T("push_dir called for %s\n", name);
    if (!name) {
        return -1;
    }

    uint32_t name_length = strlen(name);
    if (name_length == 0) {
        LOG_E("dir push Name length shouldn't be 0\n");
        return -2;
    }

    for (int i = 0; i < m_db->dir_db.count; i++) {
        if (strcmp(name, m_db->dir_db.data[i].dirname) == 0) {
            if (m_db->dir_db.data[i].length == name_length) {
                return i;
            }
        }
    }

    if (m_db->dir_db.count >= m_db->dir_db.capacity) {
        struct directory_data *next = realloc(m_db->dir_db.data, sizeof (struct directory_data) * (m_db->dir_db.capacity + 10));
        if (!next) {
            LOG_F("Realloc failed for push dir %s", name);
            exit(2);
        }
        m_db->dir_db.data = next;
        m_db->dir_db.capacity += 10;
        memset(&m_db->dir_db.data[m_db->dir_db.count], 0, sizeof (struct directory_data) * 10);
    }

    m_db->dir_db.data[m_db->dir_db.count].length = name_length;
    m_db->dir_db.data[m_db->dir_db.count].dirname = strdup(name);
    if (!m_db->dir_db.data[m_db->dir_db.count].dirname) {
        LOG_E("Strdup failed in push_dir\n");
        exit(2);
    }

    m_db->dir_db.count++;
    return m_db->dir_db.count - 1;
}


// TODO optimize
static int search_dir(struct music_db *m_db, const char *dirname)
{
    DIR *d = opendir(dirname);
    struct dirent *entry;

    int dir_id = -1;

    while ((entry = readdir(d))) {
        if (entry->d_name[0] == '.') {
            continue;
        }

        LOG_T("entry name %s\n", entry->d_name);

        char *filename = expand_dirname(dirname, entry->d_name);
        if (!filename) {
            LOG_E("Unable to expand_dirname\n");
        }

        struct stat s;
        if (!stat(filename, &s)) {
            if (S_ISREG(s.st_mode)) {
                if (probably_music(filename)) {
                    if (dir_id < 0) {
                        dir_id = push_dir(m_db, dirname);
                    }

                    int track_id = push_track(m_db, entry->d_name);
                    if (track_id >= 0 && dir_id >= 0) {
                        struct track_data *track = track_get(track_id);
                        track->dir_id = dir_id;
                        // TODO increment directory total track count
                    }
                }
            } else if (S_ISDIR(s.st_mode)) {
                LOG_T("desending %s\n", filename);
                int child_id = search_dir(m_db, filename);
                if (child_id >= 0 && dir_id < 0) {
                    dir_id = push_dir(m_db, dirname);
                }
            }
        }
        free(filename);
    }
    closedir(d);

    return dir_id;
}


static bool dir_exists(const char *dirname)
{
    if (!dirname || dirname[0] != '/') {
        return false;
    }

    struct stat s;
    if (!stat(dirname, &s)) {
        if (S_ISDIR(s.st_mode)) {
            return true;
        }
    }

    return false;
}


static int metadata_add(struct track_data *track)
{
    LOG_T("Trying to add metadata for track ");
    if (!track) {
        LOG_W("Track give was null\n");
        return 0;
    }

    if (!track->dir_id || !track->filename) {
        return -1;
    }

    struct directory_data *dir = directory_get(track->dir_id);

    LOG_T("dirname %s, filename %s\n", dir->dirname, track->filename);

    char *dirname  = expand_dirname(dir->dirname, track->filename);
    if (!dirname) {
        LOG_F("Unable to expand dirname for track %s\n", track->filename);
        exit(2);
    }

    AVFormatContext *file = avformat_alloc_context();
    if (avformat_open_input(&file, dirname, NULL, NULL) < 0) {
        LOG_E("Could not open file\n");
        free(dirname);
        return -2;
    }

    AVDictionaryEntry *dict = NULL;
    if ((dict = av_dict_get(file->metadata, "album_artist", NULL, 0))) {
        track->album_artist_id = push_artist(m_db, dict->value);
    } else if ((dict = av_dict_get(file->metadata, "artist", NULL, 0))) {
        track->album_artist_id = push_artist(m_db, dict->value);
    }

    if ((dict = av_dict_get(file->metadata, "artist", NULL, 0))) {
        track->artist_id = push_artist(m_db, dict->value);
    } else {
        track->artist_id = track->album_artist_id;
    }

    if ((dict = av_dict_get(file->metadata, "title", NULL, 0))) {
        track->md_title = strdup(dict->value);
    }

    if ((dict = av_dict_get(file->metadata, "album", NULL, 0))) {
        track->album_id = push_album(m_db, dict->value);
    }

    if ((dict = av_dict_get(file->metadata, "genre", NULL, 0))) {
        track->md_genre = strdup(dict->value);
    }

    track->file_read = true;

    avformat_close_input(&file);
    avformat_free_context(file);
    free(dirname);
    return 0;
}


static void metadata_free(struct track_data *track)
{
    if (track->md_title) {
        free(track->md_title);
        track->md_title = NULL;
    }

    if (track->md_genre) {
        free(track->md_genre);
        track->md_genre = NULL;
    }

    track->file_read = false;
}


void *find_files_thread(void *p)
{
    LOG_N("Audio search starting up\n");
    (void) p;

    m_db = calloc(1, sizeof (struct music_db));
    if (!m_db) {
        LOG_F("music_db Calloc failed\n");
        exit(2);
    }

    static const char *name = "/tmp/mnt/sda1";

    if (!dir_exists(name)) {
        LOG_E("Error for dir %s, doesn't exist!\n", name);
        return NULL;
    }

    search_dir(m_db, name);

    m_db->search_done = true;
    postmsg_audio(AMSG_TRACK_SCAN_DONE, NULL);


    struct timespec __ts_nanosleep = { .tv_nsec = _10_mSECS };
    const struct track_data *track;
    int pos = 0;
    while (pos < m_db->track_db.count) {
        track = track_get(pos++);
        metadata_add((struct track_data *)track);
        nanosleep(&__ts_nanosleep, NULL);
    }
    return NULL;
}


const struct track_data *find_track(int skip, const char *search)
{
    if (!m_db) {
        return NULL;
    }

    if (!m_db->track_db.count || !m_db->track_db.data) {
        return NULL;
    }

    if (skip >= m_db->track_db.count || skip < 0) {
        return NULL;
    }

    if (!search) {
        return track_get(skip);
    }

    struct track_db *track = &m_db->track_db;

    for (int i = 0; i < track->count; i++) {
        if (!strcmp(search, track->data[i].filename)) {
            // TODO or track name
            if (strlen(search) <= track->data[i].length) {
                if (skip--) {
                    continue;
                }

                return &track->data[i];
            }
        }
    }

    return NULL;
}


const struct artist_data *find_artist(int skip, const char *search)
{
    if (!m_db) {
        return NULL;
    }

    struct artist_db *artists = &m_db->artist_db;

    if (!search) {
        if (skip < artists->count) {
            return &artists->data[skip];
        }
        return NULL;
    }

    for (int i = 0; i < artists->count; i++) {
        if (!strcmp(search, artists->data[i].name)) {
            if (strlen(search) <= artists->data[i].length) {
                if (skip--) {
                    continue;
                }
                return &artists->data[i];
            }
            }
    }

    return NULL;
}


const struct album_data *find_album(int skip, const char *search)
{
    struct album_db *album = &m_db->album_db;

    if (!search) {
        if (skip < album->count) {
            return &album->data[skip];
        }
        return NULL;
    }

    for (int i = 0; i < album->count; i++) {
        if (!strcmp(search, album->data[i].name)) {
            if (strlen(search) <= album->data[i].length) {
                if (skip--) {
                    continue;
                }

                return &album->data[i];
            }
        }
    }

    return NULL;
}


const struct directory_data *find_dir(int skip, const char *search)
{
    struct directory_db *dir = &m_db->dir_db;

    if (!search) {
        if (skip < dir->count) {
            return directory_get(skip);
        }
        return NULL;
    }

    for (int i = 0; i < dir->count; i++) {
        if (!strcmp(search, dir->data[i].dirname)) {
            if (strlen(search) <= dir->data[i].length) {
                if (skip--) {
                    continue;
                }

                return &dir->data[i];
            }
        }
    }

    return NULL;
}


const struct track_data *audio_db_search_track(int skip, const char *search)
{
    (void) skip;
    (void) search;
    return NULL;
}


const struct track_data *audio_db_search_artist(int skip, const char *search)
{
    (void) skip;
    (void) search;
    return NULL;
}


const struct track_data *audio_db_search_album(int skip, const char *search)
{
    (void) skip;
    (void) search;
    return NULL;
}


const char *track_artist_name(const int id)
{
    if (id < 0 || id >= m_db->artist_db.count) {
        return NULL;
    }

    return m_db->artist_db.data[id].name;
}


const char *track_album_name(const int id)
{
    if (id < 0 || id >= m_db->album_db.count) {
        return NULL;
    }

    return m_db->album_db.data[id].name;
}


bool track_delete(int track_id)
{
    // TODO input validation
    struct track_data *track = track_get(track_id);
    if (track) {
        metadata_free(track);
        return true;
    }

    return false;
}
