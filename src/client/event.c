#include "event.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

// -Wall compile with unused parameters . . .
#define UNUSED(x) (void)(x)

static GLFWwindow *current_window;

void process_events(float deltatime)
{
	deltatime++;
	glfwPollEvents();
	if(glfwGetKey(current_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(current_window, GL_TRUE);
}

void on_mouse_move(GLFWwindow *window, double x, double y)
{
	UNUSED(window);
	printf("Mouse moved ! (%f,%f)\n", x,y);
}

void init_events(GLFWwindow *window)
{
	current_window = window;
	glfwSetCursorPosCallback(window, on_mouse_move);
}
