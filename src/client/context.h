#ifndef _CONTEXT_H_
#define _CONTEXT_H_
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>

#include <client/linmath.h>

extern int should_quit; /* if set to 1, game will stop asap */
extern float speed; /* in Millisecond per frame */
extern mat4x4 projection;

int init_context(void);
void close_context(void);


#endif /* _CONTEXT_H_ */
