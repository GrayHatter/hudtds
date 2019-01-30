#ifndef _HUD_DRAW_H_
#define _HUD_DRAW_H_

#include <stdint.h>


void draw_dot_c(int32_t x, int32_t y, uint32_t color);

void draw_dot(int32_t x, int32_t y);


void draw_vline_c(int32_t x, int32_t y, int32_t h, uint32_t c);

void draw_line_c(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t c);

void draw_line(int32_t x, int32_t y, int32_t w, int32_t h);

void draw_box_c(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t c);


#endif // _HUD_DRAW_H_
