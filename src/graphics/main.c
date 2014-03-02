#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>

int main(void)
{
    SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("Anko",
										  SDL_WINDOWPOS_UNDEFINED, // x
										  SDL_WINDOWPOS_UNDEFINED, // y
										  1000, // Width
										  800, // Height
										  SDL_WINDOW_SHOWN);

	if(!window)
	{
		fprintf(stderr, "Couldn't create window: %s\n", SDL_GetError());
		SDL_Quit();
		return EXIT_FAILURE;
	}

	

	SDL_Delay(3000); // wait 3s
   
	
	SDL_DestroyWindow(window);
	SDL_Quit();
	return EXIT_SUCCESS;
}
