#include "ui.h"

#include "draw.h"

#include "../hud.h"
#include "../log.h"

#include <stdlib.h>

struct ui_panel *root_panel = NULL;



bool ui_touch_down(struct ui_panel *panel, const int x, const int y, const uint32_t id, const uint32_t serial)
{
    if (panel->t_dn) {
        panel->t_dn(panel, x, y, id, serial);
    }

    struct ui_panel **children = panel->children;
    if (children) {
        struct ui_panel *p;
        while ((p = *children++)) {
          ui_touch_down(p, x, y, id, serial);
        }
    }
    return true;
}

bool ui_root_touch_down(const int x, const int y, const uint32_t id, const uint32_t serial)
{
    return ui_touch_down(root_panel, x, y, id, serial);
}

bool ui_touch_up(struct ui_panel *panel, const int x, const int y, const uint32_t id, const uint32_t serial)
{
    if (panel->t_up) {
        panel->t_up(panel, x, y, id, serial);
    }

    struct ui_panel **children = panel->children;
    if (children) {
        struct ui_panel *p;
        while ((p = *children++)) {
          ui_touch_up(p, x, y, id, serial);
        }
    }
    return true;
}


bool ui_root_touch_up(const int x, const int y, const uint32_t id, const uint32_t serial)
{
    return ui_touch_up(root_panel, x, y, id, serial);
}


bool ui_key_down(struct ui_panel *panel, const uint32_t key)
{
    (void) panel;
    (void) key;
    return true;
}


bool ui_key_up(struct ui_panel *panel, const uint32_t key)
{
    (void) panel;
    (void) key;
    return true;
}


void ui_panel_draw(struct ui_panel *panel)
{
    draw_box_c(panel->pos_x, panel->pos_y, panel->width, panel->height, panel->color);
}


bool ui_iter(struct ui_panel *panel)
{
    bool draw = false;
    // FIXME
    panel = root_panel;
    LOG_E("iter panel %p \n", panel);

    if (panel->draw) {
        panel->draw(panel);
    }

    struct ui_panel **children = panel->children;
    if (children) {
        struct ui_panel *p;
        while ((p = *children++)) {
          ui_panel_draw(p);
        }
    }
    return draw;
}


struct ui_panel *mk_panel(void)
{
    struct ui_panel *p = calloc(1, sizeof (struct ui_panel));
    p->draw = ui_panel_draw;
    p->pos_x = 50;
    p->pos_y = 10;
    p->height = 50;
    p->width = 50;
    return p;
};


struct ui_panel *init_ui(void)
{
    root_panel = calloc(1, sizeof (struct ui_panel));

    root_panel->t_dn = ui_touch_down;
    root_panel->t_dn = ui_touch_up;

    root_panel->k_dn = ui_key_down;
    root_panel->k_dn = ui_key_up;

    LOG_E("init root children");
    root_panel->children = calloc(6, sizeof (struct ui_panel *));
    root_panel->children[0] = mk_panel();
    root_panel->children[0]->color = 0xff000000;
    root_panel->children[0]->pos_x = 10 + 60 * 1;
    root_panel->children[1] = mk_panel();
    root_panel->children[1]->color = 0xff0000ff;
    root_panel->children[1]->pos_x = 10 + 60 * 2;
    root_panel->children[2] = mk_panel();
    root_panel->children[2]->color = 0xff00ff00;
    root_panel->children[2]->pos_x = 10 + 60 * 3;
    root_panel->children[3] = mk_panel();
    root_panel->children[3]->color = 0xffff0000;
    root_panel->children[3]->pos_x = 10 + 60 * 4;
    root_panel->children[4] = mk_panel();
    root_panel->children[4]->color = 0xffff00ff;
    root_panel->children[4]->pos_x = 10 + 60 * 5;
    root_panel->children[5] = NULL;

    LOG_E(" -- done\n");

    return root_panel;
}
