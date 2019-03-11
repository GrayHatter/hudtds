#include "onscreenkeys.h"

#include "../ui.h"

#include <stdlib.h>


// I break from 1TBS because code folding... sorry
static struct ui_panel
    key_q = {
        .type = PANEL_BUTTON,
        .name = "key q"
    }, key_w = {
        .type = PANEL_BUTTON,
        .name = "key w"
    }, key_e = {
        .type = PANEL_BUTTON,
        .name = "key e"
    }, key_r = {
        .type = PANEL_BUTTON,
        .name = "key r"
    }, key_t = {
        .type = PANEL_BUTTON,
        .name = "key t"
    }, key_y = {
        .type = PANEL_BUTTON,
        .name = "key y"
    }, key_u = {
        .type = PANEL_BUTTON,
        .name = "key u"
    }, key_i = {
        .type = PANEL_BUTTON,
        .name = "key i"
    }, key_o = {
        .type = PANEL_BUTTON,
        .name = "key o"
    }, key_p = {
        .type = PANEL_BUTTON,
        .name = "key p"
    };

static struct ui_panel onscreenkey_row_0 = {
    .type = PANEL_FRAME,
    .name = "onscreenkeys row 0",

    .children = (struct ui_panel*[]) {
        &key_q,
        &key_w,
        &key_e,
        &key_r,
        &key_t,
        &key_y,
        &key_u,
        &key_i,
        &key_o,
        &key_p,
        NULL
    }
};


// Row 1
static struct ui_panel
    key_a = {
        .type = PANEL_BUTTON,
        .name = "key a"
    }, key_s = {
        .type = PANEL_BUTTON,
        .name = "key s"
    }, key_d = {
        .type = PANEL_BUTTON,
        .name = "key d"
    }, key_f = {
        .type = PANEL_BUTTON,
        .name = "key f"
    }, key_g = {
        .type = PANEL_BUTTON,
        .name = "key g"
    }, key_h = {
        .type = PANEL_BUTTON,
        .name = "key h"
    }, key_j = {
        .type = PANEL_BUTTON,
        .name = "key j"
    }, key_k = {
        .type = PANEL_BUTTON,
        .name = "key k"
    }, key_l = {
        .type = PANEL_BUTTON,
        .name = "key l"
    }, key_enter = {
        .type = PANEL_BUTTON,
        .name = "key enter"
    };


static struct ui_panel onscreenkey_row_1 = {
    .type = PANEL_FRAME,
    .name = "onscreenkeys row 1",

    .children = (struct ui_panel*[]) {
        &key_a,
        &key_s,
        &key_d,
        &key_f,
        &key_g,
        &key_h,
        &key_j,
        &key_k,
        &key_l,
        &key_enter,
       NULL
    }
};



// Row 2
static struct ui_panel
    key_lshft = {
        .type = PANEL_BUTTON,
        .name = "key lshft"
    },key_z = {
        .type = PANEL_BUTTON,
        .name = "key z"
    }, key_x = {
        .type = PANEL_BUTTON,
        .name = "key x"
    }, key_c = {
        .type = PANEL_BUTTON,
        .name = "key c"
    }, key_v = {
        .type = PANEL_BUTTON,
        .name = "key v"
    }, key_b = {
        .type = PANEL_BUTTON,
        .name = "key b"
    }, key_m = {
        .type = PANEL_BUTTON,
        .name = "key m"
    }, key_n = {
        .type = PANEL_BUTTON,
        .name = "key n"
    }, key_comma = {
        .type = PANEL_BUTTON,
        .name = "key comma"
    }, key_dot = {
        .type = PANEL_BUTTON,
        .name = "key dot"
    }, key_rshft = {
        .type = PANEL_BUTTON,
        .name = "key rshft"
    };


static struct ui_panel onscreenkey_row_2 = {
    .type = PANEL_FRAME,
    .name = "onscreenkeys row 2",

    .children = (struct ui_panel*[]) {
        &key_lshft,
        &key_z,
        &key_x,
        &key_c,
        &key_v,
        &key_b,
        &key_m,
        &key_n,
        &key_comma,
        &key_dot,
        &key_rshft,
        NULL
    }
};


// Row 3
static struct ui_panel
    key_meta0 = {
        .type = PANEL_BUTTON,
        .name = "key meta0"
    },key_meta1 = {
        .type = PANEL_BUTTON,
        .name = "key meta1"
    }, key_meta2  = {
        .type = PANEL_BUTTON,
        .name = "key meta2"
    }, key_space = {
        .type = PANEL_BUTTON,
        .name = "key space"
    }, key_meta3 = {
        .type = PANEL_BUTTON,
        .name = "key meta3"
    }, key_meta4 = {
        .type = PANEL_BUTTON,
        .name = "key meta4"
    }, key_meta5  = {
        .type = PANEL_BUTTON,
        .name = "key meta5"
    };

static struct ui_panel onscreenkey_row_3 = {
    .type = PANEL_FRAME,
    .name = "onscreenkeys row 3",

    .children = (struct ui_panel*[]) {
        &key_meta0,
        &key_meta1,
        &key_meta2,
        &key_space,
        &key_meta3,
        &key_meta4,
        &key_meta5,
        NULL
    }
};



struct ui_panel onscreenkey_frame = {
    .type = PANEL_FRAME,
    .name = "onscreenkeys frame",
    .enabled = false,

    .pos_x = 0,
    .pos_y = 0,
    .width = 0,
    .height = 0,

    .children = (struct ui_panel*[]) {
        &onscreenkey_row_0,
        &onscreenkey_row_1,
        &onscreenkey_row_2,
        &onscreenkey_row_3,
        NULL
    }

};
