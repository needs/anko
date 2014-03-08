#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "particles.h"
#include "textures.h"
#include "shader.h"
#include "camera.h"
#include "renderer.h"


/* By default, a particle becomes transparent and decrese in size. */
struct partargs_t PARTARGS_DEFAULT = {
	.lifetime = 1.0,
	.opacity = {
		.start = 1.0,
		.end = 0.0,
	},
	.box = {
		.start = { 1.0, 1.0 },
		.end   = { 0.0, 0.0 },
	},
	.tex = TEX_PARTICLES_FIRE1,
};


#define PART_VERTEX_LEN  10
#define PART_VERTEX_SIZE PART_VERTEX_LEN * sizeof(float)
#define PART_NB_VERTEX   4
#define PART_SIZE        PART_NB_VERTEX * PART_VERTEX_SIZE
#define PART_LEN         PART_NB_VERTEX * PART_VERTEX_LEN


typedef struct partgen_t {
	float particles[2 * MAX_PARTICLES]; /* Lifetimes */
	GLuint vbo, vao;

	int count;		/* Number of particles */
	long offset;		/* Offset of the first particle */
} partgen_t;


partgen_t* init_particles(void)
{
	partgen_t *gen;

	if ((gen = calloc(1, sizeof (*gen))) == NULL) {
		perror("malloc(partgen)");
		return NULL;
	}

	gen->count = 0;
	gen->offset = 0;

	glGenVertexArrays(1, &gen->vao);
	glBindVertexArray(gen->vao);

	glGenBuffers(1, &gen->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, gen->vbo);
	glBufferData(GL_ARRAY_BUFFER, 2 * PART_SIZE * MAX_PARTICLES, NULL, GL_DYNAMIC_DRAW);

	GLint position = glGetAttribLocation(sh_particles, "position");
	glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, PART_VERTEX_SIZE, 0);
	glEnableVertexAttribArray(position);

	GLint uv = glGetAttribLocation(sh_particles, "UV");
	glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE, PART_VERTEX_SIZE, (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(uv);

	GLint lifetime = glGetAttribLocation(sh_particles, "lifetime");
	glVertexAttribPointer(lifetime, 2, GL_FLOAT, GL_FALSE, PART_VERTEX_SIZE, (void*)(4*sizeof(float)));
	glEnableVertexAttribArray(lifetime);

	GLint alpha = glGetAttribLocation(sh_particles, "alpha");
	glVertexAttribPointer(alpha, 2, GL_FLOAT, GL_FALSE, PART_VERTEX_SIZE, (void*)(6*sizeof(float)));
	glEnableVertexAttribArray(alpha);

	GLint target = glGetAttribLocation(sh_particles, "target");
	glVertexAttribPointer(target, 2, GL_FLOAT, GL_FALSE, PART_VERTEX_SIZE, (void*)(8*sizeof(float)));
	glEnableVertexAttribArray(target);

	glBindVertexArray(0);

	return gen;
}


void spawn_particles(partgen_t *gen, int n, float x, float y, struct partargs_t *prop)
{
	int i, j;
	GLint old_bind;
	float *buf;
	float curtime = glfwGetTime();

	assert(gen != NULL);
	assert(n > 0);
	assert(prop != NULL);
	assert(prop->lifetime > 0);
	assert(prop->tex != TEX_NONE);

	if (gen->count + n > MAX_PARTICLES)
		return;

	/* TODO: The ranged mapping may be slower than glBufferSubData() */
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &old_bind);
	glBindBuffer(GL_ARRAY_BUFFER, gen->vbo);
	if ((buf = glMapBufferRange(GL_ARRAY_BUFFER, PART_SIZE * (gen->count + gen->offset), PART_SIZE * n, GL_MAP_WRITE_BIT)) == NULL) {
		fprintf(stderr, "particles: Can't map the VBO.\n");
		return;
	}

	for (i = 0; i < n; i++) {
		float data[PART_LEN], datat[PART_LEN];
		get_sctexture(data, prop->tex, x, y, prop->box.start.x, prop->box.start.y);
		get_sctexture(datat, prop->tex, x, y, prop->box.end.x, prop->box.end.y);

		for (j = 0; j < PART_NB_VERTEX; j++) {
			const unsigned partindex = (i * PART_LEN) + j * PART_VERTEX_LEN;
			buf[partindex + 0] = data[j * 4 + 0];
			buf[partindex + 1] = data[j * 4 + 1];
			buf[partindex + 2] = data[j * 4 + 2];
			buf[partindex + 3] = data[j * 4 + 3];
			buf[partindex + 4] = curtime;
			buf[partindex + 5] = prop->lifetime;
			buf[partindex + 6] = prop->opacity.start;
			buf[partindex + 7] = prop->opacity.end;
			buf[partindex + 8] = datat[j * 4 + 0];
			buf[partindex + 9] = datat[j * 4 + 1];
		}

		gen->particles[gen->offset + gen->count + i] = curtime + prop->lifetime;
	}

	if (glUnmapBuffer(GL_ARRAY_BUFFER) == GL_FALSE)
		fprintf(stderr, "particles: Error when unmapping the VBO.\n");
	glBindBuffer(GL_ARRAY_BUFFER, old_bind); /* Avoid stupid errors. */

	gen->count += n;
}


void update_particles(partgen_t *gen)
{
	float *buf;
	float curtime = glfwGetTime();

	assert(gen != NULL);

	/* Move offset forward until we reach a living particle. */
	while (gen->count > 0 && gen->particles[gen->offset] < curtime) {
		gen->offset++;
		gen->count--;
	}

	/* When the offset reach the begining of the second half of the VBO,
	 * rewind it. */
	if (gen->offset < MAX_PARTICLES)
		return;
	else if (gen->count > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, gen->vbo);
		if ((buf = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE)) == NULL) {
			fprintf(stderr, "update-particles: Can't map the VBO.\n");
			return;
		}

		memcpy(buf, buf + (PART_LEN * gen->offset), gen->count * PART_SIZE);

		if (glUnmapBuffer(GL_ARRAY_BUFFER) == GL_FALSE)
			fprintf(stderr, "update-particles: Error when unmapping the VBO.\n");
	}

	memcpy(gen->particles, gen->particles + gen->offset, gen->count * sizeof(float));
	gen->offset = 0;
}


void render_particles(partgen_t *gen, camera_t *camera)
{
	assert(gen != NULL);
	assert(camera != NULL);

	if (gen->count == 0)
		return;

	glUseProgram(sh_particles);

	glBindVertexArray(gen->vao);
	glBindBuffer(GL_ARRAY_BUFFER, gen->vbo);

	glBindTexture(GL_TEXTURE_2D, get_texid(TEX_PARTICLES));
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(sh_particles, "tex"), 0);

	glUniform1f(glGetUniformLocation(sh_particles, "time"), glfwGetTime());
	glUniformMatrix4fv(glGetUniformLocation(sh_particles, "projection"),
			   1, GL_FALSE, (GLfloat*)projection);
	glUniformMatrix4fv(glGetUniformLocation(sh_particles, "view"),
			   1, GL_FALSE, (GLfloat*)camera->matrix);

	/* Draw the whole VBO */
	glDrawArrays(GL_QUADS, gen->offset * PART_NB_VERTEX, gen->count * PART_NB_VERTEX);
}


void free_particles(partgen_t *gen)
{
	assert(gen != NULL);
	glDeleteBuffers(1, &gen->vbo);
	glDeleteVertexArrays(1, &gen->vao);
	free(gen);
}
