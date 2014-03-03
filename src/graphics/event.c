#include <SDL2/SDL.h>
#include "context.h"
#include "event.h"

enum {
	DIR_LEFT  = 1,
	DIR_RIGHT = 2,
	DIR_UP    = 4,
	DIR_DOWN  = 8,
};

void process_events(void)
{
	SDL_Event e;

	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT)
			quit = 1;
		if (e.type == SDL_MOUSEBUTTONDOWN)
			quit = 1;
		if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_DOWN)
				camera_dir |= DIR_DOWN;
			if (e.key.keysym.sym == SDLK_UP)
				camera_dir |= DIR_UP;
			if (e.key.keysym.sym == SDLK_RIGHT)
				camera_dir |= DIR_RIGHT;
			if (e.key.keysym.sym == SDLK_LEFT)
				camera_dir |= DIR_LEFT;
		}

		if (e.type == SDL_KEYUP) {
			if (e.key.keysym.sym == SDLK_DOWN)
				camera_dir &= ~DIR_DOWN;
			if (e.key.keysym.sym == SDLK_UP)
				camera_dir &= ~DIR_UP;
			if (e.key.keysym.sym == SDLK_RIGHT)
				camera_dir &= ~DIR_RIGHT;
			if (e.key.keysym.sym == SDLK_LEFT)
				camera_dir &= ~DIR_LEFT;
		}
	}

	/* Update position of the camera */
	if (camera_dir & DIR_DOWN)
		camera.y -= 5;
	if (camera_dir & DIR_UP)
		camera.y += 5;
	if (camera_dir & DIR_RIGHT)
		camera.x -= 5;
	if (camera_dir & DIR_LEFT)
		camera.x += 5;
}
