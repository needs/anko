#include <client/context.h>
#include <client/shader.h>
#include <client/config.h>
#include <math.h>

int should_quit = 0;
float speed = 0;

mat4x4 projection;

int init_context(void)
{
	int depth = sqrt(pow(config.board_width,2)+pow(config.board_height, 2));
	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Load shaders and make them active */
	if (!load_shaders())
		goto err_shaders;
	
	mat4x4_ortho(projection, 0, config.screen_width, config.screen_height, 0, -depth, 0);
	return 1;

err_shaders:
	return 0;
}


void close_context(void)
{
    unload_shaders();
}
