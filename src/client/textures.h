#ifndef _TEXTURES_H_
#define _TEXTURES_H_

#define ADD_TEXTURE(name, path) TEX_##name,
typedef enum tex_t {
	TEX_NONE = -1,
#include "textures.def"
	TEX_TOTAL,
} tex_t;
#undef ADD_TEXTURE


int load_textures(void);
void unload_textures(void);
void render_texture(float x, float y, tex_t tex);


#endif /* _TEXTURES_H_ */
