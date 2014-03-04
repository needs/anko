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
#include "window.h"


#define MAX_FPS 150

static GLFWwindow* window;
static int frames = 0;
static double last_fps = 0;


static void init(void);
static void terminate(void);
static void update_fps(void);


static const int BOARD_WIDTH = 20;
static const int BOARD_HEIGHT = 20;


int main(void)
{
	board_t *board, *dest, *tmp;
	static float deltatime = 0;
	double last_frame = 0;
	map_t *map;

	if ((board = generate(BOARD_WIDTH, BOARD_HEIGHT, 0.7, 0.1, 0.4)) == NULL)
		goto err_board;
	if ((dest = alloc_board(BOARD_WIDTH, BOARD_HEIGHT)) == NULL)
		goto err_dest;
	if ((map = create_map(BOARD_WIDTH, BOARD_HEIGHT)) == NULL)
		goto err_map;
	
	init();
	glClearColor(0, 0, 0, 1);

	while(!glfwWindowShouldClose(window)) {
		process_events(deltatime);

		last_frame = glfwGetTime();
		update_fps();
		
		glClear(GL_COLOR_BUFFER_BIT);
		render_map(map, board);
		glfwSwapBuffers(window);
		
		deltatime = glfwGetTime() - last_frame;
		if(deltatime < (float)1/(MAX_FPS))
		{
			float sleep_time = ((float)1/MAX_FPS)-deltatime;
			usleep(sleep_time*1000*1000); // *inMilliSec*inMicroSec
			deltatime += sleep_time;
		}
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
	return EXIT_FAILURE;
}

static void update_fps(void)
{
	if(glfwGetTime() - last_fps > 1)
	{
		last_fps+=1;
		printf("FPS: %i\n", frames);
		frames = 0;
	}
	else
		frames++;
}

static void init(void)
{	
	srandom(time(NULL));

	if(!glfwInit()) {
		perror("Can't init glfw\n");
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Anko", NULL, NULL);
	if(!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	init_rendering();
	if (!load_textures())
		exit(EXIT_FAILURE);
	
	init_events(window);
}

static void terminate(void)
{
	unload_textures();
	close_rendering();
	glfwDestroyWindow(window);
	glfwTerminate();
}
