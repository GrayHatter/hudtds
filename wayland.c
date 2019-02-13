#include "wayland.h"

#include "hud.h"
#include "log.h"

#include "ivi-header-mazda.h"

#include "wl/ui.h"
#include "wl/keyboard.h"
#include "wl/touch.h"

#define __USE_XOPEN_EXTENDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>


struct wl_compositor *compositor;
struct wl_display    *display;
struct wl_seat       *seat;
struct wl_shell      *shell;
struct wl_shm        *shm;
struct wl_surface    *surface;

struct pool_data *root_pool_data;


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


void hud_surface_damage(int32_t x, int32_t y, int32_t w, int32_t h)
{
    wl_surface_damage(surface, x, y, w, h);
}


void hud_surface_commit()
{
    wl_surface_commit(surface);
}


struct wl_shm_pool *init_memory_pool(void)
{
    char fname[] = "/tmp/hudtds-shared-XXXXXX";
    int image = mkstemp(fname);
    if (image < 0) {
        LOG_E("Error opening surface image");
        return NULL;
    }

    if (ftruncate(image, HEIGHT * STRIDE) < 0) {
        close(image);
        return NULL;
    }

    struct stat stat;
    if (fstat(image, &stat) != 0) {
        return NULL;
    }

    root_pool_data = calloc(1, sizeof(struct pool_data));

    if (root_pool_data == NULL) {
        return NULL;
    }

    root_pool_data->capacity = HEIGHT * STRIDE;
    root_pool_data->size = 0;
    root_pool_data->fd = image;

    root_pool_data->memory = mmap(0, root_pool_data->capacity, PROT_READ | PROT_WRITE, MAP_SHARED, root_pool_data->fd, 0);
    if (root_pool_data->memory == MAP_FAILED) {
        free(root_pool_data);
        return NULL;
    }

    uint32_t *p = root_pool_data->memory;
    for (int i = 0; i < HEIGHT * WIDTH; i++) {
        *p++ = 0xff000000;
    }

    struct wl_shm_pool *pool = wl_shm_create_pool(shm, root_pool_data->fd, root_pool_data->capacity);
    if (pool == NULL) {
        munmap(root_pool_data->memory, root_pool_data->capacity);
        free(root_pool_data);
        return NULL;
    }

    wl_shm_pool_set_user_data(pool, root_pool_data);

    return pool;
}

void raze_memory_pool(struct wl_shm_pool *pool)
{
    struct pool_data *data = wl_shm_pool_get_user_data(pool);
    wl_shm_pool_destroy(pool);
    munmap(data->memory, data->capacity);
    free(data);
}


struct wl_buffer *init_buffer(struct wl_shm_pool *pool, unsigned width, unsigned height)
{
    struct pool_data *pool_data = wl_shm_pool_get_user_data(pool);
    struct wl_buffer *buffer = wl_shm_pool_create_buffer(pool, pool_data->size, width, height, width * sizeof(uint32_t), WL_SHM_FORMAT_ARGB8888);
    if (buffer == NULL) {
        return NULL;
    }

    pool_data->size += width * height * sizeof(uint32_t);

    return buffer;
}

void raze_buffer(struct wl_buffer *buffer)
{
    wl_buffer_destroy(buffer);
}


static void handle_ping(void *data, struct wl_shell_surface *shell_surface, uint32_t serial)
{
    (void) data;
    wl_shell_surface_pong(shell_surface, serial);
    LOG_E("Pinged and ponged\n");
}


static void handle_configure(void *data, struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width,
                             int32_t height)
{
    (void) data;
    (void) shell_surface;
    (void) edges;
    (void) width;
    (void) height;
    LOG_E("surface configure\n");
}

static void handle_popup_done(void *data, struct wl_shell_surface *shell_surface)
{
    (void) data;
    (void) shell_surface;
    LOG_E("popup done\n");
}


struct wl_shell_surface *init_root_surface(void)
{
    surface = wl_compositor_create_surface(compositor);
    if (surface == NULL) {
        LOG_E("bad surface\n");
        return NULL;
    }

    struct wl_shell_surface *shell_surface = wl_shell_get_shell_surface(shell, surface);
    if (shell_surface == NULL) {
        LOG_E("really bad surface\n");
        wl_surface_destroy(surface);
        return NULL;
    }

    const struct wl_shell_surface_listener surface_listener = {
        .ping = handle_ping,
        .configure = handle_configure,
        .popup_done = handle_popup_done,
    };
    wl_shell_surface_add_listener(shell_surface, &surface_listener, 0);
    wl_shell_surface_set_toplevel(shell_surface);
    wl_shell_surface_set_user_data(shell_surface, surface);
    wl_surface_set_user_data(surface, NULL);
    wl_shell_surface_set_title(shell_surface, "HUDTDS");

    wl_surface_damage(surface, 0, 0, WIDTH, HEIGHT);

    return shell_surface;
}


#if __arm__
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


