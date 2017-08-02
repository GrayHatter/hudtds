#ifndef _HUD_KB_H_
#define _HUD_KB_H_

#include <stdbool.h>
#include <wayland-client-protocol.h>

bool hud_kb_init(struct wl_seat *seat);

bool hud_kb_raze(void);


#endif // _HUD_KB_H_
