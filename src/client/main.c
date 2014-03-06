#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <GLFW/glfw3.h>

#include "renderer.h"
#include "textures.h"
#include "event.h"
#include "map.h"
#include "font.h"
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
static int fps = 0;
static int  init(void);
static void terminate(void);
static void update_fps(void);
static void wait_fps(void);
static void try_simulate(map_t *map, board_t **old, board_t **current);
static void swap(void **p1, void **p2);
static void draw_fps(float x, float y, float scale);

static const float STEP_TIMER_RESET = 1; // Each second we simulate
static const int BOARD_WIDTH = 100;
static const int BOARD_HEIGHT = 100;


int main(void)
{
	board_t *current, *old;
	map_t *map;
	gen_params_t gen_params = { .tree_density = 0.7, .water_density = 0.2, .water_shatter_factor = 0.4 };
	double last_frame = 0;
	
	if (!init())
		goto err_init;
	if ((current = generate(BOARD_WIDTH, BOARD_HEIGHT, gen_params)) == NULL)
		goto err_current;
	if ((old = alloc_board(BOARD_WIDTH, BOARD_HEIGHT)) == NULL)
		goto err_old;
	if ((map = create_map(current)) == NULL)
		goto err_map;

	glClearColor(0, 0, 0, 1);
	
	while(!glfwWindowShouldClose(window)) {
		last_frame = glfwGetTime();
		process_events(deltatime);
		
		update_fps();
		try_simulate(map, &old, &current);

		glClear(GL_COLOR_BUFFER_BIT);
		rtt_start();
		glClear(GL_COLOR_BUFFER_BIT);
		render_map(map);
		rtt_stop();
		rtt_draw();
		
		draw_fps(10,10,0.5);
		glfwSwapBuffers(window);
		
		deltatime = glfwGetTime() - last_frame;
		wait_fps(); // Limiting frame per second 
	}
	
	free_board(old);
	free_board(current);
	free_map(map);
	terminate();

	return EXIT_SUCCESS;

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
		step_timer = STEP_TIMER_RESET;
	} else
		step_timer -= deltatime;
}

static void draw_fps(float x, float y, float scale)
{
	char message[16];
	snprintf(message, 16, "FPS: %i", fps);
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
  
	if(!load_font())
		goto err_font;
		
	step_timer = STEP_TIMER_RESET;
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
