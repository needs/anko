#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>

#include "textures.h"
#include "map.h"
#include "shader.h"
#include "renderer.h"
#include "../board.h"


typedef struct mapcell_t {
	int x, y;
} mapcell_t;

typedef struct map_t {
	GLuint vbo_floor, vao_floor;
	GLuint vbo_entity, vao_entity;

	int width, height;
	mapcell_t **cells;
} map_t;


static void create_vao(GLuint *vao, GLuint *vbo);
static void seed_map(map_t *map, board_t *board);
static map_t* alloc_map(int width, int height);
static tex_t get_floor_tex(cell_t *c);
static tex_t get_entity_tex(cell_t *c);


map_t* create_map(board_t *board)
{
	map_t *map;

	assert(board != NULL);

	if ((map = alloc_map(board->width, board->height)) == NULL)
		return NULL;
	seed_map(map, board);

	return map;
}


void free_map(map_t *map)
{
	int i;

	assert(map != NULL);

	for (i = 0; i < map->height; i++)
		free(map->cells[i]);
	glDeleteBuffers(1, &map->vbo_floor);
	glDeleteBuffers(1, &map->vbo_entity);
	glDeleteVertexArrays(1, &map->vao_floor);
	glDeleteVertexArrays(1, &map->vao_entity);
	free(map->cells);
	free(map);
}


static map_t* alloc_map(int width, int height)
{
	int i;
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
	}

	map->width  = width;
	map->height = height;

	/* Create vbo, vao, and shader attrib */
	create_vao(&map->vao_floor, &map->vbo_floor);
	create_vao(&map->vao_entity, &map->vbo_entity);

	glBindBuffer(GL_ARRAY_BUFFER, map->vbo_floor);
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float) * map->width * map->height, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, map->vbo_entity);
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float) * map->width * map->height, NULL, GL_DYNAMIC_DRAW);

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


static void create_vao(GLuint *vao, GLuint *vbo)
{
	assert(vbo != NULL);
	assert(vao != NULL);

	glGenVertexArrays(1, vao);
	glBindVertexArray(*vao);

	glGenBuffers(1, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);

	GLint position = glGetAttribLocation(standard, "position");
	glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
	glEnableVertexAttribArray(position);

	GLint uv = glGetAttribLocation(standard, "UV");
	glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(uv);

	glBindVertexArray(0);
}


/* Introduit de la variation dans les textures
 * Précalcule les coordonnées des cases. */
static void seed_map(map_t *map, board_t *board)
{
	int i, j;
	float *buf;

	assert(map != NULL);

	/* VBOs are mapped to avoid a lot of call to glBufferSubData */

	glBindBuffer(GL_ARRAY_BUFFER, map->vbo_floor);
	buf = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	for (i = 0; i < map->height; i++) {
		for (j = 0; j < map->width; j++) {
			map->cells[i][j].x = j*-TILE_WIDTH/2 + i*TILE_WIDTH/2;
			map->cells[i][j].y = i*TILE_HEIGHT/2 + j*TILE_HEIGHT/2;
			get_ctexture(buf + ((i * map->height + j) * 16),
				     get_floor_tex(&board->cells[i][j]),
				     map->cells[i][j].x,
				     map->cells[i][j].y);
		}
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, map->vbo_entity);
	buf = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	for (i = 0; i < map->height; i++) {
		for (j = 0; j < map->width; j++) {
			get_ctexture(buf + ((i * map->height + j) * 16),
				     get_entity_tex(&board->cells[i][j]),
				     map->cells[i][j].x,
				     map->cells[i][j].y);
		}
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
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


void render_map(map_t *map)
{
	mat4x4 identity;

	assert(map != NULL);
	glUseProgram(standard);
	
	mat4x4_identity(identity);

	glBindVertexArray(map->vao_floor);
	glBindTexture(GL_TEXTURE_2D, get_texid(TEX_TILES));
	render_model(identity, 0, map->height * map->width * 4);

	glBindVertexArray(map->vao_entity);
	glBindTexture(GL_TEXTURE_2D, get_texid(TEX_ENTITIES));
	render_model(identity, 0, map->height * map->width * 4);
}


void update_map(map_t *map, board_t *current, board_t *old)
{
	int i, j;

	assert(map != NULL);
	assert(current != NULL);
	assert(old != NULL);
	assert(old->width == current->width);
	assert(old->height == current->height);

	/* Only entities may change */
	glBindBuffer(GL_ARRAY_BUFFER, map->vbo_entity);
	for (i = 0; i < map->height; i++) {
		for (j = 0; j < map->width; j++) {
			if (!cmp_cell(&current->cells[i][j], &old->cells[i][j])) {
				float data[16];
				get_ctexture(data,
					     get_entity_tex(&current->cells[i][j]),
					     map->cells[i][j].x,
					     map->cells[i][j].y);
				glBufferSubData(GL_ARRAY_BUFFER,
						(i * map->height + j) * sizeof(data),
						sizeof(data), data);
			}
		}
	}
}
