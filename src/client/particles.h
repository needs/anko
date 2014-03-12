#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#include <client/textures.h>
#include <client/camera.h>


/* Maybe made the limit configurable/dynamique ? */
#define MAX_PARTICLES 16384


typedef struct partgen_t {
	float particles[2 * MAX_PARTICLES]; /* Lifetimes */
	GLuint vbo, vao;

	int count;		/* Number of particles */
	long offset;		/* Offset of the first particle */
} partgen_t;


struct partargs_t {
	float lifetime;		/* Lifetime of a particle */
	float spawn_period;	/* Period when particles can spawn */
	tex_t tex;		/* Texture, must be in TEX_PARTICLES */

	struct {
		float start, end;
	} opacity;		/* Opacity */

	struct {
		struct { float x, y; } start;
		struct { float x, y; } end;
	} box;			/* Size of the particle */

	struct {
		float x, y;
	} dir;			/* Direction of the particle */

	struct {
		float x, y;
	} spawn_box;		/* Lower right corner of the spawning box */
};


extern struct partargs_t PARTARGS_DEFAULT;

/* Create a new particles generator. */
partgen_t* init_particles(void);

/* Spawn particles given the paramters.
 * If the maximum of particles is reached, the particles won't be rendered. */
void spawn_particles(partgen_t *gen, int n, float x, float y, float z, struct partargs_t *prop);

/* Filters out the dead particles.
 * This function has to be called sometimes but not at every frame. */
void update_particles(partgen_t *gen);

/* Render particles */
void render_particles(partgen_t *gen, camera_t *camera);

/* Free the particles generator. */
void free_particles(partgen_t *gen);


#endif /* _PARTICLES_H_ */
