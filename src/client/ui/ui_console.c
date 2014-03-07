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
	int is_hovered;
	int is_dragging;
	float last_mouse[2];
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
	//wchar_t message[255];
	ui_console_data_t *data = frame->data;
	float opacity = frame->parent->keyboard_owner == frame ? 0.8 : data->is_hovered ? 0.5 : 0.4;
	float color[] = {0.05,0.05,0.05, opacity};
	glUseProgram(gui);
	glBindVertexArray(data->vao);
	
	glUniform1i(glGetUniformLocation(gui, "has_texture"),0);
	glUniform4fv(glGetUniformLocation(gui, "color"), 1, color);
	mat4x4 id;
	mat4x4_identity(id);
	render_model(gui,id,id,0,8);
	set_font_color(1,1,1,1);

	/*strcat(message, data->buffer);

	if(frame->parent->keyboard_owner == frame && fmod(glfwGetTime(), 1) > 0.5)
	strcat(message, "|");*/
	
	render_text(data->buffer ,frame->x+5, frame->y+frame->height-(frame->height*INPUT_BOX_SIZE)+(frame->height*INPUT_BOX_SIZE)/3, 19);
}

void update_rendering(ui_frame_t *frame)
{
	ui_console_data_t *data = frame->data;
	
	float vertices[] = {
		frame->x, frame->y, 0, 0,
		frame->x+frame->width, frame->y, 0, 0,
		frame->x+frame->width, frame->y+(frame->height*TEXT_BOX_SIZE), 0, 0,
		frame->x, frame->y+(frame->height*TEXT_BOX_SIZE), 0, 0,

		frame->x, frame->y+(frame->height*TEXT_BOX_SIZE)+10, 0, 0,
		frame->x+frame->width, frame->y+(frame->height*TEXT_BOX_SIZE)+10, 0, 0,
		frame->x+frame->width, frame->y+frame->height , 0, 0,
		frame->x, frame->y+frame->height, 0, 0,
	};
	
	glBindVertexArray(data->vao);
	glBindBuffer(GL_ARRAY_BUFFER, data->vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

}


void ui_console_on_mouse_move(ui_frame_t *frame, double x, double y)
{
	ui_console_data_t *data = frame->data;
	data->is_hovered =
		x >= frame->x
		&& x <= frame->x + frame->width
		&& y >= frame->y
		&& y <= frame->y+frame->height;
	
	if(data->is_dragging)
	{
		frame->x += x-data->last_mouse[0];
		frame->y += y-data->last_mouse[1];
		update_rendering(frame);
	}

	data->last_mouse[0] = x;
	data->last_mouse[1] = y;
}

void ui_console_on_mouse_button(ui_frame_t *frame, int button, int action, int mods)
{
	ui_console_data_t *data = frame->data;

	(void)mods;
	
	if(button == GLFW_MOUSE_BUTTON_LEFT && data->is_hovered)
	{
		data->is_dragging = action == GLFW_PRESS;
	}
}

void update_console(ui_frame_t *frame, float deltatime)
{
	(void)frame;
	(void)deltatime;
}

void init_console_rendering(ui_console_data_t *data, float x, float y, float w, float h)
{
	float vertices[] = {
		x, y, 0, 0,
		x+w, y, 0, 0,
		x+w, y+(h*TEXT_BOX_SIZE), 0, 0,
		x, y+(h*TEXT_BOX_SIZE), 0, 0,

		x, y+(h*TEXT_BOX_SIZE)+10, 0, 0,
		x+w, y+(h*TEXT_BOX_SIZE)+10, 0, 0,
		x+w, y+h , 0, 0,
		x, y+h, 0, 0,
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
		data->is_hovered = 0;
		data->is_dragging = 0;
		data->buffer[0] = 0;
		data->size = 0;

		frame->data = data;
		frame->destroy = &destroy_ui_console;
		frame->draw = &draw_console;
		frame->update = &update_console;
		frame->on_mouse_move = &ui_console_on_mouse_move;
		frame->on_mouse_button = &ui_console_on_mouse_button;
		frame->on_key = &ui_console_on_key;
		frame->on_char = &ui_console_on_char;
		frame->x = x;
		frame->y = y;
		frame->width = w;
		frame->height = h;
		frame->parent = parent;
		init_console_rendering(frame->data, x, y, w, h);
		return frame;
	err_data:
		return NULL;
	}
	return NULL;
}
