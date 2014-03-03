#include <GL/glew.h>
#include <GL/gl.h>
#include "shader.h"

#include <stdio.h>
#include <stdlib.h>

static GLuint get_shader(const char * filename, GLenum type);

GLuint load_shaders(const char * vertex_filename, const char * fragment_filename)
{
	GLint info_result, info_length;
	char *info_msg;
	
	GLuint vertex_shader = get_shader(vertex_filename, GL_VERTEX_SHADER);
	if(!vertex_shader)
		goto err_vertex;
	
	GLuint fragment_shader = get_shader(fragment_filename, GL_FRAGMENT_SHADER);
	if(!fragment_shader)
		goto err_fragment;
	
	GLuint program = glCreateProgram(); 
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
		printf("program successfully linked.\n");
	
	return program;
	
err_program:
	glDeleteShader(fragment_shader);
err_fragment:
	glDeleteShader(vertex_shader);
err_vertex:
	return GL_FALSE;
}

void destroy_shader(GLuint program)
{
	// delete bound shaders
	glDeleteProgram(program);
}

static GLuint get_shader(const char * filename, GLenum type)
{
	GLuint shader;
	GLint info_result, info_length;
	FILE *f;
	char *buffer;
	char *info_msg;
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
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_length);
		info_msg = malloc(sizeof(char)*info_length);
		if(info_msg)
		{
			glGetShaderInfoLog(shader, info_length, NULL, info_msg);
			printf("compile info: %s\n", info_msg);
			free(info_msg);
		}
	}
	else
		printf("shader successfully compiled.\n\n");
	
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
