#include <stdio.h>
#include "camera.h"
#include "config.h"


static void update_camera()
{
	mat4x4_identity(camera);
	mat4x4_translate_in_place(camera, camera_pos[0], camera_pos[1], 0);
	mat4x4_scale_aniso(camera, camera, camera_scale, camera_scale, 1);
}

void set_camera(float x, float y, float scale)
{
	camera_scale = scale;
	camera_pos[0] = x + config.screen_width/2;
	camera_pos[1] = y + config.screen_height/2;
	update_camera();
}

void move_camera(float x, float y)
{
	camera_pos[0] -= x;
	camera_pos[1] += y;
	update_camera();
}

void scale_camera(float scale)
{
	if(camera_scale + scale > (float)1 / ZOOM_MIN) return;
	if(camera_scale + scale < (float)1 / ZOOM_MAX) return;
	
	camera_scale += scale;
	update_camera();
}
