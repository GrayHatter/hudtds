#include "audio.h"

#include "hud.h"
#include "log.h"

#define _POSIX_C_SOURCE 200112L
#define __USE_XOPEN_EXTENDED

#define _10_MSECS 10 * 1000

#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <vorbis/vorbisfile.h>

#include <libavutil/common.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/log.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libswresample/swresample.h>

#include <pthread.h>


#ifndef __arm__
#define CODEC_ID_MP3 AV_CODEC_ID_MP3
#define avcodec_free_frame av_frame_free
#define avcodec_alloc_frame av_frame_alloc
#endif


#define DEFAULT_CHANNELS    2
#define DEFAULT_SAMPLE_FMT  AV_SAMPLE_FMT_S16
#define DEFAULT_SAMPLE_RATE 48000

static char *device = "default";
static snd_pcm_format_t pcm_format = SND_PCM_FORMAT_S16;
static snd_pcm_t *pcm;


static void init_pcm(void)
{
    int snd_err = snd_pcm_open(&pcm, device, SND_PCM_STREAM_PLAYBACK, 0);
    if (snd_err < 0) {
        LOG_E("Playback open error: %s\n", snd_strerror(snd_err));
        return;
    }

    snd_err = snd_pcm_set_params(pcm, pcm_format, SND_PCM_ACCESS_RW_INTERLEAVED, DEFAULT_CHANNELS,
        DEFAULT_SAMPLE_RATE, 0, DEFAULT_SAMPLE_RATE);
    if (snd_err < 0) {
        LOG_E("Playback open error: %s\n", snd_strerror(snd_err));
        return;
    }


    return;
}


static int pcm_play(uint8_t *buffer, long count)
{
    long frame_cnt = snd_pcm_samples_to_bytes(pcm, snd_pcm_bytes_to_frames(pcm, count * 2));
    snd_pcm_sframes_t frames = snd_pcm_writei(pcm, buffer, frame_cnt);

    if (frames < frame_cnt) {
        LOG_E("frame error %li < %li\n", frames, frame_cnt);
        if (frames == -EPIPE) {
            LOG_E("-EPIPE %li < %li\n", frames, frame_cnt);
            snd_pcm_prepare(pcm);
            frames = snd_pcm_writei(pcm, buffer, frame_cnt);
        } else if (snd_pcm_recover(pcm, frames, 0) == 0) {
            frames = snd_pcm_writei(pcm, buffer, frame_cnt);
        } else {
            LOG_E("snd_pcm_writei failed :: %s (%p, %p, %li)\n", snd_strerror(frames), pcm, buffer, frame_cnt);
        }
    }
    return frames;
}


static AUDIO_MSG next_amsg_msg = 0;
static void *next_amsg_data = NULL;

void postmsg_audio(AUDIO_MSG msg, void *data)
{
    if (msg && next_amsg_msg) {
        usleep(1000);
    }
    next_amsg_msg = msg;
    next_amsg_data = data;
    return;
}


static AVFormatContext *open_audio_file(char *filename, bool debug, int *stream_id)
{
    LOG_D("getting format context\n");
    AVFormatContext *fcontext = avformat_alloc_context();
    if (avformat_open_input(&fcontext, filename, NULL, NULL) < 0) {
        LOG_E("Could not open file\n");
        return NULL;
    }

    LOG_D("getting pcm_format stream info\n");
    if (avformat_find_stream_info(fcontext, NULL) < 0){
        LOG_E("Could not find file info\n");
        return NULL;
    }

    if (debug || 1) {
        av_dump_format(fcontext, 0, filename, false);
    }

    LOG_D("searching streams\n");
    for (uint32_t i = 0; i < fcontext->nb_streams; i++) {
        if (fcontext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            *stream_id = i;
            break;
        }
    }

    if (*stream_id == -1) {
        LOG_W("Could not find Audio Stream\n");
        return NULL;
    }

    return fcontext;
}


