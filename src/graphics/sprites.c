#include <assert.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "sprites.h"


/* Trick to  get the tile path, used by load_sprites */
#define ADD_SPRITE(type, path) "./data/"path,
static const char sprites_path[][64] = {
#include "sprites.def"
};
#undef ADD_SPRITE


SDL_Texture** load_sprites(SDL_Renderer *renderer)
{
	SDL_Texture** sprites;
	int i;

	assert(renderer != NULL);

	if ((sprites = malloc(SP_COUNT * sizeof(*sprites))) == NULL) {
		perror("malloc(sprites)");
		goto err_sprites;
	}

	for (i = 0; i < SP_COUNT; i++) {
		SDL_Surface *surf = IMG_Load(sprites_path[i]);
		if (surf == NULL) {
			fprintf(stderr, "%s\n", SDL_GetError());
			goto err_tile;
		}
		sprites[i] = SDL_CreateTextureFromSurface(renderer, surf);
		SDL_FreeSurface(surf);
	}

	return sprites;

err_tile:
	while (i --> 0)
		SDL_DestroyTexture(sprites[i]);
	free(sprites);
err_sprites:
	return NULL;
}


void unload_sprites(SDL_Texture **sprites)
{
	int i;
	assert(sprites != NULL);
	for (i = 0; i < SP_COUNT; i++)
		SDL_DestroyTexture(sprites[i]);
	free(sprites);
}

