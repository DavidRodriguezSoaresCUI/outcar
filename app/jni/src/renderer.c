#include "renderer.h"

// textures
// |--> Game textures
SDL_Texture *road_left   = NULL;
SDL_Texture *road_middle = NULL;
SDL_Texture *road_right  = NULL;
SDL_Texture *opp_car[4]  = {NULL};
SDL_Texture *player_car  = NULL;
// |--> UI textures
SDL_Texture *font             = NULL;
SDL_Texture *clock_texture    = NULL;
SDL_Texture *fuel_gauge       = NULL;
SDL_Texture *fuel_gauge_blank = NULL;
SDL_Texture *fuel_pointer     = NULL;
SDL_Texture *lane_touch_guide = NULL;
SDL_Texture *btn_fuel_refill  = NULL;
SDL_Texture *btn_fuel_gauge_show       = NULL;
SDL_Texture *action_button_replacement = NULL;
SDL_Texture *btn_pause        = NULL;
SDL_Texture *btn_play         = NULL;
SDL_Texture *score_background = NULL;
SDL_Texture *dark_fader   = NULL;
SDL_Texture *hit_fx       = NULL;
SDL_Texture *hit_fx2      = NULL;
SDL_Texture *no_fuel_fx   = NULL;
SDL_Texture *fuel_up      = NULL;
SDL_Texture *countdown_GO = NULL;
SDL_Texture *countdown_1  = NULL;
SDL_Texture *countdown_2  = NULL;
SDL_Texture *countdown_3  = NULL;
SDL_Texture *texture_fx[FX_NONE];

char score_txt[6];

int fuel_up_w;
int fuel_up_h;

