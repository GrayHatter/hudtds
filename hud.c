#include "hud.h"

#include "info.h"
#include "log.h"
#include "wayland.h"
#include "audio.h"

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
    struct wl_buffer *buffer;
    struct wl_shm_pool *pool;
    struct wl_shell_surface *surface;

    init_wayland();
    LOG_D("wl init done\n");

    // get_info();

    pool = init_memory_pool();
    LOG_T("Pool done\n");

    surface = init_root_surface();
    LOG_T("surface done\n");

    buffer = init_buffer(pool, WIDTH, HEIGHT);
    LOG_T("buffer done\n");

    bind_buffer(buffer, surface);
    LOG_T("bind done\n");

    audio_thread_start();

    //init_cursor(pool, CURSOR_WIDTH, CURSOR_HEIGHT, CURSOR_HOT_SPOT_X, CURSOR_HOT_SPOT_Y);
    // LOG_N("cursor done\n");

    while (!done) {
        if (do_wayland() < 0) {
            LOG_E("Main loop error");
            done = true;
        }
    }

    LOG_E("Exiting HUDTDS...\n");

    raze_buffer(buffer);
    raze_surface(surface);
    raze_memory_pool(pool);

    raze_wayland();

    return EXIT_SUCCESS;
}
