#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

typedef enum state_t {
	NEUTRAL = 0,
	BURNABLE,
	BURNING,
	BURNED,
	NB_STATE,
} state_t;

#endif /* _SIMULATOR_H_ */
