#include <assert.h>
#include <string.h>
#include <GL/glew.h>
#include <GL/gl.h>

#include "textures.h"
#include "linmath.h"
#include "stb_image.h"


typedef struct texture_t {
	GLuint tex;		/* OpenGL texture */
	float data[16];		/* Vertices and coordinates */
	int   width, height;	/* Size of the texture */
	float ox, oy;		/* Position of the center */
} texture_t;
static texture_t textures[TEX_TOTAL] = {{.tex = 0, .data = {0}, .width = 0, .height = 0, .ox = 0, .oy = 0}};


static int  load_from_file(texture_t *tex, const char *path);
static void ref_texture(texture_t *tex, texture_t *ref, float x, float y, float w, float h, float ox, float oy);


int load_textures(void)
{
	int i;

	/* Init to INVALID for error handling (see err_tex) */
	for (i = 0; i < TEX_TOTAL; i++)
		textures[i].tex = GL_INVALID_VALUE;

	/* Try to load all textures, it's a bit complicated since we must
	 * mix normal textures with packed one. */
#define ADD_TEXTURE(name, path)						\
	if (!load_from_file(&textures[TEX_##name], path))		\
		goto err_tex;						\
	ref_texture(&textures[TEX_##name], &textures[TEX_##name], 0, 0, textures[TEX_##name].width, textures[TEX_##name].height, 0, 0);
#define ADD_IN_TEXTURE(name, from, x, y, w, h, ox, oy)			\
	ref_texture(&textures[TEX_##from##_##name], &textures[TEX_##from], x, y, w, h, ox, oy);
#include "textures.def"
#undef ADD_IN_TEXTURE
#undef ADD_TEXTURE

	return 1;

err_tex:
	for (i = 0; i < TEX_TOTAL; i++)
		if (textures[i].tex != GL_INVALID_VALUE)
			glDeleteTextures(1, &textures[i].tex);
	return 0;
}


void unload_textures(void)
{
	int i;
	for (i = 0; i < TEX_TOTAL; i++)
		glDeleteTextures(1, &textures[i].tex);
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

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


static void ref_texture(texture_t *tex, texture_t *ref, float x, float y, float w, float h, float ox, float oy)
{
	assert(tex != NULL);
	assert(ref != NULL);

	float tmp[16] = {
		0, 0, x / ref->width,       y / ref->height,
		w, 0, (x + w) / ref->width, y / ref->height,
		w, h, (x + w) / ref->width, (y + h) / ref->height,
		0, h, x / ref->width,       (y + h) / ref->height
	};

	tex->tex = ref->tex;
	tex->ox = ox;
	tex->oy = oy;
	memcpy(tex->data, tmp, sizeof(tmp));
}


void get_texture(float *data, tex_t tex, float x, float y)
{
	assert(data != NULL);
	assert(tex >= TEX_NONE);
	assert(tex <  TEX_TOTAL);

	float *ref = textures[tex].data;
	float tmp[16] = {
		ref[0]  + x, ref[1]  + y, ref[2],  ref[3],
		ref[4]  + x, ref[5]  + y, ref[6],  ref[7],
		ref[8]  + x, ref[9]  + y, ref[10], ref[11],
		ref[12] + x, ref[13] + y, ref[14], ref[15],
	};
	memcpy(data, tmp, sizeof(tmp));
}


void get_ctexture(float *data, tex_t tex, float x, float y)
{
	get_texture(data, tex, x - textures[tex].ox, y - textures[tex].oy);
}


GLuint get_texid(tex_t tex)
{
	return textures[tex].tex;
}
