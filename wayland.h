#ifndef _WAYLANDTDS_H_
#define _WAYLANDTDS_H_

#include <stdint.h>

#include <wayland-client-protocol.h>

struct pool_data {
    int fd;
    uint32_t *memory;
    unsigned capacity;
    unsigned size;
};

extern struct pool_data *root_pool_data;



void hud_surface_damage(int32_t x, int32_t y, int32_t w, int32_t h);
void hud_surface_commit();

struct wl_shm_pool *init_memory_pool(void);
void raze_memory_pool(struct wl_shm_pool *pool);

struct wl_buffer *init_buffer(struct wl_shm_pool *pool, unsigned width, unsigned height);
void bind_buffer(struct wl_buffer *buffer, struct wl_shell_surface *shell_surface);
void raze_buffer(struct wl_buffer *buffer);

struct wl_shell_surface *init_root_surface(void);
void raze_surface(struct wl_shell_surface *shell_surface);


struct wl_display *init_wayland(void);
int do_wayland(void);
void raze_wayland(void);


#endif // _WAYLANDTDS_H_
