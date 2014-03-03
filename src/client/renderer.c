#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>

#include "renderer.h"
#include "shader.h"


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
	GLint posAttrib = glGetAttribLocation(program, "position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib);
}


void close_rendering(void)
{
	destroy_shader(program);
}


void render_rect(float x, float y, float width, float height)
{
	float vertices[8] = {
		x, y,
		x + width, y,
		x + width, y + height,
		x, y + height,
	};

	/* Let's copy them :) */
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	/* And render them */
	glDrawArrays(GL_TRIANGLES, 0, 3);
}
