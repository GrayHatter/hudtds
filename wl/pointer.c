

/***************************************************************************************
 ***  Pointer Interface                                                              ***
 ***************************************************************************************/

/*
static void pointer_enter(void *data, struct wl_pointer *wl_pointer, uint32_t serial, struct wl_surface *surface,
                          wl_fixed_t surface_x, wl_fixed_t surface_y)
{
    LOG_E("pointer in\n");
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
    LOG_E("pointer out\n");
}

static void pointer_motion(void *data, struct wl_pointer *wl_pointer, uint32_t time, wl_fixed_t surface_x,
                           wl_fixed_t surface_y)
{
    (void) data;
    LOG_E("pointer move\n");
}

static void pointer_button(void *data, struct wl_pointer *wl_pointer, uint32_t serial, uint32_t time, uint32_t button,
                           uint32_t state)
{
    (void) data;
    LOG_E("pointer click\n");

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

void init_cursor(struct wl_shm_pool *pool, unsigned width, unsigned height, int32_t hot_spot_x,
                                int32_t hot_spot_y)
{
    struct pointer_data *data = malloc(sizeof (struct pointer_data));
    if (data == NULL) {
        LOG_E("Unable to allocate cursor");
        return;
    }

    data->hot_spot_x = hot_spot_x;
    data->hot_spot_y = hot_spot_y;
    data->surface = wl_compositor_create_surface(compositor);

    if (data->surface == NULL) {
        LOG_E("Unable to create cursor surface");
        free(data);
        return;
    }

    data->buffer = init_buffer(pool, width, height);

    if (data->buffer == NULL) {
        LOG_E("Unable to create cursor buffer");
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

