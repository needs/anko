#include <stdio.h>
#include "camera.h"
#include "window.h"

static float camera_scale;

void set_camera(float x, float y, float scale)
{
	mat4x4_identity(camera);
	move_camera(x,y);
	mat4x4_scale_aniso(camera, camera, scale, scale, 1);
	camera_scale = scale;
}

void move_camera(float x, float y)
{
	mat4x4_translate_in_place(camera, x, y, 0);
}

void scale_camera(float scale)
{
	if(camera_scale + scale > (float)1 / ZOOM_MIN) return;
	if(camera_scale + scale < (float)1 / ZOOM_MAX) return;
	
	camera_scale += scale;
	mat4x4_scale_aniso(camera, camera, (float)1+scale, (float)1+scale, 1);
}
