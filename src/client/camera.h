#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "linmath.h"

mat4x4 camera;

void set_camera(float x, float y, float scale);
void move_camera(float x, float y);
void scale_camera(float scale);

#endif /* _CAMERA_H_ */
