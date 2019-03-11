#include "ivi.h"

#include "../ivi-header-mazda.h"

#include "../hud.h"
#include "../log.h"

#include <wayland-client.h>
#include <string.h>


static struct wl_interface *ivi;
static struct wl_interface *ivi_shell;



static void ivi_screen_cb(void *data, struct wl_interface *interface, uint32_t id, void *thing)
{
    (void) data;
    (void) interface;
    // (void) thing;
    // char *name = wl_ivi_pack_surface_id(id);
    LOG_E("IVI screen cb %x %x\n", id, (int)thing);
}


static void ivi_default(void *data, struct wl_interface *ivi_interface, void *p, void *pp)
{
    (void) data;
    (void) ivi_interface;
    LOG_E("IVI default 1 %x %x\n", (int)p, (int)pp);
}


static const struct wl_ivi_listener ivi_listener = {
    .screen = ivi_screen_cb,
    .one = ivi_default,
};


static inline void ivi_set_z_index(struct wl_proxy *ani_grp, char *name, double z_index)
{
    wl_fixed_t z = wl_fixed_from_double(z_index);
    wl_proxy_marshal(ani_grp,
        IVI_SHELL_ANIMATION_GROUP_ANIMATE_SCALAR,
        name,
        IVI_SHELL_ANIMATE_SCALAR_ZINDEX,
        0, 0, z, 0, 0);

}


static inline void ivi_set_alpha(struct wl_proxy *ani_grp, char *name, int alpha)
{
    wl_fixed_t a = wl_fixed_from_int(alpha);
    wl_proxy_marshal(ani_grp,
        IVI_SHELL_ANIMATION_GROUP_ANIMATE_SCALAR,
        name,
        IVI_SHELL_ANIMATE_SCALAR_ALPHA,
        0, 0, a, 0, 0);
}


static void ivi_shell_create(void *data, struct wl_interface *interface, char *name)
{
    LOG_W("IVI SHELL created %s \n", name);
    (void) data;
    if (strcmp("HUDTDS", name) == 0) {
        struct wl_proxy *ani_grp = wl_proxy_create((struct wl_proxy *) interface, &wl_ivi_animation_group_interface);
        if (ani_grp) {
            wl_proxy_marshal((struct wl_proxy *) interface, IVI_SHELL_CREATE_ANIMATION_GROUP, ani_grp);

            ivi_set_z_index(ani_grp, name, 10000.0);

            wl_proxy_marshal(ani_grp, IVI_SHELL_ANIMATION_GROUP_ANIMATE_CUBICBEZIER2d, name,
                IVI_SHELL_ANIMATE_CUBICEBZIER2D_POSITION, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);

            wl_fixed_t o = wl_fixed_from_double(1);
            wl_proxy_marshal(ani_grp, IVI_SHELL_ANIMATION_GROUP_ANIMATE_CUBICBEZIER2d, name,
                IVI_SHELL_ANIMATE_CUBICEBZIER2D_SET_ORIGIN, 0, 0, o, o, o, o, o, o, 0, 0);
            wl_fixed_t w = wl_fixed_from_double(WIDTH);
            wl_fixed_t h = wl_fixed_from_double(HEIGHT);
            wl_proxy_marshal(ani_grp, IVI_SHELL_ANIMATION_GROUP_ANIMATE_CUBICBEZIER2d, name,
                IVI_SHELL_ANIMATE_CUBICEBZIER2D_SET_DESTSIZE, 0, 0, w, h, w, h, w, h, 0, 0);

            ivi_set_alpha(ani_grp, name, 1);

            wl_proxy_marshal(ani_grp, IVI_SHELL_ANIMATION_GROUP_START);
            wl_proxy_marshal(ani_grp, IVI_SHELL_ANIMATION_GROUP_DESTROY);
        }
    }
    // (void) thing;
    // void *pointer = wl_ivi_pack_surface_id(name);
}


static void ivi_shell_destrory(void *data, struct wl_interface *ivi_interface, void *p, void *pp)
{
    (void) data;
    (void) ivi_interface;
    LOG_W("IVI SHELL destroy %x %x\n", (int)p, (int)pp);
}


static void ivi_shell_other(void *data, void *ivi, void *p)
{
    (void) data;
    LOG_E("IVI SHELL other %x %x\n", (int)ivi, (int)p);
}


static const struct wl_ivi_shell_listener ivi_shell_listener = {
    .create = ivi_shell_create,
    .destroy = ivi_shell_destrory,
    .other = ivi_shell_other,
};



void hudivi_registry(struct wl_registry *reg, uint32_t id, const char *iface, uint32_t ver)
{
    // static struct wl_interface *ivi_animation_group;
    if (strcmp(iface, wl_ivi_shell_interface.name) == 0) {
        // wayland never met an inline function wrapper it didn't like.
        // TODO write my own ivi header that creates the inline wrapper func "just like mom used to make"
        ivi_shell = wl_registry_bind(reg, id, &wl_ivi_shell_interface, min(ver, 1));
        wl_proxy_add_listener((struct wl_proxy *)ivi_shell, (void (**)(void))&ivi_shell_listener, NULL);
        LOG_D("  --  Setting IVI SHELL %p", ivi_shell);

        // LOG_D("\n\t\t trying for animation group %p", ivi_shell);
        // ivi_animation_group = wl_proxy_create(wl_ivi_shell_interface, id, &wl_ivi_animation_group_interface, min(version, 1));
        // LOG_D(" -- ivi_ag %p", ivi_animation_group);
    } else if (strcmp(iface, wl_ivi_interface.name) == 0) {
        ivi = wl_registry_bind(reg, id, &wl_ivi_interface, min(ver, 1));
        wl_proxy_add_listener((struct wl_proxy *)ivi, (void (**)(void))&ivi_listener, NULL);
        LOG_D("  --  Setting IVI  %p", ivi);
    }
}
