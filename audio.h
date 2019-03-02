#ifndef _HUD_AUDIO_H_
#define _HUD_AUDIO_H_

#define _POSIX_C_SOURCE 200809L

#include <stdint.h>
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


void postmsg_audio(AUDIO_MSG msg, void *data);
void audio_thread_start(void);

struct track_data *audio_track_get_current(void);

#endif // _HUD_AUDIO_H_
