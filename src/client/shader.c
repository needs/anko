#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/gl.h>

#include "shader.h"


#define MAX_SHADERS 2

GLuint standard;
GLuint fx;
GLuint sh_particles;

static GLuint get_shader(const char * filename, GLenum type);
static GLuint load_program (const char * vertex_filename, const char * fragment_filename);
static void destroy_program(GLuint program);

int load_shaders()
{
	standard = load_program("data/shaders/default.vs", "data/shaders/default.fs");
	if(!standard)
		goto err_standard;
	fx = load_program("data/shaders/fx.vs", "data/shaders/fx.fs");
	if(!fx)
		goto err_fx;
	sh_particles = load_program("data/shaders/particles.vs", "data/shaders/particles.fs");
	if(!sh_particles)
		goto err_particles;
	
	return 1;
	
err_particles:
	destroy_program(fx);
err_fx:
	destroy_program(standard);
err_standard:
	return 0;
}

void unload_shaders()
{
	destroy_program(standard);
}

static GLuint get_shader(const char * filename, GLenum type)
{
	GLuint shader;
	GLint info_result;
	FILE *f;
	char *buffer;
	size_t len;

	info_result = GL_FALSE;
	
	shader = glCreateShader(type);
	if(!shader)
		goto err_shader;
	
	f = fopen(filename, "r");
	if(!f)
		goto err_file;

	// Getting file size
	fseek(f, 0, SEEK_END);
	len = ftell(f);

	// Reading shader data
	fseek(f, 0, SEEK_SET);
	buffer = malloc(sizeof(char)*(len+1));
	if(!buffer)
		goto err_alloc;
	fread(buffer,sizeof(char),len,f);
	buffer[len] = 0;
	
	printf("compiling %s ...\n", filename);
	
	glShaderSource(shader, 1, (const char**)&buffer, NULL);
	glCompileShader(shader);

	// Check errors
	glGetShaderiv(shader, GL_COMPILE_STATUS, &info_result);
	if(info_result != GL_TRUE)
	{
		GLint info_length;
		char *info_msg;
		
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_length);
		info_msg = malloc(sizeof(char)*info_length);
		if(info_msg)
		{
			glGetShaderInfoLog(shader, info_length, NULL, info_msg);
			printf("compile info: %s\n", info_msg);
			free(info_msg);
		}
		glDeleteShader(shader);
		return GL_FALSE;
	}
	else
		printf("shader successfully compiled.\n");
	
	free(buffer);
	fclose(f);
	
	return shader;

err_alloc:
	fclose(f);
err_file:
	glDeleteShader(shader);
err_shader:
	return GL_FALSE;
}

static void destroy_program(GLuint program)
{
	GLsizei count = 0;
	GLuint shaders[MAX_SHADERS];
	int i;

	if(!program) return;
	
	glGetAttachedShaders(program, MAX_SHADERS, &count, shaders); 

	for(i = 0; i < count; i++)
		glDeleteShader(shaders[i]);
	
	glDeleteProgram(program);
}

static GLuint load_program(const char * vertex_filename, const char * fragment_filename)
{
	GLuint program;
	GLint info_result;
	
	GLuint vertex_shader = get_shader(vertex_filename, GL_VERTEX_SHADER);
	if(!vertex_shader)
		goto err_vertex;
	
	GLuint fragment_shader = get_shader(fragment_filename, GL_FRAGMENT_SHADER);
	if(!fragment_shader)
		goto err_fragment;
	
	program = glCreateProgram(); 
	if(!program)
		goto err_program;

	printf("linking program ...\n");
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	// Check errors
	glGetProgramiv(program, GL_LINK_STATUS, &info_result);

	if(info_result != GL_TRUE)
	{
		GLint info_length;
		char *info_msg;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_length);
		info_msg = malloc(sizeof(char)*info_length);
		if(info_msg)
		{
			glGetProgramInfoLog(program, info_length, NULL, info_msg);
			printf("link info: %s\n", info_msg);
			free(info_msg);
		}
	}
	else
		printf("program successfully linked.\n\n");
	
	return program;
	
err_program:
	glDeleteShader(fragment_shader);
err_fragment:
	glDeleteShader(vertex_shader);
err_vertex:
	return 0;
}
