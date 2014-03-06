#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "linmath.h"

#define ZOOM_MIN 1
#define ZOOM_MAX 3

mat4x4 camera;
float camera_scale;
float camera_pos[2] = {0, 0};

void set_camera(float x, float y, float scale);
void move_camera(float x, float y);
void scale_camera(float scale);

#endif /* _CAMERA_H_ */
