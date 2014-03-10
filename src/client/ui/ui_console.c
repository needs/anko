#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <client/ui/ui_console.h>
#include <client/font.h>
#include <client/shader.h>
#include <client/context.h>
#include <stdlib.h>
#include <stdio.h>
#include <client/event.h>
#include <string.h>
#include <math.h>
#include <wchar.h>
#include <assert.h>

#define VERTEX_DATA_SIZE 32
#define MAX_MESSAGES_LENGTH 255
#define MAX_MESSAGES 128
#define FONT_SCALE 0.6

#define INPUT_BOX_SIZE ((float)1/6)
#define TEXT_BOX_SIZE (1-INPUT_BOX_SIZE)

static const int rect_size = VERTEX_DATA_SIZE*sizeof(float);
static const int rect_count = 2;
static float color[] = {0.05, 0.05, 0.05, 1};

typedef struct console_input_t
{
	wchar_t buffer[MAX_MESSAGES_LENGTH+1];
	int size;
	int pos;
} console_input_t;

typedef struct console_messages_t
{
	wchar_t data[MAX_MESSAGES][MAX_MESSAGES_LENGTH+1];
	int start;
	int count;
} console_messages_t;

typedef struct ui_console_data_t
{
	GLuint vao;
	GLuint vbo;

	console_input_t input;
	console_messages_t messages;
	console_messages_t input_history;
} ui_console_data_t;

void console_add_message(console_messages_t *messages, wchar_t *str)
{
	if(messages->count >= MAX_MESSAGES)
	{
	    messages->start = (messages->start+1)%MAX_MESSAGES;
	    messages->count--;
	}
	wcsncpy(messages->data[(messages->start+messages->count)%MAX_MESSAGES], str, MAX_MESSAGES_LENGTH+1);
	messages->count++;
}

void destroy_ui_console(ui_frame_t *frame)
{
	free(frame->data);
	free(frame);
}


static void fill_resized_input(wchar_t *dest, wchar_t *src, ui_frame_t *frame, float size)
{
	wchar_t tmp[MAX_MESSAGES_LENGTH+1] = { 0 };
	int index = MAX_MESSAGES_LENGTH;
	int len = wcslen(src);
	float tw = 0;
	int ss;
	while(len > 0 && tw < frame->width-5)
	{
		tmp[index]  = src[len-1];
		
		get_text_dim(&tmp[index], &tw, NULL, size);
		
		index--;
		len--;
	}

    ss = (MAX_MESSAGES_LENGTH-index);
	memcpy(dest, &tmp[index+1], ss*sizeof(wchar_t));
    dest[ss] = 0;
}

static void render_input(ui_frame_t *frame)
{
	wchar_t message[MAX_MESSAGES_LENGTH+1];
	wchar_t tmp[MAX_MESSAGES_LENGTH+1];
	wchar_t prompt[2] = {0x5f,0};
	ui_console_data_t *data = frame->data;
		
	set_font_color(1, 1, 1, 1);

	fill_resized_input(tmp, data->input.buffer, frame,(frame->height*INPUT_BOX_SIZE)*FONT_SCALE);
	wcscat(message, tmp);

	if(frame->parent->keyboard_owner == frame && fmod(glfwGetTime(), 1) > 0.5)
		wcscat(message, prompt);
	
	render_text(message, frame->x+5,
				frame->y+frame->height-(frame->height*INPUT_BOX_SIZE)+(frame->height*INPUT_BOX_SIZE)/3,
				(frame->height*INPUT_BOX_SIZE)*FONT_SCALE);
}

static float resized_text_height(ui_frame_t *frame, wchar_t *str, float size)
{
	wchar_t buff[MAX_MESSAGES_LENGTH+1];
	int is = 0, ib = 0; // index in str, index in buff
	float tw, th; // text width, text height
	float  ch = 0; // current width, current height
	
	while(str[is])
	{
		buff[ib] = str[is];
		ib++;
		buff[ib] = 0;

		get_text_dim(buff, &tw, &th, size);
		if(tw >= frame->width-15)
		{
			ch+=th;
			ib = 0;
			buff[ib] = 0;
		}
		is++;
	}
	get_text_dim(buff, &tw, &th, size);
	return ch+th;
}

static void render_resized_text(ui_frame_t *frame, wchar_t *str, float x, float y, float size)
{
	wchar_t buff[MAX_MESSAGES_LENGTH+1];
	int is = 0, ib = 0; // index in str, index in buff
	float tw, th; // text width, text height
	float  ch = 0; // current width, current height
	
	while(str[is])
	{
		buff[ib] = str[is];
		ib++;
		buff[ib] = 0;

		get_text_dim(buff, &tw, &th, size);
		if(tw >= frame->width-15)
		{
			render_text(buff, x, y+ch, size);
			ch+=th;
			ib = 0;
			buff[ib] = 0;
		}
		is++;
	}
	if(ib)
	{
		render_text(buff, x, y+ch, size);
		get_text_dim(buff, &tw, &th, size);
	}
}

