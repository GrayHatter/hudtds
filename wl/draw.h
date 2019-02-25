#ifndef _HUD_DRAW_H_
#define _HUD_DRAW_H_

#include <stdint.h>
#include <stdbool.h>


void draw_swap_buffer(uint32_t *buffer);

bool draw_pixel(int32_t x, int32_t y, uint32_t c);
void draw_dot_c(int32_t x, int32_t y, uint32_t c);

void draw_vline_c(int32_t x, int32_t y, int32_t h, uint32_t c);
void draw_hline_c(int32_t x, int32_t y, int32_t w, uint32_t c);

void draw_square_c(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t c);
void draw_box_c(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t c);

void draw_square(int32_t x, int32_t y, int32_t w, int32_t h);
void draw_dot(int32_t x, int32_t y);

void draw_circle_c(int32_t x, int32_t y, int32_t w, uint32_t c);
void draw_circle(int32_t x, int32_t y, int32_t w);

void draw_circle_radius_c(int32_t x, int32_t y, int32_t r, uint32_t c);
void draw_circle_radius(int32_t x, int32_t y, int32_t r);


#endif // _HUD_DRAW_H_
