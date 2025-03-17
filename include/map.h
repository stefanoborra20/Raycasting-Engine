#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <stdlib.h>

#define MAP_DEFAULT_WIDTH 20
#define MAP_DEFAULT_HEIGHT 20
#define MAP_DEFAULT_WALL_HEIGHT_3D 64

typedef struct {
    int width, height;
    int pps; // pixels per square
    int **map_data;

    int wall_h_3d;
} map_t;

void map_init(map_t*, int w, int h, int window_h);

bool load_map_from_file(char *path);

bool map_put_wall_at(map_t *, int, int);

bool map_remove_wall_at(map_t *, int, int);

#endif
