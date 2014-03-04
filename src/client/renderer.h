#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "textures.h"
#include "window.h"

int  init_rendering(void);
void close_rendering(void);
void render_rect(GLuint tex, GLint offset);

#endif /* _RENDERER_H_ */
