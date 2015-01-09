#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include <client/ui/ui_debug.h>
#include <client/shader.h>
#include <client/font.h>
#include <client/linmath.h>
#include <client/config.h>
#include <client/context.h>
#include <client/world.h>
#include <client/map.h>
#include <stdlib.h>
#include <string.h>

#define VERTEX_DATA_SIZE 32

static float color[] = {0.05, 0.05, 0.05, 0.5};

typedef struct ui_debug_data_t
{
	GLuint vao;
	GLuint vbo;
	world_t *world;
} ui_debug_data_t;


void destroy_ui_debug(ui_frame_t *frame)
{
	free(frame->data);
	free(frame);
}

void draw_debug(ui_frame_t *frame)
{
	ui_debug_data_t *data = frame->data;

	wchar_t buf[255];
	float th;
	float ch=0;
    float total = data->world->map.width*data->world->map.height;
	glUseProgram(gui);
	glBindVertexArray(data->vao);

	glUniform1i(glGetUniformLocation(gui, "has_texture"),0);

	glDrawArrays(GL_QUADS, 0, 4);

	set_font_color(1,1,1,1);

	swprintf(buf, 255, L"%i fps", (int)(1000/speed));
	render_text(buf, frame->x+5, frame->y+5, 20);
	get_text_dim(buf, NULL, &th, 20);
	ch+=th;
	swprintf(buf, 255, L"%.2f ms\n", speed);
	render_text(buf, frame->x+5, frame->y+5+ch, 20);
	get_text_dim(buf, NULL, &th, 20);
	ch+=th;
	swprintf(buf, 255, L"pos: (%.2f,%.2f)", data->world->game->players[data->world->active_player].x, data->world->game->players[data->world->active_player].y);
	render_text(buf, frame->x+5, frame->y+5+ch, 20);
	get_text_dim(buf, NULL, &th, 20);
	ch+=th;
	swprintf(buf, 255, L"b: %ix%i", config.board_width, config.board_height);
	render_text(buf, frame->x+5, frame->y+5+ch, 20);
	get_text_dim(buf, NULL, &th, 20);
	ch+=th;
	swprintf(buf, 255, L"p: %i\n", data->world->gen->count);
	render_text(buf, frame->x+5, frame->y+5+ch, 20);
	get_text_dim(buf, NULL, &th, 20);
	ch+=th;
	swprintf(buf, 255, L"w: %.2f%%", (float)100*(data->world->map.board_stats.total_water)/total);
	render_text(buf, frame->x+5, frame->y+5+ch, 20);
	get_text_dim(buf, NULL, &th, 20);
	ch+=th;
	swprintf(buf, 255, L"t: %.2f%%\n", (float)100*(data->world->map.board_stats.total_tree)/total);
	render_text(buf, frame->x+5, frame->y+5+ch, 20);
	get_text_dim(buf, NULL, &th, 20);
	ch+=th;

	swprintf(buf, 255, L"%i (%.2f%%)",
			 data->world->map.board_stats.total_tree-(data->world->map.board_stats.burning_tree+data->world->map.board_stats.burned_tree),
			 100*(data->world->map.board_stats.total_tree-(data->world->map.board_stats.burning_tree+data->world->map.board_stats.burned_tree))/total
		);
	render_text(buf, frame->x+5, frame->y+5+ch, 20);
	get_text_dim(buf, NULL, &th, 20);
	ch+=th;

	swprintf(buf, 255, L"%i (%.2f%%)",
			 data->world->map.board_stats.burning_tree,
			 100*(data->world->map.board_stats.burning_tree)/total
		);
	render_text(buf, frame->x+5, frame->y+5+ch, 20);
	get_text_dim(buf, NULL, &th, 20);
	ch+=th;

	swprintf(buf, 255, L"%i (%.2f%%)",
			 data->world->map.board_stats.burned_tree,
			 100*(data->world->map.board_stats.burned_tree)/total
		);
	render_text(buf, frame->x+5, frame->y+5+ch, 20);
	get_text_dim(buf, NULL, &th, 20);
	ch+=th;

}

void update_debug_render(ui_frame_t *frame)
{
	float *buf;
	ui_debug_data_t *data = frame->data;
	float vertices[] = {
		frame->x, frame->y, 0, 0, color[0], color[1], color[2], color[3],
		frame->x+frame->width, frame->y, 0, 0, color[0], color[1], color[2], color[3],
		frame->x+frame->width, frame->y + frame->height, 0, 0, color[0], color[1], color[2], color[3],
		frame->x, frame->y +frame->height, 0, 0, color[0], color[1], color[2], color[3]
	};

	glBindBuffer(GL_ARRAY_BUFFER, data->vbo);
	buf = glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(vertices), GL_MAP_WRITE_BIT);
	memcpy(buf, vertices, sizeof(vertices));
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void init_debug_rendering(ui_frame_t *frame)
{
	ui_debug_data_t *data = frame->data;
	glGenVertexArrays(1, &data->vao);
	glBindVertexArray(data->vao);
	glGenBuffers(1, &data->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, data->vbo);
	glBufferData(GL_ARRAY_BUFFER, VERTEX_DATA_SIZE*sizeof(float), NULL, GL_DYNAMIC_DRAW);

	GLint position = glGetAttribLocation(gui, "position");
	glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
	glEnableVertexAttribArray(position);


    GLint col = glGetAttribLocation(gui, "Color");
	glVertexAttribPointer(col, 4, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(4*sizeof(float)));
	glEnableVertexAttribArray(col);


	update_debug_render(frame);
}


ui_frame_t *init_ui_debug(ui_frame_t *parent, world_t *world, float x, float y, float w, float h)
{
	ui_frame_t *frame = create_ui();
	if(frame)
	{
		ui_debug_data_t *data = malloc(sizeof(ui_debug_data_t));
		if(!data)
			goto err_data;
		data->world = world;

		frame->data = data;
		frame->destroy = &destroy_ui_debug;
		frame->draw = &draw_debug;
		frame->update_render = &update_debug_render;
		frame->x = x;
		frame->y = y;
		frame->movable = 1;
		frame->width = w;
		frame->height = h;
		frame->parent = parent;
		init_debug_rendering(frame);
		return frame;
	err_data:
		return NULL;
	}
	return NULL;
}
