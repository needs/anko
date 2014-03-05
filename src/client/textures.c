#include <assert.h>
#include <GL/glew.h>
#include <GL/gl.h>

#include "linmath.h"
#include "textures.h"
#include "renderer.h"
#include "stb_image.h"
#include "shader.h"


/* This VBO contains the vertex of every textures. */
static GLuint vbo_tex;
static const size_t tex_size = 16 * sizeof(float);

/* VAO for linking shader to vbo_tex */
static GLuint vao_tex;


#define ADD_TEXTURE(name, path) "data/"path,
#define ADD_IN_TEXTURE(...)
static const char tex_path[][32] = {
#include "textures.def"
};
#undef ADD_IN_TEXTURE
#undef ADD_TEXTURE


typedef struct texture_t {
	GLuint tex;
	size_t offset;
	int width, height;
} texture_t;
static texture_t textures[TEX_TOTAL];


static void init_textures();
static int  load_from_file(texture_t *tex, const char *path);
static void ref_texture(texture_t *tex, texture_t *ref, float x, float y, float w, float h);


int load_textures(void)
{
	int i;

	init_textures();

	/* Init to INVALID for error handling (see err_tex) */
	for (i = 0; i < TEX_TOTAL; i++)
		textures[i].tex = GL_INVALID_VALUE;

	/* Try to load all textures, it's a bit complicated since we must
	 * mix normal textures with packed one. */
#define ADD_TEXTURE(name, path)						\
	if (!load_from_file(&textures[TEX_##name], path))		\
		goto err_tex;						\
	ref_texture(&textures[TEX_##name], &textures[TEX_##name], 0, 0, textures[TEX_##name].width, textures[TEX_##name].height);
#define ADD_IN_TEXTURE(name, from, x, y, w, h)				\
	ref_texture(&textures[TEX_##name], &textures[TEX_##from], x, y, w, h);
#include "textures.def"
#undef ADD_IN_TEXTURE
#undef ADD_TEXTURE

	return 1;

err_tex:
	for (i = 0; i < TEX_TOTAL; i++)
		if (textures[i].tex != GL_INVALID_VALUE)
			glDeleteTextures(1, &textures[i].tex);
	glDeleteBuffers(1, &vbo_tex);
	glDeleteVertexArrays(1, &vao_tex);
	return 0;
}


void unload_textures(void)
{
	int i;
	for (i = 0; i < TEX_TOTAL; i++)
		glDeleteTextures(1, &textures[i].tex);
	glDeleteBuffers(1, &vbo_tex);
	glDeleteVertexArrays(1, &vao_tex);
}


void render_texture(mat4x4 model, tex_t tex)
{
	assert(tex > TEX_NONE);
	assert(tex < TEX_TOTAL);

	glBindVertexArray(vao_tex);
	render_model(model, textures[tex].tex, textures[tex].offset);
}


static void init_textures(void)
{
	glGenVertexArrays(1, &vao_tex);
	glBindVertexArray(vao_tex);

	glGenBuffers(1, &vbo_tex);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_tex);
	glBufferData(GL_ARRAY_BUFFER, tex_size * TEX_TOTAL, NULL, GL_STATIC_DRAW);

	/* Format of the vertices for the shader */

	GLint position = glGetAttribLocation(program, "position");
	glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
	glEnableVertexAttribArray(position);

	GLint tex_coord = glGetAttribLocation(program, "tex_coord");
	glVertexAttribPointer(tex_coord, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(tex_coord);
}


static int load_from_file(texture_t *tex, const char *path)
{
	unsigned char *data;

	assert(tex != NULL);
	assert(path != NULL);

	/* Create and make the texture active */
	glGenTextures(1, &tex->tex);
	glBindTexture(GL_TEXTURE_2D, tex->tex);

	/* For now, texture are just sample image */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* Use MipMap, Note: The filtering method might not be apropriate */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	/* Note: force 4 channels because the shader only process 4 channels */
	data = stbi_load(path, &tex->width, &tex->height, NULL, 4);
	if (data == NULL) {
		perror(path);
		return 0;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->width, tex->height,
		     0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	glGenerateMipmap(GL_TEXTURE_2D);
	return 1;
}


static void ref_texture(texture_t *tex, texture_t *ref, float x, float y, float w, float h)
{
	assert(tex != NULL);
	assert(ref != NULL);

	tex->tex = ref->tex;
	tex->offset = 4 * (tex - textures);
	printf("offset = %ld\n", tex - textures);

	float vertices[16] = {
		0, 0, x / ref->width,       y / ref->height,
		w, 0, (x + w) / ref->width, y / ref->height,
		w, h, (x + w) / ref->width, (y + h) / ref->height,
		0, h, x / ref->width,       (y + h) / ref->height,
	};

	/* And send them to the graphic card */
	glBufferSubData(GL_ARRAY_BUFFER, (tex - textures) * tex_size, tex_size, vertices);
}
