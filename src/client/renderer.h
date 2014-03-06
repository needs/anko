#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "linmath.h"
#include "textures.h"
#include "window.h"

int  init_rendering(void);
void close_rendering(void);
void render_model(mat4x4 model, GLint first, GLint count);
void render_on_top(mat4x4 model, GLint offset);

#endif /* _RENDERER_H_ */
