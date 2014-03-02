#ifndef _BOARD_H_
#define _BOARD_H_

typedef enum state_t {
	ST_GRASS = 0,
	ST_WATER,
	ST_ROCK,
	ST_BURNABLE,
	ST_BURNING,
	ST_BURNED,
	ST_TOTAL,
} state_t;

void free_board(state_t **board, int width, int height);
state_t** alloc_board(int width, int height);
int get_neighbors_count(int x, int y, state_t** board, int width, int height, state_t type);

#define IS_OUT_OF_BOUNDS(x,y,w,h) (x < 0 || x >= width || y < 0 || y >= height)

#endif /* _BOARD_H_ */
