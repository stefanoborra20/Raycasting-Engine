#ifndef TEXTURES_H
#define TEXTURES_H

#include <stdbool.h>
#include <dirent.h>
#include <SDL.h>
#include <SDL_image.h>

#define RES_DIR "res"
#define MAX_TEXTURES 50

typedef struct {
    int id;
    int w, h;
    SDL_Texture *t;
} texture_container_t;

bool sdl_texture_loader_init(); 

bool textures_load(SDL_Renderer*);

bool texture_load(SDL_Renderer *, const char *, int);

SDL_Texture* texture_get(int);

int texture_get_w(int);

int texture_get_h(int);

void textures_destroy();

int texture_get_count();

#endif

