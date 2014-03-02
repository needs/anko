#ifndef _RENDER_H_
#define _RENDER_H_

#include <SDL2/SDL.h>
#include "../simulator.h"

void render(SDL_Renderer *renderer, SDL_Texture **tiles, state_t **board, int width, int height);

#endif /* _RENDER_H_ */
