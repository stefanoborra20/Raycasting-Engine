#include "map.h"

void map_init(map_t *map, int w, int h, int window_h) {
    if (w == 0 || h == 0) {
        w = MAP_DEFAULT_WIDTH;
        h = MAP_DEFAULT_HEIGHT;
    }

    map->width = w;
    map->height = h;

    map->pps = window_h / map->height;

    map->map_data = (int **) malloc(h * sizeof(int*));
    for (int i = 0; i < h; i++) map->map_data[i] = (int *) malloc(w * sizeof(int));

    /* fill borders */
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            if (i == 0 || i == h - 1 || j == 0 || j == w - 1)
                map->map_data[j][i] = 1;
            else
                map->map_data[j][i] = 0;
        }
    }
}

bool load_map_from_file(char *path) {
    // TODO
}
