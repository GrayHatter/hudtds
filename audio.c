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


static char *device = "default";

#define DEFAULT_CHANNELS    2
#define DEFAULT_SAMPLE_FMT  AV_SAMPLE_FMT_S16
#define DEFAULT_SAMPLE_RATE 48000


static int resample = 1;
static snd_pcm_format_t pcm_format = SND_PCM_FORMAT_S16;

static int sample_word_size = 1;
static int ov_signed = 0;
static int ov_endian = 0;  /* LE = 0, BE = 2 */

static snd_pcm_t *pcm;

static void play_filename(char *filename);

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


static void *audio_thread(void *p)
{
    (void) p;
    bool audio_playing = false;
    AUDIO_MSG cur_msg = AMSG_NONE;
    void *cur_data = NULL;

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
                audio_playing = true;
                play_filename(cur_data);
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


#define AUDIO_BUF_SIZE 20480
// #define AUDIO_REFILL_THRESH 4096
static void play_filename(char *filename)
{
    LOG_I("Playing %s\n", filename);
    int stream_id = -1;
    AVFormatContext *fcon = open_audio_file(filename, false, &stream_id);
    if (!fcon) {
        return;
    }

    AVCodecContext *c_ctx = init_codec(fcon->streams[stream_id]->codec, &fcon->metadata);
    if (!c_ctx) {
        return;
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
        return;
    }

    LOG_I("context data layout %lu %i %i %i \n", (long)c_ctx->channel_layout, c_ctx->channels, c_ctx->sample_rate, c_ctx->sample_fmt);


    struct SwrContext *swr = swr_alloc_set_opts(NULL,
        AV_CH_LAYOUT_STEREO, DEFAULT_SAMPLE_FMT, DEFAULT_SAMPLE_RATE,
        c_ctx->channel_layout, c_ctx->sample_fmt, c_ctx->sample_rate,
        0, NULL);
    if (!swr) {
        LOG_E("SWR alloc set opts failed\n;");
        return;
    }
    swr_init(swr);

    // int16_t *audiobuffer = NULL;
    LOG_I("Bitstream is %i channel (%lu), %iHz\n", c_ctx->channels, (long)c_ctx->channel_layout, c_ctx->sample_rate);
    LOG_I("PCM is %i channel (%i), %iHz\n", DEFAULT_CHANNELS, AV_CH_LAYOUT_STEREO, DEFAULT_SAMPLE_RATE);


    LOG_T("Main decode loop\n");
    while(av_read_frame(fcon, &avpkt) >= 0) {
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
                uint8_t *output = malloc(out_samples * 4);
                int count = swr_convert(swr, &output, out_samples,
                    (const uint8_t **)avframe->data, avframe->nb_samples);
                if (count < 0 ) {
                    LOG_E("Error in sw_convert %i\n", count);
                    continue;
                }

                // LOG_E("playing %i %li\n", count, available);
                LOG_T("Frame %i\n", count);
                // pcm_play_2chan((void**)output, available);
                pcm_play(output, count);

                // av_freep(&output);
                free(output);
            } else {
                LOG_E("bad frame, still reading\n");
                //printf("Not Finished\n");
            }
        } else {
            LOG_E("Not an audio frame\n");
        }
    }
    LOG_E("files done\n");

    avcodec_close(c_ctx);
    avformat_free_context(fcon);
    avcodec_free_frame(&avframe);

    return;
}


void audio_thread_start(void)
{
    audio_init();

    pthread_t t;
    pthread_create(&t, NULL, audio_thread, (void *)NULL);
}


void* hud_snd_play(void *p)
{
    (void) p;

    init_pcm();
    OggVorbis_File vf;
    // FILE *f = fopen("test2.ogg", "r");
    int err = ov_fopen("test2.ogg" , &vf);
    if (err != 0) {
        exit(10);
    }

    vorbis_info *vi = ov_info(&vf, -1);
    LOG_E("Bitstream is %i channel, %ldHz\n", vi->channels, vi->rate);
    LOG_E("Encoded by: %s\n\n", ov_comment(&vf, -1)->vendor);

    int snd_err = snd_pcm_set_params(pcm, pcm_format, SND_PCM_ACCESS_RW_INTERLEAVED,
        vi->channels, vi->rate, resample, vi->rate / 2);
    if (snd_err < 0) {
        LOG_E("Playback open error: %s\n", snd_strerror(snd_err));
    }
    LOG_E("expecting %f seconds of playback\n", ov_time_total(&vf, -1));

    uint8_t *buffer = malloc(2000 * vi->channels * vi->rate);
    if (!buffer) {
        exit(9);
    }

    int bitstream;
    long ov_size = 0;
    snd_pcm_sframes_t available;
    while (1) {
        ov_size = ov_read(&vf, (char *)buffer, 1000 * vi->channels * vi->rate, ov_endian, sample_word_size, ov_signed, &bitstream);
        available = snd_pcm_bytes_to_frames(pcm, ov_size);
        if (available > 0) {
            pcm_play(buffer, available);
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
    return NULL;
}
