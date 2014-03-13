#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <client/ui/ui_game.h>
#include <client/ui/ui_console.h>
#include <client/ui/ui_debug.h>
#include <client/context.h>
#include <client/world.h>
#include <client/font.h>
#include <client/rtt.h>
#include <client/camera.h>
#include <client/linmath.h>
#include <client/config.h>
#include <client/players.h>
#include <game/game.h>


#define IS_KEY_DOWN(k) (key == k &&  (action == GLFW_PRESS || action == GLFW_REPEAT))
#define CAMERA_SPEED 500 // px/s

static const float MOUSE_SCROLL_SPEED = 0.03;


typedef struct ui_game_data_t
{
	world_t *world;
	camera_t camera;
	int camx;
	int camy;
	int key_dir;
	
	int was_focused;
} ui_game_data_t;


void ui_game_input_camera(ui_game_data_t *data, int key, int scancode, int action, int mods);
void destroy_ui_game(ui_frame_t* frame);


void draw_game(ui_frame_t *frame)
{
	ui_game_data_t *data = frame->data;
	int i = 0;
	
	
	rtt_start();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable what we needs
	glEnable(GL_DEPTH_TEST);
	glAlphaFunc(GL_GREATER, 0.05); // This is necessary to avoid crop in some cases
	glEnable(GL_ALPHA_TEST);
	glDepthFunc(GL_LEQUAL);

	render_world(data->world, &data->camera);
	render_players(data->world->game->players, data->world->game->player_count, &data->camera);

	rtt_stop();
	rtt_draw();

	glDisable(GL_DEPTH_TEST);
	
	if(!frame->children)
		return;
	
	while(frame->children[i])
	{
		draw_ui(frame->children[i]);
		i++;
	}
}

