#include "hud.h"

#include "sound.h"

#include "wl/keyboard.h"

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>


#define WIDTH 800
#define HEIGHT 420
#define STRIDE WIDTH * 4
#define CURSOR_WIDTH 100
#define CURSOR_HEIGHT 59
#define CURSOR_HOT_SPOT_X 10
#define CURSOR_HOT_SPOT_Y 3

static bool done = false;

struct wl_compositor *compositor;
struct wl_display    *display;
struct wl_pointer    *pointer;
struct wl_seat       *seat;
struct wl_shell      *shell;
struct wl_shm        *shm;
struct wl_surface    *surface;
struct wl_touch      *touch;

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

struct pointer_data {
    struct wl_surface *surface;
    struct wl_buffer *buffer;
    int32_t hot_spot_x;
    int32_t hot_spot_y;
    struct wl_surface *target_surface;
};

struct pool_data {
    int fd;
    uint32_t *memory;
    unsigned capacity;
    unsigned size;
};
struct pool_data *root_pool_data;


static void touch_paint(int32_t x, int32_t y, int32_t id, bool mdown)
{
    if (x < 2 || x >= WIDTH - 2 || y < 2 || y >= HEIGHT - 2) {
        printf("no\n");
        return;
    }

    static const uint32_t colors[] = {
        0xffff0000,
        0xff00ff00,
        0xff0000ff,
        0xff00ffff,
        0xffff00ff,
        0xffffff00,
    };
    static uint32_t map[20] = {0};
    static uint8_t t = 0;

    if (mdown) {
        map[id] = colors[t++ % 6];
    }

    uint32_t c = map[id % 20];

    uint32_t *p = root_pool_data->memory + (x - 2) + (y - 2) * WIDTH;
    p[2] = c;
    p += WIDTH;
    p[1] = c;
    p[2] = c;
    p[3] = c;
    p += WIDTH;
    p[0] = c;
    p[1] = c;
    p[2] = c;
    p[3] = c;
    p[4] = c;
    p += WIDTH;
    p[1] = c;
    p[2] = c;
    p[3] = c;
    p += WIDTH;
    p[2] = c;

    wl_surface_damage(surface, x - 2, y - 2, 5, 5);
    wl_surface_commit(surface);
}

/***************************************************************************************
 ***  Touch Interface                                                                ***
 ***************************************************************************************/

static void touch_down(void *data, struct wl_touch *wl_touch, uint32_t serial, uint32_t time,
                       struct wl_surface *surface, int32_t id, wl_fixed_t f_x, wl_fixed_t f_y)
{
    (void) data;
    (void) wl_touch;
    (void) serial;
    (void) time;
    (void) surface;

    float x = wl_fixed_to_double(f_x);
    float y = wl_fixed_to_double(f_y);
    printf("touch down %f %f %u\n", x, y, id);

    touch_paint(x, y, id, true);
}

static void touch_up(void *data, struct wl_touch *wl_touch, uint32_t serial, uint32_t time, int32_t id)
{
    (void) data;
    (void) wl_touch;
    (void) serial;
    (void) time;
    (void) id;
    printf("touch up\n");
}

static void touch_motion(void *data, struct wl_touch *wl_touch, uint32_t time, int32_t id, wl_fixed_t f_x,
                         wl_fixed_t f_y)
{
    (void) data;
    (void) wl_touch;
    (void) time;

    float x = wl_fixed_to_double(f_x);
    float y = wl_fixed_to_double(f_y);
    printf("touch move %f %f\n", x, y);

    touch_paint(x, y, id, false);
}

static void touch_frame(void *data, struct wl_touch *wl_touch)
{
    (void) data;
    (void) wl_touch;
    printf("touch frame\n");
}

static void touch_cancel(void *data, struct wl_touch *wl_touch)
{
    (void) data;
    (void) wl_touch;
    printf("touch cancelled\n");
}

static const struct wl_touch_listener touch_listener = {
    .down   = touch_down,
    .up     = touch_up,
    .motion = touch_motion,
    .frame  = touch_frame,
    .cancel = touch_cancel
};


/***************************************************************************************
 ***  Pointer Interface                                                              ***
 ***************************************************************************************/

