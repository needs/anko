#include "ui_frame.h"
#include <stdlib.h>

ui_frame_t *create_ui()
{
	ui_frame_t* frame = malloc(sizeof(ui_frame_t));
	if(frame)
	{
		frame->movable = 0;
		frame->hidden = 0;
		frame->x = 0;
		frame->y = 0;
		frame->width = 0;
		frame->height = 0;
		frame->update = NULL;
		frame->draw = NULL;
		frame->on_mouse_move = NULL;
		frame->on_mouse_button = NULL;
		frame->on_mouse_scroll = NULL;
		frame->on_key = NULL;
		frame->childs = NULL;
		frame->data = NULL;
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
	if(frame->draw)
		frame->draw(frame);
}

void ui_on_mouse_move(ui_frame_t* frame, double x, double y)
{
	if(frame->on_mouse_move)
		frame->on_mouse_move(frame,x,y);
}

void ui_on_mouse_button(ui_frame_t* frame, int button, int action, int mods)
{
	if(frame->on_mouse_button)
		frame->on_mouse_button(frame, button, action, mods);
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
