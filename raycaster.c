#include "raycaster.h"

sdl_t sdl = {0};
config_t config = {0};
camera_t cam = {0};
map_t map = {0};

bool raycaster_init() {
    
    /* Initialize every component */
    if (!config_init(&config)) return false;
    if (!sdl_init(&sdl, &config)) return false;
    
    camera_init(&cam, 50, 50);
    
    map_init(&map, 0, 0, config.window_h);

    return true;
}

bool raycaster_start() {
    /* main loop here */
    bool running = true;

    uint32_t last_time = SDL_GetTicks();

    uint32_t current_time;
    float delta_time;

    /* storing camera position in float value to use normalized movement */
    vec2f_t f_cam_pos = {
        .x = (float) cam.pos.x,
        .y = (float) cam.pos.y
    };

    /* Main loop here */
    while (running)  {
        
        current_time = SDL_GetTicks();
        delta_time = (current_time - last_time) / 1000.0f; // convert ms to sec
        last_time = current_time;

        SDL_Event event = sdl_get_input(); 
        switch(event.type) {
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    case SDLK_w: cam.up = true; break;
                    case SDLK_a: cam.left = true; break;
                    case SDLK_s: cam.down = true; break;
                    case SDLK_d: cam.right = true; break;
                    case SDLK_q: running = false; break;
                }
                break;
            case SDL_KEYUP:
                switch(event.key.keysym.sym) {
                    case SDLK_w: cam.up = false; break;
                    case SDLK_a: cam.left = false; break;
                    case SDLK_s: cam.down = false; break;
                    case SDLK_d: cam.right = false; break;
                }
                break;
            case SDL_QUIT:
                running = false;
                break;
        }
        
        // move camera
        if (cam.up)    f_cam_pos.y -= cam.speed * delta_time;
        if (cam.down)  f_cam_pos.y += cam.speed * delta_time;
        if (cam.left)  f_cam_pos.x -= cam.speed * delta_time;
        if (cam.right) f_cam_pos.x += cam.speed * delta_time;
        
        cam.pos.x = (int) f_cam_pos.x;
        cam.pos.y = (int) f_cam_pos.y;

        /* render */
        sdl_clear_screen(&sdl, 0x000000);

        sdl_render_map(&sdl, &config, &map);

        sdl_render_camera(&sdl, cam.pos.x, cam.pos.y);

        // perform DDA
        DDA();

        sdl_present_result(&sdl);

    } 
}

bool raycaster_quit() {
    sdl_quit(&sdl);
}

void DDA() {
    vec2_t mouse_coords = {0}; // this will contain x1, y1
    SDL_GetMouseState(&mouse_coords.x, &mouse_coords.y);

    vec2_t dir = {0};
    dir.x = mouse_coords.x - cam.pos.x;
    dir.y = mouse_coords.y - cam.pos.y;

    /* since our coordinate system has opposite y axis direction
     * but normal degrees directions we use - sign in atan2 function
     * atan2 function only gives positive numbers when the range is 0-PI
     * so when angle is < 0 we add 2PI (360 degrees) to it */
    double angle = -atan2((double)dir.y, (double)dir.x); // angle in radians
    if (angle <= 0) angle += 2 * M_PI;

    /* AP will be our horizontal line intersection point
     * with Xa and Ya being its increment each time there's no hit */
    vec2_t AP = {0};
    int AP_Xa;
    int AP_Ya;

    /* BP will be vertical line intersection point */
    vec2_t BP = {0};
    int BP_Xa;
    int BP_Ya;

    // calculate AP.y and AP_Ya
    if (angle >= 0 && angle <= M_PI) {
        AP.y = (cam.pos.y / map.pps) * map.pps - 1;
        AP_Ya = -map.pps;
    }
    else {
        AP.y = (cam.pos.y / map.pps) * map.pps + map.pps;
        AP_Ya = map.pps;
    }
    
    // calculate AP_Xa
    AP_Xa = -AP_Ya / tan(angle);
    
    // calculate AP.x
    AP.x = cam.pos.x + (cam.pos.y - AP.y) / tan(angle);
    
    // calculate BP.x and BP_Xa
    if (angle > M_PI/2 && angle < (3*M_PI)/2) {
        // if ray facing left
        BP.x = (cam.pos.x / map.pps) * map.pps - 1;
        BP_Xa = -map.pps;
    }
    else {
        BP.x = (cam.pos.x / map.pps) * map.pps + map.pps;
        BP_Xa = map.pps;
    }
    
    // calculate BP_Ya
    BP_Ya = -BP_Xa * tan(angle);

    // find BP.y
    BP.y = cam.pos.y + (cam.pos.x - BP.x) * tan(angle);

    // current checking map tile
    vec2_t checking_map_tile = {0};
    checking_map_tile.x = AP.x / map.pps;
    checking_map_tile.y = AP.y / map.pps;

    // find horizonal wall hit
    bool h_hit = false;
    while (!h_hit && checking_map_tile.x >= 0 && checking_map_tile.x < map.width &&
            checking_map_tile.y >= 0 && checking_map_tile.y < map.height) {
        if (map.map_data[checking_map_tile.x][checking_map_tile.y] == 1) {
            h_hit = true;
        }
        else {
            AP.x += AP_Xa;
            AP.y += AP_Ya;
            checking_map_tile.x = AP.x / map.pps;
            checking_map_tile.y = AP.y / map.pps;
        }
    }
    
    checking_map_tile.x = BP.x / map.pps;
    checking_map_tile.y = BP.y / map.pps;

    // find vertical wall hit
    bool v_hit = false;
    while (!v_hit && checking_map_tile.x >= 0 && checking_map_tile.x < map.width &&
            checking_map_tile.y >= 0 && checking_map_tile.y < map.height) {
            
        if (map.map_data[checking_map_tile.x][checking_map_tile.y] == 1) {
            v_hit = true;
        }
        else {
            BP.x += BP_Xa;
            BP.y += BP_Ya;
            checking_map_tile.x = BP.x / map.pps;
            checking_map_tile.y = BP.y / map.pps;
        }
    }

    vec2_t ray_length_1d = {0};
    if (h_hit)
        ray_length_1d.x = abs(sqrt(pow(cam.pos.x - AP.x, 2) + pow(cam.pos.y - AP.y, 2)));
    else
        ray_length_1d.x = 999;

    if (v_hit)
        ray_length_1d.y = abs(sqrt(pow(cam.pos.x - BP.x, 2) + pow(cam.pos.y - BP.y, 2)));
    else 
        ray_length_1d.y = 999;

    if (ray_length_1d.x < ray_length_1d.y)
        sdl_render_ray(&sdl, &config, cam.pos.x, cam.pos.y, AP.x, AP.y);
    else
        sdl_render_ray(&sdl, &config, cam.pos.x, cam.pos.y, BP.x, BP.y);
}

