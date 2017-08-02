#include "sound.h"

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
        printf("Playback open error: %s\n", snd_strerror(snd_err));
        exit(EXIT_FAILURE);
    }

    OggVorbis_File vf;
    FILE *f = fopen("test2.ogg", "r");
    int err = ov_open(f, &vf, NULL, 0);
    if (err != 0) {
        exit(10);
    }

    vorbis_info *vi = ov_info(&vf, -1);
    printf("Bitstream is %i channel, %ldHz\n", vi->channels, vi->rate);
    printf("Encoded by: %s\n\n", ov_comment(&vf, -1)->vendor);

    snd_err = snd_pcm_set_params(handle, SND_PCM_FORMAT_U8, SND_PCM_ACCESS_RW_INTERLEAVED,
                                 vi->channels, vi->rate, 0, 500000);
    if (snd_err < 0) {
        /* 0.5sec */
        printf("Playback open error: %s\n", snd_strerror(snd_err));
        exit(EXIT_FAILURE);
    }


    char *buffer = malloc(vi->channels * vi->rate);
    if (!buffer) {
        exit(9);
    }


    while(1) {
        snd_pcm_sframes_t frames;
        int current_section;
        long ret = ov_read(&vf, buffer, sizeof(buffer), 0, 1, 0, &current_section);
        if (!ret) {
            printf("error ret == 0\n");
            break;
        } else if(ret < 0) {
            printf("error ret < 0\n");
            /* error in the stream. */
        } else {
            frames = snd_pcm_writei(handle, buffer, ret / vi->channels);
            if (frames < 0) {
                frames = snd_pcm_recover(handle, frames, 0);
            }

            if (frames < 0) {
                printf("snd_pcm_writei failed: %s\n", snd_strerror(err));
                break;
            }
       }
    }

    ov_clear(&vf);
    free(buffer);

    snd_pcm_close(handle);
    return 0;
}
