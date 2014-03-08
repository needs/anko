#ifndef _FONT_H_
#define _FONT_H_

#include <wchar.h>

#define FONT_WIDTH 1024
#define FONT_HEIGHT FONT_WIDTH
#define GLYPH_DIM 64
#define VBO_MAX_SIZE 8192

int load_font();
void set_font_color(float r, float g, float b, float alpha);
void render_text(wchar_t * str, float x, float y, float size);
void unload_font();
void font_swap_buffers();
void get_text_dim(wchar_t * str, float *w, float *h, float size);


#endif /* _FONT_H_ */

