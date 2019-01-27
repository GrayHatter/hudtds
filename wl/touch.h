#ifndef _HUD_TOUCH_H_
#define _HUD_TOUCH_H_

#include <stdbool.h>
#include <wayland-client-protocol.h>

bool hud_touch_init(struct wl_seat *seat);

bool hud_touch_raze(void);


#endif // _HUD_TOUCH_H_