void ui_game_update(ui_frame_t* frame, float deltatime)
{
	ui_game_data_t *data = frame->data;
	int i = 0;

	if(data->camy > 0)
		move_camera(&data->camera, 0, CAMERA_SPEED*deltatime);
	else if(data->camy < 0)
		move_camera(&data->camera, 0, -CAMERA_SPEED*deltatime);

	if(data->camx > 0)
		move_camera(&data->camera, CAMERA_SPEED*deltatime, 0);
	else if(data->camx < 0)
		move_camera(&data->camera, -CAMERA_SPEED*deltatime, 0);
	
	if(!frame->children)
		return;
	
	while(frame->children[i])
	{
		update_ui(frame->children[i], deltatime);
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
		{
			ui_on_key(frame->keyboard_owner, key, scancode, action, mods);
			frame->keyboard_owner = NULL;
		}
		else
			should_quit = 1;
	}

	if (key == GLFW_KEY_W) {
		if (action == GLFW_PRESS)
			data->key_dir |= DIR_UP;
		else if (action == GLFW_RELEASE)
			data->key_dir &= ~DIR_UP;
	}
	if (key == GLFW_KEY_S) {
		if (action == GLFW_PRESS)
			data->key_dir |= DIR_DOWN;
		else if (action == GLFW_RELEASE)
			data->key_dir &= ~DIR_DOWN;
	}
	if (key == GLFW_KEY_D) {
		if (action == GLFW_PRESS)
			data->key_dir |= DIR_RIGHT;
		else if (action == GLFW_RELEASE)
			data->key_dir &= ~DIR_RIGHT;
	}
	if (key == GLFW_KEY_A) {
		if (action == GLFW_PRESS)
			data->key_dir |= DIR_LEFT;
		else if (action == GLFW_RELEASE)
			data->key_dir &= ~DIR_LEFT;
	}

	if (data->key_dir == 0)
		set_player_dir(data->world->game, data->world->active_player, 0);
	else if ((data->key_dir & DIR_UP) == data->key_dir)
		set_player_dir(data->world->game, data->world->active_player, DIR_UP | DIR_LEFT);
	else if ((data->key_dir & DIR_DOWN) == data->key_dir)
		set_player_dir(data->world->game, data->world->active_player, DIR_DOWN | DIR_RIGHT);
	else if ((data->key_dir & DIR_LEFT) == data->key_dir)
		set_player_dir(data->world->game, data->world->active_player, DIR_LEFT | DIR_DOWN);
	else if ((data->key_dir & DIR_RIGHT) == data->key_dir)
		set_player_dir(data->world->game, data->world->active_player, DIR_UP | DIR_RIGHT);
	else if (data->key_dir & DIR_UP && data->key_dir & DIR_RIGHT)
		set_player_dir(data->world->game, data->world->active_player, DIR_UP);
	else if (data->key_dir & DIR_UP && data->key_dir & DIR_LEFT)
		set_player_dir(data->world->game, data->world->active_player, DIR_LEFT);
	else if (data->key_dir & DIR_DOWN && data->key_dir & DIR_RIGHT)
		set_player_dir(data->world->game, data->world->active_player, DIR_RIGHT);
	else if (data->key_dir & DIR_DOWN && data->key_dir & DIR_LEFT)
		set_player_dir(data->world->game, data->world->active_player, DIR_DOWN);

	if(frame->keyboard_owner)
	{
		data->camx = 0;
		data->camy = 0;
		data->was_focused = 0;
		ui_on_key(frame->keyboard_owner, key, scancode, action, mods);
		return;
	}
	
	if(frame->children)
	{
		int i = 0;
		while(frame->children[i] && !frame->keyboard_owner)
		{
		    ui_on_key(frame->children[i], key, scancode, action, mods);
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
	if(frame->children)
	{
		int i = 0;
		while(frame->children[i])
		{
		    ui_on_char(frame->children[i], c);
			i++;
		}
	}
}

void ui_game_on_mouse_move(ui_frame_t* frame, double x, double y)
{
	if(frame->children)
	{
		int i = 0;
		while(frame->children[i])
		{
			ui_on_mouse_move(frame->children[i], x, y);
			i++;
		}
	}
}

void ui_game_on_mouse_scroll(ui_frame_t *frame, double sx, double sy)
{
	(void)sx;
	ui_game_data_t *data = frame->data;
	scale_camera(&data->camera, sy*MOUSE_SCROLL_SPEED);
}

void ui_game_on_mouse_button(ui_frame_t* frame, int button, int action, int mods)
{
	if(frame->children)
	{
		int i = 0;
		while(frame->children[i]) // the child can decide to stop the event
		{
			ui_on_mouse_button(frame->children[i], button, action, mods);
			i++;
		}
	}
}

void destroy_ui_game(ui_frame_t* frame)
{
	if(frame->children)
	{
		int i = 0;
    	while(frame->children[i])
		{
			destroy_ui(frame->children[i]);
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
		data->key_dir = 0;
		set_camera(&data->camera, 0,0,1);
		
		frame->data = data;
		frame->draw = &draw_game;
		frame->on_key = &ui_game_on_key;
		frame->update = &ui_game_update;
		frame->on_mouse_move = &ui_game_on_mouse_move;
		frame->on_mouse_button = &ui_game_on_mouse_button;
		frame->destroy = &destroy_ui_game;
		frame->on_char = &ui_game_on_char;
		frame->on_mouse_scroll = &ui_game_on_mouse_scroll;

		frame->children = malloc(sizeof(ui_frame_t *)*(children_count+1));
		if(!frame->children)
			goto err_children;
		
		frame->children[0] = init_ui_console(frame, 10, config.screen_height - 215, 400, 200);
		frame->children[1] = init_ui_debug(frame, world,  5, 5, 125, 250);
		frame->children[2] = NULL;
		rtt_init();
		return frame;

	err_children:
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
	if (action == GLFW_REPEAT)
		return;
	
	if (key == GLFW_KEY_UP)
	{
		if(action == GLFW_PRESS)
			data->camy += 1;
		else if(action == GLFW_RELEASE && data->was_focused)
			data->camy -= 1;
		data->was_focused = 1;
	}

	if (key == GLFW_KEY_DOWN)
	{
		if(action == GLFW_PRESS)
			data->camy -= 1;
		else if(action == GLFW_RELEASE && data->was_focused)
			data->camy += 1;
		data->was_focused = 1;
	}

	if (key == GLFW_KEY_RIGHT)
	{
		if(action == GLFW_PRESS)
			data->camx += 1;
		else if(action == GLFW_RELEASE && data->was_focused)
			data->camx -= 1;
		data->was_focused = 1;
	}

	if (key == GLFW_KEY_LEFT)
	{
		if(action == GLFW_PRESS)
			data->camx -= 1;
		else if(action == GLFW_RELEASE && data->was_focused)
			data->camx += 1;
		data->was_focused = 1;
	}
	

}
