#ifndef _TEXTURES_H_
#define _TEXTURES_H_

#include "linmath.h"


#define ADD_TEXTURE(name, path) TEX_##name,
#define ADD_IN_TEXTURE(name, from, ...) TEX_##from##_##name,
typedef enum tex_t {
	TEX_NONE = 0,
#include "textures.def"
	TEX_TOTAL,
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
 * Each pack of 4 float describe one vertex and one tex. coord.
 */
void get_texture(float *data, tex_t tex, float x, float y);

/*
 * Same as above, but the vertices are centered. 
 */
void get_ctexture(float *data, tex_t tex, float x, float y);

/*
 * Return the OpenGL texture ID of the texture tex.
 */
GLuint get_texid(tex_t tex);


#endif /* _TEXTURES_H_ */
