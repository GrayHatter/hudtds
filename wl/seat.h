#ifndef _HUDTDS_WL_SEAT_H_
#define _HUDTDS_WL_SEAT_H_

#include <stdint.h>

struct wl_registry;

void hud_init_seat(struct wl_registry *registry, uint32_t id, uint32_t version);
void hud_raze_seat(void);


#endif // _HUDTDS_WL_SEAT_H_
