#include "raycaster.h"

/* Globals */
sdl_t sdl = {0};
config_t config = {0};
camera_t cam = {0};
map_t map = {0};

projection_plane_t proj_plane = {0};

int mode;

bool raycaster_init(int argc, char **argv) {
    
    /* Initialize every component */
    if (!config_init(&config, argc, argv)) return false;
    if (!sdl_init(&sdl, &config)) return false;
    
    camera_init(&cam, 50, config.window_h / 2);
    
    map_init(&map, 0, 0, config.window_h);

    // default starting mode
    mode = MODE_2D;

    // init projection plane 
    proj_plane.w = config.window_w;
    proj_plane.h = config.window_h;
    proj_plane.width_per_line =  proj_plane.w / config.num_of_rays;
    proj_plane.angle_between_rays = (cam.fov / config.num_of_rays) * M_PI / 180.0f;

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
                    case SDLK_LSHIFT: cam.speed = DEFAULT_SHIFT_SPEED; break;
                       
                    case SDLK_v:
                        if (mode == MODE_MAP_EDITOR) {
                            mode = MODE_2D;
                            sdl_set_cursor(&config, true);
                        }
                        else if (mode == MODE_2D) {
                            mode = MODE_3D;
                            sdl_set_cursor(&config, false);
                        }
                        else {
                            mode = MODE_2D;
                            sdl_set_cursor(&config, true); 
                        }
                    break;

                    case SDLK_m: mode = MODE_MAP_EDITOR; sdl_set_cursor(&config, true); break;

                    case SDLK_p: config.pixel_outlines = !config.pixel_outlines; break;
                    case SDLK_q: running = false; break;
                }
                break;
            case SDL_KEYUP:
                switch(event.key.keysym.sym) {
                    case SDLK_w: cam.up = false; break;
                    case SDLK_a: cam.left = false; break;
                    case SDLK_s: cam.down = false; break;
                    case SDLK_d: cam.right = false; break;
                    case SDLK_LSHIFT: cam.speed = DEFAULT_SPEED; break;
                }
                break;
            case SDL_WINDOWEVENT:
                   if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        sdl_on_resize(&sdl, &config, &map);
                        
                        // resize projection plane attributes
                        proj_plane.w = config.window_w;
                        proj_plane.h = config.window_h;
                        proj_plane.width_per_line =  proj_plane.w / cam.fov;
                   }
                break;
            case SDL_QUIT:
                running = false;
                break;
        }
        
        // move camera
        if (mode == MODE_2D) {
            if (cam.up)    f_cam_pos.y -= cam.speed * delta_time;
            if (cam.down)  f_cam_pos.y += cam.speed * delta_time;
            if (cam.left)  f_cam_pos.x -= cam.speed * delta_time;
            if (cam.right) f_cam_pos.x += cam.speed * delta_time;
        }
        else if (mode == MODE_3D) {
            if (cam.up) { 
                f_cam_pos.x += (cos(cam.dir_angle) * cam.speed) * delta_time; 
                f_cam_pos.y -= (sin(cam.dir_angle) * cam.speed) * delta_time; 
            }
            if (cam.down) {
                f_cam_pos.x -= (cos(cam.dir_angle) * cam.speed) * delta_time; 
                f_cam_pos.y += (sin(cam.dir_angle) * cam.speed) * delta_time; 
            }
            if (cam.right) cam.dir_angle -= cam.rotation_speed * delta_time;
            if (cam.left)  cam.dir_angle += cam.rotation_speed * delta_time;
        }

        cam.pos.x = (int) f_cam_pos.x;
        cam.pos.y = (int) f_cam_pos.y;

        /* render */
        sdl_clear_screen(&sdl, 0x000000);

        if (mode == MODE_2D) {
            sdl_render_map(&sdl, &config, &map);
            sdl_render_camera(&sdl, cam.pos.x, cam.pos.y);
        }
        else if (mode == MODE_MAP_EDITOR) {
            sdl_render_map(&sdl, &config, &map);
            switch (event.type) {
                case SDL_MOUSEBUTTONDOWN:
                    int x, y;
                    SDL_GetMouseState(&x, &y);

                    if (event.button.button == SDL_BUTTON_LEFT) {
                        /* put wall where clicked */
                        map_put_wall_at(&map, x, y);
                    }
                    else if (event.button.button == SDL_BUTTON_RIGHT) {
                        /* remove wall where clicked */
                        map_remove_wall_at(&map, x, y);
                    } 
                    break;
            }
        }

        // perform DDA
        DDA();

        sdl_present_result(&sdl);
    } 
}

bool raycaster_quit() {
    sdl_quit(&sdl);
}


