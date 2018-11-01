#ifndef INPUT_EV
    #define INPUT_EV

    #include <stdlib.h>
    #include <SDL.h>
    #include "info_struct.h"
    #include "renderer.h"

    int input_events ( info_exchange* state, SDL_Event* events );

#endif
