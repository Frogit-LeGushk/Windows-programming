#include "camera.h"

struct SCamera camera = {0, 0, 1.7, 0, 0};
void apply_camera()
{
    glRotatef(-camera.xAlpha, 1, 0, 0);
    glRotatef(-camera.zAlpha, 0, 0, 1);
    glTranslatef(-camera.x,-camera.y,-camera.z);
}
void rotation_camera(float xAlpha, float zAlpha)
{
    camera.zAlpha += zAlpha;
    if(camera.zAlpha < 0) camera.zAlpha += 360;
    if(camera.zAlpha > 360) camera.zAlpha -= 360;

    camera.xAlpha += xAlpha;
    if(camera.xAlpha < 0) camera.xAlpha = 0;
    if(camera.xAlpha > 180) camera.xAlpha = 180;
}
void rotation_camera_mouse(int centerX, int centerY, float speed)
{
    POINT mouse_pos;
    GetCursorPos(&mouse_pos);
    POINT base_mouse_pos = {centerX, centerY};
    int dx = mouse_pos.x - base_mouse_pos.x;
    int dy = mouse_pos.y - base_mouse_pos.y;
    rotation_camera(-dy / 5.0 * speed, -dx / 5.0 * speed);

    SetCursorPos(base_mouse_pos.x, base_mouse_pos.y);
}
void move_camera(int forward_move, int right_move, float speed)
{
    float alpha = -camera.zAlpha / 180 * M_PI;

    if(forward_move > 0)
        alpha += right_move > 0 ? M_PI_4 : (right_move < 0 ? -M_PI_4 : 0);
    if(forward_move < 0)
        alpha += M_PI + (right_move > 0 ? -M_PI_4 : (right_move < 0 ? M_PI_4 : 0));
    if(forward_move == 0)
        alpha += right_move > 0 ? M_PI_2 : (right_move < 0 ? -M_PI_2 : 0);

    if(speed != 0)
    {
        if(forward_move != 0 || right_move != 0)
        {
            camera.y += cos(alpha) * speed;
            camera.x += sin(alpha) * speed;
        }
    }
}







