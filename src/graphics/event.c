#include <SDL2/SDL.h>
#include "context.h"
#include "event.h"

void process_events(void)
{
	SDL_Event e;

	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT)
			quit = 1;
		if (e.type == SDL_MOUSEBUTTONDOWN)
			quit = 1;
		if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {
			case SDLK_DOWN:  camera.y -= 5; break;
			case SDLK_UP:    camera.y += 5; break;
			case SDLK_RIGHT: camera.x -= 5; break;
			case SDLK_LEFT:  camera.x += 5; break;
			default: quit = 1;
			}
		}
	}
}
