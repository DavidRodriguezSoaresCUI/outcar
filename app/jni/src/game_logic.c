#include "game_logic.h"

// the game logic function, used in main loop, calculates changes for everything
// based on time passed between frames
// dynamic memory allocation system for opponent cars (cars only limited by RAM)
int game_logic(info_exchange *state, double *partial_scroll_state)
{
    int lane_relative_width = state->road_size * state->scaling_mode;

    // handling touch event
    touch_event_handler(state, lane_relative_width);

    // new frame description calculation
    opp_car_list *curr_el = state->opp_cars;
    opp_car_list *prev_el = NULL;
    // scroll calculation for cars & road
    // |--> modulus is used because scroll pos. 0 == scroll pos. (dim. of tile)
    uint32_t time_curr = SDL_GetTicks();
    uint32_t time_diff = time_curr - state->time_last_check_tick;

    double opp_new_add_pos = 0;
    double player_new_add_pos = 0;

    // refueling mechanic
    if (state->refueling)
    {
        if (state->fuel + 30 * (double) time_diff / 1000 < state->max_fuel)
            state->fuel += 30 * (double) time_diff / 1000;
        else
        {
            state->fuel = state->max_fuel;
            state->refueling = SDL_FALSE;
        }
        calc_fuel_pointer_position(state);
    }

    if (state->fuel_countdown)
    {
        if (state->fuel_up_msg_time_left > time_diff)
            state->fuel_up_msg_time_left -= time_diff;
        else state->fuel_countdown = SDL_FALSE;
    }

    // adding scroll/movement values (only if player has fuel!)
    if (state->fuel)
    {
        // vertical movement

        // scroll calc. for cars
        opp_new_add_pos = 120 * ((double) time_diff / 1000.0) * ((double) state->rel_speed / 100.0);
        *partial_scroll_state += 2 * opp_new_add_pos; // scroll calc. for road
        int goal_pos = state->scaling_mode * 10 +
                       lane_relative_width * state->player_car_pos_logical;
        // horizontal movement
        if (state->player_car_pos_renderer != goal_pos)
        {
            player_new_add_pos = 20 * opp_new_add_pos; // 16!
            if (state->player_car_pos_renderer < goal_pos)
            {
                if (state->player_car_pos_renderer + player_new_add_pos >= goal_pos)
                {
                    state->player_car_pos_renderer = goal_pos;
                }
                else
                {
                    state->player_car_pos_renderer += player_new_add_pos;
                }
            }
            else
            {
                if (state->player_car_pos_renderer + player_new_add_pos <= goal_pos)
                {
                    state->player_car_pos_renderer = goal_pos;
                }
                else
                {
                    state->player_car_pos_renderer -= player_new_add_pos;
                }
            }
        }
    }
    if (*partial_scroll_state >= (double) state->road_size)
    {
        *partial_scroll_state = fmod(*partial_scroll_state,
                                     (double) state->road_size); // modulus calculation
    }
    state->scroll_state = (uint8_t) round(*partial_scroll_state);
    // adding position changes & removing cars that are past the screen & adding new cars
    while (curr_el != NULL)
    {
        // removing cars
        if (curr_el->pos >= 600)
        {
            if (curr_el == state->opp_cars) state->opp_cars = NULL;
            else if (prev_el != NULL) prev_el->next_el = NULL;

            if (state->player_hit || time_curr - state->time_game_start < 1000)
                state->player_hit = SDL_FALSE;
            else
            {
                SCORE_AVOID(state)
            }

            free_slots(curr_el);
            curr_el = NULL;
        }
        else
        {
            // new opponent line (cars) generation
            if (3000 < time_curr - state->time_last_car_gen)
            {
                generate_new_opp_car_line(state);
            }
            if (state->player_hit == SDL_FALSE && curr_el->pos >= 600 - 2 * state->car_size &&
                curr_el->gap != state->player_car_pos_logical)
            {
                player_collision(state);
            }

            curr_el->pos += opp_new_add_pos;
            prev_el = curr_el;
            curr_el = curr_el->next_el;
        }
    }
    // if there's EVER a case where there's absolutely NO cars then we generate one forcibly
    if (state->opp_cars == NULL)
    {
        // new opponent line (cars) generation
        generate_new_opp_car_line(state);
    }

    // updating time value in state
    state->time_last_check_tick = time_curr;

    // per second events
    if (time_curr - state->time_last_second_tick >= 1000)
    {
        // fuel
        if (state->need_to_refuel)
        {
            if (state->fuel > MIN_FUEL + 1)
            {
                state->fuel--;
#ifdef DISPLAY_DEBUG_MSG
                char tmp[100];
                sprintf( tmp, "fuel:%03d", state->fuel );
                push_string_linked( &(state->debug_messages), tmp );
#endif
            }
            else
                out_of_fuel(state);
        }

        calc_fuel_pointer_position(state);

        // remaining time update
        if (state->time_left > (time_curr - state->time_last_second_tick))
        {
            state->time_left = state->time_left + state->time_last_second_tick - time_curr;
#ifdef DISPLAY_DEBUG_MSG
            char tmp[20];
            sprintf(tmp, "timer: %d", state->time_left);
            push_string_linked(&(state->debug_messages), tmp);
#endif
        }
        else
        {
            state->time_left = 0;
            state->pause = SDL_TRUE;
            state->end = SDL_TRUE;
            state->time_game_end = SDL_GetTicks();
            show_end_screen(NULL, NULL, 0, SDL_TRUE); // Necessary to bugfix (see implementation)
        }
        // |-> for the renderer
        uint8_t temp_min = (uint8_t) (state->time_left / 60000);
        uint8_t temp_sec = (uint8_t) ((state->time_left / 1000) % 60);

        if (temp_min < 100)
            sprintf(state->numeric_clock, "%02d:%02d", temp_min, temp_sec);
        else
            sprintf(state->numeric_clock, "99:59");

        // updating timer
        state->time_last_second_tick = time_curr;

        // auto pause audio device
        auto_pause_sfx(state->audio_device_id);
    }

    return 0;
}

