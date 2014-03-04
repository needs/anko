#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "textures.h"
#include "window.h"

void init_rendering(void);
void close_rendering(void);
void render_rect(float x, float y, float width, float height, GLuint tex);

#endif /* _RENDERER_H_ */
