#include "sound.h"

#include "log.h"

#define _POSIX_C_SOURCE 200112L

#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vorbis/vorbisfile.h>


static char *device = "default";
static int resample = 1;
static snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
static int sample_word_size = 2;
static int ov_signed = 2;
static int endian = 0;  /* LE = 0, BE = 2 */
void* hud_snd_play(void *p)
{
    (void) p;

    snd_pcm_t *pcm;
    int snd_err = snd_pcm_open(&pcm, device, SND_PCM_STREAM_PLAYBACK, 0);
    if (snd_err < 0) {
        LOG_E("Playback open error: %s\n", snd_strerror(snd_err));
        exit(EXIT_FAILURE);
    }

    OggVorbis_File vf;
    // FILE *f = fopen("test2.ogg", "r");
    int err = ov_fopen("test.ogg" , &vf);
    if (err != 0) {
        exit(10);
    }

    vorbis_info *vi = ov_info(&vf, -1);
    LOG_E("Bitstream is %i channel, %ldHz\n", vi->channels, vi->rate);
    LOG_E("Encoded by: %s\n\n", ov_comment(&vf, -1)->vendor);

    snd_err = snd_pcm_set_params(pcm, format, SND_PCM_ACCESS_RW_INTERLEAVED,
        vi->channels, vi->rate, resample, vi->rate);
    if (snd_err < 0) {
        LOG_E("Playback open error: %s\n", snd_strerror(snd_err));
        exit(EXIT_FAILURE);
    }
    LOG_E("expecting %f seconds of playback\n", ov_time_total(&vf, -1));

    char *buffer = malloc(sample_word_size * vi->channels * vi->rate);
    if (!buffer) {
        exit(9);
    }

    int bitstream;
    long ov_size = 0;
    snd_pcm_sframes_t frames = snd_pcm_bytes_to_frames(pcm, ov_size);
    while (1) {
        ov_size = ov_read(&vf, buffer, sizeof buffer, endian, sample_word_size, ov_signed, &bitstream);
        frames = snd_pcm_bytes_to_frames(pcm, ov_size);
        if (frames > 0) {
            frames = snd_pcm_writei(pcm, buffer, frames);
            if (frames < 0) {
                LOG_E("frame error %li", frames);
                // if (frames == -EPIPE) {
                //     snd_pcm_prepare(pcm);
                //     frames = snd_pcm_writei(pcm, buffer, frames);
                // }
                if (snd_pcm_recover(pcm, frames, 0) == 0) {
                    frames = snd_pcm_writei(pcm, buffer, frames);
                } else {
                    LOG_E("snd_pcm_writei failed: %s\n", snd_strerror(err));
                }
            }
        } else if(ov_size < 0) {
            LOG_E("error ret < 0\n");
            /* error in the stream. */
        } else {
            LOG_E("error ret == 0\n");
            break;
       }
    }

    ov_clear(&vf);
    free(buffer);

    snd_pcm_close(pcm);
    return 0;
}
