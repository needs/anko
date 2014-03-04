#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <GLFW/glfw3.h>

#include "shader.h"
#include "renderer.h"
#include "textures.h"


int main(void)
{
	GLFWwindow* window;

	srandom(time(NULL));

	if(!glfwInit()) {
		perror("Can't init glfw\n");
		return EXIT_FAILURE;
	}

	glfwWindowHint(GLFW_RESIZABLE, 0);
	window = glfwCreateWindow(1024, 720, "Anko", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	init_rendering();
	if (!load_textures())
		return EXIT_FAILURE;
	
	glClearColor(0.0, 0.0, 0.0, 1);
	

	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		
		glClear(GL_COLOR_BUFFER_BIT);
		render_rect(0, 0, 0.3, 0.3, TEX_GRASS);

		glfwSwapBuffers(window);
	}

	unload_textures();
	close_rendering();
	glfwDestroyWindow(window);
	glfwTerminate();
	
	return EXIT_SUCCESS;
}