static void ivi_shell_create(void *data, struct wl_interface *interface, char *name)
{
    LOG_W("IVI SHELL created %s \n", name);
    (void) data;
    if (strcmp("HUDTDS", name) == 0) {
        struct wl_proxy *ani_grp = wl_proxy_create((struct wl_proxy *) interface, &wl_ivi_animation_group_interface);
        if (ani_grp) {
            wl_proxy_marshal((struct wl_proxy *) interface, IVI_SHELL_CREATE_ANIMATION_GROUP, ani_grp);
            wl_proxy_marshal(ani_grp, IVI_SHELL_ANIMATION_GROUP_ANIMATE_SCALAR, name, 2, 0, 0, wl_fixed_from_double(1.0), 0, 0);
            wl_proxy_marshal(ani_grp, IVI_SHELL_ANIMATION_GROUP_ANIMATE_SCALAR, name, 1, 0, 0, wl_fixed_from_double(10000.0), 0, 0);
            wl_proxy_marshal(ani_grp, IVI_SHELL_ANIMATION_GROUP_START);
            wl_proxy_marshal(ani_grp, IVI_SHELL_ANIMATION_GROUP_FINISH);
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

static const struct wl_ivi_listener ivi_listener = {
    .screen = ivi_screen_cb,
    .one = ivi_default,
};
#endif

static void registry_global(void *data, struct wl_registry *registry, uint32_t id, const char *interface,
                            uint32_t version)
{
#if __arm__
    static struct wl_interface *ivi;
    static struct wl_interface *ivi_shell;
    // static struct wl_interface *ivi_animation_group;
#endif

    (void) data;
    LOG_D("Got a registry event for %s id %d", interface, id);

    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        LOG_D("  --  Setting Compositor");
        compositor = wl_registry_bind(registry, id, &wl_compositor_interface, min(version, 4));
    } else if (strcmp(interface, wl_shm_interface.name) == 0) {
        LOG_D("  --  Setting shm");
        shm = wl_registry_bind(registry, id, &wl_shm_interface, min(version, 1));
    } else if (strcmp(interface, wl_shell_interface.name) == 0) {
        LOG_D("  --  Setting Shell");
        shell = wl_registry_bind(registry, id, &wl_shell_interface, min(version, 1));
    } else if (strcmp(interface, wl_seat_interface.name) == 0) {
        LOG_D("  --  Setting Seat");
        seat = wl_registry_bind(registry, id, &wl_seat_interface, min(version, 2));
        wl_seat_add_listener(seat, &seat_listener, NULL);
#if __arm__
    } else if (strcmp(interface, wl_ivi_shell_interface.name) == 0) {
        // wayland never met an inline function wrapper it didn't like.
        // TODO write my own ivi header that creates the inline wrapper func "just lime mom used to make"
        ivi_shell = wl_registry_bind(registry, id, &wl_ivi_shell_interface, min(version, 1));
        wl_proxy_add_listener((struct wl_proxy *)ivi_shell, (void (**)(void))&ivi_shell_listener, NULL);
        LOG_D("  --  Setting IVI SHELL %p", ivi_shell);

        // LOG_D("\n\t\t trying for animation group %p", ivi_shell);
        // ivi_animation_group = wl_proxy_create(wl_ivi_shell_interface, id, &wl_ivi_animation_group_interface, min(version, 1));
        // LOG_D(" -- ivi_ag %p", ivi_animation_group);
    } else if (strcmp(interface, wl_ivi_interface.name) == 0) {
        ivi = wl_registry_bind(registry, id, &wl_ivi_interface, min(version, 1));
        wl_proxy_add_listener((struct wl_proxy *)ivi, (void (**)(void))&ivi_listener, NULL);
        LOG_D("  --  Setting IVI  %p", ivi);
#endif
    }
    LOG_D("\n");
}


static void registry_global_remove(void *data, struct wl_registry *reg, uint32_t id)
{
    (void) data;
    (void) reg;
    (void) id;
    LOG_E("Registry_remover\n");
}


struct wl_display *init_wayland(void)
{
    display = wl_display_connect(NULL);
    if (display == NULL) {
        LOG_E("Error opening display");
        exit(EXIT_FAILURE);
    }

    const struct wl_registry_listener registry_listener = {
        .global = registry_global,
        .global_remove = registry_global_remove
    };

    struct wl_registry *registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, NULL);

    wl_display_dispatch(display);
    wl_display_roundtrip(display);

    // root_panel = init_ui();
    // TODO localize the panel for better memory safety
    init_ui();

    return display;
}


int do_wayland(void)
{

    if (ui_iter(NULL)) {
        hud_surface_commit();
    }

    int res = wl_display_dispatch(display);
    if (res < 0) {
        LOG_E("Display loop error\n");
    }

    return res;
}


void raze_wayland(void)
{
    wl_seat_destroy(seat);
    wl_shell_destroy(shell);
    wl_shm_destroy(shm);
    wl_compositor_destroy(compositor);
    wl_display_disconnect(display);
}


void raze_surface(struct wl_shell_surface *shell_surface)
{
    struct wl_surface *surface = wl_shell_surface_get_user_data(shell_surface);
    wl_shell_surface_destroy(shell_surface);
    wl_surface_destroy(surface);
}


void bind_buffer(struct wl_buffer *buffer, struct wl_shell_surface *shell_surface)
{
    struct wl_surface *surface = wl_shell_surface_get_user_data(shell_surface);
    wl_surface_attach(surface, buffer, 0, 0);
    wl_surface_commit(surface);
}