/*
static void pointer_enter(void *data, struct wl_pointer *wl_pointer, uint32_t serial, struct wl_surface *surface,
                          wl_fixed_t surface_x, wl_fixed_t surface_y)
{
    printf("pointer in\n");
    struct pointer_data *pointer_data;

    pointer_data = wl_pointer_get_user_data(wl_pointer);
    pointer_data->target_surface = surface;
    wl_surface_attach(pointer_data->surface, pointer_data->buffer, 0, 0);
    wl_surface_commit(pointer_data->surface);
    wl_pointer_set_cursor(wl_pointer, serial, pointer_data->surface, pointer_data->hot_spot_x, pointer_data->hot_spot_y);
}

static void pointer_leave(void *data, struct wl_pointer *wl_pointer, uint32_t serial, struct wl_surface *wl_surface)
{
    (void) data;
    printf("pointer out\n");
}

static void pointer_motion(void *data, struct wl_pointer *wl_pointer, uint32_t time, wl_fixed_t surface_x,
                           wl_fixed_t surface_y)
{
    (void) data;
    printf("pointer move\n");
}

static void pointer_button(void *data, struct wl_pointer *wl_pointer, uint32_t serial, uint32_t time, uint32_t button,
                           uint32_t state)
{
    (void) data;
    printf("pointer click\n");

}

static void pointer_axis(void *data, struct wl_pointer *wl_pointer, uint32_t time, uint32_t axis, wl_fixed_t value){}

static const struct wl_pointer_listener pointer_listener = {
    .enter = pointer_enter,
    .leave = pointer_leave,
    .motion = pointer_motion,
    .button = pointer_button,
    .axis = pointer_axis
};
*/

static void seat_handle_capabilities(void *data, struct wl_seat *seat, enum wl_seat_capability caps)
{
    (void) data;
    printf("seat cap\n");

    if (caps & WL_SEAT_CAPABILITY_KEYBOARD) {
        hud_kb_init(seat);
    } else {
        // TODO
        hud_kb_raze();
    }

    /*
    if ((caps & WL_SEAT_CAPABILITY_POINTER) && !pointer) {
        printf("setting pointer\n");
        pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(pointer, &pointer_listener, NULL);
    } else if (!(caps & WL_SEAT_CAPABILITY_POINTER) && pointer) {
        printf("killing pointer\n");
        wl_pointer_destroy(pointer);
        pointer = NULL;
    }
    */

    if ((caps & WL_SEAT_CAPABILITY_TOUCH) && !touch) {
        printf("setting touch\n");
        touch = wl_seat_get_touch(seat);
        wl_touch_add_listener(touch, &touch_listener, NULL);
    } else if (!(caps & WL_SEAT_CAPABILITY_TOUCH) && touch) {
        printf("killing touch\n");
        wl_touch_destroy(touch);
        touch = NULL;
    }
}

static const struct wl_seat_listener seat_listener = {
    .capabilities = seat_handle_capabilities,
};


static struct wl_shm_pool *init_memory_pool(void)
{
    char fname[] = "/tmp/hudtds-shared-XXXXXX";
    int image = mkstemp(fname);
    if (image < 0) {
        printf("Error opening surface image");
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

    root_pool_data->capacity = HEIGHT * WIDTH * sizeof(uint32_t);
    root_pool_data->size = 0;
    root_pool_data->fd = image;

    root_pool_data->memory = mmap(0, root_pool_data->capacity, PROT_READ | PROT_WRITE, MAP_SHARED, root_pool_data->fd, 0);
    if (root_pool_data->memory == MAP_FAILED) {
        free(root_pool_data);
        return NULL;
    }

    memset(root_pool_data->memory, 0xff, HEIGHT * STRIDE);

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
    printf("Pinged and ponged\n");
}

static void handle_configure(void *data, struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width,
                             int32_t height)
{
    (void) data;
    (void) shell_surface;
    (void) edges;
    (void) width;
    (void) height;
    printf("surface configure\n");
}

static void handle_popup_done(void *data, struct wl_shell_surface *shell_surface)
{
    (void) data;
    (void) shell_surface;
    printf("popup done\n");
}

struct wl_shell_surface *init_root_surface(void)
{
    surface = wl_compositor_create_surface(compositor);
    if (surface == NULL) {
        printf("bad surface\n");
        return NULL;
    }

