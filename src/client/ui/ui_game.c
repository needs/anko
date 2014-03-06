#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "ui_game.h"
#include "../context.h"
#include "../map.h"
#include "../font.h"
#include "../renderer.h"
#include "../camera.h"
#include <stdlib.h>
#include "../linmath.h"

#define CAMERA_SPEED 500 // px/s

#define IS_KEY_DOWN(k) (key == k &&  (action == GLFW_PRESS || action == GLFW_REPEAT))

typedef struct ui_game_data_t
{
	map_t *map;
	camera_t camera;
    int camx;
	int camy;
} ui_game_data_t;

void ui_game_input_camera(ui_game_data_t *data, int key, int scancode, int action, int mods);

void draw_game(ui_frame_t *frame)
{
	ui_game_data_t *data = frame->data;
	rtt_start();
	glClear(GL_COLOR_BUFFER_BIT);
	render_map(data->map, &data->camera);
	rtt_stop();
	rtt_draw();
	render_text("We are in game ui", 10, 40, 0.6);
}

void ui_game_update(ui_frame_t* frame, float deltatime)
{
	ui_game_data_t *data = frame->data;

	if(data->camy > 0)
		move_camera(&data->camera, 0, CAMERA_SPEED*deltatime);
	else if(data->camy < 0)
		move_camera(&data->camera, 0, -CAMERA_SPEED*deltatime);

	if(data->camx > 0)
		move_camera(&data->camera, CAMERA_SPEED*deltatime, 0);
	else if(data->camx < 0)
		move_camera(&data->camera, -CAMERA_SPEED*deltatime, 0);
}

int ui_game_on_key(ui_frame_t* frame, int key, int scancode, int action, int mods)
{
	ui_game_data_t *data = frame->data;
	(void)scancode;
	(void)mods;

	if(key == GLFW_KEY_ESCAPE)
		should_quit = 1;
	
	ui_game_input_camera(data, key, scancode, action, mods);

	if(key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		rtt_effect = (rtt_effect +1)%6;
	}
	
	return 0;
}

void destroy_ui_game(ui_frame_t* frame)
{
	free(frame->data);
	free(frame);
}

ui_frame_t* init_ui_game(map_t *map)
{
	ui_frame_t *frame = create_ui();
	if(frame)
	{
		ui_game_data_t *data = malloc(sizeof(ui_game_data_t));
		data->map = map;
		data->camx = 0;
		data->camy = 0;
		set_camera(&data->camera, 0,0,1);
		
		frame->data = data;
		frame->draw = &draw_game;
		frame->on_key = &ui_game_on_key;
		frame->update = &ui_game_update;
	}
	return frame;
}

void ui_game_input_camera(ui_game_data_t *data, int key, int scancode, int action, int mods)
{
	(void)scancode;
	(void)mods;
	if(key == GLFW_KEY_UP)
	{
		if(action == GLFW_PRESS)
			data->camy += 1;
		else if(action == GLFW_RELEASE)
			data->camy -= 1;
	}

	if(key == GLFW_KEY_DOWN)
	{
		if(action == GLFW_PRESS)
			data->camy -= 1;
		else if(action == GLFW_RELEASE)
			data->camy += 1;
	}

	if(key == GLFW_KEY_RIGHT)
	{
		if(action == GLFW_PRESS)
			data->camx += 1;
		else if(action == GLFW_RELEASE)
			data->camx -= 1;
	}

	if(key == GLFW_KEY_LEFT)
	{
		if(action == GLFW_PRESS)
			data->camx -= 1;
		else if(action == GLFW_RELEASE)
			data->camx += 1;
	}
}
