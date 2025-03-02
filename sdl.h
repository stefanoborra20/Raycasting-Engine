#ifndef SDL_H 
#define SDL_H

#include <stdbool.h>
#include <SDL.h>
#include "map.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} sdl_t;

typedef struct {
    char *title;
    int window_x, window_y;
    int window_w, window_h;

    // colors
    uint32_t walls_color; // wall color in 2D vision
    uint32_t rays_color; // rays color in 2D vision

    // others
    bool pixel_outlines;
    bool show_cursor;
} config_t;

bool config_init(config_t *);

bool sdl_init(sdl_t *, config_t *config);

bool sdl_quit(sdl_t *);

SDL_Event sdl_get_input();

//void sdl_on_resize(config_t*, map_t *);

// rendering
void sdl_clear_screen(sdl_t *, uint32_t);

void sdl_present_result(sdl_t *);

bool sdl_render_camera(sdl_t *, int, int);

void sdl_render_map(sdl_t *, config_t *, map_t *);

void sdl_render_ray(sdl_t *, config_t *, int, int, int, int);

#endif
