#include "sound.h"

#include "log.h"

#define _XOPEN_SOURCE 500
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vorbis/vorbisfile.h>


static char *device = "default";      /* playback device */
void* hud_snd_play(void *p)
{
    (void) p;

    int snd_err = 0;
    snd_pcm_t *handle;
    if ((snd_err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        LOG_E("Playback open error: %s\n", snd_strerror(snd_err));
        exit(EXIT_FAILURE);
    }

    OggVorbis_File vf;
    // FILE *f = fopen("test2.ogg", "r");
    int err = ov_fopen("test2.ogg" , &vf);
    if (err != 0) {
        exit(10);
    }

    vorbis_info *vi = ov_info(&vf, -1);
    LOG_E("Bitstream is %i channel, %ldHz\n", vi->channels, vi->rate);
    LOG_E("Encoded by: %s\n\n", ov_comment(&vf, -1)->vendor);

    snd_err = snd_pcm_set_params(handle, SND_PCM_FORMAT_U8, SND_PCM_ACCESS_RW_INTERLEAVED,
                                 vi->channels, vi->rate, 1, vi->rate);
    if (snd_err < 0) {
        LOG_E("Playback open error: %s\n", snd_strerror(snd_err));
        exit(EXIT_FAILURE);
    }
    LOG_E("expecting %f seconds of playback\n", ov_time_total(&vf, -1));

    char *buffer = malloc(100 * vi->channels * vi->rate);
    if (!buffer) {
        exit(9);
    }

    snd_pcm_sframes_t frames;
    int current_section;
    long ret = 0;
    while (1) {
        ret = ov_read(&vf, buffer, sizeof(buffer), 0, 1, 0, &current_section);
        int play_count = snd_pcm_bytes_to_frames(handle, ret);
        if (ret > 0) {
            frames = snd_pcm_writei(handle, buffer, play_count);
            if (frames == -EPIPE) {
                snd_pcm_prepare(handle);
                frames = snd_pcm_writei(handle, buffer, play_count);
            }

            if (frames < 0) {
                frames = snd_pcm_recover(handle, frames, 1);
                if (frames < 0) {
                    LOG_E("snd_pcm_writei failed: %s\n", snd_strerror(err));
                    break;
                } else {
                    frames = snd_pcm_writei(handle, buffer, play_count);
                }
            }
        } else if(ret < 0) {
            LOG_E("error ret < 0\n");
            /* error in the stream. */
        } else {
            LOG_E("error ret == 0\n");
            break;
       }
    }

    ov_clear(&vf);
    free(buffer);

    snd_pcm_close(handle);
    return 0;
}
