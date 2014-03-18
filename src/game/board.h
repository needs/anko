#ifndef _BOARD_H_
#define _BOARD_H_


#define IS_OUT_OF_BOUNDS(b ,x , y) ((x) < 0 || (x) >= (b)->width || (y) < 0 || (y) >= (b)->height)


typedef enum cell_type_t
{
	CT_GRASS = 0,
	CT_WATER,
	CT_ROCK,
	CT_TREE,
	CT_TOTAL
} cell_type_t;

enum tree_specie_t
{
	TS_OAK,
	TS_APPLE,
	TS_BIRCH,
	TS_PINE,
	TS_TOTAL
};

typedef struct tree_data_t
{
	enum tree_specie_t specie;
	float life;
} tree_data_t;

typedef struct cell_t
{
	enum cell_type_t type;
	union {
		struct tree_data_t tree;
	} data;
} cell_t;

typedef struct board_stats_t
{
	int total_tree;
	int burning_tree;
	int burned_tree;
	int total_water;
} board_stats_t;

typedef struct board_t
{
	cell_t **cells;
	int width;
	int height;
	board_stats_t stats;
} board_t;


board_t* alloc_board(int width, int height);
void free_board(board_t *board);
int get_neighbors_count(int x, int y, board_t* board, cell_type_t type, int(*predicate)(void*));

/* Return 1 when equals, 0 when different. */
int cmp_cell(cell_t *c1, cell_t *c2);


#endif /* _BOARD_H_ */
