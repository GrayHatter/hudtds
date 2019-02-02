#if !defined(_IVI_HEADER_H_) || !defined(_IVI_HEADER_MAZDA_H_)
#define _IVI_HEADER_H_
#define _IVI_HEADER_MAZDA_H_

#include <stdint.h>

extern struct wl_interface wl_ivi_shell_interface;
extern struct wl_interface wl_ivi_interface;
extern struct wl_interface wl_ivi_animation_group_interface;

typedef enum {
    IVI_SHELL_CREATE_ANIMATION_GROUP = 0
} IVI_SHELL_CREATE_OPCODES;

typedef enum {
    IVI_SHELL_ANIMATION_GROUP_START = 0,
    IVI_SHELL_ANIMATION_GROUP_STOP,
    IVI_SHELL_ANIMATION_GROUP_FINISH,
    IVI_SHELL_ANIMATION_GROUP_DESTORY,
    IVI_SHELL_ANIMATION_GROUP_ADD_TRIGGER,
    IVI_SHELL_ANIMATION_GROUP_ANIMATE_SCALAR,
    IVI_SHELL_ANIMATION_GROUP_ANIMATE_CUBICBEZIER2d,
    IVI_SHELL_ANIMATION_GROUP_TRIGGER,
    IVI_SHELL_ANIMATION_GROUP_SEGV
} IVI_SHELL_RUN_OPCODES;

struct wl_ivi_shell_listener {
    void (*create)(void *data,
        struct wl_interface *ivi_interface,
        char *name);
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

#endif // _IVI_HEADER_H_ || _IVI_HEADER_MAZDA_H_

