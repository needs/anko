#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <GLFW/glfw3.h>

#include "context.h"
#include "renderer.h"
#include "textures.h"
#include "event.h"
#include "map.h"
#include "font.h"
#include "config.h"
#include "../board.h"
#include "../generator.h"
#include "../simulator.h"
#include "ui/ui_frame.h"
#include "ui/ui_game.h"


#define MAX_FPS 3000

static GLFWwindow* window;
static int frames = 0;
static double last_fps = 0;
static float deltatime = 0;
static float step_timer;
static int fps = 0;
static ui_frame_t *current_ui;

static int  init(void);
static void terminate(void);
static void update_fps(void);
static void wait_fps(void);
static void try_simulate(map_t *map, board_t **old, board_t **current);
static void swap(void **p1, void **p2);
static void draw_fps(float x, float y, float scale);


int main(int argc, char **argv)
{
	board_t *current, *old;
	map_t *map;
	double last_frame = 0;
	
	if (config_from_file("anko.cfg", 0) == 2)
		return EXIT_FAILURE;
	if (config_from_args(argc, argv))
		return EXIT_FAILURE;

	if (!init())
		goto err_init;
	if ((current = generate(config.board_width, config.board_height, config.gen_params)) == NULL)
		goto err_current;
	if ((old = alloc_board(config.board_width, config.board_height)) == NULL)
		goto err_old;
	if ((map = create_map(current)) == NULL)
		goto err_map;
	if((current_ui = init_ui_game(map)) == NULL)
		goto err_ui;

	events_link_frame(&current_ui);
	glClearColor(0, 0, 0, 1);
	
	while(!glfwWindowShouldClose(window)) {
		last_frame = glfwGetTime();
		glfwPollEvents();

		// Update
		update_ui(current_ui, deltatime);
		update_fps();
		try_simulate(map, &old, &current);
		if(should_quit)
			glfwSetWindowShouldClose(window, GL_TRUE);

		// Rendering
		glClear(GL_COLOR_BUFFER_BIT);
		draw_ui(current_ui);
		draw_fps(10,10,0.5);
		glfwSwapBuffers(window);
		
		deltatime = glfwGetTime() - last_frame;
		wait_fps(); // Limiting frame per second

	}

	destroy_ui(current_ui);
	free_board(old);
	free_board(current);
	free_map(map);
	terminate();

	return EXIT_SUCCESS;
err_ui:
	free_map(map);
err_map:
	free_board(old);
err_old:
	free_board(current);
err_current:
	terminate();
err_init:
	return EXIT_FAILURE;
}


static void try_simulate(map_t *map, board_t **old, board_t **current)
{
	if (step_timer < deltatime) {
		step(*old, *current);
		swap((void**)current, (void**)old);
		update_map(map, *current, *old);
		step_timer = config.sim_speed;
	} else
		step_timer -= deltatime;
}

static void draw_fps(float x, float y, float scale)
{
	char message[16];
	snprintf(message, 16, "FPS: %i", fps);
	set_font_color(0,0.63,1,1);
	render_text(message, x, y, scale);
}

static void update_fps(void)
{
	if (glfwGetTime() - last_fps > 1) {
		last_fps+=1;
		fps = frames;
		frames = 0;
	}
	else
		frames++;
}


static int init(void)
{
	srandom(time(NULL));

	if (!glfwInit()) {
		perror("Can't init glfw\n");
		goto err_glfw;
	}

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 16);
	window = glfwCreateWindow(config.screen_width, config.screen_height, "Anko", NULL, NULL);
	if (!window) {
		glfwTerminate();
		goto err_window;
	}
	glfwMakeContextCurrent(window);

	printf("OpenGL Version : %s\n\n", glGetString(GL_VERSION));
	
	if (!init_rendering())
		goto err_rendering;
	if (!load_textures())
		goto err_texture;
	init_events(window);
  
	if(!load_font())
		goto err_font;
		
	step_timer = config.sim_speed;
	return 1;

err_font:
	unload_textures();
err_texture:
	close_rendering();
err_rendering:
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
	close_rendering();
	glfwDestroyWindow(window);
	glfwTerminate();
}


static void wait_fps(void)
{
	if (deltatime < (float)1/(MAX_FPS)) {
		float sleep_time = ((float)1/MAX_FPS)-deltatime;
		usleep(sleep_time * 1000 * 1000); // * inMilliSec * inMicroSec
		deltatime += sleep_time;
	}
}


static void swap(void **p1, void **p2)
{
	void *tmp;

	assert(p1 != NULL);
	assert(p2 != NULL);
	tmp = *p1;
	*p1 = *p2;
	*p2 = tmp;
}
