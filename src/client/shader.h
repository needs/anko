#ifndef _SHADER_H_
#define _SHADER_H_

#include <GL/gl.h>

extern GLuint standard;
extern GLuint fx;
extern GLuint sh_particles;
extern GLuint gui;

int load_shaders();
void unload_shaders();

#endif /* _SHADER_H_ */
