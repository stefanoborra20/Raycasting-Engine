#include "camera.h"

void camera_init(camera_t *camera, int x, int y) {
    camera->pos.x = x;
    camera->pos.y = y;

    camera->dir.x = -1;
    camera->dir.y = 0;
}




