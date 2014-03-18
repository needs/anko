#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>

#include <stdio.h>
#include <client/event.h>
#include <client/camera.h>


static GLFWwindow *current_window;

ui_frame_t **linked_frame = NULL;

void events_link_frame(ui_frame_t **frame)
{
	linked_frame = frame;
}

void on_key(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    (void)window;
	if(linked_frame)
		ui_on_key(*linked_frame, key, scancode, action, mods);
}

void on_mouse_button(GLFWwindow *window, int button, int action, int mods)
{
   (void)window;
   if(linked_frame)
	   ui_on_mouse_button(*linked_frame, button, action, mods);
}

void on_mouse_scroll(GLFWwindow *window, double offx, double offy)
{
	(void)window;
	if(linked_frame)
		ui_on_mouse_scroll(*linked_frame, offx, offy);
}

void on_mouse_move(GLFWwindow *window, double x, double y)
{
	(void)window;
	if(linked_frame)
		ui_on_mouse_move(*linked_frame, x, y);
}

void on_char(GLFWwindow *window, unsigned int c)
{
	(void)window;
	if(linked_frame)
		ui_on_char(*linked_frame, c);
}

void init_events(GLFWwindow *window)
{
	current_window = window;
	glfwSetScrollCallback(window, on_mouse_scroll);
	glfwSetMouseButtonCallback(window, on_mouse_button);
	glfwSetKeyCallback(window, on_key);
	glfwSetCursorPosCallback(window, on_mouse_move);
	glfwSetCharCallback(window, on_char);
}
