#ifndef CAMERA_H
#define CAMERA_H

typedef struct {
    int x, y;
} vec2_t;

typedef struct {
    vec2_t pos;
    vec2_t dir;
    vec2_t plane;

    float fov;
} camera_t;

void camera_init(camera_t *, int x, int y);

#endif
