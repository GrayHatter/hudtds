#include "ui.h"

#include "draw.h"
#include "text.h"

#include "../hud.h"
#include "../log.h"
#include "../wayland.h"

#include "../gui/root.h"

#include <stdlib.h>

static struct ui_panel *ui_root_panel = NULL;

#define XYWH_TO_CHILD() do {\
    l_x = panel->pos_x < 0 ? w + panel->pos_x : x + panel->pos_x; \
    l_y = panel->pos_y < 0 ? h + panel->pos_y : y + panel->pos_y; \
    l_w = panel->width <= 0 ? w + panel->width : l_x + panel->width; \
    l_h = panel->height <= 0 ? h + panel->height : l_y + panel->height; \
} while (0)


bool ui_touch_down(struct ui_panel *panel, const int mx, const int my, const int x, const int y,
    const uint32_t w, const uint32_t h, const uint32_t id, const uint32_t serial)
{
    LOG_D("ui touch down %s %i %i %i %i %u %u\n", panel->name, mx, my, x, y, w, h);

    int32_t l_x = x, l_y = y, l_w = w, l_h = h;
    XYWH_TO_CHILD();

    struct ui_panel **children = panel->children;
    if (children) {
        struct ui_panel *p;
        while ((p = *children++)) {
            if (ui_touch_down(p, mx, my, l_x, l_y, l_w, l_h, id, serial)) {
                return true;
            }
        }
    }

    LOG_D("ui touch down %s %i %i %i %i %u %u\n", panel->name, mx, my, l_x, l_y, l_w, l_h);
    if (panel->t_dn && l_x <= mx && mx <= l_w && l_y <= my && my <= l_h) {
        return panel->t_dn(panel, mx, my, x, y, w, h, id, serial);
    }
    return false;
}

bool ui_root_touch_down(const int x, const int y, const uint32_t id, const uint32_t serial)
{
    return ui_touch_down(ui_root_panel, x, y, 0, 0, ui_root_panel->width, ui_root_panel->height, id, serial);
}

bool ui_touch_up(struct ui_panel *panel, const int x, const int y, const uint32_t w,
    const uint32_t h, const uint32_t id, const uint32_t serial)
{
    if (panel->t_up) {
        panel->t_up(panel, x, y, w, h, id, serial);
    }

    struct ui_panel **children = panel->children;
    if (children) {
        struct ui_panel *p;
        while ((p = *children++)) {
            ui_touch_up(p, x, y, w, h, id, serial);
        }
    }
    return true;
}


bool ui_touch_up_root(const uint32_t id, const uint32_t serial)
{
    return ui_touch_up(ui_root_panel, 0, 0, ui_root_panel->width, ui_root_panel->height, id, serial);
}


static struct ui_panel *find_focused(struct ui_panel *panel)
{
    struct ui_panel *focused = NULL;

    struct ui_panel **children = panel->children;
    if (children) {
        struct ui_panel *p;
        while ((p = *children++)) {
            if (!focused && p->focused) {
                focused = p;
            } else {
                p->focused = false;
            }
        }
    }

    return focused;
}


bool ui_key_down(struct ui_panel *panel, const uint32_t key, const uint32_t serial)
{
    LOG_T("Keydown on %s\n", panel->name);

    struct ui_panel *focused = find_focused(panel);
    if (focused) {
        if (ui_key_down(focused, key, serial)) {
            return true;
        }
    }

    if (panel->k_dn) {
        if (panel->k_dn(panel, key, serial)) {
            return true;
        }
    }
    return false;
}


bool ui_key_up(struct ui_panel *panel, const uint32_t key, const uint32_t serial)
{
    LOG_T("Keyup on %s", panel->name);

    struct ui_panel *focused = find_focused(panel);
    if (focused) {
        if (ui_key_up(focused, key, serial)) {
            return true;
        }
    }

    if (panel->k_up) {
        if (panel->k_up(panel, key, serial)) {
            return true;
        }
    }
    return false;
}


bool ui_key_down_root(const uint32_t key, const uint32_t serial)
{
    return ui_key_down(ui_root_panel, key, serial);
}


bool ui_key_up_root(const uint32_t key, const uint32_t serial)
{
    return ui_key_up(ui_root_panel, key, serial);
}


void ui_panel_draw(struct ui_panel *panel, int32_t x, int32_t y, int32_t w, int32_t h)
{
    LOG_D("ui panel draw %p\n", panel);
    if (panel->draw) {
        panel->draw(panel, x, y, w, h);
    } else if (panel->color) {
        // hack to avoid boxing the ui_root_panel
        int32_t l_x = x, l_y = y, l_w = w, l_h = h;
        l_x = panel->pos_x < 0 ? w + panel->pos_x : x + panel->pos_x;
        l_y = panel->pos_y < 0 ? h + panel->pos_y : y + panel->pos_y;
        l_w = panel->width <= 0 ? w + panel->width : x + panel->width;
        l_h = panel->height <= 0 ? h + panel->height : y + panel->height;

        (void) l_w;
        (void) l_h;
        draw_box_c(l_x, l_y, l_x + panel->width, l_y + panel->height, panel->color);
    }

    struct ui_panel **children = panel->children;
    if (children) {
        struct ui_panel *p;
        int32_t l_x = x, l_y = y, l_w = w, l_h = h;
        XYWH_TO_CHILD();
        while ((p = *children++)) {
            ui_panel_draw(p, l_x, l_y, l_w, l_h);
        }
    }
}


bool ui_iter(struct ui_panel *panel)
{
    bool draw = false;
    // FIXME
    panel = ui_root_panel;
    // LOG_W("iter panel %p \n", panel);

    hud_surface_damage(30, 30, 60, 60);
    hud_surface_commit();

    ui_panel_draw(panel, 0, 0, panel->width, panel->height);


    return draw;
}


struct ui_panel *mk_panel(void)
{
    struct ui_panel *p = calloc(1, sizeof (struct ui_panel));
    // p->draw = ui_panel_draw;
    p->pos_x = 80;
    p->pos_y = -80;
    p->height = 80;
    p->width = 80;
    return p;
};





struct ui_panel *init_ui(void)
{
    ui_root_panel = gui_build_root();
    init_text();
    return ui_root_panel;
}
