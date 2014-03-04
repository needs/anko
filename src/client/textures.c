#include <assert.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include "textures.h"
#include "renderer.h"
#include "stb_image.h"


#define ADD_TEXTURE(name, path) "data/"path,
static const char tex_path[][32] = {
#include "textures.def"
};
#undef ADD_TEXTURE

struct texture_t {
	GLuint tex, vbo;
	int width, height;
};

static struct texture_t textures[TEX_TOTAL];
static int load_texture(tex_t tex, const char *path);


int load_textures(void)
{
	int i;

	for (i = 0; i < TEX_TOTAL; i++)
		if (!load_texture(i, tex_path[i]))
			goto err_tex;
	return 1;

err_tex:
	while (i --> 0) {
		glDeleteTextures(1, &textures[i].tex);
		glDeleteBuffers(1, &textures[i].vbo);
	}
	return 0;
}


void unload_textures(void)
{
	int i;
	for (i = 0; i < TEX_TOTAL; i++) {
		glDeleteTextures(1, &textures[i].tex);
		glDeleteBuffers(1, &textures[i].vbo);
	}
}


void render_texture(float x, float y, tex_t tex)
{
	assert(tex > TEX_NONE);
	assert(tex < TEX_TOTAL);

	(void)x; (void)y;

	/* Make the vbo of the texture activ */
	glBindBuffer(GL_ARRAY_BUFFER, textures[tex].vbo);
	render_rect(textures[tex].tex);
}


static int load_texture(tex_t tex, const char *path)
{
	unsigned char *data;

	assert(tex > TEX_NONE);
	assert(tex < TEX_TOTAL);
	assert(path != NULL);

	/* Create and make the texture active */
	glGenTextures(1, &textures[tex].tex);
	glBindTexture(GL_TEXTURE_2D, textures[tex].tex);

	/* For now, texture are just sample image */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* Use MipMap, Note: The filtering method might not be apropriate */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	/* Note: force 4 channels because the shader only process 4 channels */
	data = stbi_load(path, &textures[tex].width, &textures[tex].height, NULL, 4);
	if (data == NULL) {
		perror(path);
		return 0;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textures[tex].width, textures[tex].height,
		     0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	glGenerateMipmap(GL_TEXTURE_2D);

	/* Put vertices in the vbo */
	glGenBuffers(1, &textures[tex].vbo);
	glBindBuffer(GL_ARRAY_BUFFER, textures[tex].vbo);
	float vertices[16] = {
		0, 0, 0.0, 0.0,
		textures[tex].width, 0, 1.0, 0.0,
		0 + textures[tex].width, 0 + textures[tex].height, 1.0, 1.0,
		0, 0 + textures[tex].height, 0.0, 1.0,
	};

	/* And send them to the graphic card */
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	return 1;
}
