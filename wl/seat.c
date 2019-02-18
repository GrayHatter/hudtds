#include "seat.h"

#include "keyboard.h"
#include "touch.h"

#include "../hud.h"
#include "../log.h"

#include <wayland-client.h>


static struct wl_seat *seat = NULL;

static void seat_handle_capabilities(void *data, struct wl_seat *seat, enum wl_seat_capability caps)
{
    (void) data;
    LOG_D("seat cap\n");

    if (caps & WL_SEAT_CAPABILITY_KEYBOARD) {
        hud_kb_init(seat);
    } else if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD)) {
        // TODO
        hud_kb_raze();
    }

    if (caps & WL_SEAT_CAPABILITY_TOUCH) {
        hud_touch_init(seat);
    } else if (!(caps & WL_SEAT_CAPABILITY_TOUCH)) {
        hud_touch_raze();
    }
    /*
    if ((caps & WL_SEAT_CAPABILITY_POINTER) && !pointer) {
        LOG_E("setting pointer\n");
        pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(pointer, &pointer_listener, NULL);
    } else if (!(caps & WL_SEAT_CAPABILITY_POINTER) && pointer) {
        LOG_E("killing pointer\n");
        wl_pointer_destroy(pointer);
        pointer = NULL;
    }
    */
}


static const struct wl_seat_listener seat_listener = {
    .capabilities = seat_handle_capabilities,
};

void hud_init_seat(struct wl_registry *registry, uint32_t id, uint32_t version)
{
    seat = wl_registry_bind(registry, id, &wl_seat_interface, min(version, 2));
    wl_seat_add_listener(seat, &seat_listener, NULL);
}


void hud_raze_seat(void)
{
    wl_seat_destroy(seat);
}
