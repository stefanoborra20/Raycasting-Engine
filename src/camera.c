#include "camera.h"

void camera_init(camera_t *camera, int x, int y) {
    camera->pos.x = x;
    camera->pos.y = y;

    camera->speed = DEFAULT_SPEED; 
    camera->rotation_speed = DEFAULT_ROTATION_SPEED;

    camera->fov = DEFAULT_FOV;
    camera->dir_angle = 0;
}




