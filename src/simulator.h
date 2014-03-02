#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

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

void step(state_t** dest, state_t** src, int width, int height);

#endif /* _SIMULATOR_H_ */
