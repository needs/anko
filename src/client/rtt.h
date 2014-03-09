#ifndef _RENDERER_H_
#define _RENDERER_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>

#include <client/linmath.h>

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

void rtt_init();

#endif /* _RENDERER_H_ */
