#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include "generator.h"
#include "board.h"

/*
  Fire spread simulation using cellular automata
  
  Nb : number of burning neightbours
  Transition rule :

  if Nb < 1 then
      BURNABLE -> BURNABLE
  else
      BURNABLE -> BURNING (with probability p)
	  NEUTRAL  -> NEUTRAL
	  BURNABLE -> BURNED (add burning time ?)
	  BURNED   -> BURNED
	  
  p(Nb) = Nb/8
*/

void step(board_t* dest, board_t* src);
void simulate();
int init_simulator(board_t **dest, int width, int height, gen_params_t params);
void terminate_simulator();

#endif /* _SIMULATOR_H_ */
