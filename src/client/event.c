#include "event.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "camera.h"

// -Wall compile with unused parameters . . .
#define UNUSED(x) (void)(x)

static GLFWwindow *current_window;

static const float CAMERA_SPEED = 500; // pixels per second
static const float ZOOM_SPEED = 3;
static const float MOUSE_SPEED = 0.05;

static double scroll = 0;
short is_dragging = 0;
static short drag_trigger = 0;

static double last_mouse_pos[2] = {0,0};

void process_events(float deltatime)
{
	glfwPollEvents();
	if (IS_KEY_PRESSED(current_window,GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(current_window, GL_TRUE);
	
	if (IS_KEY_PRESSED(current_window,GLFW_KEY_UP))
		move_camera(0, CAMERA_SPEED * deltatime);
	else if (IS_KEY_PRESSED(current_window,GLFW_KEY_DOWN))
		move_camera(0, -CAMERA_SPEED * deltatime);
	if (IS_KEY_PRESSED(current_window,GLFW_KEY_RIGHT))
		move_camera(-CAMERA_SPEED * deltatime, 0);
	else if (IS_KEY_PRESSED(current_window,GLFW_KEY_LEFT))
		move_camera(CAMERA_SPEED * deltatime, 0);

	if(scroll) {
		scale_camera(scroll*ZOOM_SPEED*deltatime);
		scroll = 0;
	}

	if(drag_trigger)
	{
		/*  // DO NOT DELETE ME WILL BE USEFUL LATER
		if(is_dragging)
			glfwSetCursorPos(current_window, last_mouse_pos[0], last_mouse_pos[1]);
		else
			glfwGetCursorPos(current_window, last_mouse_pos, last_mouse_pos+1);
		*/
		is_dragging = !is_dragging;
		drag_trigger = 0;
	}
	
	if(is_dragging)
	{
		double current_mouse_pos[2];
		//glfwSetInputMode(current_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		
		glfwGetCursorPos(current_window, current_mouse_pos, current_mouse_pos+1);
		move_camera((1/(2*camera_scale))*MOUSE_SPEED*CAMERA_SPEED*(current_mouse_pos[0]-last_mouse_pos[0])*deltatime,
					(1/(2*camera_scale))*MOUSE_SPEED*CAMERA_SPEED*(current_mouse_pos[1]-last_mouse_pos[1])*deltatime);
		//glfwSetCursorPos(current_window, last_mouse_pos[0], last_mouse_pos[1]);
	 }
	/* else
	   glfwSetInputMode(current_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); */

	glfwGetCursorPos(current_window, last_mouse_pos, last_mouse_pos+1);
}

void on_mouse_button(GLFWwindow *window, int button, int action, int mods)
{
	UNUSED(window);
	UNUSED(mods);
	UNUSED(action);
	if(button == GLFW_MOUSE_BUTTON_LEFT)
	    drag_trigger = 1;
}

void on_mouse_scroll(GLFWwindow *window, double offx, double offy)
{
	UNUSED(window);
	UNUSED(offx);
	scroll = offy;
}

void init_events(GLFWwindow *window)
{
	current_window = window;
	glfwSetScrollCallback(window, on_mouse_scroll);
	glfwSetMouseButtonCallback(window, on_mouse_button);
}
