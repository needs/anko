#ifndef _GENERATOR_H_
#define _GENERATOR_H_

#include <game/board.h>

typedef struct gen_params
{
	float tree_density;
	float water_density;
	float water_shatter_factor;
} gen_params_t;

board_t* generate(int width, int height, gen_params_t *params);

#endif /* _GENERATOR_H_ */
