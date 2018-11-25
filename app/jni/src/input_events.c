#include "input_events.h"

uint32_t tmp_time_diff;

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
                switch (events->window.event)
                {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        state->display.w = events->window.data1;
                        state->display.h = events->window.data2;
                        calc_scaling(state);
                        calc_rendering_areas(state);
                        break;
                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        state->pause = SDL_TRUE;
                        break;
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        tmp_time_diff = SDL_GetTicks() - state->time_last_check_tick;
                        state->time_last_check_tick += tmp_time_diff;
                        state->time_last_car_gen += tmp_time_diff;
                        state->time_last_second_tick += tmp_time_diff;
                        state->pause = SDL_FALSE;
                    default:
                        break;
                }
            case SDL_FINGERDOWN:
                if (state->end)
                {
                    state->quit = SDL_TRUE;
                    return 0;
                }
                state->e_touch_processed = SDL_FALSE;
                state->e_touch = events->tfinger;

                float e_touch_rel_x = state->e_touch.x * state->display.w;
                float pos_pause_btn_left =
                        (LEFT_HANDED == (state->hand & LEFT_HANDED)) ?
                        state->play_area.x :
                        state->play_area.x + 2 * state->road_size * state->scaling_mode;
                float pos_pause_btn_right =
                        (LEFT_HANDED == (state->hand & LEFT_HANDED)) ?
                        state->play_area.x + 2 * state->road_size * state->scaling_mode :
                        state->play_area.x + 3 * state->road_size * state->scaling_mode;

                if (0 < state->e_touch.y * state->display.h &&
                    state->road_size * state->scaling_mode > state->e_touch.y * state->display.h &&
                    pos_pause_btn_left < e_touch_rel_x &&
                    e_touch_rel_x < pos_pause_btn_right)
                {
                    if (state->pause)
                    {
                        tmp_time_diff = SDL_GetTicks() - state->time_last_check_tick;
                        state->time_last_check_tick += tmp_time_diff;
                        state->time_last_car_gen += tmp_time_diff;
                        state->time_last_second_tick += tmp_time_diff;
                        state->pause = SDL_FALSE;
                    }
                    else
                    {
                        state->pause = SDL_TRUE;
                        // storing difference between last second tick and current tick at the
                        // moment of pausing
                    }
                }
                break;
            default:
                break;
        }
    }
    return 0;
}
