#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <client/context.h>
#include <client/textures.h>
#include <client/event.h>
#include <client/world.h>
#include <client/font.h>
#include <client/config.h>
#include <game/board.h>
#include <game/generator.h>
#include <game/simulator.h>
#include <game/game.h>
#include <client/ui/ui_frame.h>
#include <client/ui/ui_game.h>

#define SPEED_UPDATE 0.3
#define MAX_MSPF 1.0 / config.max_fps


static GLFWwindow* window;
static float last_speed_update = 0;
static float speed_frames = 0;
static float speed_time = 0;
static ui_frame_t *current_ui;

static int  init(void);
static void terminate(void);
static void update_speed(float deltatime);


int main(int argc, char **argv)
{
	world_t *world;
	game_t *game;
	double last_time = 0;
	double current_time = 0;
	float deltatime = 0;
	float sleep_time;
	
	if (config_from_file("anko.cfg", 0) == 2)
		return EXIT_FAILURE;
	if (config_from_args(argc, argv))
		return EXIT_FAILURE;

	if (!init())
		goto err_init;
	if ((game = new_game(config.board_width, config.board_height, &config.gen_params, config.sim_speed * 1000)) == NULL)
		goto err_game;
	if ((world = create_world(game)) == NULL)
		goto err_world;
	if ((current_ui = init_ui_game(world)) == NULL)
		goto err_ui;

	world_set_active_player(world, add_player(game, TEAM_BURNER));
	events_link_frame(&current_ui); // link window event to game ui frame
	glClearColor(0, 0, 0, 1);

	last_time = 0;
	while(!glfwWindowShouldClose(window)) {
	    current_time = glfwGetTime();
		deltatime = current_time - last_time;
		update_speed(deltatime);
		glfwPollEvents();

		// Update 
		update_ui(current_ui, deltatime);
		if (update_game(game, deltatime * 1000))
			update_world(world);
		if(should_quit)
			glfwSetWindowShouldClose(window, GL_TRUE);
		
		// Rendering
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		draw_ui(current_ui);
		font_swap_buffers();
		glfwSwapBuffers(window);

		// Update speed and sleep if necessary
		last_time = current_time;
		sleep_time = (current_time + MAX_MSPF - glfwGetTime()) * 1000 * 1000;
		if(sleep_time > 0)
			usleep(sleep_time);
	}

	destroy_ui(current_ui);
	game_over(game);
	end_of_the_world(world); // Tin tin tin
	terminate();

	return EXIT_SUCCESS;
err_ui:
	end_of_the_world(world);
err_world:
	game_over(game);
err_game:
	terminate();
err_init:
	return EXIT_FAILURE;
}


static void update_speed(float deltatime)
{
	speed_time += deltatime;
	if (glfwGetTime() - last_speed_update > SPEED_UPDATE) {
		speed = (speed_time/speed_frames)*1000;
		last_speed_update+=SPEED_UPDATE;
		speed_frames = 0;
		speed_time = 0;
	}
	else
		speed_frames++;
}


static int init(void)
{
	srandom(time(NULL));

	if (!glfwInit()) {
		perror("Can't init glfw\n");
		goto err_glfw;
	}

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(config.screen_width, config.screen_height, "Anko", NULL, NULL);
	if (!window) {
		glfwTerminate();
		goto err_window;
	}
	glfwMakeContextCurrent(window);

	printf("OpenGL Version : %s\n\n", glGetString(GL_VERSION));
	
	if (!init_context())
		goto err_context;
	if (!load_textures())
		goto err_texture;
	init_events(window);
  
	if(!load_font())
		goto err_font;
		
	return 1;

err_font:
	unload_textures();
err_texture:
	close_context();
err_context:
	glfwDestroyWindow(window);
err_window:
	glfwTerminate();
err_glfw:
	return 0;
}


static void terminate(void)
{
	unload_font();
	unload_textures();
	close_context();
	glfwDestroyWindow(window);
	glfwTerminate();
}
