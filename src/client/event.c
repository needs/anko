#include "event.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "camera.h"

// -Wall compile with unused parameters . . .
#define UNUSED(x) (void)(x)

static GLFWwindow *current_window;

static const float CAMERA_SPEED = 2.5;
static const float ZOOM_SPEED = 0.05;
static double scroll = 0;

void process_events(float deltatime)
{
	deltatime++;
	glfwPollEvents();
	if(glfwGetKey(current_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(current_window, GL_TRUE);

	if(glfwGetKey(current_window, GLFW_KEY_UP) == GLFW_PRESS)
		move_camera(0,CAMERA_SPEED*deltatime);
	if(glfwGetKey(current_window, GLFW_KEY_DOWN) == GLFW_PRESS)
		move_camera(0,-CAMERA_SPEED*deltatime);
	if(glfwGetKey(current_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		move_camera(-CAMERA_SPEED*deltatime,0);
	if(glfwGetKey(current_window, GLFW_KEY_LEFT) == GLFW_PRESS)
		move_camera(CAMERA_SPEED*deltatime,0);

	if(scroll)
	{
		scale_camera(scroll*ZOOM_SPEED*deltatime);
		scroll = 0;
	}
}

void on_scroll(GLFWwindow *window, double offx, double offy)
{
	UNUSED(window);
	UNUSED(offx);
	scroll = offy;
}

void init_events(GLFWwindow *window)
{
	current_window = window;
	glfwSetScrollCallback(window, on_scroll);
}
