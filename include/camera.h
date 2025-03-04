#ifndef CAMERA_H
#define CAMERA_H

#include <stdbool.h>
#include "utils.h"

#define DEFAULT_SPEED 100.0f
#define DEFAULT_SHIFT_SPEED 60.0f
#define DEFAULT_FOV 60

typedef struct {
    vec2_t pos;
    double dir_angle;
    float speed; // pixel movement per second
    bool up, down, left, right;
    float fov;
} camera_t;

void camera_init(camera_t *, int x, int y);

#endif
