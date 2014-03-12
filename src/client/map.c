#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>

#include <client/textures.h>
#include <client/map.h>
#include <client/shader.h>
#include <client/context.h>
#include <game/board.h>

static void create_vao(GLuint *vao, GLuint *vbo);
static int seed_map(map_t *map, board_t *board);
static map_t* alloc_map(int width, int height);
static tex_t get_floor_tex(cell_t *c);
static tex_t get_entity_tex(cell_t *c);


map_t* create_map(board_t *board)
{
	map_t *map;

	assert(board != NULL);

	if ((map = alloc_map(board->width, board->height)) == NULL)
		return NULL;
	if (!seed_map(map, board))
		return NULL;

	memcpy(&map->board_stats, &board->stats, sizeof(board_stats_t));
	
	return map;
}


void free_map(map_t *map)
{
	int i;

	assert(map != NULL);

	for (i = 0; i < map->height; i++)
		free(map->cells[i]);
	glDeleteBuffers(1, &map->vbo);
	glDeleteVertexArrays(1, &map->vao);
	free(map->cells);
	free(map);
}


/* Allocate needed memory and initialise width, height, x, and y. */
static map_t* alloc_map(int width, int height)
{
	int i, j;
	map_t *map;

	assert(width > 0);
	assert(height > 0);

	if ((map = malloc(sizeof(*map))) == NULL) {
		perror("malloc(map)");
		goto err_map;
	}

	if ((map->cells = malloc(height * sizeof(*map->cells))) == NULL) {
		perror("malloc(map->cells)");
		goto err_height;
	}

	for (i = 0; i < height; i++) {
		if ((map->cells[i] = malloc(width * sizeof(**map->cells))) == NULL) {
			perror("malloc(map->cells[i])");
			goto err_width;
		}

		for (j = 0; j < width; j++) {
			map->cells[i][j].x = i*-TILE_WIDTH/2 + j*TILE_WIDTH/2;
			map->cells[i][j].y = i*TILE_HEIGHT/2 + j*TILE_HEIGHT/2;
			map->cells[i][j].z = i+j+1;
		}
	}

	map->width  = width;
	map->height = height;
	map->vsize = TEXTURE_VERTEX_SIZE * map->width * map->height;

	create_vao(&map->vao, &map->vbo);

	return map;

err_width:
	while (i --> 0)
		free(map->cells[i]);
	free(map->cells);
err_height:
	free(map);
err_map:
	return NULL;
}


/* Create VAO and her associated VBO, the VBO is not allocated. */
static void create_vao(GLuint *vao, GLuint *vbo)
{
	assert(vbo != NULL);
	assert(vao != NULL);

	glGenVertexArrays(1, vao);
	glBindVertexArray(*vao);

	glGenBuffers(1, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);

	GLint position = glGetAttribLocation(standard, "position");
	glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, TEXTURE_VERTEX_SIZE, 0);
	glEnableVertexAttribArray(position);

	GLint uv = glGetAttribLocation(standard, "UV");
	glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE, TEXTURE_VERTEX_SIZE, (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(uv);

	glBindVertexArray(0);
}


/* Introduce variations for map tiles/entities.
 * Also used to precomputes coordinate of each tiles/entities. */
static int seed_map(map_t *map, board_t *board)
{
	int i, j;
	float *buf;

	assert(map != NULL);
	assert(board != NULL);
	assert(board->width  == map->width);
	assert(board->height == map->height);

	/* VBO is mapped to avoid a lot of call to glBufferSubData */
	/* Note: we loop until glUnmapBuffer works, we do that because normally
	 * the only error case is when the buffer gets corrupted, which is
	 * pretty rare. */

	glBindBuffer(GL_ARRAY_BUFFER, map->vbo);
	do {
		glBufferData(GL_ARRAY_BUFFER, 2 * map->vsize * sizeof(float), NULL, GL_DYNAMIC_DRAW);
		if ((buf = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)) == NULL) {
			fprintf(stderr, "seeding map: Can't map the VBO.\n");
			return 0;
		}
		for (i = 0; i < map->height; i++) {
			for (j = 0; j < map->width; j++) {
				get_ctexture(buf + ((i * map->width + j) * TEXTURE_VERTEX_SIZE),
					     get_floor_tex(&board->cells[i][j]),
					     map->cells[i][j].x,
					     map->cells[i][j].y,
					     0);
				get_ctexture(buf + map->vsize + ((i * map->width + j) * TEXTURE_VERTEX_SIZE),
					     get_entity_tex(&board->cells[i][j]),
					     map->cells[i][j].x,
					     map->cells[i][j].y,
					     map->cells[i][j].z);
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER, map->vbo);
	} while (glUnmapBuffer(GL_ARRAY_BUFFER) == GL_FALSE);

	return 1;
}


static tex_t get_floor_tex(cell_t *c)
{
	assert(c != NULL);

	if (c->type == CT_WATER)
		return TEX_TILES_WATER;
	else
		return TEX_TILES_GRASS;
}


