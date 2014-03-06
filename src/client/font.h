#ifndef _FONT_H_
#define _FONT_H_

#define FONT_WIDTH 512
#define FONT_HEIGHT FONT_WIDTH
#define GLYPH_DIM 48

int load_font();
void render_text(char * str, float x, float y, float scale);
void unload_font();
void get_text_dim(char * str, float *w, float *h, float scale);

#endif /* _FONT_H_ */