void touch_event_handler(info_exchange *state, int lane_relative_width)
{
    int touch_relative_x = (int) (state->e_touch.x * state->display.w);
    int touch_relative_y = (int) (state->e_touch.y * state->display.h);

    // handling touch event
    if (!state->e_touch_processed)
    {
        if (!state->e_touch_processed &&
            touch_relative_x > state->play_area.x &&
            touch_relative_x < state->play_area.x + state->play_area.w &&
            touch_relative_y < state->play_area.y + state->play_area.h &&
            touch_relative_y > state->play_area.h + state->play_area.y - lane_relative_width
                )
        {
            // A touch event happened on a 'lane_touch_guide'
            if (touch_relative_x < lane_relative_width)
            {
                // Left lane selected
                state->player_car_pos_logical = LEFT_LANE;
            }
            else if (
                    touch_relative_x >= lane_relative_width &&
                    touch_relative_x <= lane_relative_width * 2
                    )
            {
                // Middle lane selected
                state->player_car_pos_logical = MIDDLE_LANE;
            }
            else
            {
                // Right lane selected
                state->player_car_pos_logical = RIGHT_LANE;
            }
            state->e_touch_processed = SDL_TRUE;
        }
        else if (touch_relative_y > state->menu_area.y &&
                 touch_relative_y < state->menu_area.y + state->menu_area.h)
        {
            // A touch event happened on the fuel control area
            int touch_x_handed = touch_relative_x;
            if (state->hand == RIGHT_HANDED) // horizontal flip for handedness
                touch_x_handed = state->menu_area.w - touch_relative_x;

            if (touch_x_handed > state->menu_area.x &&
                touch_x_handed < state->menu_area.x + state->menu_area.w - 3 * state->menu_area.h &&
                state->need_to_refuel)
            {
                // A touch event happened on the 'refuel' button
                manual_refuel(state);
                state->e_touch_processed = SDL_TRUE;
            }
            else if (
                    touch_x_handed > state->menu_area.x + state->menu_area.h &&
                    touch_x_handed <
                    state->menu_area.x + state->menu_area.w - 2 * state->menu_area.h &&
                    state->need_to_refuel)
            {
                // A touch event happened on the 'show' button
                show_fuel(state);
                state->e_touch_processed = SDL_TRUE;
            }
        }

    }
}

void calc_fuel_pointer_position(info_exchange *state)
{
    state->fuel_pointer_position =
            (uint8_t) round( state->fuel / (state->max_fuel) * 132.0);
}

// freeing up memory starting from a point in the chained list until the end
void free_slots(opp_car_list *in)
{
    if (in != NULL)
    {
        free_slots(in->next_el);
        free(in);
    }
}

void player_collision(info_exchange *state)
{
    state->player_hit = SDL_TRUE;
    SCORE_CRASH(state)

    push_uint16_linked(state->hit_times, get_timer(state));
    (state->current_texture_fx).texture = FX_CRASH;
    (state->current_texture_fx).end_timestamp = state->time_last_check_tick + 2000;
    // play crash sound effect
    if (play_sfx(state->audio_device_id, state->sfx_wav_length, state->sfx_wav_buffer) != 0)
    {
        log_SDL_error("play sfx error");
        state->quit = SDL_TRUE;
    }
}

uint16_t get_timer(info_exchange *state)
{
    return (uint16_t) floor((state->time_last_check_tick - state->time_game_start) / 1000.0);
}

void out_of_fuel(info_exchange *state)
{
    state->fuel = MIN_FUEL;
    state->refueling = SDL_TRUE;
    push_uint16_linked(state->auto_refuel_times, get_timer(state));
    (state->current_texture_fx).texture = FX_NO_FUEL;
    (state->current_texture_fx).end_timestamp = state->time_last_check_tick + 2000;

    SCORE_NOFUEL(state)
}

void manual_refuel(info_exchange *state)
{
    if (state->fuel <= state->max_fuel * 0.25)
    {
        SCORE_REFUEL(state)
        state->refueling = SDL_TRUE;
        state->fuel_countdown = SDL_TRUE;
        state->fuel_up_msg_time_left = 3000;
        push_uint16_linked(state->refuel_times, get_timer(state));
    }
    else
    {
        push_uint16_linked(state->void_times, get_timer(state));
    }
}

void show_fuel(info_exchange *state)
{
    push_uint16_linked(state->show_times, get_timer(state));
    state->time_last_show_fuel_tick = SDL_GetTicks();
}

void generate_new_opp_car_line(info_exchange *state)
{
    opp_car_list *new_opp_car_line = (opp_car_list *) malloc(sizeof(opp_car_list));

    new_opp_car_line->gap = (uint8_t) (rand() % 3);
    new_opp_car_line->pos = -state->car_size;
    new_opp_car_line->car_design[0] = (uint8_t) (rand() % 4);
    new_opp_car_line->car_design[1] = (uint8_t) (rand() % 4);
    new_opp_car_line->next_el = state->opp_cars;

    state->opp_cars = new_opp_car_line;
    state->time_last_car_gen = SDL_GetTicks();
}
