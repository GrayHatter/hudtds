#ifndef _IVI_HEADER_H_
#define _IVI_HEADER_H_
#define _IVI_HEADER_MAZDA_H_

#include <stdint.h>

extern struct wl_interface wl_ivi_shell_interface;
extern struct wl_interface wl_ivi_interface;
extern struct wl_interface wl_ivi_animation_group_interface;

struct wl_ivi_shell_listener {
    void (*create)(void *data,
        struct wl_interface *ivi_interface,
        char *name,
        void *screen,
        uint32_t id2);
    void (*destroy)(void *data, struct wl_interface *, void *, void *);
    void (*other)(void *, void *, void *);
};


struct wl_ivi_listener {
    void (*screen)(void *data,
        struct wl_interface *ivi_interface,
        uint32_t id,
        void *screen);
    void (*one)(void *data, struct wl_interface *, void *, void *);
};


char *wl_ivi_pack_surface_id(char *string);
char *wl_ivi_unpack_surface_id(void *pointer); // FIXME I haven't even tried to verify/test this


#elif defined(_IVI_HEADER_H_) && !defined(_IVI_HEADER_MAZDA_H_)
#error "Don't included multiple ivi headers!"

#endif // _IVI_HEADER_H_

