#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <math.h>
#include "utils.h"
#include "sdl.h"
#include "camera.h"
#include "map.h"

#ifndef M_PI
    #define M_PI 3.142592
#endif

#define ONE_RADIANT 0.0174533

bool raycaster_init();

bool raycaster_start();

bool raycaster_quit();

void DDA();

#endif
