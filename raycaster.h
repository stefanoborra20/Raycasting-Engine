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

bool raycaster_init();

bool raycaster_start();

bool raycaster_quit();

void DDA();

#endif
