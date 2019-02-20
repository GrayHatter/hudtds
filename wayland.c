#include "wayland.h"

#include "hud.h"
#include "log.h"

#include "wl/ui.h"
#include "wl/ivi.h"
#include "wl/seat.h"
#include "wl/draw.h"

#include <wayland-client.h>

#define __USE_XOPEN_EXTENDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


static struct wl_compositor    *compositor;
static struct wl_display       *display;
static struct wl_shell         *shell;
static struct wl_shm           *shm;
static struct wl_registry      *registry;


struct surface *root_surface;



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


static struct surface *init_surface(struct surface *surface, uint32_t width, uint32_t height, uint32_t stride, uint8_t count)
{
    surface->width = width;
    surface->height = height;
    surface->stride = stride;
    surface->count = count;

    surface->surface = wl_compositor_create_surface(compositor);
    if (surface->surface == NULL) {
        LOG_E("bad surface\n");
        return NULL;
    }
    wl_surface_set_user_data(surface->surface, NULL);

    surface->shell_surface = wl_shell_get_shell_surface(shell, surface->surface);
    if (surface->shell_surface == NULL) {
        LOG_E("really bad surface\n");
        wl_surface_destroy(surface->surface);
        return NULL;
    }
    wl_shell_surface_set_user_data(surface->shell_surface, NULL);
    wl_shell_surface_add_listener(surface->shell_surface, &surface_listener, 0);


    LOG_D("init_root_surface done\n");
    return surface;
}


static struct surface *init_memory_pool(struct surface *surface)
{
    LOG_E("init_pool startup\n");
    surface->mem_capacity = surface->height * surface->stride * surface->count;
    surface->frame_size = surface->height * surface->stride;

    surface->fd = shm_open("/hudtds-frame-buffer", O_RDWR | O_CREAT, S_IRWXO | S_IRWXG | S_IRWXU);
    if (surface->fd < 0) {
        LOG_F("Unable to shmopen\n");
        exit(1);
    }

    if (ftruncate(surface->fd, surface->mem_capacity) < 0) {
        close(surface->fd);
        LOG_F("ftruncate error\n");
        exit(1);
    }

    surface->memory = mmap(0, surface->mem_capacity, PROT_READ | PROT_WRITE, MAP_SHARED, surface->fd, 0);
    if (surface->memory == MAP_FAILED) {
        LOG_F("Error in truncation for surface image\n");
        exit(1);
    }

    LOG_E("write\n");
    memset(surface->memory, 0xff, surface->mem_capacity);

    draw_swap_buffer(root_surface->memory);

    // // memset(surface->memory, 0x00, HEIGHT * STRIDE);
    // uint32_t *p = surface->memory;
    // for (int i = 0; i < HEIGHT * WIDTH; i++) {
    //     *p++ = 0xff000000;
    // }

    surface->pool = wl_shm_create_pool(shm, surface->fd, surface->mem_capacity);
    if (surface->pool == NULL) {
        munmap(surface->memory, surface->mem_capacity);
        LOG_F("unable to create pool :/\n");
        exit(3);
        return NULL;
    }

    wl_shm_pool_set_user_data(surface->pool, NULL);

    LOG_D("Pool done\n");

    for (uint32_t i = 0; i < surface->count; i++) {
        surface->buffer[i] = wl_shm_pool_create_buffer(surface->pool, surface->frame_size * i, surface->width, surface->height,
        surface->stride, WL_SHM_FORMAT_ARGB8888);
        if (surface->buffer[i] == NULL) {
            return NULL;
        }
    }

    wl_surface_attach(surface->surface, surface->buffer[0], 0, 0);
    wl_surface_commit(surface->surface);

    // opaque_region = wl_compositor_create_region(compositor);
    // wl_region_add(opaque_region, 0, 0, WIDTH, HEIGHT);
    // wl_surface_set_opaque_region(surface, opaque_region);

    LOG_D("buffer done\n");
    return surface;
}


static void raze_memory_pool(struct wl_shm_pool *pool)
{
    wl_shm_pool_destroy(pool);
    munmap(root_surface->memory, root_surface->mem_capacity);
    free(root_surface);
}



static void raze_buffer(struct wl_buffer *buffer)
{
    wl_buffer_destroy(buffer);
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

    struct surface *surface = calloc(1, sizeof(struct surface));
    if (surface == NULL) {
        return NULL;
    }

    root_surface = surface;

    init_surface(surface, WIDTH, HEIGHT, STRIDE, 2);
    init_memory_pool(surface);

    wl_shell_surface_set_title(root_surface->shell_surface, "HUDTDS");
    wl_shell_surface_set_toplevel(root_surface->shell_surface);
    wl_shell_surface_set_fullscreen(root_surface->shell_surface, WL_SHELL_SURFACE_FULLSCREEN_METHOD_DEFAULT, 0, NULL);

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


static void raze_surface(struct surface *surface)
{
    wl_shell_surface_destroy(surface->shell_surface);
    wl_surface_destroy(surface->surface);
}


void raze_wayland(void)
{

    raze_buffer(root_surface->buffer[1]);
    raze_buffer(root_surface->buffer[0]);
    raze_surface(root_surface);
    raze_memory_pool(root_surface->pool);

    hud_raze_seat();

    wl_shell_destroy(shell);
    wl_shm_destroy(shm);
    wl_compositor_destroy(compositor);
    wl_display_disconnect(display);
}


void hud_surface_damage(int32_t x, int32_t y, int32_t w, int32_t h)
{
    wl_surface_damage(root_surface->surface, x, y, w, h);
}


void hud_surface_commit()
{
    LOG_E("surface commit\n");
    static uint8_t next = 0;
    uint32_t wl_offset = ++next % root_surface->count;
    uint32_t draw_offset = (next + 1) % root_surface->count;

    draw_swap_buffer(root_surface->memory + ((root_surface->width * root_surface->height) * draw_offset));

    wl_surface_attach(root_surface->surface, root_surface->buffer[wl_offset], 0, 0);
    wl_surface_commit(root_surface->surface);
}