    struct wl_shell_surface *shell_surface = wl_shell_get_shell_surface(shell, surface);
    if (shell_surface == NULL) {
        printf("really bad surface\n");
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

void init_cursor(struct wl_shm_pool *pool, unsigned width, unsigned height, int32_t hot_spot_x,
                                int32_t hot_spot_y)
{
    struct pointer_data *data = malloc(sizeof(struct pointer_data));
    if (data == NULL) {
        printf("Unable to allocate cursor");
        return;
    }

    data->hot_spot_x = hot_spot_x;
    data->hot_spot_y = hot_spot_y;
    data->surface = wl_compositor_create_surface(compositor);

    if (data->surface == NULL) {
        printf("Unable to create cursor surface");
        free(data);
        return;
    }

    data->buffer = init_buffer(pool, width, height);

    if (data->buffer == NULL) {
        printf("Unable to create cursor buffer");
        wl_pointer_set_user_data(pointer, data);
        free(data);
    }
    return;
}

void raze_cursor(void)
{
    struct pointer_data *data = wl_pointer_get_user_data(pointer);
    wl_buffer_destroy(data->buffer);
    wl_surface_destroy(data->surface);
    free(data);
    wl_pointer_set_user_data(pointer, NULL);
}


static void registry_global(void *data, struct wl_registry *registry, uint32_t id, const char *interface,
                            uint32_t version)
{
    (void) data;
    printf("Got a registry event for %s id %d", interface, id);

    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        printf("  --  Setting Compositor");
        compositor = wl_registry_bind(registry, id, &wl_compositor_interface, min(version, 4));
    } else if (strcmp(interface, wl_shm_interface.name) == 0) {
        printf("  --  Setting shm");
        shm = wl_registry_bind(registry, id, &wl_shm_interface, min(version, 1));
    } else if (strcmp(interface, wl_shell_interface.name) == 0) {
        printf("  --  Setting Shell");
        shell = wl_registry_bind(registry, id, &wl_shell_interface, min(version, 1));
    } else if (strcmp(interface, wl_seat_interface.name) == 0) {
        printf("  --  Setting Seat");
        seat = wl_registry_bind(registry, id, &wl_seat_interface, min(version, 2));
        wl_seat_add_listener(seat, &seat_listener, NULL);
    }
    printf("\n");
}

static void registry_global_remove(void *data, struct wl_registry *reg, uint32_t id)
{
    (void) data;
    (void) reg;
    (void) id;
    printf("Registry_remover\n");
}

void init_wayland(void)
{
    display = wl_display_connect(NULL);
    if (display == NULL) {
        printf("Error opening display");
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
}

static void raze_wayland(void)
{
    wl_pointer_destroy(pointer);
    wl_seat_destroy(seat);
    wl_shell_destroy(shell);
    wl_shm_destroy(shm);
    wl_compositor_destroy(compositor);
    wl_display_disconnect(display);
}

int main(void)
{
    struct wl_buffer *buffer;
    struct wl_shm_pool *pool;
    struct wl_shell_surface *surface;

    init_wayland();
    printf("wl init done\n");


    pool = init_memory_pool();
    printf("Pool done\n");

    surface = init_root_surface();
    printf("surface done\n");

    buffer = init_buffer(pool, WIDTH, HEIGHT);
    printf("buffer done\n");

    bind_buffer(buffer, surface);
    printf("bind done\n");

    //init_cursor(pool, CURSOR_WIDTH, CURSOR_HEIGHT, CURSOR_HOT_SPOT_X, CURSOR_HOT_SPOT_Y);
    // printf("cursor done\n");

    while (!done) {
        if (wl_display_dispatch(display) < 0) {
            printf("Main loop error");
            done = true;
        }
    }

    printf("Exiting sample wayland client...\n");

    raze_cursor();
    raze_buffer(buffer);
    raze_surface(surface);
    raze_memory_pool(pool);


    raze_wayland();

    return EXIT_SUCCESS;
}
