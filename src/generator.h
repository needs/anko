#ifndef _GENERATOR_H_
#define _GENERATOR_H_

#include "simulator.h"

state_t **alloc_board(int width, int height);

state_t** generate(int width, int height);

#endif /* _GENERATOR_H_ */
