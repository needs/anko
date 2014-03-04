#include <assert.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include "textures.h"
#include "stb_image.h"


#define ADD_TEXTURE(name, path) "data/"path,
static const char tex_path[][32] = {
#include "textures.def"
};
#undef ADD_TEXTURE

static GLuint textures[TEX_TOTAL];
static int load_texture(GLuint tex, const char *path);


int load_textures(void)
{
	int i;

	glGenTextures(TEX_TOTAL, textures);

	for (i = 0; i < TEX_TOTAL; i++)
		if (!load_texture(textures[i], tex_path[i]))
			goto err_tex;

	return 1;
err_tex:
	glDeleteTextures(i, textures);
	return 0;
}


void unload_textures(void)
{
	glDeleteTextures(TEX_TOTAL, textures);
}


GLuint get_texture(tex_t tex)
{
	assert(tex > TEX_NONE);
	assert(tex < TEX_TOTAL);
	return textures[tex];
}


static int load_texture(GLuint tex, const char *path)
{
	int width, height;
	unsigned char *data;

	assert(path != NULL);

	/* Make the texture active */
	glBindTexture(GL_TEXTURE_2D, tex);

	/* For now, texture are just sample image */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* Use MipMap, Note: The filtering method might not be apropriate */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	/* Note: force 4 channels because the shader only process 4 channels */
	data = stbi_load(path, &width, &height, NULL, 4);
	if (data == NULL) {
		perror(path);
		return 0;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
		     GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	glGenerateMipmap(GL_TEXTURE_2D);
	
	return 1;
}
