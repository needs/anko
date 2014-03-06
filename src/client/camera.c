#include <stdio.h>
#include "camera.h"
#include "config.h"


static void update_camera(camera_t *camera)
{
	mat4x4_identity(camera->matrix);
	mat4x4_translate_in_place(camera->matrix, camera->camera_pos[0], camera->camera_pos[1], 0);
	mat4x4_scale_aniso(camera->matrix, camera->matrix, camera->camera_scale, camera->camera_scale, 1);
}

void set_camera(camera_t *camera, float x, float y, float scale)
{
	camera->camera_scale = scale;
	camera->camera_pos[0] = x + config.screen_width/2;
	camera->camera_pos[1] = y + config.screen_height/2;
	update_camera(camera);
}

void move_camera(camera_t *camera, float x, float y)
{
	camera->camera_pos[0] -= x;
	camera->camera_pos[1] += y;
	update_camera(camera);
}

void scale_camera(camera_t *camera, float scale)
{
	if(camera->camera_scale + scale > (float)1 / ZOOM_MIN) return;
	if(camera->camera_scale + scale < (float)1 / ZOOM_MAX) return;
	
	camera->camera_scale += scale;
	update_camera(camera);
}
