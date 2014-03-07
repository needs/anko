#include "font.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include "shader.h"
#include "renderer.h"
#include "linmath.h"
#include "textures.h"
#include <stdio.h>
#include <string.h>

#include <GLFW/glfw3.h>

static GLuint font_vbo;
static GLuint font_vao;

static size_t glyph_data_size = 16*sizeof(float);

typedef struct font_data
{
	int32_t map_width;
	int32_t map_height;
	int32_t cell_width;
	int32_t cell_height;
	char start_char;
	char width[256];
} font_data_t;

static font_data_t droid;

float glyph_data[256][16] = {{0}};

float color[] = {1,1,1,1};

int load_font()
{
	int i, j;
	FILE* f;
	float xsize = (float)GLYPH_DIM/FONT_WIDTH;
	float ysize = (float)GLYPH_DIM/FONT_HEIGHT;
	
	f = fopen("data/fonts/droid.dat", "r");
	if(!f)
		return 0;
	
	fread(&droid, sizeof(font_data_t), 1, f);
	fclose(f);
	
	// OpenGL buffer creation
	glGenVertexArrays(1, &font_vao);
	glBindVertexArray(font_vao);
	
	glGenBuffers(1, &font_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, font_vbo);
	glBufferData(GL_ARRAY_BUFFER, BUFFER_MAX_SIZE*glyph_data_size, NULL, GL_DYNAMIC_DRAW );
	
	GLint position = glGetAttribLocation(gui, "position");
	glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
	glEnableVertexAttribArray(position);

	GLint uv = glGetAttribLocation(gui, "UV");
	glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(uv);

	// Fill the vbo
	for(i = 0; i < (FONT_HEIGHT / GLYPH_DIM); i++)
	{
		for(j = 0; j < (FONT_WIDTH / GLYPH_DIM); j++)
		{
			int current_char = i*(FONT_WIDTH/GLYPH_DIM)+j;
			int current_char_width = droid.width[droid.start_char+current_char]; // Each caracter can have different width
			
			float vertices[16] = {
				0, 0, j*xsize, i*ysize,
				0, GLYPH_DIM, j*xsize, (i+1)*ysize,
				current_char_width, GLYPH_DIM, j*xsize + ((float)current_char_width/FONT_WIDTH), (i+1)*ysize, 
			    current_char_width, 0, j*xsize + ((float)current_char_width/FONT_WIDTH), i*ysize
			};

			memcpy(&glyph_data[droid.start_char + current_char], vertices, sizeof(vertices));
			//glBufferSubData(GL_ARRAY_BUFFER, current_char*glyph_data_size, glyph_data_size, vertices);
		}
	}
	return 1;
}


/* Set w to the width (in pixels) and h with the height (in pixels) needed to display str */
void get_text_dim(char * str, float *w, float *h, float size)
{
	float scale = size / GLYPH_DIM;
	char *c = (char*)str;
	float max_width = 0, max_height = size, cur_width = 0, cur_height = size;
	while(*c)
	{
		if(*c >= 32 && *c < 127)
		{
			cur_width += (droid.width[(int)*c]*scale);
			if(max_width < cur_width)
				max_width = cur_width;
		}
		else if(*c == '\n')
		{
			cur_height += GLYPH_DIM*scale;
			if(max_height < cur_height)
				max_height = cur_height;
			cur_width = 0;
		}

		c++; // Yeah !
	}
	if(w)
		*w = max_width;
	if(h)
		*h = max_height;
}

void set_font_color(float r, float g, float b, float alpha)
{
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = alpha;
}

/*
  			
			float vertices[16] = {
				0, 0, j*xsize, i*ysize,
				0, GLYPH_DIM, j*xsize, (i+1)*ysize,
				current_char_width, GLYPH_DIM, j*xsize + ((float)current_char_width/FONT_WIDTH), (i+1)*ysize, 
			    current_char_width, 0, j*xsize + ((float)current_char_width/FONT_WIDTH), i*ysize
			};
*/
void render_text(char * str, float x, float y, float size)
{
	float scale = size / GLYPH_DIM;
	int len = strlen(str);
	int i;
	int buf_index = 0;
	float cur_x = 0;
	float cur_y = 0;
	float *buf;

	mat4x4 model;
	mat4x4_translate(model, x, y, 0);
	mat4x4_scale_aniso(model, model, scale, scale, 0);
	mat4x4 id;
	mat4x4_identity(id);

	if(len > BUFFER_MAX_SIZE)
		return;
	
	glUseProgram(gui);
	glBindVertexArray(font_vao);
	glBindBuffer(GL_ARRAY_BUFFER, font_vbo);
	glBindTexture(GL_TEXTURE_2D, get_texid(TEX_FONT_DROID));
	
	glUniform1i(glGetUniformLocation(gui, "has_texture"), 1);
	glUniform4fv(glGetUniformLocation(gui, "color"), 1,  color);
	
	buf = glMapBufferRange(GL_ARRAY_BUFFER, 0, len*glyph_data_size, GL_MAP_WRITE_BIT);
	for(i = 0; i < len; i++)
	{
		float *vertices = buf + buf_index * 16;
		int cc = str[i];
		int cc_w = droid.width[cc]; // current char width

		if(cc == '\n')
		{
			cur_x = 0;
			cur_y += GLYPH_DIM;
			continue;
		}
		
		vertices[0] = cur_x;
		vertices[1] = cur_y;
		vertices[2] = glyph_data[cc][2];
		vertices[3] = glyph_data[cc][3];

		vertices[4] = cur_x;
		vertices[5] = cur_y + GLYPH_DIM;
		vertices[6] = glyph_data[cc][6];
		vertices[7] = glyph_data[cc][7];

		vertices[8] = cur_x + cc_w;
		vertices[9] = cur_y + GLYPH_DIM;
		vertices[10] = glyph_data[cc][10];
		vertices[11] = glyph_data[cc][11];

		vertices[12] = cur_x + cc_w;
		vertices[13] = cur_y;
		vertices[14] = glyph_data[cc][14];
		vertices[15] = glyph_data[cc][15];
		cur_x += cc_w;
		buf_index++;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	render_model(gui, id, model, 0, 4*(buf_index));
}

void unload_font()
{
	glDeleteBuffers(1,&font_vbo);
	glDeleteVertexArrays(1, &font_vao);
}
