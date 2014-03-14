#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <client/particles.h>
#include <client/textures.h>
#include <client/shader.h>
#include <client/camera.h>
#include <client/context.h>


/* By default, a particle becomes transparent and decrese in size. */
struct partargs_t PARTARGS_DEFAULT = {
	.lifetime = 1.0,
	.tex = TEX_PARTICLES_FIRE1,
	.spawn_period = 1.0,
	.opacity = {
		.start = 1.0,
		.end = 0.0,
	},
	.box = {
		.start = { 1.0, 1.0 },
		.end   = { 0.0, 0.0 },
	},
	.dir = {
		0.0, -10.0,
		.rotate = 0,
		.dispersion = 0
	},
	.spawn_box = { 0.0, 0.0 },
};


#define PART_VERTEX_LEN  16
#define PART_VERTEX_SIZE PART_VERTEX_LEN * sizeof(float)
#define PART_NB_VERTEX   4
#define PART_SIZE        PART_NB_VERTEX * PART_VERTEX_SIZE
#define PART_LEN         PART_NB_VERTEX * PART_VERTEX_LEN


/* Write the vertices in the buffer buf (the VBO) */
static float add_particle(float *buf, struct partargs_t *prop, float x, float y, float z);

/* Rotate particle */
static void rotate_vec2(float *x, float *y, float angle, float cx, float cy);


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
	glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, PART_VERTEX_SIZE, 0);
	glEnableVertexAttribArray(position);

	GLint uv = glGetAttribLocation(sh_particles, "UV");
	glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE, PART_VERTEX_SIZE, (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(uv);

	GLint lifetime = glGetAttribLocation(sh_particles, "lifetime");
	glVertexAttribPointer(lifetime, 2, GL_FLOAT, GL_FALSE, PART_VERTEX_SIZE, (void*)(5*sizeof(float)));
	glEnableVertexAttribArray(lifetime);

	GLint alpha = glGetAttribLocation(sh_particles, "alpha");
	glVertexAttribPointer(alpha, 2, GL_FLOAT, GL_FALSE, PART_VERTEX_SIZE, (void*)(7*sizeof(float)));
	glEnableVertexAttribArray(alpha);

	GLint target = glGetAttribLocation(sh_particles, "target");
	glVertexAttribPointer(target, 2, GL_FLOAT, GL_FALSE, PART_VERTEX_SIZE, (void*)(9*sizeof(float)));
	glEnableVertexAttribArray(target);

	glBindVertexArray(0);

	return gen;
}


void spawn_particles(partgen_t *gen, int n, float x, float y, float z, struct partargs_t *prop)
{
	GLint old_bind;
	float *buf;
	int i;

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
		gen->particles[gen->offset + gen->count + i] = add_particle(buf, prop, x, y, z);
		buf += PART_LEN;
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


static float add_particle(float *buf,
			  struct partargs_t *prop,float x, float y, float z)
{
	float origin[PART_LEN], target[PART_LEN];
	float start_time;
	float ox, oy;
	float angle = 0;
	float dir_x, dir_y;
	int j;

	assert(buf  != NULL);
	assert(prop != NULL);

	/* ox, oy are the random offset of the spawning position */
	ox = (((float)random() / RAND_MAX) * prop->spawn_box.x);
	oy = (((float)random() / RAND_MAX) * prop->spawn_box.y);

	/* start_time is the timestamp from when the particle is showed */
	start_time = glfwGetTime() + (((float)random() / RAND_MAX) * prop->spawn_period);

	/* Rotate the direction given the dispersion */
	dir_x = prop->dir.x;
	dir_y = prop->dir.y;
	rotate_vec2(&dir_x, &dir_y,
		    ((float)random() / RAND_MAX) * prop->dir.dispersion, 0, 0);

	/* Get position of the origin vertices and destination vertices.
	 * (Interpolation is made in the shader) */
	get_sctexture(origin, prop->tex,
		      x + ox, y + oy, z,
		      prop->box.start.x, prop->box.start.y);
	get_sctexture(target, prop->tex,
		      x + ox + dir_x * prop->lifetime,
		      y + oy + dir_y * prop->lifetime,
		      z, prop->box.end.x, prop->box.end.y);

	if (prop->dir.rotate)
		angle = atan2f(dir_y, dir_x) + M_PI_2;

	for (j = 0; j < PART_NB_VERTEX; j++) {
		const unsigned i = j * PART_VERTEX_LEN;

		rotate_vec2(&origin[j * TEXTURE_VERTEX_LEN + 0],
			    &origin[j * TEXTURE_VERTEX_LEN + 1],
			    angle, x + ox, y + oy);
		rotate_vec2(&target[j * TEXTURE_VERTEX_LEN + 0],
			    &target[j * TEXTURE_VERTEX_LEN + 1],
			    angle,
			    x + ox + dir_x * prop->lifetime,
			    y + oy + dir_y * prop->lifetime);

		buf[i + 0]  = origin[j * TEXTURE_VERTEX_LEN + 0];
		buf[i + 1]  = origin[j * TEXTURE_VERTEX_LEN + 1];
		buf[i + 2]  = origin[j * TEXTURE_VERTEX_LEN + 2];
		buf[i + 3]  = origin[j * TEXTURE_VERTEX_LEN + 3];
		buf[i + 4]  = origin[j * TEXTURE_VERTEX_LEN + 4];
		buf[i + 5]  = start_time;
		buf[i + 6]  = prop->lifetime;
		buf[i + 7]  = prop->opacity.start;
		buf[i + 8]  = prop->opacity.end;
		buf[i + 9]  = target[j * TEXTURE_VERTEX_LEN + 0];
		buf[i + 10] = target[j * TEXTURE_VERTEX_LEN + 1];
	}

	return start_time + prop->lifetime;
}


static void rotate_vec2(float *x, float *y, float angle, float cx, float cy)
{
	float tmp;

	assert(x != NULL);
	assert(y != NULL);

	*x -= cx; *y -= cy; tmp = *x;

	*x = tmp * cos(angle) - *y * sin(angle);
	*y = tmp * sin(angle) + *y * cos(angle);

	*x += cx; *y += cy;
}
