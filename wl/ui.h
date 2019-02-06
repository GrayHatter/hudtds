#ifndef _HUD_UI_H_
#define _HUD_UI_H_

#include <stdbool.h>
#include <stdint.h>

struct ui_panel;

typedef bool touch_dn(struct ui_panel *, const int, const int, const int, const int, const uint32_t, const uint32_t,
    const uint32_t, const uint32_t);
typedef bool touch_mv(struct ui_panel *, const int, const int, const int, const int, const uint32_t, const uint32_t,
    const uint32_t, const uint32_t);
typedef bool touch_up(struct ui_panel *, const int, const int, const uint32_t, const uint32_t, const uint32_t,
    const uint32_t);

typedef bool key_dn(struct ui_panel *, const uint32_t);
typedef bool key_up(struct ui_panel *, const uint32_t);

typedef uint32_t color_t;

typedef enum {
    PANEL_NONE = 0,
    PANEL_FRAME,
    PANEL_BUTTON,
    PANEL_LIST,


    PANEL_OTHER,
    PANEL_MAX = 255
} PANEL_TYPE;


struct ui_panel {
    bool draw_needed;
    PANEL_TYPE type;

    char *name;

    touch_dn *t_dn;
    touch_up *t_mv;
    touch_up *t_up;

    key_dn *k_dn;
    key_up *k_up;

    void (*draw)(struct ui_panel *, int32_t, int32_t, int32_t, int32_t);

    bool focused;
    int32_t pos_x;
    int32_t pos_y;
    int32_t width;
    int32_t height;

    struct ui_panel *prev;
    struct ui_panel *next;

    color_t color;

    struct ui_panel **children;
};




bool ui_root_touch_down(const int x, const int y, const uint32_t id, const uint32_t serial);
bool ui_touch_move_root(const int x, const int y, const uint32_t id, const uint32_t serial);
bool ui_touch_up_root(const uint32_t id, const uint32_t serial);

bool ui_iter(struct ui_panel *panel);

struct ui_panel *init_ui(void);



#endif // _HUD_UI_H_
