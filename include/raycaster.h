#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <math.h>
#include <limits.h>
#include "utils.h"
#include "sdl.h"
#include "camera.h"
#include "map.h"

#ifndef M_PI
    #define M_PI 3.142592
#endif

#define ONE_RADIANT 0.0174533

#define MODE_2D 0
#define MODE_3D 1
#define MODE_MAP_EDITOR 3

typedef struct {
    int w, h;
    int width_per_line;
    double angle_between_rays;
} projection_plane_t;

bool raycaster_init(int, char **);

bool raycaster_start();

bool raycaster_quit();

void DDA();

#endif
