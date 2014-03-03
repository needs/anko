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


static void scrolling(unsigned dir);


void process_events(void)
{
	SDL_Event e;
	int x, y;

	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT)
			quit = 1;
		if (e.type == SDL_MOUSEBUTTONDOWN)
			quit = 1;

		if (e.type == SDL_KEYUP) {
			if (e.key.keysym.sym == SDLK_DOWN)
				scroll_dir &= ~DIR_DOWN;
			if (e.key.keysym.sym == SDLK_UP)
				scroll_dir &= ~DIR_UP;
			if (e.key.keysym.sym == SDLK_RIGHT)
				scroll_dir &= ~DIR_RIGHT;
			if (e.key.keysym.sym == SDLK_LEFT)
				scroll_dir &= ~DIR_LEFT;
		}

		if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_DOWN)
				scroll_dir |= DIR_DOWN;
			if (e.key.keysym.sym == SDLK_UP)
				scroll_dir |= DIR_UP;
			if (e.key.keysym.sym == SDLK_RIGHT)
				scroll_dir |= DIR_RIGHT;
			if (e.key.keysym.sym == SDLK_LEFT)
				scroll_dir |= DIR_LEFT;
		}
	}

	SDL_GetMouseState(&x, &y);
	if (x < MOUSE_SCROLL_ZONE)
		scrolling(DIR_LEFT);
	if (x > camera.w - MOUSE_SCROLL_ZONE)
		scrolling(DIR_RIGHT);
	if (y < MOUSE_SCROLL_ZONE)
		scrolling(DIR_UP);
	if (y > camera.h - MOUSE_SCROLL_ZONE)
		scrolling(DIR_DOWN);

	scrolling(scroll_dir);
}


static void scrolling(unsigned dir)
{
	/* Update position of the camera */
	if (dir & DIR_DOWN)
		camera.y -= 5;
	if (dir & DIR_UP)
		camera.y += 5;
	if (dir & DIR_RIGHT)
		camera.x -= 5;
	if (dir & DIR_LEFT)
		camera.x += 5;
}
