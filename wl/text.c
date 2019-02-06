#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
// #include FT_CONFIG_MODULES_H

#include "../hud.h"
#include "../wayland.h"

#include "../log.h"

void draw_char(FT_Bitmap *bm, uint32_t x, uint32_t y)
{
    uint32_t *p = root_pool_data->memory + x + y * WIDTH;
    uint8_t *g = bm->buffer;
    uint32_t d;

    for (int i = bm->rows; i > 0; i--) {
        for (int j = bm->width; j > 0; j--) {
            d = 0xff000000 | (*g) << 16 | (*g) << 8 | *g;
            *p++ = ~d;
            g++;
        }
        p += WIDTH - bm->width;
    }
}

void init_text(void)
{
    FT_Library library;
    FT_Init_FreeType(&library);

    FT_Face face;
    FT_New_Face(library, "./SCP.otf", 0, &face);

    // FIXME magic numbers width, height
    FT_Set_Char_Size(face, 0, 10*64, 800, 480);


    int glyph_index = FT_Get_Char_Index(face, 'a' );
    FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
    FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

}

void tmp_draw_helper(int x, int y)
{
    FT_Library library;
    FT_Init_FreeType(&library);

    FT_Face face;
    FT_New_Face(library, "./SCP.otf", 0, &face);

    // FIXME magic numbers width, height
    FT_Set_Char_Size(face, 0, 4*64, 800, 480);


    char *name = "Hudtds!";
    int len = strlen(name);

    int glyph_index;
    for (int i = 0; i < len; i++) {
        glyph_index = FT_Get_Char_Index(face, name[i]);
        FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
        FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
        draw_char(&face->glyph->bitmap, x + face->glyph->bitmap_left, y - face->glyph->bitmap_top);
        x += face->glyph->advance.x >> 6;
    }

}
