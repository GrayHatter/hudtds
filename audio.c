#include "audio.h"

#include "hud.h"
#include "log.h"
#include "audio_search.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include <alsa/asoundlib.h>

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


#define _10_mSECS 1000 * 1000 * 10
#define _10_uSECS 1000 * 10
#define _100_uSECS 1000 * 100

#define DEFAULT_CHANNELS    2
#define DEFAULT_SAMPLE_FMT  AV_SAMPLE_FMT_S16
#define DEFAULT_SAMPLE_RATE 48000

static char *device = "default";
static snd_pcm_format_t pcm_format = SND_PCM_FORMAT_S16;
static snd_pcm_t *pcm;
static struct track_data *current_track = NULL;


struct playback_data {
    struct track_data *track;
    bool running;
    bool paused;
    bool clean_exit;
};


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
        struct timespec __ts_nanosleep = { .tv_nsec = _10_uSECS };
        nanosleep(&__ts_nanosleep, NULL);
    }
    next_amsg_msg = msg;
    next_amsg_data = data;
    return;
}


static AVFormatContext *open_audio_track(char *filename, int *stream_id)
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


#define AUDIO_BUF_SIZE 20480
static void *playback_thread(void *d)
{
    struct playback_data *data = d;

    LOG_N("Playing %s\n", data->track->filename);

    const struct directory_data *dir = find_dir(data->track->dir_id, NULL);
    char *filename  = expand_dirname(dir->dirname, data->track->filename);
    LOG_N("location %s\n", filename);

    int stream_id = -1;
    // So... I'm pretty sure filename only needs to exist until this function returns
    // if you get odd crashes... that obviously not correct!
    AVFormatContext *fcon = open_audio_track(filename, &stream_id);
    if (!fcon) {
        data->clean_exit = true;
        free(filename);
        return NULL;
    }
    free(filename);
    filename = NULL;

    AVCodecContext *c_ctx = init_codec(fcon->streams[stream_id]->codec, &fcon->metadata);
    if (!c_ctx) {
        LOG_E("Could not allocate init codec\n");
        data->clean_exit = true;
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
        data->clean_exit = true;
        return NULL;
    }

    LOG_I("context data layout %lu %i %i %i \n", (long)c_ctx->channel_layout, c_ctx->channels, c_ctx->sample_rate, c_ctx->sample_fmt);

    struct SwrContext *swr = swr_alloc_set_opts(NULL,
        AV_CH_LAYOUT_STEREO, DEFAULT_SAMPLE_FMT, DEFAULT_SAMPLE_RATE,
        c_ctx->channel_layout, c_ctx->sample_fmt, c_ctx->sample_rate,
        0, NULL);
    if (!swr) {
        LOG_E("SWR alloc set opts failed\n;");
        data->clean_exit = true;
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
        while (data->paused) {
            struct timespec __ts_nanosleep = { .tv_nsec = _10_mSECS };
            nanosleep(&__ts_nanosleep, NULL);
        }

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
    avformat_close_input(&fcon);
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
                // LOG_T("AMSG_THREAD UNHANDLED msg AMSG_NONE\n");
                break;
            }
            case AMSG_PLAY: {
                LOG_I("AMSG_THREAD Play\n");
                if (audio_playing) {
                    if (current_playback) {
                        current_playback->running = false;
                        while (!current_playback->clean_exit) {
                            struct timespec __ts_nanosleep = { .tv_nsec = _10_uSECS };
                            nanosleep(&__ts_nanosleep, NULL);
                        }

                        free(current_playback);
                        current_playback = NULL;
                        current_track = NULL;
                    }
                }

                struct playback_data *d = calloc(1, sizeof (struct playback_data));
                if (!d) {
                    LOG_E("unable to calloc\n");
                    break;
                }
                d->track = cur_data;
                current_track = cur_data;

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
                LOG_D("AMSG_PAUSE\n");
                if (current_playback) {
                    current_playback->paused = !current_playback->paused;
                }
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
            case AMSG_TRACK_SCAN_DONE: {
                LOG_N("AMSG Track Scan done!\n");
                break;
            }
        }
        if (audio_playing) {
            struct timespec __ts_nanosleep = { .tv_nsec = _100_uSECS };
            nanosleep(&__ts_nanosleep, NULL);
        } else {
            struct timespec __ts_nanosleep = { .tv_nsec = _10_mSECS };
            nanosleep(&__ts_nanosleep, NULL);
        }
    }

    return NULL;
}


static void avlog_cb(void *ptr, int level, const char *fmt, va_list vl)
{
    (void) ptr;
    (void) level;

    if (LVL(DEBUG)) {
        vfprintf(stdout, fmt, vl);
    }
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
    av_log_set_callback(avlog_cb);

    LOG_T("init pcm\n");
    init_pcm();
}


struct track_data *audio_track_get_current(void)
{
    return current_track;
}


void audio_thread_start(void)
{
    LOG_N("audio_thread_start\n");

    audio_init();

    pthread_t ms;
    pthread_create(&ms, NULL, find_files_thread, NULL);

    pthread_t a;
    pthread_create(&a, NULL, audio_thread, NULL);
}
