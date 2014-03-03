#ifndef _GENERATOR_H_
#define _GENERATOR_H_

#include "simulator.h"
#include "board.h"

board_t* generate(int width, int height, float tree_density, float water_density);

#endif /* _GENERATOR_H_ */
