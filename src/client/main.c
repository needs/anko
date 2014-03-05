#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <GLFW/glfw3.h>

#include "renderer.h"
#include "textures.h"
#include "event.h"
#include "map.h"
#include "../board.h"
#include "../generator.h"
#include "../simulator.h"
#include "window.h"


#define MAX_FPS 150

static GLFWwindow* window;
static int frames = 0;
static double last_fps = 0;
static float deltatime = 0;
static float step_timer;

static int  init(void);
static void terminate(void);
static void update_fps(void);
static void wait_fps(void);
static void try_simulate(void);

static const float STEP_TIMER_RESET = 1; // Each second we simulate
static const int BOARD_WIDTH = 100;
static const int BOARD_HEIGHT = 100;


int main(void)
{
	board_t *board;
	
	double last_frame = 0;
	map_t *map;

	gen_params_t gen_params = { .tree_density = 0.7, .water_density = 0.2, .water_shatter_factor = 0.4 };
	
	if (!init())
		goto err_init;

	last_frame = glfwGetTime();
	if(!init_simulator(&board, BOARD_WIDTH, BOARD_HEIGHT, gen_params))
		goto err_simulator;				
	printf("board generated in %lfms\n", 1000*(glfwGetTime()-last_frame));
	
	if ((map = create_map(BOARD_WIDTH, BOARD_HEIGHT)) == NULL)
		goto err_map;

	glClearColor(0, 0, 0, 1);
	
	while(!glfwWindowShouldClose(window)) {
		last_frame = glfwGetTime();
		process_events(deltatime);
		
		update_fps();
		
		//simulate();
		try_simulate();
		
		glClear(GL_COLOR_BUFFER_BIT);
		render_map(map, board);
		glfwSwapBuffers(window);
		
		deltatime = glfwGetTime() - last_frame;
		wait_fps(); // Limiting frame per second 
	}
	
	terminate();
	terminate_simulator();
	free_map(map);

	return EXIT_SUCCESS;


err_map:
	terminate_simulator();
err_simulator:
	terminate();
err_init:
	return EXIT_FAILURE;
}

static void try_simulate(void)
{
	if(step_timer < deltatime)
	{
		simulate();
		step_timer = STEP_TIMER_RESET;
	}
	else
		step_timer -= deltatime;
}

static void update_fps(void)
{
	if (glfwGetTime() - last_fps > 1) {
		last_fps+=1;
		printf("FPS: %i\n", frames);
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

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Anko", NULL, NULL);
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

	step_timer = STEP_TIMER_RESET;
	return 1;

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

