#include "textures.h"

texture_container_t textures[MAX_TEXTURES] = {0};
int texture_count = 0;

bool sdl_texture_loader_init() {
    // Initialize PNG loading
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("Unable to initialize SDL image:%s\n", SDL_GetError());
        return false;
    }
    return true;
}

bool textures_load(SDL_Renderer *renderer) {
   struct dirent *entry;
   int i = 0;

   DIR *dir = opendir(RES_DIR);
   if (!dir) {
       fprintf(stderr, "Failed to open directory: %s\n", RES_DIR);
       perror("opendir");
       return false;
   }

   while ((entry = readdir(dir)) != NULL) {
       if (strstr(entry->d_name, ".png")) {
           char full_path[256];
           snprintf(full_path, sizeof(full_path), "%s/%s", RES_DIR, entry->d_name);

           if (texture_load(renderer, full_path, i)) {
#ifdef DEBUG 
           printf("Loaded %s texture\n", full_path);
#endif
                ++i;
                texture_count++;
            } else {
                return false;
            }
       }
   }
   
   closedir(dir);


   SDL_Rect dst = {50, 50, 120, 120};
   SDL_RenderClear(renderer);
   SDL_RenderCopy(renderer, textures[0].t, NULL, &dst);
   SDL_RenderPresent(renderer);

   return true;
}

bool texture_load(SDL_Renderer *renderer, const char *path, int i) {
    SDL_Surface *surface = IMG_Load(path);
    if (!surface) {
        SDL_Log("Unable to load SDL surface:%s\n", SDL_GetError());
        return false;
    }

    // create texture from surface
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface); 
    if (!texture) {
        SDL_Log("Unable to create texture from surface:%s\n", SDL_GetError());
        // destroy loaded surface
        SDL_FreeSurface(surface);
        return false;
    }

    textures[i].id = i;
    textures[i].t = texture;

    SDL_FreeSurface(surface);
    return true;
}

SDL_Texture* texture_get(int id) {
    if (id > 0 && id < texture_count) {
        return textures[id].t;
    }

    return NULL;
}

void textures_destroy() {
    for (int i = 0; i < texture_count; i++) {
        SDL_DestroyTexture(textures[i].t);
    }
}

