#include <client/font.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <client/shader.h>
#include <client/linmath.h>
#include <client/textures.h>
#include <stdio.h>
#include <client/context.h>
#include <string.h>
#include <assert.h>
#include <GLFW/glfw3.h>

#define VERTEX_DATA_SIZE 32

static GLuint font_vbo;
static GLuint font_vao;
static int vbo_index = 0;
static const size_t glyph_data_size = VERTEX_DATA_SIZE*sizeof(float);

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
	float ysizebot = ((float)GLYPH_DIM+0.2)/FONT_HEIGHT;
	float ysizetop = ((float)GLYPH_DIM-0.2 )/FONT_HEIGHT;
	
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
	glBufferData(GL_ARRAY_BUFFER, VBO_MAX_SIZE*glyph_data_size, NULL, GL_STREAM_DRAW );
	
	GLint position = glGetAttribLocation(gui, "position");
	glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
	glEnableVertexAttribArray(position);

	GLint uv = glGetAttribLocation(gui, "UV");
	glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(uv);

	GLint col = glGetAttribLocation(gui, "Color");
	glVertexAttribPointer(col, 4, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(4*sizeof(float)));
	glEnableVertexAttribArray(col);
	
	// Precompute UV and char dimensions
	for(i = 0; i < (FONT_HEIGHT / GLYPH_DIM); i++)
	{
		for(j = 0; j < (FONT_WIDTH / GLYPH_DIM); j++)
		{
			int current_char = i*(FONT_WIDTH/GLYPH_DIM)+j;
			int current_char_width = droid.width[droid.start_char+current_char]; // Each caracter can have different width
			
			float vertices[16] = {
				0, 0, j*xsize, i*ysizebot,
				0, GLYPH_DIM, j*xsize, (i+1)*ysizetop,
				current_char_width, GLYPH_DIM, j*xsize + ((float)current_char_width/FONT_WIDTH), (i+1)*ysizetop, 
			    current_char_width, 0, j*xsize + ((float)current_char_width/FONT_WIDTH), i*ysizebot
			};

			memcpy(&glyph_data[droid.start_char + current_char], vertices, sizeof(vertices));
		}
	}
	return 1;
}


/* Set w to the width (in pixels) and h with the height (in pixels) needed to display str */
void get_text_dim(wchar_t * str, float *w, float *h, float size)
{
	float scale = size / GLYPH_DIM;
	wchar_t *c = str;
	float max_width = 0, max_height = size, cur_width = 0, cur_height = size;
	while(*c)
	{
		if(*c > 256 || *c < 0)
		{
			c++;
			continue;
		}
		
		if(*c == '\n')
		{
			cur_height += GLYPH_DIM*scale;
			if(max_height < cur_height)
				max_height = cur_height;
			cur_width = 0;
		}
		else
		{
			cur_width += (droid.width[(int)*c]*scale);
			if(max_width < cur_width)
				max_width = cur_width;
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

void render_text(wchar_t * str, float x, float y, float size)
{
	float scale = size / GLYPH_DIM;
	int len = wcslen(str);
	int i;
	int buf_index = 0;
	float cur_x = 0;
	float cur_y = 0;
	float *buf;

	if(vbo_index + len > VBO_MAX_SIZE)
		return;
	
	glBindBuffer(GL_ARRAY_BUFFER, font_vbo);
	
	buf = glMapBufferRange(GL_ARRAY_BUFFER, vbo_index*glyph_data_size, (vbo_index+len)*glyph_data_size, GL_MAP_WRITE_BIT);
	for(i = 0; i < len; i++)
	{
		float *vertices = buf + buf_index * VERTEX_DATA_SIZE;
		int cc = str[i];
		if(cc > 256 || cc < 0) continue; // well we don't know this char
		int cc_w = droid.width[cc]; // current char width

		if(cc == '\n')
		{
			cur_x = 0;
			cur_y += GLYPH_DIM*scale;
			continue;
		}

		/* pos */
		vertices[0] = x+cur_x;
		vertices[1] = y+cur_y;
		/* uv */
		vertices[2] = glyph_data[cc][2];
		vertices[3] = glyph_data[cc][3];
		/* color */
		memcpy(&vertices[4], color, 4*sizeof(float));

		/* pos */
		vertices[8] = x+cur_x;
		vertices[9] = y+cur_y + GLYPH_DIM*scale;
		/* uv */
		vertices[10] = glyph_data[cc][6];
		vertices[11] = glyph_data[cc][7];
		/* color */
		memcpy(&vertices[12], color, 4*sizeof(float));

		/* pos */
		vertices[16] = x+cur_x + cc_w*scale;
		vertices[17] = y+cur_y + GLYPH_DIM*scale;
		/* uv */
		vertices[18] = glyph_data[cc][10];
		vertices[19] = glyph_data[cc][11];
		/* color */
		memcpy(&vertices[20], color, 4*sizeof(float));
		
		vertices[24] = x+cur_x + cc_w*scale;
		vertices[25] = y+cur_y;
		vertices[26] = glyph_data[cc][14];
		vertices[27] = glyph_data[cc][15];
		memcpy(&vertices[28], color, 4*sizeof(float));
		
		cur_x += cc_w*scale;
		buf_index++; // how many data wrote
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	vbo_index+=buf_index;
}

void unload_font()
{
	glDeleteBuffers(1,&font_vbo);
	glDeleteVertexArrays(1, &font_vao);
}

void font_swap_buffers()
{
	glUseProgram(gui);
	glBindVertexArray(font_vao);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, get_texid(TEX_FONT_DROID)); // bind texture to channel
	
	glUniform1i(glGetUniformLocation(gui, "has_texture"), 1);
	glUniform4fv(glGetUniformLocation(gui, "color"), 1,  color);

	glUniform1i(glGetUniformLocation(gui, "tex"), 0);
	glUniformMatrix4fv(glGetUniformLocation(gui, "projection"),
					   1, GL_FALSE, (GLfloat*)projection);
	
	/* And render them */
	glDrawArrays(GL_QUADS, 0, 4*(vbo_index));
	vbo_index = 0;
}
