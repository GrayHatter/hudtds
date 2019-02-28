#include "hud.h"

#include "log.h"
#include "wayland.h"

#include "audio.h"
#include "gps.h"

#include <stdlib.h>
#include <stdbool.h>

#define CURSOR_WIDTH 100
#define CURSOR_HEIGHT 59
#define CURSOR_HOT_SPOT_X 10
#define CURSOR_HOT_SPOT_Y 3

static bool done = false;

LOGLVL curr_log_lvl = LOGLVL_NOTE;


int main(void)
{
    char *log_lvl = getenv("HUDTDS_DEBUG");
    if (log_lvl) {
        int level = atoi(log_lvl);
        if (level && level < LOGLVL_WTF) {
            curr_log_lvl = level;
        }
    }

    LOG_D("Main startup\n");


    init_wayland();
    LOG_D("wl init done\n");

    LOG_D("Starting Audio Thread\n");
    audio_thread_start();

    LOG_D("Starting GPS Thread\n");
    gps_thread_start();

    while (!done) {
        if (do_wayland() < 0) {
            LOG_E("Main loop error");
            done = true;
        }
    }

    LOG_E("Exiting HUDTDS...\n");

    raze_wayland();

    return EXIT_SUCCESS;
}
