#ifndef _WAYLANDTDS_H_
#define _WAYLANDTDS_H_

#include <stdint.h>

struct surface {
    struct wl_shell_surface *shell_surface;
    struct wl_surface *surface;
    struct wl_shm_pool *pool;

    int fd;
    uint8_t count;
    uint32_t *memory;
    struct wl_buffer *buffer[3];
    unsigned mem_capacity;

    unsigned frame_size;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
};

void hud_surface_damage(int32_t x, int32_t y, int32_t w, int32_t h);
void hud_surface_commit();

struct wl_display *init_wayland(void);
int do_wayland(void);
void raze_wayland(void);


#endif // _WAYLANDTDS_H_
