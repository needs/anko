#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

/*
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

typedef enum state_t {
	NEUTRAL = 0,
	BURNABLE,
	BURNING,
	BURNED,
	NB_STATE,
} state_t;

void step(state_t** dest, state_t** src, int width, int height);

#endif /* _SIMULATOR_H_ */
