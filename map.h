#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <stdlib.h>

#define MAP_DEFAULT_WIDTH 20
#define MAP_DEFAULT_HEIGHT 20

typedef struct {
    int width, height;
    int pps; // pixel per square
    int **map_data;
} map_t;

void map_init(map_t*, int w, int h, int window_h);

bool load_map_from_file(char *path);

#endif
