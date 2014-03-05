#ifndef _BOARD_H_
#define _BOARD_H_

typedef enum cell_type_t
{
	CT_GRASS = 0,
	CT_WATER,
	CT_ROCK,
	CT_TREE,
	CT_TOTAL,
} cell_type_t;

enum tree_specie_t
{
	TS_OAK,
	TS_APPLE,
	TS_BIRCH,
	TS_PINE
};

typedef struct tree_data_t
{
	enum tree_specie_t specie;
	int life;
	//int age;
} tree_data_t;

typedef struct cell_t
{
	enum cell_type_t type;
	union {
		struct tree_data_t tree;
	} data;
} cell_t;

typedef struct
{
	cell_t** cells;
	int width;
	int height;
} board_t;

void free_board(board_t *board);
board_t* alloc_board(int width, int height);
int get_neighbors_count(int x, int y, board_t* board, cell_type_t type, int(*predicate)(void*));

#define IS_OUT_OF_BOUNDS(b,x,y) (x < 0 || x >= b->width || y < 0 || y >= b->height)

#endif /* _BOARD_H_ */
