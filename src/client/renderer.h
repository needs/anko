#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "linmath.h"
#include "textures.h"
#include "window.h"

int  init_rendering(void);
void close_rendering(void);
void render_model(mat4x4 model, GLint first, GLint count);

#endif /* _RENDERER_H_ */
