#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#include "textures.h"
#include "camera.h"


/* Maybe made the limit configurable/dynamique ? */
#define MAX_PARTICLES 16384

typedef struct partgen_t partgen_t;

struct partargs_t {
	float lifetime;
	tex_t tex;

	struct {
		float start, end;
	} opacity;

	struct {
		struct { float x, y; } start;
		struct { float x, y; } end;
	} box;
};

extern struct partargs_t PARTARGS_DEFAULT;

/* Create a new particles generator. */
partgen_t* init_particles(void);

/* Spawn particles given the paramters.
 * If the maximum of particles is reached, the particles won't be rendered. */
void spawn_particles(partgen_t *gen, int n, float x, float y, struct partargs_t *prop);

/* Filters out the dead particles.
 * This function has to be called sometimes but not at every frame. */
void update_particles(partgen_t *gen);

/* Render particles */
void render_particles(partgen_t *gen, camera_t *camera);

/* Free the particles generator. */
void free_particles(partgen_t *gen);


#endif /* _PARTICLES_H_ */
