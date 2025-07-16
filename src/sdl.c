#include "sdl.h"

bool config_init(config_t *config, int argc, char **argv) {
    // set defaults
    config->title = "RayCaster Engine";
    config->window_x = SDL_WINDOWPOS_CENTERED;
    config->window_h = SDL_WINDOWPOS_CENTERED;
    config->window_w = 1366;
    config->window_h = 768;
    
    // colors
    config->walls_color = 0xFF000000; // red 
    config->rays_color = 0xFFFF0000; // yellow
    config->walls_3d_color = 0x00900000; // green
    config->walls_side_3d_color = 0x00700000; // dark green
    config->floor_color = 0x65432100; // brown
    
    // other
    config->num_of_rays = DEFAULT_RAYS_NUM; 
    config->pixel_outlines = true; 
    config->show_cursor = true;
    
    // set flags
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-rn", strlen("-rn")) == 0) config->num_of_rays = strtoul(argv[++i], NULL, 10);
    }

    return true;
}

bool sdl_init(sdl_t *sdl, config_t *config) {
    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    /* Initialize window */
    sdl->window = SDL_CreateWindow(config->title,
                                   config->window_x,
                                   config->window_y,
                                   config->window_w,
                                   config->window_h,
                                   SDL_WINDOW_RESIZABLE);

    if (!sdl->window) {
        SDL_Log("Unable to initialize SDL window: %s\n", SDL_GetError());
        return false;
    }

    /* Initialize renderer */
    sdl->renderer = SDL_CreateRenderer(sdl->window,
                                      -1,
                                      SDL_RENDERER_ACCELERATED); 

    if (!sdl->renderer) {
        SDL_Log("Unable to initialize SDL window: %s\n", SDL_GetError());
        return false;
    }

    if (!config->show_cursor)
        SDL_ShowCursor(SDL_DISABLE);


    /* Load Textures */
    sdl_texture_loader_init();
    if (!textures_load(sdl->renderer)) {
        return false;
    }


    
    return true;
}

bool sdl_quit(sdl_t *sdl) {
    /* Destroy SDL components */
    textures_destroy();
    SDL_DestroyRenderer(sdl->renderer);
    SDL_DestroyWindow(sdl->window);

    SDL_Quit();
    return true;
}

SDL_Event sdl_get_input() {
    SDL_Event e;

    while (SDL_PollEvent(&e)) {
        return e;
    }
}

void sdl_on_resize(sdl_t *sdl, config_t *config, map_t *map) {
    int new_w, new_h;
    SDL_GetWindowSize(sdl->window, &new_w, &new_h);
    config->window_w = new_w;
    config->window_h = new_h;

    map->pps = config->window_h / map->height;
}

void sdl_tick_cursor(config_t *config) {
    config->show_cursor = !config->show_cursor;
    if (config->show_cursor) SDL_ShowCursor(SDL_ENABLE);
    else SDL_ShowCursor(SDL_DISABLE);
}

void sdl_set_cursor(config_t *config, bool value) {
    config->show_cursor = value;
    if (config->show_cursor) SDL_ShowCursor(SDL_ENABLE);
    else SDL_ShowCursor(SDL_DISABLE);
}

void sdl_clear_screen(sdl_t *sdl, uint32_t color) {
    uint8_t r = (color >> 24) & 0xFF;
    uint8_t g = (color >> 16) & 0xFF;
    uint8_t b = (color >> 8) & 0xFF;
    uint8_t a = (color >> 0) & 0xFF;

    SDL_SetRenderDrawColor(sdl->renderer, r, g, b, a);
    SDL_RenderClear(sdl->renderer);
}


void sdl_present_result(sdl_t *sdl) {
    SDL_RenderPresent(sdl->renderer);
}

bool sdl_render_camera(sdl_t *sdl, int x, int y) {
    SDL_SetRenderDrawColor(sdl->renderer, 46, 204, 113, 0);
    SDL_Rect rect = {x, y, 10, 10};
    SDL_RenderFillRect(sdl->renderer, &rect);

    return true;
}

void sdl_render_map(sdl_t *sdl, config_t *config, map_t *map) {
    SDL_Rect rect = {0, 0, map->pps, map->pps};

    // get walls rgba
    uint8_t r = (config->walls_color >> 24) & 0xFF;
    uint8_t g = (config->walls_color >> 16) & 0xFF;
    uint8_t b = (config->walls_color >> 8) & 0xFF;
    uint8_t a = (config->walls_color >> 0) & 0xFF;

    for (int i = 0; i < map->height; i++) {
        for (int j = 0; j < map->width; j++) {
            if (map->map_data[i][j] == 1) {
                SDL_SetRenderDrawColor(sdl->renderer, r, g, b, a);
                SDL_RenderFillRect(sdl->renderer, &rect);
            }
            else {
                SDL_SetRenderDrawColor(sdl->renderer, 0, 0, 0, 0);
                SDL_RenderFillRect(sdl->renderer, &rect);
            }

            if (config->pixel_outlines) {
                // render pixel outlines
                SDL_SetRenderDrawColor(sdl->renderer, 255, 255, 255, 0);
                SDL_RenderDrawRect(sdl->renderer, &rect);
            }
            
            rect.y += map->pps;
        }
        rect.x += map->pps;
        rect.y = 0;
    }
}

