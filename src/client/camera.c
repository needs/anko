#include "camera.h"

void set_camera(float x, float y, float scale)
{
	mat4x4_identity(camera);
	mat4x4_translate(camera,x,y,0);
	scale++;
}

void move_camera(float x, float y)
{
	mat4x4_translate_in_place(camera,x,y,0);
}

void scale_camera(float scale)
{
	scale++;
}
