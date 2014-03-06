#include "font.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include "shader.h"
#include "renderer.h"
#include "linmath.h"
#include "textures.h"
#include <stdio.h>

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


int load_font()
{
	int i, j;
	size_t data_size;
	FILE* f;
	float xsize = (float)GLYPH_DIM/FONT_WIDTH;
	float ysize = (float)GLYPH_DIM/FONT_HEIGHT;
	
	f = fopen("data/fonts/droid.dat", "r");
	if(!f)
		return 0;
	
	fread(&droid, sizeof(font_data_t), 1, f);
	fclose(f);
	
    data_size =  (FONT_WIDTH / GLYPH_DIM) * (FONT_HEIGHT / GLYPH_DIM) * glyph_data_size;

	// OpenGL buffer creation
	glGenVertexArrays(1, &font_vao);
	glBindVertexArray(font_vao);
	
	glGenBuffers(1, &font_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, font_vbo);
	glBufferData(GL_ARRAY_BUFFER, data_size, NULL, GL_STATIC_DRAW );
	
	GLint position = glGetAttribLocation(program, "position");
	glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
	glEnableVertexAttribArray(position);

	GLint tex_coord = glGetAttribLocation(program, "tex_coord");
	glVertexAttribPointer(tex_coord, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(tex_coord);

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
			
			glBufferSubData(GL_ARRAY_BUFFER, current_char*glyph_data_size, glyph_data_size, vertices);
		}
	}
	return 1;
}


void get_text_dim(char * str, float *w, float *h, float scale)
{
	char *c = (char*)str;
	float max_width = 0, max_height = GLYPH_DIM*scale, cur_width = 0, cur_height = GLYPH_DIM*scale;
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

void render_text(char * str, float x, float y, float scale)
{
	char * c = (char *)str;
	float cur_x = x, cur_y = y;
	
	glBindBuffer(GL_ARRAY_BUFFER, font_vbo);
	glBindVertexArray(font_vao);
	glBindTexture(GL_TEXTURE_2D, get_texid(TEX_FONT_DROID));

	mat4x4 start;
	
	while(*c)
	{
		if(*c >= 32 && *c < 127)
		{
			mat4x4_translate(start, cur_x, cur_y,0);
			mat4x4_scale_aniso(start, start, scale, scale, 0);
			render_on_top(start, (*c-32)*sizeof(float));
			cur_x += (droid.width[(int)*c])*scale;
		}
		else if(*c == '\n')
		{
			cur_y += GLYPH_DIM*scale;
			cur_x = x;
		}

		c++; // Yeah !
	}
}

void unload_font()
{
	glDeleteBuffers(1,&font_vbo);
	glDeleteVertexArrays(1, &font_vao);
}
