#include <stdio.h>
#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "shader.h"

GLuint program;
GLuint vbo;
GLuint vao;

static void render_rect();
static void init_rendering();


int main(void)
{
	GLFWwindow* window;

	if(!glfwInit())
	{
		perror("Can't init glfw\n");
		return EXIT_FAILURE;
	}

	window = glfwCreateWindow(1024, 720, "Anko", NULL, NULL);
	if(!window)
	{
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	init_rendering(window);
	
	glClearColor(0.3, 0.4, 0.7, 1);
	

	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		
		glClear(GL_COLOR_BUFFER_BIT);
		render_rect();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	
	return EXIT_SUCCESS;
}


static void render_rect()
{
	float vertices[] = {
		 0.5,  0.5,
		-0.5,  0.5,
		-0.5, -0.5,
		 0.5, -0.5,
	};

	/* Let's copy them :) */
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	/* And render them */
	glDrawArrays(GL_TRIANGLES, 0, 3);
}


static void init_rendering()
{
	glewExperimental = GL_TRUE;
	glewInit();

	/* Get a Vertex Buffer Object, */
	/* and make it active, so we can copy data on it */
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	/* Load shaders and make them active */
	program = load_shaders("data/shaders/default.vs", "data/shaders/default.fs");
	glBindFragDataLocation(program, 0, "outColor");
	glLinkProgram(program);
	glUseProgram(program);

	/* Vertex Array Object */
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	/* Format of the vertices */
	GLint posAttrib = glGetAttribLocation(program, "position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib);
}
