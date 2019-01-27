#ifndef _HUD_POINTER_H_
#define _HUD_POINTER_H_

#include <stdbool.h>
#include <wayland-client-protocol.h>

struct pointer_data {
    struct wl_surface *surface;
    struct wl_buffer *buffer;
    int32_t hot_spot_x;
    int32_t hot_spot_y;
    struct wl_surface *target_surface;
};

void init_cursor(struct wl_shm_pool *pool, unsigned width, unsigned height, int32_t hot_spot_x, int32_t hot_spot_y);
void raze_cursor(void);

#endif // _HUD_POINTER_H_
