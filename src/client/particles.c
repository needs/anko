#include <assert.h> 
#include <stdio.h>
#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h> 
#include <GLFW/glfw3.h>

#include "particles.h"
#include "textures.h"
#include "shader.h"
#include "camera.h"
#include "renderer.h"


typedef struct particle_t {
	int lifetime;
} particle_t;

typedef struct partgen_t {
	particle_t particles[MAX_PARTICLES];
	GLuint vbo, vao;
	int count;
} partgen_t;


partgen_t* init_particles(void)
{
	partgen_t *gen;

	if ((gen = calloc(1, sizeof (*gen))) == NULL) {
		perror("malloc(partgen)");
		return NULL;
	}

	glGenVertexArrays(1, &gen->vao);
	glBindVertexArray(gen->vao);

	glGenBuffers(1, &gen->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, gen->vbo);
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float) * MAX_PARTICLES, NULL, GL_DYNAMIC_DRAW);

	GLint position = glGetAttribLocation(sh_particles, "position");
	glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
	glEnableVertexAttribArray(position);

	GLint uv = glGetAttribLocation(sh_particles, "UV");
	glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(uv);

	glBindVertexArray(0);

	return gen;
}


void spawn_particles(partgen_t *gen, int n, tex_t tex, float x, float y)
{
	int i;
	float *buf;

	assert(gen != NULL);
	assert(n > 0);

	if (gen->count + n >= MAX_PARTICLES) {
		fprintf(stderr, "particles: Maximum number of particles reached (%d).\n", MAX_PARTICLES);
		return;
	}

	/* TODO: The ranged mapping may be slower than glBufferSubData() */
	glBindBuffer(GL_ARRAY_BUFFER, gen->vbo);
	if ((buf = glMapBufferRange(GL_ARRAY_BUFFER, 16 * sizeof(float) * gen->count, 16 * sizeof(float) * n, GL_MAP_WRITE_BIT)) == NULL) {
		fprintf(stderr, "particles: Can't map the VBO.\n");
		return;
	}

	/* Write the VBO sequentialy, not in reverse order. */
	for (i = 0; i < n; i++)
		get_ctexture(buf + (i * 16), tex, x, y);

	glBindBuffer(GL_ARRAY_BUFFER, gen->vbo); /* Avoid stupid errors. */
	if (glUnmapBuffer(GL_ARRAY_BUFFER) == GL_FALSE)
		fprintf(stderr, "particles: Error when unmapping the VBO.\n");
	gen->count += n;
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
	glDrawArrays(GL_QUADS, 0, gen->count * 4);
}


void free_particles(partgen_t *gen)
{
	assert(gen != NULL);
	glDeleteBuffers(1, &gen->vbo);
	glDeleteVertexArrays(1, &gen->vao);
	free(gen);
}
