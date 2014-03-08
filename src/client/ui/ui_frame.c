#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include "ui_frame.h"

ui_frame_t *create_ui()
{
	ui_frame_t *frame = calloc(1, sizeof(*frame));

	if (frame == NULL)
		perror("calloc(frame)");

	return frame;
}

void update_ui(ui_frame_t *frame, float deltatime)
{
	if(frame->update)
		frame->update(frame,deltatime);
}

void update_render_ui(ui_frame_t *frame)
{
	if(frame->update_render)
		frame->update_render(frame);
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
		update_render_ui(frame);
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
