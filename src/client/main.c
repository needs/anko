#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>

int main(void)
{
	GLFWwindow* window;

	if(!glfwInit())
	{
		perror("Can't init glfw\n");
		return EXIT_FAILURE;
	}

	window = glfwCreateWindow(1024,720,"Anko", NULL, NULL);
	if(!window)
	{
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glClearColor(0.3,0.4,0.7,1);
	
	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	
	return EXIT_SUCCESS;
}
