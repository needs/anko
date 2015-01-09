#ifndef _MAP_H_
#define _MAP_H_

#include <client/linmath.h>
#include <game/board.h>
#include <client/camera.h>
#include <client/particles.h>


#define TILE_WIDTH  68
#define TILE_HEIGHT 34


typedef struct mapcell_t {
	float x, y, z;
} mapcell_t;

typedef struct map_t {
	GLuint vbo, vao;
	long vsize;		/* Size for floor/entity VBO */

	int width, height;
	mapcell_t **cells;
	board_stats_t board_stats;
} map_t;

/* Allocate the map based on the generated board. 0 on error. */
int create_map(map_t *map, board_t *board);
void free_map(map_t *map);

/* Change the map based on the differences between old and current. */
void update_map(map_t *map, partgen_t *gen, board_t *current, board_t *old);

/* Just draw the map, nothing is computed here. */
void render_map(map_t *map, camera_t *camera);

/* Compute the coordinate on a map. */
void get_map_coord(float x, float y, float *rx, float *ry, float *rz);

#endif /* _MAP_H_ */
