#include <assert.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>

#include <client/players.h>
#include <client/textures.h>
#include <client/context.h>
#include <client/camera.h>
#include <client/map.h>
#include <client/shader.h>
#include <game/game.h>


/* No need to have a player_rendering structure because there is
 * absolutely no context when rendering players, or he can be passed
 * through function parameters. */
static GLuint vao, vbo;


static tex_t get_player_tex(int team, int dir);


void init_players_rendering(void)
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	GLint position = glGetAttribLocation(standard, "position");
	glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, TEXTURE_VERTEX_SIZE, 0);
	glEnableVertexAttribArray(position);

	GLint uv = glGetAttribLocation(standard, "UV");
	glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE, TEXTURE_VERTEX_SIZE, (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(uv);

	glBufferData(GL_ARRAY_BUFFER, TEXTURE_VERTEX_SIZE * TEXTURE_VERTEX_NB * MAX_PLAYERS, NULL, GL_DYNAMIC_DRAW);

	glBindVertexArray(0);
}


void render_players(player_t *players, int player_count, camera_t *camera)
{
	int i, count = 0;
	float vertices[TEXTURE_VERTEX_LEN * TEXTURE_VERTEX_NB * MAX_PLAYERS];

	assert(players != NULL);
	assert(camera  != NULL);

	if (player_count == 0)
		return;

	for (i = 0; i < MAX_PLAYERS; i++) {
		if (players->is_used && players->team != TEAM_NONE) {
			float x, y, z;
			get_map_coord(players->x, players->y, &x, &y, &z);
			get_ctexture(vertices + (count * TEXTURE_VERTEX_LEN * TEXTURE_VERTEX_NB),
				     get_player_tex(players->team, players->dir),
				     x, y, z+1);
			count++;
		}
		players++;
	}

	assert(count == player_count);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, count * TEXTURE_VERTEX_LEN * TEXTURE_VERTEX_NB * sizeof(float), vertices);

	glActiveTexture(GL_TEXTURE0);

	glUseProgram(standard);
	glUniform1i(glGetUniformLocation(standard, "tex"), 0);
	glUniformMatrix4fv(glGetUniformLocation(standard, "projection"),
			   1, GL_FALSE, (GLfloat*)projection);
	glUniformMatrix4fv(glGetUniformLocation(standard, "view"),
			   1, GL_FALSE, (GLfloat*)camera->matrix);

	glBindTexture(GL_TEXTURE_2D, get_texid(TEX_CHARS));
	glDrawArrays(GL_QUADS, 0, count * TEXTURE_VERTEX_NB);
}


void terminate_players_rendering(void)
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}


static tex_t get_player_tex(int team, int dir)
{
	if (team == TEAM_NONE)
		return TEX_NONE;

	if ((dir & DIR_UP) == dir)
		return team == TEAM_ARBRIST ? TEX_CHARS_ARBRIST_N : TEX_CHARS_BURNER_N;
	if ((dir & DIR_DOWN) == dir)
		return team == TEAM_ARBRIST ? TEX_CHARS_ARBRIST_S : TEX_CHARS_BURNER_S;
	if ((dir & DIR_LEFT) == dir)
		return team == TEAM_ARBRIST ? TEX_CHARS_ARBRIST_W : TEX_CHARS_BURNER_W;
	if ((dir & DIR_RIGHT) == dir)
		return team == TEAM_ARBRIST ? TEX_CHARS_ARBRIST_E : TEX_CHARS_BURNER_E;
	if (dir & DIR_UP && dir & DIR_RIGHT)
		return team == TEAM_ARBRIST ? TEX_CHARS_ARBRIST_NE : TEX_CHARS_BURNER_NE;
	if (dir & DIR_UP && dir & DIR_LEFT)
		return team == TEAM_ARBRIST ? TEX_CHARS_ARBRIST_NW : TEX_CHARS_BURNER_NW;
	if (dir & DIR_DOWN && dir & DIR_RIGHT)
		return team == TEAM_ARBRIST ? TEX_CHARS_ARBRIST_SE : TEX_CHARS_BURNER_SE;
	return team == TEAM_ARBRIST ? TEX_CHARS_ARBRIST_SW : TEX_CHARS_BURNER_SW;
}
