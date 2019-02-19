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
    IVI_SHELL_ANIMATION_GROUP_DESTROY,
    IVI_SHELL_ANIMATION_GROUP_ADD_TRIGGER,
    IVI_SHELL_ANIMATION_GROUP_ANIMATE_SCALAR,
    IVI_SHELL_ANIMATION_GROUP_ANIMATE_CUBICBEZIER2d,
    IVI_SHELL_ANIMATION_GROUP_TRIGGER,
    IVI_SHELL_ANIMATION_GROUP_SEGV
} IVI_SHELL_RUN_OPCODES;


typedef enum {
    IVI_SHELL_ANIMATE_SCALAR_SOMETHING = 0,
    IVI_SHELL_ANIMATE_SCALAR_ZINDEX = 1,
    IVI_SHELL_ANIMATE_SCALAR_ALPHA = 2,
} IVI_SHELL_ANIMATE_SCALAR_OPCODES;


// Calling format
// animate_cubicbezier2d("HUDTDS", 0, 0, 0, -50.000000, -50.000000, -50.000000, -50.000000, -50.000000, -50.000000, 0, 0)
// animate_cubicbezier2d("HUDTDS", 4, 0, 0, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 0, 0)
// animate_cubicbezier2d("HUDTDS", 5, 0, 0, 800.000000, 480.000000, 800.000000, 480.000000, 800.000000, 480.000000, 0, 0)
// animate_cubicbezier2d("HUDTDS", 6, 0, 0, 800.000000, 480.000000, 800.000000, 480.000000, 800.000000, 480.000000, 0, 0)
// animate_cubicbezier2d("HUDTDS", 7, 0, 0, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0, 0)
// animate_cubicbezier2d("HUDTDS", 8, 0, 0, 800.000000, 0.000000, 800.000000, 0.000000, 800.000000, 0.000000, 0, 0)
// animate_cubicbezier2d("HUDTDS", 9, 0, 0, 0.000000, 490.000000, 0.000000, 490.000000, 0.000000, 490.000000, 0, 0)
// animate_cubicbezier2d("HUDTDS", 10, 0, 0, 800.000000, 480.000000, 800.000000, 480.000000, 800.000000, 480.000000, 0, 0)


typedef enum {
    IVI_SHELL_ANIMATE_CUBICEBZIER2D_POSITION = 0,
    // IVI_SHELL_ANIMATE_CUBICEBZIER2D_,

    IVI_SHELL_ANIMATE_CUBICEBZIER2D_SET_ORIGIN = 4,
    IVI_SHELL_ANIMATE_CUBICEBZIER2D_SET_CROPPING,
    IVI_SHELL_ANIMATE_CUBICEBZIER2D_SET_DESTSIZE,
    IVI_SHELL_ANIMATE_CUBICEBZIER2D_TOP_LEFT = 7,
    IVI_SHELL_ANIMATE_CUBICEBZIER2D_TOP_RIGHT,
    IVI_SHELL_ANIMATE_CUBICEBZIER2D_LOWER_LEFT,
    IVI_SHELL_ANIMATE_CUBICEBZIER2D_LOWER_RIGHT,
    // IVI_SHELL_ANIMATE_CUBICEBZIER2D_,
    // IVI_SHELL_ANIMATE_CUBICEBZIER2D_,
    // IVI_SHELL_ANIMATE_CUBICEBZIER2D_
} IVI_SHELL_ANIMATE_CUBICEBZIER2D_OPCODES;

struct wl_ivi_shell_listener {
    void (*create)(void *data,
        struct wl_interface *ivi_interface,
        char *name);
    void (*destroy)(void *data, struct wl_interface *, void *, void *);
    // I don't actually know if other exists
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

