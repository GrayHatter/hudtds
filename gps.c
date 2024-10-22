#include "gps.h"

#include "hud.h"
#include "log.h"

#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <gps.h>
#include <errno.h>


static bool gps_running = false;
static bool gps_exit = false;

static struct hud_gps_data hud_gps_data;


void copy_data(struct gps_data_t *data)
{
    hud_gps_data.lat = data->fix.latitude;
    hud_gps_data.lon = data->fix.longitude;
    hud_gps_data.alt = data->fix.altitude;
    hud_gps_data.gnd_speed = data->fix.speed;
    hud_gps_data.vert_speed = data->fix.climb;
    hud_gps_data.satellites_visible = data->satellites_visible;
    hud_gps_data.satellites_used = data->satellites_used;

    for (int i = 0; i < data->satellites_visible && i < MAXCHANNELS; i++) {
        memset(&hud_gps_data.satellites[i], 0, sizeof (struct hud_sat_data));
        hud_gps_data.satellites[i].visable = true;
        hud_gps_data.satellites[i].PRN = data->PRN[i];
        hud_gps_data.satellites[i].elevation = data->elevation[i];
        hud_gps_data.satellites[i].azimuth = data->azimuth[i];
        hud_gps_data.satellites[i].snr = data->ss[i];
        for (int j = 0; j < data->satellites_used; j++) {
            if (data->used[j] == data->PRN[i]) {
                hud_gps_data.satellites[i].used = true;
            }
        }
    }
}


static void *gps_thread(void *p)
{
    if (!p) {
        LOG_E("Early exit from gps thread!\n");
        return NULL;
    }
    struct gps_data_t *gps = p;

    gps_running = true;

    while (!gps_exit) {
        /* Put this in a loop with a call to a high resolution sleep () in it. */
        if (gps_waiting(gps, 500)) {
            if (gps_read(gps) == -1) {
                LOG_E("gps_read error state\n");
            } else {
                if (gps->set) {
                    copy_data(gps);
                }
            }
        }
    struct timespec __ts_nanosleep = { .tv_nsec = _TENTH_SEC };
    nanosleep(&__ts_nanosleep, NULL);
    }

    return gps;
}


static struct gps_data_t *gps_init(void)
{
    struct gps_data_t *gps = calloc(1, sizeof (struct gps_data_t));
    if (!gps) {
        LOG_E("Unable to calloc for gps data\n");
        return NULL;
    }

    if (gps_open("127.0.0.1", NULL, gps)) {
        LOG_E("Error calling gps_open... ");
        LOG_E("cgps: no gpsd running or network error: %d, %s\n", errno, gps_errstr(errno));
        return NULL;
    }

    gps_stream(gps, WATCH_ENABLE | WATCH_JSON, NULL);

    return gps;
}


static void gps_raze(struct gps_data_t *gps)
{
    gps_stream(gps, WATCH_DISABLE, NULL);
    gps_close(gps);
}


static pthread_t t = 0;

void gps_thread_start(void)
{
    LOG_E("gps thread starting up\n");

    struct gps_data_t *gps  = gps_init();
    pthread_create(&t, NULL, gps_thread, gps);
}


void gps_thread_stop(void)
{
    gps_exit = true;

    struct gps_data_t *gps;
    pthread_join(t, (void **)&gps);
    gps_raze(gps);
}


struct hud_gps_data *gps_get_data(void)
{
    struct hud_gps_data *data = calloc(1, sizeof (struct hud_gps_data));
    if (!data) {
        return NULL;
    }

    memcpy(data, &hud_gps_data, sizeof (struct hud_gps_data));
    return data;
}
