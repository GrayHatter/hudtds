#include "ui.h"

#include "draw.h"

#include "../hud.h"
#include "../log.h"

#include <stdlib.h>

struct ui_panel *root_panel = NULL;

#define XYWH_TO_CHILD() do {\
    l_x = panel->pos_x < 0 ? w + panel->pos_x : x + panel->pos_x; \
    l_y = panel->pos_y < 0 ? h + panel->pos_y : y + panel->pos_x; \
    l_w = panel->width <= 0 ? w + panel->width : x + panel->width; \
    l_h = panel->height <= 0 ? h + panel->height : y + panel->height; \
} while (0)


bool ui_touch_down(struct ui_panel *panel, const int x, const int y, const uint32_t id, const uint32_t serial)
{
    struct ui_panel **children = panel->children;
    if (children) {
        struct ui_panel *p;
        while ((p = *children++)) {
            if (x >= p->pos_x && y >= p->pos_y && x <= p->pos_x + p->width && y <= p->pos_y + p->height) {
                if (ui_touch_down(p, x, y, id, serial)) {
                    return true;
                }
            }
        }
    }

    if (panel->t_dn
        && x >= panel->pos_x && y >= panel->pos_y
        && x <= panel->pos_x + panel->width && y <= panel->pos_y + panel->height) {
            return panel->t_dn(panel, x, y, id, serial);
    }

    return false;
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


void ui_panel_draw(struct ui_panel *panel, int32_t x, int32_t y, int32_t w, int32_t h)
{
    LOG_E("ui panel draw %p\n", panel);
    if (panel->draw) {
        LOG_E("drawing\n");
        panel->draw(panel, x, y, w, h);
    }

    // hack to avoid boxing the root_panel
    if (panel->color) {
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
        l_x = panel->pos_x < 0 ? w - 1 + panel->pos_x : x + panel->pos_x;
        l_y = panel->pos_y < 0 ? h - 1 + panel->pos_y : y + panel->pos_x;
        l_w = panel->width <= 0 ? w + panel->width : x + panel->width;
        l_h = panel->height <= 0 ? h + panel->height : y + panel->height;
        while ((p = *children++)) {
            ui_panel_draw(p, l_x, l_y, l_w, l_h);
        }
    }
}


bool ui_iter(struct ui_panel *panel)
{
    bool draw = false;
    // FIXME
    panel = root_panel;
    // LOG_E("iter panel %p \n", panel);

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


static bool touch_test_1(struct ui_panel *p, const int x, const int y, const uint32_t id, const uint32_t serial)
{
    (void) p;
    (void) x;
    (void) y;
    (void) id;
    (void) serial;
    LOG_E("touch test 1\n");

    return true;
}


#include <pthread.h>
#include "../sound.h"

static bool touch_test_2(struct ui_panel *p, const int x, const int y, const uint32_t id, const uint32_t serial)
{
    (void) p;
    (void) x;
    (void) y;
    (void) id;
    (void) serial;

    pthread_t t;
    pthread_create(&t, NULL, hud_snd_play, (void *)NULL);
    LOG_E("touch test 2\n");
    return true;
}


static bool touch_test_3(struct ui_panel *p, const int x, const int y, const uint32_t id, const uint32_t serial)
{
    (void) p;
    (void) x;
    (void) y;
    (void) id;
    (void) serial;
    LOG_E("touch test 3\n");
    return true;
}


static bool touch_test_4(struct ui_panel *p, const int x, const int y, const uint32_t id, const uint32_t serial)
{
    (void) p;
    (void) x;
    (void) y;
    (void) id;
    (void) serial;
    LOG_E("touch test 4\n");
    return true;
}


static bool touch_test_5(struct ui_panel *p, const int x, const int y, const uint32_t id, const uint32_t serial)
{
    (void) p;
    (void) x;
    (void) y;
    (void) id;
    (void) serial;
    LOG_E("touch test 5\n");
return true;
}


struct ui_panel *init_ui(void)
{
    root_panel = calloc(1, sizeof (struct ui_panel));

    root_panel->t_dn = ui_touch_down;
    root_panel->t_dn = ui_touch_up;

    root_panel->k_dn = ui_key_down;
    root_panel->k_dn = ui_key_up;

    root_panel->width = WIDTH;
    root_panel->height = HEIGHT;

    LOG_E("init root children");
    root_panel->children = calloc(6, sizeof (struct ui_panel *));
    root_panel->children[0] = mk_panel();
    root_panel->children[0]->t_dn = touch_test_1;
    root_panel->children[0]->color = 0xff000000;
    root_panel->children[0]->pos_x *= 0;
    root_panel->children[1] = mk_panel();
    root_panel->children[1]->t_dn = touch_test_2;
    root_panel->children[1]->color = 0xff0000ff;
    root_panel->children[1]->pos_x *= 1;
    root_panel->children[2] = mk_panel();
    root_panel->children[2]->t_dn = touch_test_3;
    root_panel->children[2]->color = 0xff00ff00;
    root_panel->children[2]->pos_x *= 2;
    root_panel->children[3] = mk_panel();
    root_panel->children[3]->t_dn = touch_test_4;
    root_panel->children[3]->color = 0xffff0000;
    root_panel->children[3]->pos_x *= 3;
    root_panel->children[4] = mk_panel();
    root_panel->children[4]->t_dn = touch_test_5;
    root_panel->children[4]->color = 0xffff00ff;
    root_panel->children[4]->pos_x *= 4;
    root_panel->children[5] = NULL;

    LOG_E(" -- done\n");

    return root_panel;
}
