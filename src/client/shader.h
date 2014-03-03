#ifndef _SHADER_H_
#define _SHADER_H_

#include <GL/gl.h>

GLuint load_shaders(const char * vertex_filename, const char * fragment_filename);
void destroy_shaders(GLuint program);

#endif /* _SHADER_H_ */
