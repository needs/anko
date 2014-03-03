#include <SDL2/SDL.h>
#include "context.h"
#include "event.h"

#define MOUSE_SCROLL_ZONE 60

enum {
	DIR_LEFT  = 1,
	DIR_RIGHT = 2,
	DIR_UP    = 4,
	DIR_DOWN  = 8,
};

static const int CAMERA_SPEED = 500;
static const float ZOOM_SPEED = 1;
static const float ZOOM_MIN = 0.3;

static void scrolling(unsigned dir, float deltatime);

void process_events(float deltatime)
{
	SDL_Event e;
	int x, y;

	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT)
			quit = 1;
		else if (e.type == SDL_KEYUP) {
			if (e.key.keysym.sym == SDLK_DOWN)
				scroll_dir &= ~DIR_DOWN;
			else if (e.key.keysym.sym == SDLK_UP)
				scroll_dir &= ~DIR_UP;
			else if (e.key.keysym.sym == SDLK_RIGHT)
				scroll_dir &= ~DIR_RIGHT;
			else if (e.key.keysym.sym == SDLK_LEFT)
				scroll_dir &= ~DIR_LEFT;
		}

		else if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_DOWN)
				scroll_dir |= DIR_DOWN;
			else if (e.key.keysym.sym == SDLK_UP)
				scroll_dir |= DIR_UP;
			else if (e.key.keysym.sym == SDLK_RIGHT)
				scroll_dir |= DIR_RIGHT;
			else if (e.key.keysym.sym == SDLK_LEFT)
				scroll_dir |= DIR_LEFT;
			else if (e.key.keysym.sym == SDLK_ESCAPE)
				quit = 1;
			else if (e.key.keysym.sym == SDLK_q && e.key.keysym.mod == KMOD_LCTRL)
				quit = 1;
		}
		else if(e.type == SDL_MOUSEWHEEL)
		{
			scale += (float)e.wheel.y*deltatime*ZOOM_SPEED;
			if(scale > 1) scale = 1;
			else if(scale < ZOOM_MIN) scale = ZOOM_MIN;
		}
	}

	SDL_GetMouseState(&x, &y);
	if (x < MOUSE_SCROLL_ZONE)
		scrolling(DIR_LEFT, deltatime);
	if (x > camera.w - MOUSE_SCROLL_ZONE)
		scrolling(DIR_RIGHT, deltatime);
	if (y < MOUSE_SCROLL_ZONE)
		scrolling(DIR_UP, deltatime);
	if (y > camera.h - MOUSE_SCROLL_ZONE)
		scrolling(DIR_DOWN,deltatime);

	scrolling(scroll_dir, deltatime);
}

// Cannot move int value efficiently because of fucking integer
static void scrolling(unsigned dir, float deltatime)
{
	/* Update position of the camera */
	if (dir & DIR_DOWN)
		camera.y -= CAMERA_SPEED * deltatime;
	if (dir & DIR_UP)
		camera.y += CAMERA_SPEED * deltatime;
	if (dir & DIR_RIGHT)
		camera.x -= CAMERA_SPEED * deltatime;
	if (dir & DIR_LEFT)
		camera.x += CAMERA_SPEED * deltatime;
}