bool sdl_render_map_editor(sdl_t* sdl, config_t *config, map_t *map) {
    sdl_render_map(sdl, config, map);

    int texture_count = texture_get_count();
    if (texture_count <= 0) {
        printf("No textures loaded\n");
        return false;
    }

    int screen_x = map->width * map->pps + 1, screen_y = 1;
    SDL_Rect src;
    SDL_Rect dst = {screen_x, screen_y, 0, 0};

    /* Render Textures */
    for (int i = 0; i < texture_count; i++) {
        SDL_Texture *t = texture_get(i);
        src = (SDL_Rect) {0, 0, texture_get_w(i), texture_get_h(i)}; 
        dst = (SDL_Rect) {screen_x, screen_y, texture_get_w(i), texture_get_h(i)};

        SDL_RenderCopy(sdl->renderer, t, &src, &dst);

        screen_x += texture_get_w(i) + 1;
        if (screen_x > config->window_w) {
            screen_x = map->width * map->pps + 1;
            screen_y += texture_get_h(1) + 1;
        }
    }
}

void sdl_render_ray(sdl_t *sdl, config_t *config, int x0, int y0, int x1, int y1) {
    uint8_t r = (config->rays_color >> 24) & 0xFF;
    uint8_t g = (config->rays_color >> 16) & 0xFF;
    uint8_t b = (config->rays_color >> 8) & 0xFF;
    uint8_t a = (config->rays_color >> 0) & 0xFF;

    SDL_SetRenderDrawColor(sdl->renderer, r, g, b, a);

    // render ray
    SDL_RenderDrawLine(sdl->renderer, x0 + 5, y0 + 5, x1, y1);
}


void sdl_render_floor(sdl_t *sdl, config_t *config, int x, int y, int w, int h) {

    // render floor 
    SDL_Rect floor = {
        .x = x,
        .y = y + h,
        .w = w,
        .h = config->window_h - h - y 
    };

    uint8_t r = (config->floor_color >> 24) & 0xFF;
    uint8_t g = (config->floor_color >> 16) & 0xFF;
    uint8_t b = (config->floor_color >> 8) & 0xFF;
    uint8_t a = (config->floor_color >> 4) & 0xFF; 
    
    SDL_SetRenderDrawColor(sdl->renderer, r, g, b, a);
    SDL_RenderFillRect(sdl->renderer, &floor);
    
    if (config->pixel_outlines) {
        SDL_SetRenderDrawColor(sdl->renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(sdl->renderer, &floor);
    }
}

void sdl_render_col(sdl_t *sdl, config_t *config, int x, int y, int w, int h, bool side) {
    
    uint32_t color = side == true ? config->walls_3d_color : config->walls_side_3d_color;

    SDL_Rect col = {
        .x = x,
        .y = y,
        .w = w,
        .h = h
    };

    uint8_t r = (color >> 24) & 0xFF;
    uint8_t g = (color >> 16) & 0xFF;
    uint8_t b = (color >> 8) & 0xFF;
    uint8_t a = (color >> 4) & 0xFF; 
    
    SDL_SetRenderDrawColor(sdl->renderer, r, g, b, a);
    SDL_RenderFillRect(sdl->renderer, &col);

    if (config->pixel_outlines) {
       SDL_SetRenderDrawColor(sdl->renderer, 255, 255, 255, 255);
       SDL_RenderDrawRect(sdl->renderer, &col);
    }

    // render floor 
    sdl_render_floor(sdl, config, x, y, w, h);

    // render ceiling
    SDL_Rect ceiling = {
        .x = x,
        .y = 0,
        .w = w,
        .h = y
    };
}

void sdl_render_textured_col(sdl_t *sdl, config_t *config, int x, int y, int w, int h, float wall_offset, int texture_id) {
    SDL_Texture *t = texture_get(texture_id);
    int texture_x = (int) (wall_offset * texture_get_w(texture_id));
    int texture_sample_width = ceil((float)texture_get_w(texture_id) / (float) config->num_of_rays); 

    SDL_Rect src = {texture_x, 0, texture_sample_width, texture_get_h(texture_id)};
    SDL_Rect dst = {x, y, w, h};
    
    SDL_RenderCopy(sdl->renderer, t, &src, &dst);
}

