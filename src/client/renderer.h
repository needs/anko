#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "linmath.h"
#include "textures.h"
#include "window.h"

int  init_rendering(void);
void close_rendering(void);
void render_rect(mat4x4 model, GLuint tex, GLint offset);

#endif /* _RENDERER_H_ */
