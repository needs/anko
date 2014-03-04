#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>

#include "renderer.h"
#include "shader.h"
#include "textures.h"
#include "linmath.h"
#include "camera.h"

static GLuint program;
static GLuint vao;
static mat4x4 projection;

void init_rendering(void)
{
	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	mat4x4_ortho(projection, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);
	set_camera(0, 0, 1);
}


void close_rendering(void)
{
	glDeleteVertexArrays(1, &vao);
	destroy_shaders(program);
}


void render_rect(GLuint tex)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(glGetUniformLocation(program, "tex"), 0);
	
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, (GLfloat*)projection);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, (GLfloat*)camera);
	
	/* And render them */
	glDrawArrays(GL_QUADS, 0, 4);
}