static tex_t get_entity_tex(cell_t *c)
{
	assert(c != NULL);

	static tex_t tree[TS_TOTAL] = {
		TEX_ENTITIES_TREE,
		TEX_ENTITIES_TREE2,
		TEX_ENTITIES_TREE3,
		TEX_ENTITIES_TREE,
	};

	static tex_t burning[TS_TOTAL] = {
		TEX_ENTITIES_BURNING_TREE,
		TEX_ENTITIES_BURNING_TREE2,
		TEX_ENTITIES_BURNING_TREE3,
		TEX_ENTITIES_BURNING_TREE,
	};

	static tex_t burned[TS_TOTAL] = {
		TEX_ENTITIES_BURNED_TREE,
		TEX_ENTITIES_BURNED_TREE2,
		TEX_ENTITIES_BURNED_TREE3,
		TEX_ENTITIES_BURNED_TREE,
	};

	switch (c->type) {
	case CT_TREE:
		if (c->data.tree.life == 100)
			return tree[c->data.tree.specie];
		else if (c->data.tree.life)
			return burning[c->data.tree.specie];
		else
			return burned[c->data.tree.specie];
	default:
		return TEX_NONE;
	}
}


void render_map(map_t *map, camera_t *camera)
{
	assert(map != NULL);

	glBindVertexArray(map->vao);
	glBindBuffer(GL_ARRAY_BUFFER, map->vbo);
	glActiveTexture(GL_TEXTURE0);

	glUseProgram(standard);
	glUniform1i(glGetUniformLocation(standard, "tex"), 0);
	glUniformMatrix4fv(glGetUniformLocation(standard, "projection"),
			   1, GL_FALSE, (GLfloat*)projection);
	glUniformMatrix4fv(glGetUniformLocation(standard, "view"),
			   1, GL_FALSE, (GLfloat*)camera->matrix);

	glBindTexture(GL_TEXTURE_2D, get_texid(TEX_TILES));
	glDrawArrays(GL_QUADS, 0, map->vsize / TEXTURE_VERTEX_LEN);

	glBindTexture(GL_TEXTURE_2D, get_texid(TEX_ENTITIES));
	glDrawArrays(GL_QUADS, map->vsize / TEXTURE_VERTEX_LEN, map->vsize / TEXTURE_VERTEX_LEN);
}


void update_map(map_t *map, partgen_t *gen, board_t *current, board_t *old)
{
	int i, j;
	float *buf;

	assert(map != NULL);
	assert(gen != NULL);
	assert(current != NULL);
	assert(old != NULL);
	assert(old->width  == current->width &&  old->width  == map->width);
	assert(old->height == current->height && old->height == map->height);

	memcpy(&map->board_stats, &current->stats, sizeof(board_stats_t));
	
	/* Only entities may change */
	glBindBuffer(GL_ARRAY_BUFFER, map->vbo);
	buf = glMapBufferRange(GL_ARRAY_BUFFER, map->vsize * sizeof(float), map->vsize * sizeof(float), GL_MAP_WRITE_BIT);
	for (i = 0; i < map->height; i++) {
		for (j = 0; j < map->width; j++) {
			if (!cmp_cell(&current->cells[i][j], &old->cells[i][j])) {
				get_ctexture(buf + ((i * map->width + j) * TEXTURE_VERTEX_SIZE),
					     get_entity_tex(&current->cells[i][j]),
					     map->cells[i][j].x,
					     map->cells[i][j].y,
					     map->cells[i][j].z);
			}

			/* Spawn some particles when a tree is burning */
			if (current->cells[i][j].type == CT_TREE &&
			    current->cells[i][j].data.tree.life < 100 && current->cells[i][j].data.tree.life > 0) {
				struct partargs_t prop = PARTARGS_DEFAULT;
				prop.lifetime = 1.0;
				prop.tex = TEX_PARTICLES_FIRE1;
				prop.box.start.x = 1.0;
				prop.box.start.y = 1.0;
				prop.box.end.x = 4.0;
				prop.box.end.y = 4.0;
				prop.dir.y = -50.0;
				prop.spawn_period = 1.0;
				prop.spawn_box.x = 20.0;
				prop.spawn_box.y = -70.0;

				spawn_particles(gen, 10, map->cells[i][j].x - 10, map->cells[i][j].y, map->cells[i][j].z+1, &prop);
			}
		}
	}
	
	/* Rebind the buffer because other function might bind something else
	 * to GL_BUFFER_ARRAY */
	glBindBuffer(GL_ARRAY_BUFFER, map->vbo);
	glUnmapBuffer(GL_ARRAY_BUFFER);
}


void get_map_coord(float x, float y, float *rx, float *ry, float *rz)
{
	assert(rx != NULL);
	assert(ry != NULL);
	assert(rz != NULL);

	x -= 1.0; y -= 1.0;
	*rx = y*-TILE_WIDTH/2 + x*TILE_WIDTH/2;
	*ry = y*TILE_HEIGHT/2 + x*TILE_HEIGHT/2;
	*rz = (int)x + (int)y + 1;
}