void DDA() {

    bool last_side;

    if (config.show_cursor) {
        vec2_t mouse_coords = {0};
        SDL_GetMouseState(&mouse_coords.x, &mouse_coords.y);

        vec2_t dir = {0};
        dir.x = mouse_coords.x - cam.pos.x;
        dir.y = mouse_coords.y - cam.pos.y;
        /* Since y-axis is inverted but angle system is regular we add
         * (-) to atan2 function, then because atan2 only gives numbers 
         * in the range of 0-PI and -PI-0 we add 2xPI when angle is <0 */
        cam.dir_angle = -atan2((double)dir.y, (double)dir.x); // angle in radiants
    }

    if (cam.dir_angle < 0) cam.dir_angle += 2 * M_PI;
    else if (cam.dir_angle > 2 * M_PI) cam.dir_angle -= 2 * M_PI;
    double angle = cam.dir_angle;
    angle += (cam.fov / 2) * ONE_RADIANT;
    if (angle > 2 * M_PI) angle -= 2 * M_PI;

    vec2f_t hp = {0}; // Horizontal intersection point
    vec2f_t vp = {0}; // Vertical intersection point
    float hp_xa, hp_ya;
    float vp_xa, vp_ya;

    for (int r = 0; r < config.num_of_rays; r++) {
        // Calculate horizontal point first xa and ya
        if (angle >= 0 && angle <= M_PI) {
            hp.y = (cam.pos.y / map.pps) * map.pps - 0.001f;
            hp_ya = -map.pps;
        } else {
            hp.y = (cam.pos.y / map.pps) * map.pps + map.pps;
            hp_ya = map.pps; 
        }

        hp_xa = -hp_ya / tan(angle);

        hp.x = cam.pos.x + (cam.pos.y - hp.y) / tan(angle);
        
        // Calculate vertical point first xa and ya
        if (angle >= M_PI/2 && angle <= (3*M_PI)/2) {
            vp.x = (cam.pos.x / map.pps) * map.pps - 0.001f;
            vp_xa = -map.pps;
        } else {
            vp.x = (cam.pos.x / map.pps) * map.pps + map.pps;
            vp_xa = map.pps;
        }

        vp_ya = -vp_xa * tan(angle);

        vp.y = cam.pos.y + (cam.pos.x - vp.x) * tan(angle);

        vec2_t map_tile = {0};
        map_tile.x = hp.x / map.pps;
        map_tile.y = hp.y / map.pps;
        
        // Find horizontal intersection
        bool h_hit = false;
        while (!h_hit && map_tile.x >= 0 && map_tile.x < MAP_DEFAULT_WIDTH &&
                map_tile.y >= 0 && map_tile.y < MAP_DEFAULT_HEIGHT) {
            if (map.map_data[map_tile.x][map_tile.y] == 1) {
                h_hit = true;
            } else {
                hp.x += hp_xa;
                hp.y += hp_ya;
                map_tile.x = hp.x / map.pps;
                map_tile.y = hp.y / map.pps;
            }
        }

        map_tile.x = vp.x / map.pps;
        map_tile.y = vp.y / map.pps; 

        // Find vertical intersection
        bool v_hit = false;
        while (!v_hit && map_tile.x >= 0 && map_tile.x < MAP_DEFAULT_WIDTH &&
               map_tile.y >= 0 && map_tile.y < MAP_DEFAULT_HEIGHT) {
            if (map.map_data[map_tile.x][map_tile.y] == 1) {
                v_hit = true;
            } else {
                vp.x += vp_xa;
                vp.y += vp_ya;
                map_tile.x = vp.x / map.pps;
                map_tile.y = vp.y / map.pps;
            }
        } 

        vec2f_t ray_length = {0};
        if (h_hit) ray_length.x = abs(sqrt(pow(cam.pos.x - hp.x, 2) + pow(cam.pos.y - hp.y, 2)));
        else       ray_length.x = INT_MAX;

        if (v_hit) ray_length.y = abs(sqrt(pow(cam.pos.x - vp.x, 2) + pow(cam.pos.y - vp.y, 2)));
        else       ray_length.y = INT_MAX;

        if (mode == MODE_2D) {
            if (ray_length.x < ray_length.y) sdl_render_ray(&sdl, &config, cam.pos.x, cam.pos.y, hp.x, hp.y);
            else                             sdl_render_ray(&sdl, &config, cam.pos.x, cam.pos.y, vp.x, vp.y);
        } 
        else if (mode == MODE_3D) {
            bool side;
            float distance;

            if (ray_length.x < ray_length.y) {
                distance = ray_length.x;
                side = false;
                last_side = side;
            } 
            else if (ray_length.y < ray_length.x) {
                distance = ray_length.y;
                side = true;
                last_side = side;
            }
            else {
                distance = ray_length.x;
                side = last_side;
            }

    #ifdef DEBUG
            printf("Side: %d\n", side);
    #endif

            // adjust fish-eye
            double adj_angle = cam.dir_angle - angle;
            if (adj_angle < 0) adj_angle += 2 * M_PI;
            if (adj_angle > 2 * M_PI) adj_angle -= 2 * M_PI;
            distance *= cos(adj_angle);

            double projected_wall_height = (map.wall_h_3d * proj_plane.w) / distance;
            int x = r * proj_plane.width_per_line;
            int y = (proj_plane.h / 2) - (projected_wall_height / 2);
            int w = proj_plane.width_per_line;
             
            // render column
            sdl_render_col(&sdl, &config, x, y, w, projected_wall_height, side);
        }

   #ifdef DEBUG
        printf("Camera Angle: %.2f\nRay Angle: %.2f\n"
                "Ray[%d]\nLength horizontal: %.2f\nLength vertical: %.2f\n\n", cam.dir_angle, angle, r, ray_length.x, ray_length.y);
        if (mode == MODE_2D) {
            if (h_hit) {
                SDL_SetRenderDrawColor(sdl.renderer, 0, 255, 0, 0);
                SDL_Rect r1 = {hp.x, hp.y, 5, 5};
                SDL_RenderFillRect(sdl.renderer, &r1);
            }
            if (v_hit) {
                SDL_SetRenderDrawColor(sdl.renderer, 0, 0, 255, 0);
                SDL_Rect r2 = {vp.x, vp.y, 5, 5};
                SDL_RenderFillRect(sdl.renderer, &r2); 
            } 
        } 
    #endif   

        // Increment angle for the next ray
        angle -= proj_plane.angle_between_rays;
        if (angle < 0) angle += 2 * M_PI;
    } 
}