static void render_messages(ui_frame_t *frame)
{
	ui_console_data_t *data = frame->data;
	wchar_t message[MAX_MESSAGES_LENGTH+1];
	int i;
	float y_offset = 0;
	float th;
	
	set_font_color(1, 1, 1, 1);

	i = 0;

	while( i < data->messages.count && y_offset < frame->height*TEXT_BOX_SIZE)
	{
		wcsncpy(message, data->messages.data[((data->messages.start+data->messages.count)-i-1)%MAX_MESSAGES], MAX_MESSAGES_LENGTH+1);
		th = resized_text_height(frame, message, (frame->height*INPUT_BOX_SIZE)*FONT_SCALE);
		if(y_offset + th < frame->height*TEXT_BOX_SIZE)
		{
			render_resized_text(frame, message, frame->x+5,
								frame->y+(frame->height*TEXT_BOX_SIZE)-(y_offset + th),
								(frame->height*INPUT_BOX_SIZE)*FONT_SCALE);
		}
	
		y_offset+=th;
		i++;
	}
}

void update_console_render(ui_frame_t *frame)
{
	float *buf;
	ui_console_data_t *data = frame->data;
	float vertices[] = {
		frame->x, frame->y, 0, 0, color[0], color[1], color[2], color[3],
		frame->x+frame->width, frame->y, 0, 0, color[0], color[1], color[2], color[3],
		frame->x+frame->width, frame->y + (frame->height*TEXT_BOX_SIZE)+5, 0, 0, color[0], color[1], color[2], color[3],
		frame->x, frame->y + (frame->height*TEXT_BOX_SIZE)+5, 0, 0, color[0], color[1], color[2], color[3],

		frame->x, frame->y + (frame->height*TEXT_BOX_SIZE)+10, 0, 0, color[0], color[1], color[2], color[3],
		frame->x+frame->width, frame->y + (frame->height*TEXT_BOX_SIZE)+10, 0, 0, color[0], color[1], color[2], color[3],
		frame->x+frame->width, frame->y + frame->height , 0, 0, color[0], color[1], color[2], color[3],
		frame->x, frame->y + frame->height, 0, 0, color[0], color[1], color[2], color[3]
	};

	glBindBuffer(GL_ARRAY_BUFFER, data->vbo);
	buf = glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(vertices), GL_MAP_WRITE_BIT);
	memcpy(buf, vertices, rect_count*rect_size);
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void draw_console(ui_frame_t *frame)
{
	float opacity;
	
	ui_console_data_t *data = frame->data;

	if(frame->parent->keyboard_owner == frame)
		opacity = 0.8;
	else
		if(frame->parent->focused_child == frame && frame->is_hovered)
			opacity = 0.5;
		else
			opacity = 0.4;

	color[3] = opacity;

	update_console_render(frame);

	glUseProgram(gui);
	glBindVertexArray(data->vao);
	
	glUniform1i(glGetUniformLocation(gui, "has_texture"),0);
	
	glDrawArrays(GL_QUADS, 0, 4*rect_count);

	render_input(frame);
	render_messages(frame);
}

void init_console_rendering(ui_frame_t *frame)
{

	ui_console_data_t *data = frame->data;
	
	glGenVertexArrays(1, &data->vao);
	glBindVertexArray(data->vao);
	glGenBuffers(1, &data->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, data->vbo);
	glBufferData(GL_ARRAY_BUFFER, rect_count*rect_size, NULL, GL_DYNAMIC_DRAW);

	GLint position = glGetAttribLocation(gui, "position");
	glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
	glEnableVertexAttribArray(position);

    GLint col = glGetAttribLocation(gui, "Color");
	glVertexAttribPointer(col, 4, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(4*sizeof(float)));
	glEnableVertexAttribArray(col);

	update_console_render(frame);
}
	
void ui_console_on_key(ui_frame_t* frame, int key, int scancode, int action, int mods)
{
	(void)scancode;
	(void)mods;
	ui_console_data_t *data = frame->data;

	if(key == GLFW_KEY_ESCAPE)
	{
		data->input.size = 0;
		data->input.buffer[data->input.size] = 0;
	}
	
	if(key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		if(frame->parent->keyboard_owner == frame)
		{
			frame->parent->keyboard_owner = NULL;
			if(data->input.size)
			{
				console_add_message(&data->messages, data->input.buffer);
				console_add_message(&data->input_history, data->input.buffer);
			}
			
			data->input.size = 0;
			data->input.buffer[data->input.size] = 0;
		}
		else
			frame->parent->keyboard_owner = frame->parent->keyboard_owner == frame ? NULL : frame;
	}
	if(key == GLFW_KEY_BACKSPACE && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		if(data->input.size > 0)
		{
			data->input.size--;
			data->input.buffer[data->input.size] = 0;
		}
	}
}

void ui_console_on_char(ui_frame_t *frame, unsigned int c)
{
	ui_console_data_t *data = frame->data;
	if(frame->parent->keyboard_owner == frame)
	{
		if(data->input.size < MAX_MESSAGES_LENGTH)
		{
			data->input.buffer[data->input.size] = c;
			data->input.size++;
			data->input.buffer[data->input.size] = 0;
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
		data->input.buffer[0] = 0;
		data->input.size = 0;

		data->messages.start = 0;
		data->messages.count = 0;
		data->input_history.start = 0;
		data->input_history.count = 0;

		frame->data = data;
		frame->destroy = &destroy_ui_console;
		frame->draw = &draw_console;
		frame->on_key = &ui_console_on_key;
		frame->update_render = &update_console_render;
		frame->on_char = &ui_console_on_char;
		frame->x = x;
		frame->y = y;
		frame->width = w;
		frame->height = h;
		frame->parent = parent;
		frame->movable = 1;
		init_console_rendering(frame);
		return frame;
	err_data:
		return NULL;
	}
	return NULL;
}
