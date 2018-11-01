#ifndef RENDERER
    #define RENDERER

    #include <stdlib.h>
    #include <math.h>
    #include <SDL.h>
    #include <SDL_image.h>
    #include "info_struct.h"
    #include "sdl2_func.h"
    #include "../SDL2-2.0.8/include/SDL_blendmode.h"
    #include "values.h"

    void rescale_size(info_exchange* state);
    void calc_rendering_areas(info_exchange* state);
    int rendering_init_textures(SDL_Renderer* renderer, info_exchange* state);
    void textures_free();
    void rendering_state(info_exchange* state, SDL_Renderer* renderer);
    int show_fuel_gauge(info_exchange *state);
    int show_end_screen(info_exchange *state, SDL_Renderer *renderer, int scaled_road_size,
                        int reset );

#endif
