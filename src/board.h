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

typedef struct {
	state_t** cells;
	int width;
	int height;
} board_t;

void free_board(board_t *board);
board_t* alloc_board(int width, int height);
int get_neighbors_count(int x, int y, board_t* board, state_t type);

#define IS_OUT_OF_BOUNDS(b,x,y) (x < 0 || x >= b->width || y < 0 || y >= b->height)

#endif /* _BOARD_H_ */
