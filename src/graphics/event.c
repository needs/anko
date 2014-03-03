#include <SDL2/SDL.h>
#include "context.h"
#include "event.h"

void process_events(void)
{
	SDL_Event e;

	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT)
			quit = 1;
		if (e.type == SDL_KEYDOWN)
			quit = 1;
		if (e.type == SDL_MOUSEBUTTONDOWN)
			quit = 1;
	}
}
