#include "event.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "camera.h"

// -Wall compile with unused parameters . . .
#define UNUSED(x) (void)(x)

static GLFWwindow *current_window;

static const float CAMERA_SPEED = 2.5;

void process_events(float deltatime)
{
	deltatime++;
	glfwPollEvents();
	if(glfwGetKey(current_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(current_window, GL_TRUE);

	if(glfwGetKey(current_window, GLFW_KEY_UP) == GLFW_PRESS)
		move_camera(0,-CAMERA_SPEED*deltatime);
	if(glfwGetKey(current_window, GLFW_KEY_DOWN) == GLFW_PRESS)
		move_camera(0,CAMERA_SPEED*deltatime);
	if(glfwGetKey(current_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		move_camera(CAMERA_SPEED*deltatime,0);
	if(glfwGetKey(current_window, GLFW_KEY_LEFT) == GLFW_PRESS)
		move_camera(-CAMERA_SPEED*deltatime,0);
}

void on_mouse_move(GLFWwindow *window, double x, double y)
{
	UNUSED(window);
	UNUSED(x);
	UNUSED(y);
//printf("Mouse moved ! (%f,%f)\n", x,y);
}

void init_events(GLFWwindow *window)
{
	current_window = window;
	glfwSetCursorPosCallback(window, on_mouse_move);
}
