#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "linmath.h"
#include "textures.h"

int  init_rendering(void);
void close_rendering(void);
void render_model(mat4x4 model, GLint first, GLint count);
void render_on_top(mat4x4 model, GLint offset);

extern int rtt_effect;

/*
  Start rendering to texture (RTT)
  All renders after this call will
  be rendered to a texture until a rtt_stop() call
 */
void rtt_start();

/*
  Stop rendering to texture (RTT)
 */
void rtt_stop();

/*
  Draw the framebuffer to screen
 */
void rtt_draw();

#endif /* _RENDERER_H_ */
