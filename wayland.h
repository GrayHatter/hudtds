#ifndef _WAYLANDTDS_H_
#define _WAYLANDTDS_H_

#include <stdint.h>

struct pool_data {
    int fd;
    uint32_t *memory;
    unsigned capacity;
    unsigned size;
};

extern struct pool_data *root_pool_data;


void hud_surface_damage(int32_t x, int32_t y, int32_t w, int32_t h);
void hud_surface_commit();

struct wl_display *init_wayland(void);
int do_wayland(void);
void raze_wayland(void);


#endif // _WAYLANDTDS_H_
