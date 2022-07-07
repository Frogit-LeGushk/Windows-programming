#include <windows.h>
#include <gl/gl.h>
#include <math.h>

#ifndef H_CAMERA
#define H_CAMERA

struct SCamera {
    float x, y, z;
    float xAlpha, zAlpha;
} camera;

void apply_camera();
void rotation_camera(float xAlpha, float zAlpha);
void rotation_camera_mouse(int centerX, int centerY, float speed);
void move_camera(int forward_move, int right_move, float speed);

#endif // H_CAMERA
