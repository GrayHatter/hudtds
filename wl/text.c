#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
// #include FT_CONFIG_MODULES_H

#include "../hud.h"
#include "../wayland.h"


void draw_char(FT_Bitmap *bm, uint32_t x, uint32_t y)
{
    uint32_t *p = root_pool_data->memory + x + y * WIDTH;
    uint8_t *g = bm->buffer;

    for (int i = bm->rows; i > 0; i--) {
        for (int j = bm->width; j > 0; j--) {
            *p++ = *g++;
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
    FT_Set_Char_Size(face, 0, 16*64, 800, 480);


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
    FT_Set_Char_Size(face, 0, 1*64, 800, 480);


    int glyph_index = FT_Get_Char_Index(face, 'a' );
    FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
    FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);


    draw_char(&face->glyph->bitmap, x, y);
}
