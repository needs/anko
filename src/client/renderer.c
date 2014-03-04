#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>

#include "renderer.h"
#include "shader.h"
#include "textures.h"


static GLuint program;
static GLuint vbo;
static GLuint vao;


void init_rendering(void)
{
	glewExperimental = GL_TRUE;
	glewInit();

	/* Get a Vertex Buffer Object, */
	/* and make it active, so we can copy data on it */
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	/* Load shaders and make them active */
	program = load_shaders("data/shaders/default.vs", "data/shaders/default.fs");
	glBindFragDataLocation(program, 0, "outColor");
	glLinkProgram(program);
	glUseProgram(program);

	/* Vertex Array Object */
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	/* Format of the vertices */
	GLint position = glGetAttribLocation(program, "position");
	glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
	glEnableVertexAttribArray(position);

	GLint tex_coord = glGetAttribLocation(program, "tex_coord");
	glVertexAttribPointer(tex_coord, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(tex_coord);
}


void close_rendering(void)
{
	destroy_shaders(program);
}


void render_rect(float x, float y, float width, float height, tex_t tex)
{
	float vertices[16] = {
		x, y,                  0.0, 0.0,
		x + width, y,          1.0, 0.0,
		x + width, y + height, 1.0, 1.0,
		x, y + height,         0.0, 1.0,
	};

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, get_texture(tex));
	glUniform1i(glGetUniformLocation(program, "tex"), 0);

	/* Let's copy them :) */
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	/* And render them */
	glDrawArrays(GL_QUADS, 0, 4);
}
