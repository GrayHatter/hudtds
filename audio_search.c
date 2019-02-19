#include "audio_search.h"

#include "audio.h"
#include "hud.h"
#include "log.h"

#define __USE_XOPEN_EXTENDED

#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


const char *supported_ext[] = {
    "mp3",
    "ogg",
    "flac",
};


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


// static int add_file(struct music_db *m_dir, char *filename)
// {
//     (void) m_dir;
//     if (probably_music(filename)) {
//         LOG_D("Music %s\n", filename);
//         return 1;
//     }
//     return 0;
// }


static inline struct music_dir *ensure_dir(struct music_dir *dir)
{
    if (dir) {
        return dir;
    }

    dir = calloc(1, sizeof (struct music_dir));
    if (!dir) {
        LOG_E("Unable to calloc for music_dir\n");
        exit(10);
    }
    return dir;
}


// TODO optimize
static struct music_dir *search_dir(const char *dirname)
{
    DIR *d = opendir(dirname);
    struct dirent *entry;

    struct music_dir *thisdir = NULL;

    while((entry = readdir(d))) {
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
                    thisdir = ensure_dir(thisdir);

                    if (!thisdir->track_count) {
                        thisdir->tracks = calloc(1, sizeof(struct audio_track));
                        if (!thisdir->tracks) {
                            LOG_E("can't alloc for tracks\n");
                            free(filename);
                            return NULL;
                        }
                    } else {
                        struct audio_track *next = realloc(thisdir->tracks, sizeof (struct audio_track) * (thisdir->track_count + 1));
                        if (!next) {
                            free(filename);
                            return NULL;
                        }
                        thisdir->tracks = next;
                    }
                    thisdir->tracks[thisdir->track_count].filename = strdup(entry->d_name);
                    thisdir->tracks[thisdir->track_count++].dirname = strdup(dirname);
                    thisdir->total_track_count++;
                }
            } else if (S_ISDIR(s.st_mode)) {
                LOG_T("desending %s\n", filename);
                struct music_dir *child = search_dir(filename);
                if (child) {
                    thisdir = ensure_dir(thisdir);
                    if (child->dir_count || child->track_count) {
                        struct music_dir *next = realloc(thisdir->subdirs, sizeof (struct music_dir) * (thisdir->dir_count + 1));
                        if (!next) {
                            free(filename);
                            return NULL;
                        }
                        thisdir->subdirs = next;
                        memcpy(&thisdir->subdirs[thisdir->dir_count++], child, sizeof (struct music_dir));
                        thisdir->total_track_count += child->total_track_count;
                        free(child);
                    }
                }
            }
        }
        free(filename);
    }
    closedir(d);

    if (thisdir) {
        thisdir->dirname = strdup(dirname);
        LOG_T("Completed %s (with %i tracks and %i dirs)\n", dirname, thisdir->track_count, thisdir->dir_count);
    } else {
        LOG_T("Completed %s (with NO tracks)\n", dirname);
    }
    return thisdir;
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


void *find_files_thread(void *db_)
{
    static const char *name = "/tmp/mnt/sda1/music";
    LOG_E("Audio search starting up\n");
    struct music_db *db = db_;

    if (!dir_exists(name)) {
        LOG_E("Error for dir %s, doesn't exist!\n", name);
        return NULL;
    }

    db->dir_count = 1;
    db->dirs = search_dir(name);

    LOG_D("dir count %i\n", db->dir_count);
    LOG_D("subdir %p \n", db->dirs);
    LOG_D("dir name %s\n", db->dirs[0].dirname);
    LOG_D("subdir count %i\n", db->dirs[0].dir_count);
    LOG_D("subdir track count %i\n", db->dirs[0].track_count);
    LOG_D("subdir track 0 name %s\n", db->dirs[0].tracks[0].filename);
    LOG_D("subdir track 0 location %s\n", expand_dirname(db->dirs[0].dirname, db->dirs[0].tracks[0].filename));


    db->search_done = true;
    postmsg_audio(AMSG_TRACK_SCAN_DONE, db);
    return NULL;
}
