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


struct partargs_t PARTARGS_DEFAULT = {
	.lifetime = 1.0,
	.alpha = {
		.begin = 1.0,
		.middle = 1.0,
		.end = 0.0,
		.fadein = 0.0,
		.fadeout = 0.0,
	},
	.tex = TEX_PARTICLES_FIRE1,
};


#define PARTICLE_SIZE 4 * 5 * sizeof(float)


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
	glBufferData(GL_ARRAY_BUFFER, 2 * PARTICLE_SIZE * MAX_PARTICLES, NULL, GL_DYNAMIC_DRAW);

	GLint position = glGetAttribLocation(sh_particles, "position");
	glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
	glEnableVertexAttribArray(position);

	GLint uv = glGetAttribLocation(sh_particles, "UV");
	glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(uv);

	GLint lifetime = glGetAttribLocation(sh_particles, "lifetime");
	glVertexAttribPointer(lifetime, 1, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(4*sizeof(float)));
	glEnableVertexAttribArray(lifetime);

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
	if ((buf = glMapBufferRange(GL_ARRAY_BUFFER, PARTICLE_SIZE * (gen->count + gen->offset), PARTICLE_SIZE * n, GL_MAP_WRITE_BIT)) == NULL) {
		fprintf(stderr, "particles: Can't map the VBO.\n");
		return;
	}

	for (i = 0; i < n; i++) {
		float data[20];
		get_ctexture(data, prop->tex, x, y);

		for (j = 0; j < 4; j++) {
			buf[(i * 20) + j * 5]     = data[j * 4];
			buf[(i * 20) + j * 5 + 1] = data[j * 4 + 1];
			buf[(i * 20) + j * 5 + 2] = data[j * 4 + 2];
			buf[(i * 20) + j * 5 + 3] = data[j * 4 + 3];
			buf[(i * 20) + j * 5 + 4] = curtime + prop->lifetime;
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

		memcpy(buf, buf + (20 * gen->offset), gen->count * PARTICLE_SIZE);

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
	glDrawArrays(GL_QUADS, gen->offset * 4, gen->count * 4);
}


void free_particles(partgen_t *gen)
{
	assert(gen != NULL);
	glDeleteBuffers(1, &gen->vbo);
	glDeleteVertexArrays(1, &gen->vao);
	free(gen);
}
