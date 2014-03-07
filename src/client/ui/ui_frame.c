#include "ui_frame.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>

ui_frame_t *create_ui()
{
	ui_frame_t* frame = malloc(sizeof(ui_frame_t));
	if(frame)
	{
		frame->hidden = 0;
		frame->x = 0;
		frame->y = 0;
		frame->width = 0;
		frame->height = 0;
		frame->destroy = NULL;
		frame->update = NULL;
		frame->draw = NULL;
		frame->on_mouse_move = NULL;
		frame->on_mouse_button = NULL;
		frame->on_mouse_scroll = NULL;
		frame->on_key = NULL;
		frame->on_char = NULL;
		frame->childs = NULL;
		frame->data = NULL;
		frame->keyboard_owner = NULL;
		frame->mouse_owner = NULL;
		frame->parent = NULL;
		frame->movable = 0;
		frame->is_dragging = 0;
		frame->last_mouse[0] = 0;
		frame->last_mouse[0] = 0;
		frame->is_hovered = 0;
	}
	return frame;
}

void update_ui(ui_frame_t *frame, float deltatime)
{
	if(frame->update)
		frame->update(frame,deltatime);
}

void draw_ui(ui_frame_t *frame)
{
	if(frame->draw && !frame->hidden )
		frame->draw(frame);
}

void ui_on_mouse_move(ui_frame_t* frame, double x, double y)
{
	if(frame->on_mouse_move)
		frame->on_mouse_move(frame,x,y);

	frame->is_hovered =
		x >= frame->x
		&& x <= frame->x+frame->width
		&& y >= frame->y
		&& y <= frame->y+frame->height;
	
	if(frame->movable && frame->is_dragging)
	{
		frame->x += x-frame->last_mouse[0];
		frame->y += y-frame->last_mouse[1];
	}
	frame->last_mouse[0] = x;
	frame->last_mouse[1] = y;
}

void ui_on_mouse_button(ui_frame_t* frame, int button, int action, int mods)
{
	if(frame->on_mouse_button)
		frame->on_mouse_button(frame, button, action, mods);

	if(button == GLFW_MOUSE_BUTTON_LEFT && frame->is_hovered)
		frame->is_dragging = action == GLFW_PRESS;
}

void ui_on_mouse_scroll(ui_frame_t* frame, double sx, double sy)
{
	if(frame->on_mouse_scroll)
		frame->on_mouse_scroll(frame,sx,sy);
}

void ui_on_key(ui_frame_t* frame, int key, int scancode, int action, int mods)
{
	if(frame->on_key)
		frame->on_key(frame,key,scancode,action,mods);
}

void ui_on_char(ui_frame_t* frame, unsigned int c)
{
	if(frame->on_char)
		frame->on_char(frame, c);
}

void destroy_ui(ui_frame_t *frame)
{
	if(frame->destroy)
		frame->destroy(frame);
}
