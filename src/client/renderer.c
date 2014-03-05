#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>

#include "renderer.h"
#include "shader.h"
#include "textures.h"
#include "linmath.h"
#include "camera.h"

static mat4x4 projection;

int init_rendering(void)
{
	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Load shaders and make them active */
	if (!load_shaders("data/shaders/default.vs", "data/shaders/default.fs"))
		goto err_shaders;
	glBindFragDataLocation(program, 0, "outColor");
	glLinkProgram(program);
	glUseProgram(program);

	mat4x4_ortho(projection, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);
	set_camera(0, 0, 1);
	return 1;

err_shaders:
	return 0;
}


void close_rendering(void)
{
	destroy_shaders(program);
}


void render_model(mat4x4 model, GLint first, GLint count)
{
	glActiveTexture(GL_TEXTURE0);

	glUniform1i(glGetUniformLocation(program, "tex"), 0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"),
			   1, GL_FALSE, (GLfloat*)model);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"),
			   1, GL_FALSE, (GLfloat*)projection);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"),
			   1, GL_FALSE, (GLfloat*)camera);

	/* And render them */
	glDrawArrays(GL_QUADS, first, count);
}