// load textures into vram
int rendering_init_textures(SDL_Renderer *renderer, info_exchange *state)
{
    road_left = load_texture("res/road_left.png", renderer);
    road_middle = load_texture("res/road_middle.png", renderer);
    road_right = load_texture("res/road_right.png", renderer);
    opp_car[0] = load_texture("res/mini_van_scale_100px.png", renderer);
    opp_car[1] = load_texture("res/mini_truck_scale_100px.png", renderer);
    opp_car[2] = load_texture("res/taxi_scale_100px.png", renderer);
    opp_car[3] = load_texture("res/black_viper_scale_100px.png", renderer);
    player_car = load_texture("res/car.png", renderer);
    font = load_texture("res/geebeeyay-8x8.png", renderer);
    btn_pause     = load_texture("res/pause_120px.png", renderer);
    btn_play      = load_texture("res/play_120px.png", renderer);
    fuel_pointer  = load_texture("res/fuel_pointer_3.png", renderer);
    clock_texture = load_texture("res/lcd_clock.png", renderer);
    lane_touch_guide = load_texture("res/road_lane_touch_guide_120px.png", renderer);
    score_background = load_texture("res/score_background.png", renderer);
    dark_fader   = load_texture("res/dark_fader.png", renderer);
    fuel_up      = load_texture("res/fuel_up.png", renderer);
    countdown_GO = load_texture("res/countdown_GO.png", renderer);
    countdown_1  = load_texture("res/countdown_1.png", renderer);
    countdown_2  = load_texture("res/countdown_2.png", renderer);
    countdown_3  = load_texture("res/countdown_3.png", renderer);

    hit_fx = load_texture("res/crash_b_fx.png", renderer);
    hit_fx2 = load_texture("res/crash_fx_360x240.png", renderer);
    no_fuel_fx = load_texture("res/no_fuel_fx_360x240.2.png", renderer);

    // Indexing textures to be displayed
    texture_fx[0] = hit_fx2;
    texture_fx[1] = no_fuel_fx;
    texture_fx[2] = fuel_up;
    texture_fx[3] = countdown_GO;
    texture_fx[4] = countdown_1;
    texture_fx[5] = countdown_2;
    texture_fx[6] = countdown_3;

    if (state->hand == LEFT_HANDED)
    {
        // the user is either left-handed or ambidextruous
        btn_fuel_refill     = load_texture("res/buton_refuel_dark_90px_padded_2.png", renderer);
        btn_fuel_gauge_show = load_texture("res/buton_display_fuel_dark_90px_padded.png", renderer);
        fuel_gauge          = load_texture("res/fuel_display_180x90_padded_2.png", renderer);
        fuel_gauge_blank    = load_texture("res/diplay_background_blank_180x90px_padded_2.png",
                                        renderer);
        if (!state->need_to_refuel)
        {
            action_button_replacement = load_texture(
                    "res/diplay_background_blank_180x90px_padded_1.png", renderer);
        }
    }
    else
    {
        // the user is right-handed
        fuel_gauge = load_texture("res/fuel_display_180x90_padded_1.png", renderer);
        fuel_gauge_blank = load_texture("res/diplay_background_blank_180x90px_padded_1.png",
                                        renderer);
        btn_fuel_gauge_show = load_texture("res/buton_display_fuel_dark_90px_padded.png", renderer);
        btn_fuel_refill = load_texture("res/buton_refuel_dark_90px_padded_1.png", renderer);
        if (!state->need_to_refuel)
        {
            action_button_replacement = load_texture(
                    "res/diplay_background_blank_180x90px_padded_2.png", renderer);
        }
    }

    if (road_left == NULL || road_middle == NULL || road_right == NULL || opp_car[0] == NULL ||
        opp_car[1] == NULL || opp_car[2] == NULL || opp_car[3] == NULL ||
        player_car == NULL || font == NULL || btn_pause == NULL || btn_play == NULL ||
        btn_fuel_refill == NULL || btn_fuel_gauge_show == NULL || clock_texture == NULL ||
        fuel_gauge == NULL || fuel_gauge_blank == NULL || fuel_pointer == NULL ||
        lane_touch_guide == NULL || score_background == NULL || hit_fx == NULL || hit_fx2 == NULL ||
        fuel_up == NULL || no_fuel_fx == NULL)
    {
        SDL_Log("load error: %s\n", IMG_GetError());
        textures_free();
        return 1;
    }

    SDL_QueryTexture(fuel_up, NULL, NULL, &fuel_up_w, &fuel_up_h);

    // player car can become transparent when hit
    SDL_SetTextureBlendMode(player_car, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(player_car, 255);
    // pause button is transparent
    SDL_SetTextureBlendMode(btn_pause, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(btn_pause, 120);
    SDL_SetTextureBlendMode(btn_play, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(btn_play, 120);
    SDL_SetTextureBlendMode(hit_fx2, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(hit_fx2, 120);
    SDL_SetTextureBlendMode(no_fuel_fx, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(no_fuel_fx, 120);
    SDL_SetTextureBlendMode(fuel_up, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(fuel_up, 180);
    SDL_SetTextureBlendMode(dark_fader, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(lane_touch_guide, 80);
    SDL_SetTextureBlendMode(lane_touch_guide, SDL_BLENDMODE_BLEND);

    rescale_size(state);

    // initing player_car_pos_renderer;
    state->player_car_pos_renderer =
            state->scaling_mode * 6 +
            state->road_size * state->scaling_mode * state->player_car_pos_logical;

    // clock character size init
    SDL_QueryTexture(clock_texture, NULL, NULL, &state->clock_texture_char_size.w,
                     &state->clock_texture_char_size.h);
    state->clock_texture_char_size.w = state->clock_texture_char_size.w / 11;
    return 0;
}

// free memory allocations for textures in VRAM
void textures_free()
{
    cleanup(road_left);
    cleanup(road_middle);
    cleanup(road_right);
    cleanup(opp_car[0]);
    cleanup(opp_car[1]);
    cleanup(opp_car[2]);
    cleanup(opp_car[3]);
    cleanup(player_car);
    cleanup(font);
    cleanup(clock_texture);
    cleanup(fuel_gauge);
    cleanup(fuel_gauge_blank);
    cleanup(lane_touch_guide);
    cleanup(btn_fuel_refill);
    cleanup(btn_pause);
    cleanup(btn_play);
    cleanup(score_background);
    cleanup(hit_fx);
    cleanup(hit_fx2);
}

// measure road tile size (nothing to do with scale calculation)
// initialises 'road_size' and 'car_size'
void rescale_size(info_exchange *state)
{
    SDL_QueryTexture(road_middle, NULL, NULL, &state->road_size, NULL);
    SDL_QueryTexture(opp_car[0], NULL, NULL, &state->car_size, NULL);
}

// play_area and menu_area scaled sizes are calculated here
void calc_rendering_areas(info_exchange *state)
{
    state->play_area.w = 3 * state->road_size * state->scaling_mode;
    state->play_area.h = 5 * state->road_size * state->scaling_mode;
    state->menu_area.w = state->play_area.w;

    SDL_QueryTexture(btn_fuel_refill, NULL, NULL, NULL, &state->menu_area.h);
    state->menu_area.h = state->menu_area.h * state->scaling_mode;
    if (state->display.h > state->display.w)
    {
        state->play_area.x = (state->display.w - state->play_area.w) >> 1;
        state->menu_area.x = state->play_area.x;
    }
    else
    {
        state->play_area.x = 0;
        state->menu_area.x = 0;
    }

    state->menu_area.y = state->display.h - state->menu_area.h;
    state->play_area.y = state->menu_area.y - state->play_area.h;
}

// the rendering function, to be used in the main program loop
void rendering_state(info_exchange *state, SDL_Renderer *renderer)
{
    SDL_RenderClear(renderer);

    // play area rendering
    if (SDL_RenderSetViewport(renderer, &state->play_area))
    {
        log_SDL_error("Viewport");
        state->quit = SDL_TRUE;
        return;
    }
    // road rendering
    int scaled_road_size = state->road_size * state->scaling_mode;
    int scaled_scrolling = state->scroll_state * state->scaling_mode;
    for (int screen_y = (scaled_scrolling - scaled_road_size);
         screen_y <= scaled_road_size * 5; screen_y = screen_y + scaled_road_size)
    {
        render_texture_scaling(road_left, renderer, 0, screen_y, scaled_road_size, scaled_road_size,
                               NULL);
        render_texture_scaling(road_middle, renderer, scaled_road_size, screen_y, scaled_road_size,
                               scaled_road_size, NULL);
        render_texture_scaling(road_right, renderer, scaled_road_size * 2, screen_y,
                               scaled_road_size, scaled_road_size, NULL);
    }

    // opponent cars rendering
    opp_car_list *curr_el = state->opp_cars;
    int scaled_car_size = state->car_size * state->scaling_mode;
    while (curr_el != NULL)
    {
        for (int i = 0, j = 0; i < 3; i++)
        {
            if (curr_el->gap != i)
            {
                j = (j == 0) ? 1 : 0;
                int car_choice = curr_el->car_design[j];
                if (car_choice < 0 || 3 < car_choice)
                    car_choice = 0;
                render_texture_scaling(opp_car[car_choice], renderer,
                                       10 * state->scaling_mode + i * scaled_road_size,
                                       (int) round(curr_el->pos) * state->scaling_mode,
                                       scaled_car_size, scaled_car_size, NULL);
            }
        }
        curr_el = curr_el->next_el;
    }

    // texture FX rendering and player car transparency management
    // if there is a texture on screen (supposedly FX_CRASH,FX_NO_FUEL or FX_REFUEL)
    if ((state->current_texture_fx).texture != FX_NONE)
    {
        timed_texture_fx *t_fx = &(state->current_texture_fx);
        // timeout management
        if (t_fx->end_timestamp < state->time_last_check_tick) {
            t_fx->texture = FX_NONE;
            t_fx->end_timestamp = 0;
            SDL_SetTextureAlphaMod(player_car, 255);
        }
        else
        {
            render_texture_scaling(texture_fx[t_fx->texture], renderer, 0, scaled_road_size * 2,
                                   scaled_road_size * 3, scaled_road_size * 2, NULL);
        }
        if (t_fx->texture == FX_CRASH)
        {
            SDL_SetTextureAlphaMod(player_car, 100);
        }
    }
    else if( (state->disable_all_feedback) && state->player_hit ) {
        if ( state->time_last_check_tick < state->time_player_hit ) {
            SDL_SetTextureAlphaMod(player_car, 100);
        }
    }
    else
    {
        SDL_SetTextureAlphaMod(player_car, 255);
    }
    // player car rendering

    render_texture_scaling(player_car, renderer,
                           (int) round(state->player_car_pos_renderer),
                           5 * scaled_car_size, scaled_car_size, scaled_car_size, NULL);

    // Lane touch guide rendering
    render_texture_scaling(lane_touch_guide, renderer, 0, 4 * scaled_road_size, scaled_road_size,
                           scaled_road_size, NULL);
    render_texture_scaling(lane_touch_guide, renderer, scaled_road_size, 4 * scaled_road_size,
                           scaled_road_size, scaled_road_size, NULL);
    render_texture_scaling(lane_touch_guide, renderer, 2 * scaled_road_size, 4 * scaled_road_size,
                           scaled_road_size, scaled_road_size, NULL);

    // rendering bottom menu
    if (SDL_RenderSetViewport(renderer, &state->menu_area))
    {
        log_SDL_error("Viewport");
        state->quit = SDL_TRUE;
        return;
    }
    // Handedness correction
    int pos01, pos02, pos03;
    if (state->hand == LEFT_HANDED)
    {
        pos01 = 0;
        pos02 = state->menu_area.h;
        pos03 = state->menu_area.h * 2;
    }
    else
    {
        pos01 = (state->need_to_refuel) ? state->menu_area.h * 3 : state->menu_area.h * 2;
        pos02 = state->menu_area.h * 2;
        pos03 = 0;
    }

    if (state->need_to_refuel)
    {
        render_texture_scaling(btn_fuel_refill, renderer, pos01, 0, state->menu_area.h,
                               state->menu_area.h, NULL);
        render_texture_scaling(btn_fuel_gauge_show, renderer, pos02, 0, state->menu_area.h,
                               state->menu_area.h, NULL);
    }
    else
    {
        render_texture_scaling(action_button_replacement, renderer, pos01, 0,
                               state->menu_area.h * 2, state->menu_area.h, NULL);
    }

    if (show_fuel_gauge(state))
    {
        render_texture_scaling(fuel_gauge, renderer, pos03, 0, state->menu_area.h * 2,
                               state->menu_area.h, NULL);
        //SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h, DL_Rect *clip
        render_texture_scaling(fuel_pointer, renderer,
                               pos03 + (15 + state->fuel_pointer_position) * state->scaling_mode,
                               10 * state->scaling_mode, 18 * state->scaling_mode,
                               80 * state->scaling_mode, NULL);
    }
    else
    {
        render_texture_scaling(fuel_gauge_blank, renderer, pos03, 0, state->menu_area.h * 2,
                               state->menu_area.h, NULL);
        if (state->display_numeric_clock)
        {
            SDL_Rect rect_clock_temp;
            rect_clock_temp.w = state->menu_area.h * 2 - 20 * state->scaling_mode;
            rect_clock_temp.h = state->menu_area.h - 20 * state->scaling_mode;

            if (state->hand == LEFT_HANDED)
                rect_clock_temp.x = state->menu_area.h * 2 + 10 * state->scaling_mode;
            else
                rect_clock_temp.x = 10 * state->scaling_mode;

            rect_clock_temp.y = 10 * state->scaling_mode;
            render_lcd_clock(clock_texture, renderer, state->clock_texture_char_size,
                             state->numeric_clock, rect_clock_temp);
        }
    }

    // disabling viewport
    if (SDL_RenderSetViewport(renderer, NULL))
    {
        log_SDL_error("Viewport");
        state->quit = SDL_TRUE;
        return;
    }

    if (!state->end)
    {
        // render pause/play button
        if (state->display_pause_button)
        {
            int pause_btn_location = (state->hand == LEFT_HANDED) ?
                                     state->play_area.x + 20 * state->scaling_mode :
                                     state->play_area.x + 20 * state->scaling_mode +
                                     2 * state->road_size * state->scaling_mode;
            render_texture_scaling((state->pause) ? btn_play : btn_pause, renderer,
                                   pause_btn_location,
                                   20 * state->scaling_mode, 80 * state->scaling_mode,
                                   80 * state->scaling_mode, NULL);
        }

        // render the score display
        render_texture_scaling(score_background, renderer, state->play_area.x + scaled_road_size, 0,
                               scaled_road_size,
                               scaled_road_size, NULL);
        sprintf(score_txt, "%05d", state->score);
        render_text_8x8_capital(font, renderer, "score", state->scaling_mode * 16,
                                state->play_area.x + scaled_road_size + state->scaling_mode * 20,
                                state->scaling_mode * 4);
        render_text_8x8_capital(font, renderer, score_txt, state->scaling_mode * 16,
                                state->play_area.x + scaled_road_size + state->scaling_mode * 20,
                                state->scaling_mode * 24);

        // Render the new points
        int_lt *curr = (state->new_score_points).first;
        int_lt *next = NULL;
        while (curr != NULL)
        {
            next = curr->next;
            uint32_t time_diff = state->time_last_check_tick - curr->timestamp;
            if (SCORE_POINT_MSG_TIMEOUT < time_diff || curr->val == 0) {
                delete_int_linked(&(state->new_score_points), curr);
            }
            else
            {
                float pos_y = ((float)time_diff / SCORE_POINT_MSG_TIMEOUT) * 40.0f;
                char *points = points_s_to_i(curr->val);
                if (points)
                {
                    render_text_8x8_capital(font, renderer, points, state->scaling_mode * 16,
                                state->play_area.x + scaled_road_size + state->scaling_mode * 20,
                                (int) (state->scaling_mode * 64 - state->scaling_mode * pos_y));
                    free(points);
                }
            }

            curr = next;
        }
    }

#ifdef DISPLAY_DEBUG_MSG

    string_lt *currentMsg = (state->debug_messages).first;
    for( int i = 0; currentMsg != NULL; i+=20 )
    {
        if( (SDL_GetTicks() - currentMsg->timestamp) < DEBUG_MSG_TIMEOUT )
        {
            render_text_8x8_capital(font,
                                    renderer,
                                    currentMsg->str, state->scaling_mode * 8,
                                    10, i);
        } else {
            i -= 20;
        }

        currentMsg = currentMsg->next;
    }

#endif

#ifdef DISPLAY_DEBUG_INFO
    // Debug info
    char *text_curr_fuel, *text_info_1, *text_info_2, *text_info_3, *text_info_4, *text_info_5, *text_info_6, *text_info_7, *text_temp_timer, *text_score;
    if (asprintf(&text_curr_fuel, "fuel: %d", state->fuel) == -1 ||
        asprintf(&text_info_1, "argc: %d", state->argc) == -1 ||
        asprintf(&text_info_2, "argv: 0='%s'", state->argv[0]) == -1 ||
        asprintf(&text_info_3, "argv: 1='%s', 2='%s'", state->argv[1], state->argv[2]) == -1 ||
        asprintf(&text_info_4, "      3='%s', 4='%s'", state->argv[3], state->argv[4]) == -1 ||
        asprintf(&text_info_5, "5='%s'", state->argv[5]) == -1 ||
        asprintf(&text_info_6, "6='%s'", state->argv[6]) == -1 ||
        asprintf(&text_info_7, "hand='%d'", state->hand) == -1 ||
        asprintf(&text_temp_timer, "time left: %d", state->time_left) == -1 ||
        asprintf(&text_score, "score: %d", state->score) == -1) {
        state->quit = SDL_TRUE;
        SDL_Log("ALLOCATION ERROR!\n");
        return;
    }
    render_text_8x8_capital(font, renderer, text_curr_fuel, state->scaling_mode * 8,
                            state->play_area.x + state->play_area.w -
                            25 * 8 * state->scaling_mode,
                            0);
    render_text_8x8_capital(font, renderer, text_info_1, state->scaling_mode * 8,
                            state->play_area.x + state->play_area.w -
                            25 * 8 * state->scaling_mode,
                            20);
    render_text_8x8_capital(font, renderer, text_info_2, state->scaling_mode * 8,
                            state->play_area.x + state->play_area.w -
                            25 * 8 * state->scaling_mode,
                            40);
    render_text_8x8_capital(font, renderer, text_info_3, state->scaling_mode * 8,
                            state->play_area.x + state->play_area.w -
                            25 * 8 * state->scaling_mode,
                            60);
    render_text_8x8_capital(font, renderer, text_info_4, state->scaling_mode * 8,
                            state->play_area.x + state->play_area.w -
                            25 * 8 * state->scaling_mode,
                            80);
    render_text_8x8_capital(font, renderer, text_info_5, state->scaling_mode * 8,
                            state->play_area.x + state->play_area.w -
                            43 * 8 * state->scaling_mode,
                            100);
    render_text_8x8_capital(font, renderer, text_info_6, state->scaling_mode * 8,
                            state->play_area.x + state->play_area.w -
                            43 * 8 * state->scaling_mode,
                            120);
    render_text_8x8_capital(font, renderer, state->date_at_launch, state->scaling_mode * 8,
                            state->play_area.x + state->play_area.w -
                            25 * 8 * state->scaling_mode,
                            140);
    char *str01 = uint16_linked_toString(state->hit_times);
    render_text_8x8_capital(font, renderer, str01, state->scaling_mode * 8,
                            state->play_area.x + state->play_area.w -
                            35 * 8 * state->scaling_mode,
                            160);
    render_text_8x8_capital(font, renderer, text_info_7, state->scaling_mode * 8,
                            state->play_area.x + state->play_area.w -
                            25 * 8 * state->scaling_mode,
                            180);
    /*render_text_8x8_capital(font, renderer, state->debug_message, state->scaling_mode * 8,
                            state->play_area.x + state->play_area.w -
                            43 * 8 * state->scaling_mode,
                            200);*/
    render_text_8x8_capital(font, renderer, text_temp_timer, state->scaling_mode * 8,
                            state->play_area.x + state->play_area.w -
                            25 * 8 * state->scaling_mode,
                            220);
    render_text_8x8_capital(font, renderer, text_score, state->scaling_mode * 8,
                            state->play_area.x + state->play_area.w -
                            25 * 8 * state->scaling_mode,
                            240);

    free(str01);
    free(text_curr_fuel);
    free(text_info_1);
    free(text_info_2);
    free(text_info_3);
    free(text_info_4);
    free(text_info_5);
    free(text_info_6);
    free(text_info_7);
    free(text_temp_timer);
#endif


    if (state->end)
    {
        if (show_end_screen(state, renderer, scaled_road_size, SDL_FALSE) < 0)
        {
            state->quit = SDL_TRUE;
            return;
        }
    }
    // presenting
    SDL_RenderPresent(renderer);
}

int show_fuel_gauge(info_exchange *state)
{
    return (
            (state->time_last_check_tick - state->time_last_show_fuel_tick <
             state->show_fuel_duration_ms && state->time_last_show_fuel_tick != 0)
            || state->refueling);
}

int show_end_screen(info_exchange *state, SDL_Renderer *renderer, int scaled_road_size, int reset)
{
    static uint8_t recalculate_transparency = SDL_TRUE, fader_trans = 0;

    /* This is necessary because the static variables may not be reset automatically on the next
     * instance of the game
     */
    if (reset)
    {
        recalculate_transparency = SDL_TRUE;
        fader_trans = 0;
        return 0;
    }

    if (recalculate_transparency)
    {
        double fader_trans_tmp = floor((SDL_GetTicks() - state->time_game_end) / 5.0);

        if (fader_trans_tmp > GAME_OVER_MAX_TRANSPARENCY)
        {
            recalculate_transparency = SDL_FALSE;
            fader_trans = GAME_OVER_MAX_TRANSPARENCY;
        }
        else
        {
            fader_trans = (uint8_t) fader_trans_tmp;
        }
        SDL_SetTextureAlphaMod(dark_fader, fader_trans);
    }
    //SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h, SDL_Rect *clip
    render_texture_scaling(dark_fader, renderer, state->play_area.x, state->play_area.y,
                           scaled_road_size * 3,
                           scaled_road_size * 6, NULL);

    if (!recalculate_transparency)
    {
        /*
        char *score = NULL;
        int length = asprintf(&score, "Your score: %d", state->score);
        if (length == -1)
        {
            SDL_Log("asprintf alloc error!\n");
            state->quit = SDL_TRUE;
            return -1;
        }

        render_text_8x8_capital(font, renderer, "Game Over", state->display.h / 40,
                                state->display.w / 2 - (9 * state->display.h / 40) / 2,
                                state->display.h / 2 - state->display.h / 80);
        render_text_8x8_capital(font, renderer, score, state->display.h / 40,
                                  state->display.w / 2 - (length * state->display.h / 40) / 2,
                                  state->display.h/2 - state->display.h/80 + state->display.h/40);
        free(score);
        render_text_8x8_capital(font, renderer, "Tap to continue", state->display.h / 40,
                                state->display.w / 2 - (15 * state->display.h / 40) / 2,
                                state->display.h/2 - state->display.h/80 + state->display.h/40);*/




        render_text_8x8_capital(font, renderer, "Game Over", state->display.h / 40,
                                state->display.w / 2 - (9 * state->display.h / 40) / 2,
                                state->display.h / 4 - state->display.h / 80);
        /*render_text_8x8_capital(font, renderer, score, state->display.h / 40,
                                  state->display.w / 2 - (length * state->display.h / 40) / 2,
                                  state->display.h/2 - state->display.h/80 + state->display.h/40);
        free(score);*/
        render_text_8x8_capital(font, renderer, "Tap to continue", state->display.h / 40,
                                state->display.w / 2 - (15 * state->display.h / 40) / 2,
                                state->display.h / 4 - state->display.h / 80 +
                                state->display.h / 40);

        char your_score[50];
        snprintf(your_score, 50, "Your score: %05d", state->score);

        render_text_8x8_capital(font, renderer, your_score, state->display.h / 40,
                                state->display.w / 2 - (16 * state->display.h / 40) / 2,
                                state->display.h / 4 - state->display.h / 80 +
                                11 * state->display.h / 80);

        render_text_8x8_capital(font, renderer, "Previous Scores:", state->display.h / 40,
                                state->display.w / 2 - (15 * state->display.h / 40) / 2,
                                state->display.h / 2 - state->display.h / 80);

        for (int i = 0; i < 5; i++)
        {
            render_text_8x8_capital(font, renderer, state->score_hist[i], state->display.h / 40,
                                    state->display.w / 2 - (9 * state->display.h / 40) / 2,
                                    state->display.h / 2 - state->display.h / 80 +
                                    (i + 1) * state->display.h / 40);
        }
    }

    return 0;
}
