#include <stdlib.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "ui_game.h"
#include "../context.h"
#include "../world.h"
#include "../font.h"
#include "../renderer.h"
#include "../camera.h"
#include "../linmath.h"
#include "../config.h"
#include "ui_console.h"
#include "ui_debug.h"
#include <assert.h>
#include <stdio.h>

#define CAMERA_SPEED 500 // px/s

#define IS_KEY_DOWN(k) (key == k &&  (action == GLFW_PRESS || action == GLFW_REPEAT))

typedef struct ui_game_data_t
{
	world_t *world;
	camera_t camera;
	int camx;
	int camy;

	int was_focused;
} ui_game_data_t;

void ui_game_input_camera(ui_game_data_t *data, int key, int scancode, int action, int mods);
void destroy_ui_game(ui_frame_t* frame);

void draw_game(ui_frame_t *frame)
{
	ui_game_data_t *data = frame->data;
	int i = 0;
	rtt_start();
	glClear(GL_COLOR_BUFFER_BIT);
	render_world(data->world, &data->camera);
	rtt_stop();
	rtt_draw();

	if(!frame->childs)
		return;
	
	while(frame->childs[i])
	{
		draw_ui(frame->childs[i]);
		i++;
	}
}

void ui_game_update(ui_frame_t* frame, float deltatime)
{
	ui_game_data_t *data = frame->data;
	int i;

	if(data->camy > 0)
		move_camera(&data->camera, 0, CAMERA_SPEED*deltatime);
	else if(data->camy < 0)
		move_camera(&data->camera, 0, -CAMERA_SPEED*deltatime);

	if(data->camx > 0)
		move_camera(&data->camera, CAMERA_SPEED*deltatime, 0);
	else if(data->camx < 0)
		move_camera(&data->camera, -CAMERA_SPEED*deltatime, 0);
	
	if(!frame->childs)
		return;
	
	while(frame->childs[i])
	{
		update_ui(frame->childs[i], deltatime);
		i++;
	}
}

void ui_game_on_key(ui_frame_t* frame, int key, int scancode, int action, int mods)
{
	ui_game_data_t *data = frame->data;
	(void)scancode;
	(void)mods;

	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		if(frame->keyboard_owner)
			frame->keyboard_owner = NULL;
		else
			should_quit = 1;
	}

	if(frame->keyboard_owner)
	{
		data->camx = 0;
		data->camy = 0;
		data->was_focused = 0;
		ui_on_key(frame->keyboard_owner, key, scancode, action, mods);
		return;
	}
	
	if(frame->childs)
	{
		int i = 0;
		while(frame->childs[i] && !frame->keyboard_owner)
		{
		    ui_on_key(frame->childs[i], key, scancode, action, mods);
			i++;
		}
	}

	ui_game_input_camera(data, key, scancode, action, mods);
		
	if(key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		rtt_effect = (rtt_effect +1)%6;
	}
}

void ui_game_on_char(ui_frame_t* frame, unsigned int c)
{
	if(frame->childs)
	{
		int i = 0;
		while(frame->childs[i])
		{
		    ui_on_char(frame->childs[i], c);
			i++;
		}
	}
}

void ui_game_on_mouse_move(ui_frame_t* frame, double x, double y)
{
	if(frame->childs)
	{
		int i = 0;
		while(frame->childs[i])
		{
			ui_on_mouse_move(frame->childs[i], x, y);
			i++;
		}
	}
}

void ui_game_on_mouse_button(ui_frame_t* frame, int button, int action, int mods)
{
	if(frame->childs)
	{
		int i = 0;
		while(frame->childs[i]) // the child can decide to stop the event
		{
			ui_on_mouse_button(frame->childs[i], button, action, mods);
			i++;
		}
	}
}

void destroy_ui_game(ui_frame_t* frame)
{
	if(frame->childs)
	{
		int i = 0;
    	while(frame->childs[i])
		{
			destroy_ui(frame->childs[i]);
			i++;
		}
	}
	
	free(frame->data);
	free(frame);
}

ui_frame_t* init_ui_game(world_t *world)
{
	const int children_count = 2;
	ui_frame_t *frame = create_ui();
	if(frame)
	{
		ui_game_data_t *data = malloc(sizeof(ui_game_data_t));
		if(!data)
			goto err_data;
		
		data->world = world;
		data->camx = 0;
		data->camy = 0;
		data->was_focused = 1;
		set_camera(&data->camera, 0,0,1);
		
		frame->data = data;
		frame->draw = &draw_game;
		frame->on_key = &ui_game_on_key;
		frame->update = &ui_game_update;
		frame->on_mouse_move = &ui_game_on_mouse_move;
		frame->on_mouse_button = &ui_game_on_mouse_button;
		frame->destroy = &destroy_ui_game;
		frame->on_char = &ui_game_on_char;

		frame->childs = malloc(sizeof(ui_frame_t *)*(children_count+1));
		if(!frame->childs)
			goto err_childs;
		
		frame->childs[0] = init_ui_console(frame, 10, config.screen_height - 215,400,200);
		frame->childs[1] = init_ui_debug(frame, 10,10,250,100);
		frame->childs[2] = NULL;
		
		return frame;

	err_childs:
		free(data);
	err_data:
		return NULL;
	}
	return NULL;
}

void ui_game_input_camera(ui_game_data_t *data, int key, int scancode, int action, int mods)
{
	(void)scancode;
	(void)mods;

	// We don't deal with repeated key
	if(action == GLFW_REPEAT)
		return;
	
	if(key == GLFW_KEY_UP || key == GLFW_KEY_W)
	{
		if(action == GLFW_PRESS)
			data->camy += 1;
		else if(action == GLFW_RELEASE && data->was_focused)
			data->camy -= 1;
		data->was_focused = 1;
	}

	if(key == GLFW_KEY_DOWN || key == GLFW_KEY_S)
	{
		if(action == GLFW_PRESS)
			data->camy -= 1;
		else if(action == GLFW_RELEASE && data->was_focused)
			data->camy += 1;
		data->was_focused = 1;
	}

	if(key == GLFW_KEY_RIGHT || key == GLFW_KEY_D)
	{
		if(action == GLFW_PRESS)
			data->camx += 1;
		else if(action == GLFW_RELEASE && data->was_focused)
			data->camx -= 1;
		data->was_focused = 1;
	}

	if(key == GLFW_KEY_LEFT || key == GLFW_KEY_A)
	{
		if(action == GLFW_PRESS)
			data->camx -= 1;
		else if(action == GLFW_RELEASE && data->was_focused)
			data->camx += 1;
		data->was_focused = 1;
	}
	

}
