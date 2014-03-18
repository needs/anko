#ifndef _TEXTURES_H_
#define _TEXTURES_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>

#include <client/linmath.h>


#define TEXTURE_VERTEX_NB  4
#define TEXTURE_VERTEX_LEN 5
#define TEXTURE_VERTEX_SIZE (TEXTURE_VERTEX_LEN * sizeof(float))


#define ADD_TEXTURE(name, path)                        TEX_##name,
#define ADD_IN_TEXTURE(name, from, x, y, w, h, ox, oy) TEX_##from##_##name,
typedef enum tex_t {
	TEX_NONE = 0,
#include "textures.def"
	TEX_TOTAL
} tex_t;
#undef ADD_IN_TEXTURE
#undef ADD_TEXTURE


/*
 * Read textures from files and load them in memory.
 * Every following functions must be called after this one.
 */
int load_textures(void);

/*
 * Remove textures from memory.
 * Every following functions musn't be called until textures are reloaded
 * with load_textures().
 */
void unload_textures(void);

/*
 * Put in data the informations of texture tex.
 * data must be at least 16 float wide. data is only written.
 * Each set of 4 float describe one vertex and one tex. coord.
 * Return 4 sets.
 */
void get_texture(float *data, tex_t tex, float x, float y, float z);

/*
 * Same as above, but the vertices are centered. 
 */
void get_ctexture(float *data, tex_t tex, float x, float y, float z);

/*
 * Same as above, but the vertices are scaled.
 */
void get_stexture(float *data, tex_t tex, float x, float y, float z, float sx, float sy);

/*
 * Same as above, but the vertices are centered and scaled.
 */
void get_sctexture(float *data, tex_t tex, float x, float y, float z, float sx, float sy);

/*
 * Return the OpenGL texture ID of the texture tex.
 */
GLuint get_texid(tex_t tex);


#endif /* _TEXTURES_H_ */
