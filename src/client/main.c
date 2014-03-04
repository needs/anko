#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <GLFW/glfw3.h>

#define MAX_FPS 150

#include "renderer.h"
#include "textures.h"
#include "event.h"

static void init(void);
static void terminate(void);

static void update_fps(void);

static GLFWwindow* window;
static int frames = 0;
static double last_fps = 0;

int main(void)
{
	static float deltatime = 0;
	double last_frame = 0;
	
	init();
	glClearColor(0.0, 0.0, 0.0, 1);

	while(!glfwWindowShouldClose(window)) {
		process_events(deltatime);

		last_frame = glfwGetTime();
		update_fps();
		
		glClear(GL_COLOR_BUFFER_BIT);
		render_rect(100, 100, 70, 63, TEX_GRASS);
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
	return EXIT_SUCCESS;
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
	window = glfwCreateWindow(WIDTH, HEIGHT, "Anko", NULL, NULL);
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
