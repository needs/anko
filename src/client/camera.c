#include "camera.h"

static float scale_;

void set_camera(float x, float y, float scale)
{
	mat4x4_identity(camera);
	move_camera(x,y);
	mat4x4_scale_aniso(camera, camera, scale,scale,1);
	scale_ = scale;
}

void move_camera(float x, float y)
{
	mat4x4_translate_in_place(camera,x,y,0);
}

void scale_camera(float scale)
{
	if(scale_ + scale > ZOOM_MIN) return;
	if(scale_ + scale < 1 / ZOOM_MAX) return;
	
	scale_ += scale;
    mat4x4_scale_aniso(camera, camera, 1+scale,1+scale,1);
}
