#ifndef _HUDTDS_WL_TEXT_
#define _HUDTDS_WL_TEXT_

#include <stdint.h>

extern struct ui_panel nav_frame;

void init_text(void);


void text_string_c(const char *string, uint32_t x, uint32_t y, uint32_t c);
void text_string_width_c(const char *string, uint32_t x, uint32_t y, uint32_t w, uint32_t c);


void text_string(const char *string, uint32_t x, uint32_t y);
void text_string_width(const char *string, uint32_t x, uint32_t y, uint32_t w);

void text_string_ralign(const char *string, uint32_t right, uint32_t y);
void text_string_ralign_width(const char *string, uint32_t right, uint32_t y, uint32_t max_left);

#endif // _HUDTDS_WL_TEXT_
