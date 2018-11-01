#ifndef OUTCAR_ANDROID_0_3_MAIN_H
    #define OUTCAR_ANDROID_0_3_MAIN_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <time.h>
    #include <math.h>
    #include <SDL.h>
    #include <SDL_image.h>
    #include "sdl2_func.h"
    #include "info_struct.h"
    #include "input_events.h"
    #include "game_logic.h"
    #include "renderer.h"
    #include "audio_sfx.h"
    #include "values.h"

    #define EXIT_ON_FAILURE IMG_Quit(); \
                            SDL_Quit(); \
                            return 1;
    #define NEWLINE(fileStream) SDL_RWwrite( fileStream, "\n", 1, 1 );

    void load_conf(info_exchange *state);

    void verify_conf(info_exchange *state);

    void log_results(const info_exchange state);

    int load_score(info_exchange *state);

#endif //OUTCAR_ANDROID_0_3_MAIN_H
