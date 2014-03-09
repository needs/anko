#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <client/rtt.h>
#include <client/shader.h>
#include <client/config.h>


static GLuint rtt_fbo;
static GLuint rtt_tex;
static GLuint rtt_vbo;
static GLuint rtt_vao;
int rtt_effect = 0; // will use an enum later

void rtt_start()
{
	glBindFramebuffer(GL_FRAMEBUFFER, rtt_fbo);
}

void rtt_stop()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void rtt_draw()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, rtt_tex);
	
	mat4x4 id;
	mat4x4_identity(id);
	
	glUseProgram(fx);
	glUniform1i(glGetUniformLocation(fx, "tex"), 0);
	glUniform1f(glGetUniformLocation(fx, "time"), glfwGetTime());
	glUniform1i(glGetUniformLocation(fx, "effect"), rtt_effect);
	glBindVertexArray(rtt_vao);
	glBindBuffer(GL_ARRAY_BUFFER, rtt_vbo);
	glDrawArrays(GL_QUADS, 0, 4);
}

void rtt_init()
{
	glGenFramebuffers(1, &rtt_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, rtt_fbo);
	glGenTextures(1, &rtt_tex);
	glBindTexture(GL_TEXTURE_2D, rtt_tex);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, config.screen_width, config.screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rtt_tex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float vertices[] = {
		-1, -1, 0, 0,
		-1, 1, 0, 1,
		1, 1, 1, 1,
		1, -1, 1, 0
	};
	
	glGenVertexArrays(1, &rtt_vao);
	glBindVertexArray(rtt_vao);
	
	glGenBuffers(1, &rtt_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, rtt_vbo);

	GLint position = glGetAttribLocation(fx, "position");
	glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
	glEnableVertexAttribArray(position);

	GLint uv = glGetAttribLocation(fx, "UV");
	glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(uv);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}
