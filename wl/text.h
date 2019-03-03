#ifndef _HUDTDS_WL_TEXT_
#define _HUDTDS_WL_TEXT_

#include <stdint.h>

extern struct ui_panel nav_frame;

void init_text(void);

void text_draw_string(const char *string, uint32_t x, uint32_t y);
void text_draw_string_width(const char *string, uint32_t x, uint32_t y, uint32_t w);


#endif // _HUDTDS_WL_TEXT_
