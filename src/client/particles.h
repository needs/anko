#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h> 

#include "textures.h"


/* Maybe made the limit configurable ? */
#define MAX_PARTICLES 2048

typedef struct partgen_t partgen_t;

partgen_t* init_particles(void);
void spawn_particles(partgen_t *gen, int n, tex_t tex, float x, float y);
void render_particles(partgen_t *gen, mat4x4 *view, mat4x4 *projection);
void free_particles(partgen_t *gen);


#endif /* _PARTICLES_H_ */
