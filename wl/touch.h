#ifndef _HUD_TOUCH_H_
#define _HUD_TOUCH_H_

#include <stdbool.h>

struct wl_seat;

bool hud_touch_init(struct wl_seat *seat);

bool hud_touch_raze(void);


#endif // _HUD_TOUCH_H_
