#define GLEW_STATIC
#include "GL/glew.h"
#include "GL/gl.h"
#include <GLFW/glfw3.h>
#include "ui_console.h"
#include "../font.h"
#include "../shader.h"
#include "../renderer.h"
#include <stdlib.h>
#include <stdio.h>
#include "../event.h"
#include <string.h>
#include <math.h>
#include <wchar.h>

#define INPUT_BOX_SIZE ((float)1/6)
#define TEXT_BOX_SIZE (1-INPUT_BOX_SIZE)

typedef struct ui_console_data_t
{
	GLuint vao;
	GLuint vbo;
	
    wchar_t buffer[255];
	int size;
} ui_console_data_t;

void destroy_ui_console(ui_frame_t *frame)
{
	free(frame->data);
	free(frame);
}

void draw_console(ui_frame_t *frame)
{
	wchar_t message[255];
	wchar_t prompt[2] = {0x5f,0};
	ui_console_data_t *data = frame->data;
	float opacity = frame->parent->keyboard_owner == frame ? 0.8 : frame->is_hovered ? 0.5 : 0.4;
	float color[] = {0.05,0.05,0.05, opacity};
	glUseProgram(gui);
	glBindVertexArray(data->vao);
	
	glUniform1i(glGetUniformLocation(gui, "has_texture"),0);
	glUniform4fv(glGetUniformLocation(gui, "color"), 1, color);
	mat4x4 id;
	mat4x4_identity(id);
	mat4x4 model;
	mat4x4_translate(model, frame->x, frame->y, 0);
	render_model(gui, id, model, 0, 8);
	set_font_color(1, 1, 1, 1);

	wcscat(message, data->buffer);

	if(frame->parent->keyboard_owner == frame && fmod(glfwGetTime(), 1) > 0.5)
		wcscat(message, prompt);
	
		render_text(message ,frame->x+5, frame->y+frame->height-(frame->height*INPUT_BOX_SIZE)+(frame->height*INPUT_BOX_SIZE)/3, 19);
}


void init_console_rendering(ui_console_data_t *data, float w, float h)
{
	float vertices[] = {
		0, 0, 0, 0,
		w, 0, 0, 0,
		w, (h*TEXT_BOX_SIZE)+5, 0, 0,
		0, (h*TEXT_BOX_SIZE)+5, 0, 0,

		0, (h*TEXT_BOX_SIZE)+10, 0, 0,
		w, (h*TEXT_BOX_SIZE)+10, 0, 0,
		w, h , 0, 0,
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

void ui_console_on_key(ui_frame_t* frame, int key, int scancode, int action, int mods)
{
	(void)scancode;
	(void)mods;
	ui_console_data_t *data = frame->data;

	if(key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		if(frame->parent->keyboard_owner == frame)
		{
			frame->parent->keyboard_owner = NULL;
			data->size = 0;
			data->buffer[data->size] = 0;
		}
		else
			frame->parent->keyboard_owner = frame->parent->keyboard_owner == frame ? NULL : frame;
	}
	if(key == GLFW_KEY_BACKSPACE && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		if(data->size > 0)
		{
			data->size--;
			data->buffer[data->size] = 0;
		}
	}
}

void ui_console_on_char(ui_frame_t *frame, unsigned int c)
{
	ui_console_data_t *data = frame->data;
	if(frame->parent->keyboard_owner == frame)
	{
		if(data->size < 255)
		{
			data->buffer[data->size] = c;
			data->size++;
			data->buffer[data->size] = 0;
		}
	}
}

ui_frame_t *init_ui_console(ui_frame_t *parent, float x, float y, float w, float h)
{
	ui_frame_t *frame = create_ui();
	if(frame)
	{
		ui_console_data_t *data = malloc(sizeof(ui_console_data_t));
		if(!data)
			goto err_data;
		data->buffer[0] = 0;
		data->size = 0;

		frame->data = data;
		frame->destroy = &destroy_ui_console;
		frame->draw = &draw_console;
		frame->on_key = &ui_console_on_key;
		frame->on_char = &ui_console_on_char;
		frame->x = x;
		frame->y = y;
		frame->width = w;
		frame->height = h;
		frame->parent = parent;
		frame->movable = 1;
		init_console_rendering(frame->data, w, h);
		return frame;
	err_data:
		return NULL;
	}
	return NULL;
}
