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
static const char tex_path[][32] = {
#include "textures.def"
};
#undef ADD_TEXTURE


struct texture_t {
	GLuint tex;
	int width, height;
};
static struct texture_t textures[TEX_TOTAL];


static void init_textures();
static int load_texture(tex_t tex, const char *path);


int load_textures(void)
{
	int i;

	init_textures();

	for (i = 0; i < TEX_TOTAL; i++)
		if (!load_texture(i, tex_path[i]))
			goto err_tex;
	return 1;

err_tex:
	while (i --> 0)
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
	render_rect(model, textures[tex].tex, tex * 4);
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

	float vertices[16] = {
		-textures[tex].width/2, -textures[tex].height/2, 0.0, 0.0,
		 textures[tex].width/2, -textures[tex].height/2, 1.0, 0.0,
		 textures[tex].width/2,  textures[tex].height/2, 1.0, 1.0,
		-textures[tex].width/2,  textures[tex].height/2, 0.0, 1.0,
	};

	/* And send them to the graphic card */
	glBufferSubData(GL_ARRAY_BUFFER, tex * tex_size, tex_size, vertices);

	return 1;
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
