#include "gps.h"

#include "hud.h"
#include "log.h"

#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <gps.h>
#include <errno.h>

#define _TENTH_SEC 1000 * 1000 * 100


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
    hud_gps_data.num_sats_connected = data->satellites_used;
    hud_gps_data.num_sats_visible = data->satellites_visible;
}


static void *gps_thread(void *p)
{
    if (!p) {
        LOG_E("Early exit from gps thread!\n");
        return NULL;
    }

    struct gps_data_t *gps = p;
    LOG_E("pointer %p\n", gps);

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
    LOG_E("pointer %p\n", gps);
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
