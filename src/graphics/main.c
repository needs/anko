#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "context.h"
#include "event.h"
#include "render.h"
#include "../board.h"
#include "../generator.h"
#include "../simulator.h"


static const int FPS = 60;
static const int WIDTH  = 20;
static const int HEIGHT = 20;
static const int STEP_TIMER = 1000;

int main(void)
{
	board_t *board, *dest, *tmp;
	unsigned frames = 0;
	unsigned last_fps = 0;
	unsigned last_time = 0;
	unsigned deltatime = 0;
	unsigned step_current_timer = STEP_TIMER;
	
	srandom(time(NULL));
	
	if (!init_context())
		goto err_context;

	if ((board = generate(WIDTH, HEIGHT, 0.7, 0.1, 0.4)) == NULL)
		goto err_board;
	if ((dest = alloc_board(WIDTH, HEIGHT)) == NULL)
		goto err_dest;

	while(!quit) {
		last_time = SDL_GetTicks();

		process_events((float)deltatime/1000);
		if(step_current_timer < deltatime)
		{
			step_current_timer = STEP_TIMER - step_current_timer;
			step(dest, board);
			tmp = board;
			board = dest;
			dest = tmp;
		}
		else
			step_current_timer -= deltatime;

		if(SDL_GetTicks()- last_fps >= 1000)
		{
			last_fps = SDL_GetTicks();
			printf("FPS : %i\n", frames);
			frames = 0;
		}
		else
			frames++;

		render(board);

		deltatime = SDL_GetTicks() - last_time;
		
		if(deltatime < (unsigned)1000/FPS)
			SDL_Delay(1000/FPS - deltatime);
	}
		
	close_context();

	return EXIT_SUCCESS;

err_dest:
	free_board(board);
err_board:
	close_context();
err_context:
	return EXIT_FAILURE;
}
