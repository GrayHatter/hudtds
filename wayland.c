#include "wayland.h"

#include "hud.h"
#include "log.h"

#include "wl/ui.h"
#include "wl/ivi.h"
#include "wl/seat.h"

#include <wayland-client.h>

#define __USE_XOPEN_EXTENDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>


static struct wl_compositor    *compositor;
static struct wl_region        *opaque_region;
static struct wl_display       *display;
static struct wl_shell         *shell;
static struct wl_shm           *shm;
static struct wl_surface       *surface;
static struct wl_shell_surface *shell_surface;
static struct wl_registry      *registry;

static struct wl_shm_pool *pool;
static struct wl_buffer   *buffer;

struct pool_data *root_pool_data;


static struct wl_shm_pool *init_memory_pool(void)
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

    // memset(root_pool_data->memory, 0x00, HEIGHT * STRIDE);
    uint32_t *p = root_pool_data->memory;
    for (int i = 0; i < HEIGHT * WIDTH; i++) {
        *p++ = 0xff000000;
    }

    pool = wl_shm_create_pool(shm, root_pool_data->fd, root_pool_data->capacity);
    if (pool == NULL) {
        munmap(root_pool_data->memory, root_pool_data->capacity);
        free(root_pool_data);
        return NULL;
    }

    wl_shm_pool_set_user_data(pool, NULL);

    LOG_D("Pool done\n");
    return pool;
}


static void raze_memory_pool(struct wl_shm_pool *pool)
{
    wl_shm_pool_destroy(pool);
    munmap(root_pool_data->memory, root_pool_data->capacity);
    free(root_pool_data);
}


static struct wl_buffer *init_buffer(struct wl_shm_pool *pool)
{
    buffer = wl_shm_pool_create_buffer(pool, root_pool_data->size, WIDTH, HEIGHT, STRIDE, WL_SHM_FORMAT_ARGB8888);

    if (buffer == NULL) {
        return NULL;
    }

    root_pool_data->size += WIDTH * HEIGHT * sizeof(uint32_t);

    wl_surface_attach(surface, buffer, 0, 0);
    opaque_region = wl_compositor_create_region(compositor);
    wl_region_add(opaque_region, 0, 0, WIDTH, HEIGHT);
    wl_surface_set_opaque_region(surface, opaque_region);
    wl_surface_commit(surface);

    LOG_D("buffer done\n");
    return buffer;
}


static void raze_buffer(struct wl_buffer *buffer)
{
    wl_buffer_destroy(buffer);
}


static void handle_ping(void *data, struct wl_shell_surface *shell_surface, uint32_t serial)
{
    LOG_E("Pinged and ponged\n");
    (void) data;
    wl_shell_surface_pong(shell_surface, serial);
}


static void handle_configure(void *data, struct wl_shell_surface *surface, uint32_t edges, int32_t width,
    int32_t height)
{
    (void) data;
    LOG_E("surface configure%p %u %u x %u\n", surface, edges, width, height);
}


static void handle_popup_done(void *data, struct wl_shell_surface *shell_surface)
{
    (void) data;
    (void) shell_surface;
    LOG_E("popup done\n");
}


// TODO seems to segv when HUDTDS is set to full screen...
static const struct wl_shell_surface_listener surface_listener = {
    .ping = handle_ping,
    .configure = handle_configure,
    .popup_done = handle_popup_done,
};


static struct wl_shell_surface *init_root_surface(void)
{
    surface = wl_compositor_create_surface(compositor);
    if (surface == NULL) {
        LOG_E("bad surface\n");
        return NULL;
    }
    wl_surface_set_user_data(surface, NULL);

    shell_surface = wl_shell_get_shell_surface(shell, surface);
    if (shell_surface == NULL) {
        LOG_E("really bad surface\n");
        wl_surface_destroy(surface);
        return NULL;
    }
    wl_shell_surface_set_user_data(shell_surface, NULL);
    wl_shell_surface_add_listener(shell_surface, &surface_listener, 0);

    LOG_D("init_root_surface done\n");
    return shell_surface;
}


static void registry_global(void *data, struct wl_registry *registry, uint32_t id, const char *interface,
    uint32_t version)
{
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
        hud_init_seat(registry, id, version);
    } else {
        #ifdef __arm__
        hudivi_registry(registry, id, interface, version);
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


static const struct wl_registry_listener registry_listener = {
    .global = registry_global,
    .global_remove = registry_global_remove
};


struct wl_display *init_wayland(void)
{
    display = wl_display_connect(NULL);
    if (display == NULL) {
        LOG_E("Error opening display");
        exit(EXIT_FAILURE);
    }
    // get_info();

    registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, NULL);

    wl_display_dispatch(display);
    wl_display_roundtrip(display);

    // root_panel = init_ui();
    // TODO localize the panel for better memory safety
    init_ui();

    init_memory_pool();
    wl_display_dispatch(display);
    wl_display_roundtrip(display);

    init_root_surface();
    init_buffer(pool);
    wl_shell_surface_set_title(shell_surface, "HUDTDS");
    wl_shell_surface_set_toplevel(shell_surface);
    wl_shell_surface_set_fullscreen(shell_surface, WL_SHELL_SURFACE_FULLSCREEN_METHOD_DEFAULT, 0, NULL);

    return display;
}


int do_wayland(void)
{
    if (ui_iter(NULL)) {
        hud_surface_damage(0, 0, WIDTH, HEIGHT);
        hud_surface_commit();
    }

    int res = wl_display_dispatch(display);
    if (res < 0) {
        LOG_E("Display loop error\n");
    }

    return res;
}


static void raze_surface(struct wl_shell_surface *shell_surface)
{
    wl_shell_surface_destroy(shell_surface);
    wl_surface_destroy(surface);
}


void raze_wayland(void)
{

    raze_buffer(buffer);
    raze_surface(shell_surface);
    raze_memory_pool(pool);

    hud_raze_seat();

    wl_shell_destroy(shell);
    wl_shm_destroy(shm);
    wl_compositor_destroy(compositor);
    wl_display_disconnect(display);
}


void hud_surface_damage(int32_t x, int32_t y, int32_t w, int32_t h)
{
    wl_surface_damage(surface, x, y, w, h);
}


void hud_surface_commit()
{
    wl_surface_commit(surface);
}
