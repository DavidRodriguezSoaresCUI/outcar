#include "input_events.h"

int input_events(info_exchange *state, SDL_Event *events)
{
    while (SDL_PollEvent(events) != 0)
    {
        switch (events->type)
        {
            case SDL_QUIT:
                state->quit = SDL_TRUE;
                break;
            case SDL_KEYDOWN:
                switch (events->key.keysym.sym)
                {
                    case SDLK_LEFT:
                        if (state->player_car_pos_logical > 0)
                            state->player_car_pos_logical--;
                        break;
                    case SDLK_RIGHT:
                        if (state->player_car_pos_logical < 2)
                            state->player_car_pos_logical++;
                        break;
                    case SDLK_p:
                        if (state->pause)
                            state->pause = SDL_FALSE;
                        else
                            state->pause = SDL_TRUE;
                        break;
                    case SDLK_ESCAPE:
                        state->quit = SDL_TRUE;
                        break;
                    case SDLK_AC_BACK: // When the 'back' key is pressed, we quit to menu
                        state->quit = SDL_TRUE;
                        break;
                    default:
                        break;
                }
            case SDL_WINDOWEVENT:
                if (events->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    state->display.w = events->window.data1;
                    state->display.h = events->window.data2;
                    calc_scaling(state);
                    calc_rendering_areas(state);
                }
            case SDL_FINGERDOWN:
                if(state->end)
                {
                    state->quit = SDL_TRUE;
                    return 0;
                }
                state->e_touch_processed = SDL_FALSE;
                state->e_touch = events->tfinger;

                float e_touch_rel_x = state->e_touch.x * state->display.w;
                float pos_pause_btn_left =
                        ( LEFT_HANDED == (state->hand & LEFT_HANDED) ) ?
                            state->play_area.x :
                            state->play_area.x + 2 * state->road_size * state->scaling_mode;
                float pos_pause_btn_right =
                        ( LEFT_HANDED == (state->hand & LEFT_HANDED) ) ?
                            state->play_area.x + 2 * state->road_size * state->scaling_mode :
                            state->play_area.x + 3 * state->road_size * state->scaling_mode;

                if (0 < state->e_touch.y * state->display.h &&
                    state->road_size * state->scaling_mode > state->e_touch.y * state->display.h &&
                    pos_pause_btn_left < e_touch_rel_x &&
                    e_touch_rel_x < pos_pause_btn_right )
                {
                    if (state->pause)
                    {
                        state->pause = SDL_FALSE;
                        // restoring difference from when we paused
                        state->time_last_second_tick =
                                state->time_last_check_tick - state->time_last_second_tick;
                    } else
                    {
                        state->pause = SDL_TRUE;
                        // storing difference between last second tick and current tick at the
                        // moment of pausing
                        state->time_last_second_tick =
                                state->time_last_check_tick - state->time_last_second_tick;
                    }
                }
                break;
            default:
                break;
        }
    }
    return 0;
}
