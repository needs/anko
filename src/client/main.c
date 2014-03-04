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
static void simulate(board_t **main, board_t **temp);

static const float STEP_TIMER_RESET = 1; // Each second we simulate
static const int BOARD_WIDTH = 100;
static const int BOARD_HEIGHT = 100;

int main(void)
{
	board_t *board, *dest;
	
	double last_frame = 0;
	map_t *map;

	if (!init())
		goto err_init;
	if ((board = generate(BOARD_WIDTH, BOARD_HEIGHT, 0.5, 0.1, 0.4)) == NULL)
		goto err_board;
	if ((dest = alloc_board(BOARD_WIDTH, BOARD_HEIGHT)) == NULL)
		goto err_dest;
	if ((map = create_map(BOARD_WIDTH, BOARD_HEIGHT)) == NULL)
		goto err_map;

	glClearColor(0, 0, 0, 1);
	
	while(!glfwWindowShouldClose(window)) {
		last_frame = glfwGetTime();
		process_events(deltatime);
		
		update_fps();
		
		simulate(&board, &dest);
		
		glClear(GL_COLOR_BUFFER_BIT);
		render_map(map, board);
		glfwSwapBuffers(window);
		
		deltatime = glfwGetTime() - last_frame;
		wait_fps(); // Limiting frame per second 
	}
	
	terminate();
	free_map(map);
	free_board(dest);
	free_board(board);

	return EXIT_SUCCESS;


err_map:
	free_board(dest);
err_dest:
	free_board(board);
err_board:
	terminate();
err_init:
	return EXIT_FAILURE;
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

				
static void simulate(board_t **main, board_t **temp)
{
	if (step_timer < deltatime) {
		board_t *tmp;
		step(*temp, *main);
		tmp = *main;
		*main = *temp;
		*temp = tmp;
		step_timer = STEP_TIMER_RESET;
	} else
		step_timer -= deltatime;
}
