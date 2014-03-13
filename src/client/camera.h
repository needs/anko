#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <client/linmath.h>

#define ZOOM_MIN 1
#define ZOOM_MAX 10

typedef struct camera_t
{
	mat4x4 matrix;
	float camera_scale;
	float camera_pos[2];
} camera_t;

void set_camera(camera_t *camera, float x, float y, float scale);
void move_camera(camera_t *camera, float x, float y);
void scale_camera(camera_t *camera, float scale);
void set_camera_pos(camera_t *camera, float x, float y);

#endif /* _CAMERA_H_ */