static AVCodecContext *init_codec(AVCodecContext *ctx, AVDictionary **metadata)
{

    LOG_D("getting codec\n");
    AVCodec *codec = avcodec_find_decoder(ctx->codec_id);
    // AVCodec *codec = avcodec_find_decoder(CODEC_ID_MP3);
    if (!codec) {
        LOG_E("Codec not found\n");
        return NULL;
    }

    LOG_D("opening codec\n");
    if (avcodec_open2(ctx, codec, metadata) < 0) {
        LOG_E("Could not open codec\n");
        return NULL;
    }

    LOG_I("codec id %i, mp3 %i\n", ctx->codec_id, CODEC_ID_MP3);
    return ctx;
}


struct playback_data {
    char *name;
    bool running;
    bool clean_exit;
};


#define AUDIO_BUF_SIZE 20480
static void *playback_thread(void *d)
{
    struct playback_data *data = d;

    LOG_N("Playing %s\n", data->name);
    int stream_id = -1;
    AVFormatContext *fcon = open_audio_file(data->name, false, &stream_id);
    if (!fcon) {
        LOG_E("Could not open file\n");
        return NULL;
    }

    AVCodecContext *c_ctx = init_codec(fcon->streams[stream_id]->codec, &fcon->metadata);
    if (!c_ctx) {
        LOG_E("Could not allocate init codec\n");
        return NULL;
    }

    AVPacket avpkt;
    av_init_packet(&avpkt);

    uint8_t inbuf[AUDIO_BUF_SIZE + 256];
    avpkt.data = inbuf;
    avpkt.size = AUDIO_BUF_SIZE;

    LOG_D("Alloc a frame\n");
    AVFrame *avframe = avcodec_alloc_frame();
    if (!avframe) {
        LOG_E("Could not allocate audio frame\n");
        return NULL;
    }

    LOG_I("context data layout %lu %i %i %i \n", (long)c_ctx->channel_layout, c_ctx->channels, c_ctx->sample_rate, c_ctx->sample_fmt);

    struct SwrContext *swr = swr_alloc_set_opts(NULL,
        AV_CH_LAYOUT_STEREO, DEFAULT_SAMPLE_FMT, DEFAULT_SAMPLE_RATE,
        c_ctx->channel_layout, c_ctx->sample_fmt, c_ctx->sample_rate,
        0, NULL);
    if (!swr) {
        LOG_E("SWR alloc set opts failed\n;");
        return NULL;
    }
    swr_init(swr);

    // int16_t *audiobuffer = NULL;
    LOG_I("Bitstream is %i channel (%lu), %iHz\n", c_ctx->channels, (long)c_ctx->channel_layout, c_ctx->sample_rate);
    LOG_I("PCM is %i channel (%i), %iHz\n", DEFAULT_CHANNELS, AV_CH_LAYOUT_STEREO, DEFAULT_SAMPLE_RATE);

    LOG_T("Main decode loop\n");

    data->running = true;

    uint8_t *output = malloc(AUDIO_BUF_SIZE * 4);
    while (av_read_frame(fcon, &avpkt) >= 0 && data->running) {
        if (avpkt.stream_index == stream_id) {
            LOG_T("correct stream_id\n");
            int have_frame = 0;

            if (avcodec_decode_audio4(c_ctx, avframe, &have_frame, &avpkt) < 0) {
                LOG_E("Error while decoding\n");
                break;
            }

            if (have_frame) {
                LOG_T("have_frame\n");

                int out_samples = av_rescale_rnd(swr_get_delay(swr, c_ctx->sample_rate) + avframe->nb_samples,
                    DEFAULT_SAMPLE_RATE, c_ctx->sample_rate, AV_ROUND_UP);
                output = realloc(output, out_samples * 4);
                int count = swr_convert(swr, &output, out_samples,
                    (const uint8_t **)avframe->data, avframe->nb_samples);
                if (count < 0 ) {
                    LOG_E("Error in sw_convert %i\n", count);
                    continue;
                }

                LOG_T("Frame %i\n", count);
                pcm_play(output, count);
            } else {
                LOG_E("bad frame, still reading\n");
            }
        } else {
            LOG_E("Not an audio frame\n");
        }
    }
    LOG_E("files done\n");

    free(output);
    avcodec_close(c_ctx);
    avformat_free_context(fcon);
    avcodec_free_frame(&avframe);

    data->clean_exit = true;
    return NULL;
}


