#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
// #include FT_CONFIG_MODULES_H

#include "../hud.h"
#include "../wl/draw.h"

#include "../log.h"

static FT_Library library = NULL;
static FT_Face face = NULL;

#define CURRENT_FACE_HEIGHT face->height / 64


void draw_char_c(FT_Bitmap *bm, uint32_t x, uint32_t y, uint32_t c)
{
    uint8_t *g = bm->buffer;
    uint32_t d;
    for (uint32_t i = 0; i < bm->rows; i++) {
        for (uint32_t j = 0; j < bm->width; j++) {
            if (*g) {
                d = (0xff000000 | c) & ((*g) << 16 | (*g) << 8 | *g);
                draw_pixel(x + j, y + i, d);
            }
            g++;
        }
    }
}


void init_text(void)
{
    FT_Init_FreeType(&library);
    FT_New_Face(library, "/jci/scripts/SCP.otf", 0, &face);

    // FIXME magic numbers width, height
    FT_Set_Char_Size(face, 0, 3*64, 800, 480);

    // int glyph_index = FT_Get_Char_Index(face, 'a' );
    // FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
    // FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
}


void text_string_c(const char *string, uint32_t x, uint32_t y, uint32_t c)
{
    if (!string) {
        return;
    }

    // FIXME h/w checks
    y += CURRENT_FACE_HEIGHT;
    int glyph_index;
    for (unsigned int i = 0; i < strlen(string); i++) {
        glyph_index = FT_Get_Char_Index(face, string[i]);
        FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
        FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
        if (x + face->glyph->bitmap_left + face->glyph->bitmap.width < WIDTH) {
            draw_char_c(&face->glyph->bitmap, x + face->glyph->bitmap_left, y - face->glyph->bitmap_top, c);
        }
        x += face->glyph->advance.x >> 6;
    }
}


void text_string_width_c(const char *string, uint32_t x, uint32_t y, uint32_t w, uint32_t c)
{
    if (!string) {
        return;
    }

    y += CURRENT_FACE_HEIGHT;
    int glyph_index;
    for (unsigned int i = 0; i < strlen(string); i++) {
        glyph_index = FT_Get_Char_Index(face, string[i]);
        FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
        FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

        if (x + face->glyph->bitmap_left + face->glyph->bitmap.width < w) {
            draw_char_c(&face->glyph->bitmap, x + face->glyph->bitmap_left, y - face->glyph->bitmap_top, c);
        }

        x += face->glyph->advance.x >> 6;
    }
}


void text_string(const char *string, uint32_t x, uint32_t y)
{
    text_string_c(string, x, y, 0xffffffff);
}


void text_string_width(const char *string, uint32_t x, uint32_t y, uint32_t w)
{
    text_string_width_c(string, x, y, w, 0xffffffff);
}


void text_string_ralign(const char *string, uint32_t right, uint32_t y)
{
    // FIXME doesn't actually ralign :P
    text_string_c(string, right, y, 0xffffffff);
}

void text_string_ralign_width(const char *string, uint32_t right, uint32_t y, uint32_t max_left)
{
    // FIXME doesn't actually ralign :P
    text_string_width_c(string, right, y, max_left, 0xffffffff);
}
