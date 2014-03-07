#define GLEW_STATIC
#include "GL/glew.h"
#include "GL/gl.h"
#include "ui_debug.h"
#include "../shader.h"
#include "../renderer.h"
#include "../font.h"
#include "../linmath.h"
#include "../config.h"
#include "../context.h"
#include <stdlib.h>

typedef struct ui_debug_data_t
{
	GLuint vao;
	GLuint vbo;
} ui_debug_data_t;


void destroy_ui_debug(ui_frame_t *frame)
{
	free(frame->data);
	free(frame);
}

void draw_debug(ui_frame_t *frame)
{
	ui_debug_data_t *data = frame->data;
	float color[] = {0.05,0.05,0.05, 0.5};
	wchar_t buf[255];
	float th;
	float ch=0;
	glUseProgram(gui);
	glBindVertexArray(data->vao);
			
	glUniform1i(glGetUniformLocation(gui, "has_texture"),0);
	glUniform4fv(glGetUniformLocation(gui, "color"), 1, color);
	mat4x4 id;
	mat4x4_identity(id);
	mat4x4 model;
	mat4x4_translate(model, frame->x, frame->y, 0);
	render_model(gui, id, model, 0, 8);
	set_font_color(1,1,1,1);
	
	swprintf(buf, 255, L"%i fps", (int)(1000/speed));
	render_text(buf, frame->x+5, frame->y+5, 20);
	get_text_dim(buf, NULL, &th, 20);
	ch+=th;
	swprintf(buf, 255, L"b: %ix%i", config.board_width, config.board_height);
	render_text(buf, frame->x+5, frame->y+5+ch, 20);
	get_text_dim(buf, NULL, &th, 20);
	ch+=th;
	swprintf(buf, 255, L"useless text", config.board_width, config.board_height);
	render_text(buf, frame->x+5, frame->y+5+ch, 20);
	get_text_dim(buf, NULL, &th, 20);
	ch+=th;
	swprintf(buf, 255, L"tin tin tin", config.board_width, config.board_height);
	render_text(buf, frame->x+5, frame->y+5+ch, 20);
	get_text_dim(buf, NULL, &th, 20);
	ch+=th;
	swprintf(buf, 255, L"utf-8 geré, héhéhéhéêù³²¡÷", config.board_width, config.board_height);
	render_text(buf, frame->x+5, frame->y+ch, 20);
	get_text_dim(buf, NULL, &th, 20);
	ch+=th;
}

void init_debug_rendering(ui_debug_data_t *data,float w, float h)
{
	float vertices[] = {
		0, 0, 0, 0,
		w, 0, 0, 0,
		w, h, 0, 0,
		0, h, 0, 0,
	};

	glGenVertexArrays(1, &data->vao);
	glBindVertexArray(data->vao);
	glGenBuffers(1, &data->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, data->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	GLint position = glGetAttribLocation(gui, "position");
	glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
	glEnableVertexAttribArray(position);
}


ui_frame_t *init_ui_debug(ui_frame_t *parent, float x, float y, float w, float h)
{
	ui_frame_t *frame = create_ui();
	if(frame)
	{
		ui_debug_data_t *data = malloc(sizeof(ui_debug_data_t));
		if(!data)
			goto err_data;

		frame->data = data;
		frame->destroy = &destroy_ui_debug;
		frame->draw = &draw_debug;
		frame->x = x;
		frame->y = y;
		frame->movable = 1;
		frame->width = w;
		frame->height = h;
		frame->parent = parent;
		init_debug_rendering(frame->data, w, h);
		return frame;
	err_data:
		return NULL;
	}
	return NULL;
}