static void *audio_thread(void *p)
{
    (void) p;

    // thread state
    AUDIO_MSG cur_msg = AMSG_NONE;
    void *cur_data = NULL;

    // Audio state
    bool audio_playing = false;
    struct playback_data *current_playback = NULL;


    while (next_amsg_msg != AMSG_THREAD_EXIT) {
        cur_msg = next_amsg_msg;
        cur_data = next_amsg_data;
        next_amsg_data = NULL;
        next_amsg_msg = AMSG_NONE;

        switch (cur_msg) {
            case AMSG_THREAD_EXIT: {
                LOG_E("AMSG_THREAD UNHANDLED msg AMSG_THREAD_EXIT\n");
                return NULL;
            }
            case AMSG_NONE: {
                LOG_T("AMSG_THREAD UNHANDLED msg AMSG_NONE\n");
                break;
            }
            case AMSG_PLAY: {
                LOG_I("AMSG_THREAD Play\n");
                if (audio_playing) {
                    if (current_playback) {
                        current_playback->running = false;
                        while (!current_playback->clean_exit) {
                            usleep(100);
                        }

                        free(current_playback->name);
                        free(current_playback);
                        current_playback = NULL;
                    }
                }

                struct playback_data *d = calloc(1, sizeof (struct playback_data));
                if (!d) {
                    LOG_E("unable to calloc\n");
                    break;
                }
                d->name = strdup(cur_data);

                pthread_t new_thread;
                if (pthread_create(&new_thread, NULL, playback_thread, d)) {
                    LOG_E("Unable to create thread\n");
                    break;
                }

                current_playback = d;
                audio_playing = true;
                break;
            }

            case AMSG_PAUSE: {
                LOG_E("AMSG_THREAD UNHANDLED msg AMSG_PAUSE\n");
                break;
            }
            case AMSG_PLAY_PAUSE: {
                LOG_E("AMSG_THREAD UNHANDLED msg AMSG_PLAY_PAUSE\n");
                break;
            }
            case AMSG_STOP: {
                LOG_E("AMSG_THREAD UNHANDLED msg AMSG_STOP\n");
                break;
            }
            case AMSG_CHANGE_TRACK: {
                LOG_E("AMSG_THREAD UNHANDLED msg AMSG_CHANGE_TRACK\n");
                break;
            }
            case AMSG_SEEK_TO: {
                LOG_E("AMSG_THREAD UNHANDLED msg AMSG_SEEK_TO\n");
                break;
            }
            case AMSG_PREV: {
                LOG_E("AMSG_THREAD UNHANDLED msg AMSG_PREV\n");
                break;
            }
            case AMSG_RESTART: {
                LOG_E("AMSG_THREAD UNHANDLED msg AMSG_RESTART\n");
                break;
            }
            case AMSG_NEXT: {
                LOG_E("AMSG_THREAD UNHANDLED msg AMSG_NEXT\n");
                break;
            }
        }
        if (audio_playing) {
            usleep(100);
        } else {
            usleep(_10_MSECS);
        }
    }

    return NULL;
}


static void audio_init(void)
{
    LOG_T("init libav*\n");
    LOG_I("libavcodec version %u %u %u u\n",
      avcodec_version() >> 16,
      avcodec_version() >> 8 & 0xff,
      avcodec_version() & 0xff);

    av_register_all();
    avcodec_register_all();

    LOG_T("init pcm\n");
    init_pcm();

}


void audio_thread_start(void)
{
    audio_init();

    pthread_t t;
    pthread_create(&t, NULL, audio_thread, (void *)NULL);
}
