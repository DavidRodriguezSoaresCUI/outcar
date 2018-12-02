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


int main(int argc, char **argv)
{
    /* Argument norm
     * 0: Useless
     * 1: Integer, ID/Code
     * 2: Integer, Age
     * 3: String, Sex
     * 4: String, Handedness
     * 5: String, file location for stat database
     * 6: String, file location for game config
     */

    // Setup for random number generation
    srand((unsigned int) time(NULL));

    // Setup for program state
    info_exchange program_state;
    init_info_exchange(&program_state, argc, argv);
    load_conf(&program_state);
    verify_conf(&program_state);
    if (load_score(&program_state) < 0)
        return -10;

    /*snprintf( program_state.debug_message, DEBUG_STR_LEN - 2, "'%d' '%d' '%d' '%d'",
              program_state.time_left, program_state.max_fuel,
              program_state.rel_speed, program_state.show_fuel_duration_ms);*/

    double f_scrollstate = 0;

    // Init and screen setup, verifies whether everything is okay
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        log_SDL_error("SDL_Init");
        EXIT_ON_FAILURE
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        SDL_Log("SDL_image init failed: %s\n", IMG_GetError());
        EXIT_ON_FAILURE
    }

    if (SDL_GetCurrentDisplayMode(0, &program_state.display))
    {
        log_SDL_error("GetDisplayMode");
        EXIT_ON_FAILURE
    }
    SDL_Log("Display Mode Res: %dx%d", program_state.display.w, program_state.display.h);

    // setting (initial) scaling mode
    calc_scaling(&program_state);


    // open window
    SDL_Window *window = SDL_CreateWindow("OutCar", SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, program_state.display.w,
                                          program_state.display.h,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL)
    {
        log_SDL_error("CreateWindow");
        EXIT_ON_FAILURE
    }

    // renderer init

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_ACCELERATED |
                                                SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        log_SDL_error("CreateRenderer");
        cleanup (window);
        EXIT_ON_FAILURE
    }

    // create event handler and load textures into VRAM
    SDL_Event e;
    if (rendering_init_textures(renderer, &program_state))
        return 1;


    // Time management
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    if (asprintf(&(program_state.date_at_launch), "%d-%02d-%02d %02d:%02d:%02d\n",
                 tm.tm_year + 1900,
                 tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec) == -1)
    {
        program_state.quit = SDL_TRUE;
    }

    // init audio

    if (init_sfx("res/Roland-GR-1-Trumpet-C5.wav", &program_state.audio_device_id,
                 &program_state.sfx_wav_length, &program_state.sfx_wav_buffer) != 0)
    {
        log_SDL_error("init sfx");
        program_state.quit = SDL_TRUE;
    }

    // main program loop
    //program_state.scaling_mode = 2;
    calc_rendering_areas(&program_state);
    while (!program_state.quit)
    {
        input_events(&program_state, &e);
        if (program_state.countdown) {
            countdown_to_race(&program_state);
        }
        else if (!program_state.pause) {
            game_logic(&program_state, &f_scrollstate);
        }

        rendering_state(&program_state, renderer);
    }

    // -- Exiting --
    if (program_state.time_game_end == 0)
        program_state.time_game_end = SDL_GetTicks();

    // Log results TODO
    log_results(program_state);

    // Cleanup and exit
    free_slots(program_state.opp_cars);
    textures_free();
    exit_sfx(program_state.audio_device_id, program_state.sfx_wav_buffer);
    free_info_exchange(&program_state);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

void load_conf(info_exchange *state)
{
    // Loads game configuration from file
    char *data = read_file_SDL(state->argv[ARGV_CONFFILE]); // Read conf file
    if (!data)
        return;

    uint32_t nb_lines = 0;
    char **lines = split_str_lines(data, &nb_lines); // Split lines
    int32_t i = 0, lineFound = -1;

    while (i < nb_lines) // find the first line with data
    {
        strip_comments(lines[i]);
        if (10 < strlen(lines[i]))
        {
            lineFound = i;
            break;
        }
        else
        {
            free(lines[i]);  // freeing memory
        }
        i++;
    }

    if (lineFound == -1)
        return;

    char *conf_line_CSV = lines[lineFound];
    uint32_t nb_items = 0;
    char **conf_data = split_CSV_line(conf_line_CSV, &nb_items); // split the CSV items
    free(conf_line_CSV);
    free(lines);

    if (!conf_data || nb_items != CONF_EXPECTED_NB_ITEMS)
        return;

    int conf_data_i[CONF_EXPECTED_NB_ITEMS] = {0};

    for (i = 0; i < nb_items; i++) // items: char* -> int
    {
        conf_data_i[i] = atoi(conf_data[i]);
    }

    state->time_total            = (uint16_t) conf_data_i[CONF_GAME_DURATION];
    state->max_fuel              = (uint8_t)  conf_data_i[CONF_FUEL_DURATION];
    state->rel_speed             = (uint8_t)  conf_data_i[CONF_REL_SPEED];
    state->show_fuel_duration_ms = (uint16_t) conf_data_i[CONF_SHOW_DURATION];
    state->display_numeric_clock = (uint8_t)  conf_data_i[CONF_DSPLY_NUM_CLK];
    state->need_to_refuel        = (uint8_t)  conf_data_i[CONF_NEED_TO_REFUEL];
    state->display_pause_button  = (uint8_t)  conf_data_i[CONF_DSPLY_PAUSE_BTN];
    state->avoid_reward          = (uint16_t) conf_data_i[CONF_AVOID_REWARD];
    state->refuel_reward         = (uint16_t) conf_data_i[CONF_REFUEL_REWARD];
    state->crash_penalty         = (uint16_t) conf_data_i[CONF_CRASH_PENALTY];
    state->nofuel_penalty        = (uint16_t) conf_data_i[CONF_NOFUEL_PENALTY];

    for (i = 0; i < nb_items; i++) // freeing memory
    {
        free(conf_data[i]);
    }
    free(conf_data);
#ifdef DISPLAY_DEBUG_MSG
    push_string_linked(&(state->debug_messages), "Conf. loaded !");
#endif
}

void verify_conf(info_exchange *state)
{
    // Checks that the obtained values are within bounds
    // See values.h for norm

#ifdef DISPLAY_DEBUG_MSG
    if (check_bounds16(&(state->time_total), "warning: time_total OOB\n", CONF_GAME_DURATION_MIN,
                   CONF_GAME_DURATION_MAX, CONF_GAME_DURATION_STD)
    || check_bounds16(&(state->max_fuel), "warning: max_fuel OOB\n", CONF_FUEL_DURATION_MIN,
                   CONF_FUEL_DURATION_MAX, CONF_FUEL_DURATION_STD)
    || check_bounds8(&(state->rel_speed), "warning: rel_speed OOB\n", CONF_REL_SPEED_MIN,
                  CONF_REL_SPEED_MAX, CONF_REL_SPEED_STD)
    || check_bounds16(&(state->show_fuel_duration_ms), "warning: show_fuel_duration_ms OOB\n",
                   CONF_SHOW_DURATION_MIN, CONF_SHOW_DURATION_MAX, CONF_SHOW_DURATION_STD)
    || check_bounds16(&(state->avoid_reward), "warning: avoid_reward OOB\n", CONF_AVOID_REWARD_MIN,
                   CONF_AVOID_REWARD_MAX, CONF_AVOID_REWARD_STD)
    || check_bounds16(&(state->refuel_reward), "warning: refuel_reward OOB\n", CONF_REFUEL_REWARD_MIN,
                   CONF_REFUEL_REWARD_MAX, CONF_REFUEL_REWARD_STD)
    || check_bounds16(&(state->crash_penalty), "warning: crash_penalty OOB\n", CONF_CRASH_PENALTY_MIN,
                   CONF_CRASH_PENALTY_MAX, CONF_CRASH_PENALTY_STD)
    || check_bounds16(&(state->nofuel_penalty), "warning: nofuel_penalty OOB\n",
                   CONF_NOFUEL_PENALTY_MIN, CONF_NOFUEL_PENALTY_MAX, CONF_NOFUEL_PENALTY_STD))
        push_string_linked(&(state->debug_messages), "Conf. OOB errors !");
#else

    check_bounds16(&(state->time_total),
                   "warning: time_total OOB\n",
                   CONF_GAME_DURATION_MIN,
                   CONF_GAME_DURATION_MAX,
                   CONF_GAME_DURATION_STD);

    check_bounds16(&(state->max_fuel),
                   "warning: max_fuel OOB\n",
                   CONF_FUEL_DURATION_MIN,
                   CONF_FUEL_DURATION_MAX,
                   CONF_FUEL_DURATION_STD);

    check_bounds8(&(state->rel_speed),
                  "warning: rel_speed OOB\n",
                  CONF_REL_SPEED_MIN,
                  CONF_REL_SPEED_MAX,
                  CONF_REL_SPEED_STD);

    check_bounds16(&(state->show_fuel_duration_ms),
                   "warning: show_fuel_duration_ms OOB\n",
                   CONF_SHOW_DURATION_MIN,
                   CONF_SHOW_DURATION_MAX,
                   CONF_SHOW_DURATION_STD);

    check_bounds8_bool(&(state->display_numeric_clock),
                  "warning: display_numeric_clock OOB\n",
                  CONF_DSPLY_NUM_CLK_STD);

    check_bounds8_bool(&(state->need_to_refuel),
                  "warning: need_to_refuel OOB\n",
                  CONF_NEED_TO_REFUEL_STD);

    check_bounds8_bool(&(state->display_pause_button),
                  "warning: display_pause_button OOB\n",
                  CONF_DSPLY_PAUSE_BTN_STD);

    check_bounds16(&(state->avoid_reward),
                   "warning: avoid_reward OOB\n",
                   CONF_AVOID_REWARD_MIN,
                   CONF_AVOID_REWARD_MAX,
                   CONF_AVOID_REWARD_STD);

    check_bounds16(&(state->refuel_reward),
                   "warning: refuel_reward OOB\n",
                   CONF_REFUEL_REWARD_MIN,
                   CONF_REFUEL_REWARD_MAX,
                   CONF_REFUEL_REWARD_STD);

    check_bounds16(&(state->crash_penalty),
                   "warning: crash_penalty OOB\n",
                   CONF_CRASH_PENALTY_MIN,
                   CONF_CRASH_PENALTY_MAX,
                   CONF_CRASH_PENALTY_STD);

    check_bounds16(&(state->nofuel_penalty),
                   "warning: nofuel_penalty OOB\n",
                   CONF_NOFUEL_PENALTY_MIN,
                   CONF_NOFUEL_PENALTY_MAX,
                   CONF_NOFUEL_PENALTY_STD);

#endif
    state->display_numeric_clock =
            (state->display_numeric_clock == 0) ? (uint8_t) SDL_FALSE : (uint8_t) SDL_TRUE;

    state->time_left = (uint32_t) state->time_total * 1000;
    state->fuel = state->max_fuel;

    sprintf(state->numeric_clock, "%02d:%02d", state->time_left / 60000,
            (state->time_left / 1000) % 60);
#ifdef DISPLAY_DEBUG_MSG
    push_string_linked(&(state->debug_messages), "Conf. verfied !");
#endif
}

void log_results(const info_exchange state)
{
    // Calcul du temps de jeu
    uint32_t timer = (uint32_t) floor((state.time_game_end - state.time_game_start) / 1000.0);

    // File creation (to log results)
    char **argv = state.argv;
    SDL_RWops *db = SDL_RWFromFile(argv[ARGV_STATFILE], "a");
    if (db == NULL)
        return;

    // Creating the CSV-compliant data line
    char *dataline = NULL;
    if (asprintf(&dataline,
             "%s,%s,%s,%s,%s,\"%d,%d,%d,%d,%d,%d,%d\",%d,%d,%d,%s,%d,%s,%d,%s,%d,%s,%d,%s,%s,%s",
             argv[ARGV_IDCODE],
             argv[ARGV_AGE],
             argv[ARGV_SEX],
             argv[ARGV_HAND],
             state.date_at_launch,
             state.time_total,
             state.max_fuel,
             state.rel_speed,
             state.show_fuel_duration_ms,
             state.display_numeric_clock,
             state.need_to_refuel,
             state.display_pause_button,
             timer,
             state.score,
             uint16_linked_count(state.hit_times),
             uint16_linked_toString(state.hit_times),
             uint16_linked_count(state.refuel_times),
             uint16_linked_toString(state.refuel_times),
             uint16_linked_count(state.auto_refuel_times),
             uint16_linked_toString(state.auto_refuel_times),
             uint16_linked_count(state.show_times),
             uint16_linked_toString(state.show_times),
             uint16_linked_count(state.void_times),
             uint16_linked_toString(state.void_times),
             uint16_linked_toString(state.pause_time),
             uint32_linked_toString(state.pause_duration)) != -1)
    {
        strip_char(dataline, '\n');

        size_t len = strlen(dataline);
        SDL_RWwrite(db, dataline, 1, len); // write line to database
        free(dataline);
    }
    NEWLINE(db);

    SDL_RWclose(db);
}

int load_score(info_exchange *state)
{
    // Loads game configuration from file
    char *data = read_file_SDL(state->argv[ARGV_STATFILE]); // read stat file
    if (!data)
        return -1;

    uint32_t nb_lines = 0; // number of lines found by the line splitter
    char **lines = split_str_lines(data, &nb_lines); // split lines
    for (uint32_t i = 0; i < nb_lines; i++)
    {
        strip_comments(lines[i]);
    }

    // int32_t scores[nb_lines]; // scores are saved here
    uint32_t nb_items; // number of items found by the CSV line splitter
    char **conf_data; // separated data strings
    int32_t temp_score, score_index = 7;
    int score_hist_int[HIST_SCORES];
    for (uint32_t i = 0; i < HIST_SCORES; i++)
    {
        score_hist_int[i] = MIN_SCORE - 1;
    }

    for (uint32_t i = 0; i < nb_lines; i++) // find the first line with data
    {
        strip_comments(lines[i]);
        conf_data = split_CSV_line(lines[i], &nb_items); // split the CSV items
        if (nb_items == EXPECTED_STAT_ITEMS && conf_data != NULL) // if valid line
        {
            temp_score = (int32_t) atoi(conf_data[score_index]); // convert string data to int
            for (int k = 0;
                 k < HIST_SCORES; k++) // insert (if possible) into 5 highest scores in state
            {
                if (temp_score > score_hist_int[k])
                {
                    for (int j = HIST_SCORES - 1; j > k; j--)
                        score_hist_int[j] = score_hist_int[j - 1];
                    score_hist_int[k] = temp_score;
                    break;
                }
            }
        }
        free(lines[i]);  // freeing memory
    }
    free(lines); // yet again...

    // for the renderer
    int score_limited;
    int j = 0;
    for (int i = 0; i < 5; i++)
    {
        if (score_hist_int[i] < MIN_SCORE || score_hist_int[i] == 0)
        {
            //score_limited = MIN_SCORE;
            continue;
        }
        else if (score_hist_int[i] > MAX_SCORE)
        {
            score_limited = MAX_SCORE;
        }
        else
        {
            score_limited = score_hist_int[i];
        }

        if (snprintf(state->score_hist[j], HIST_SCORES_LEN, "%d, %05d", j + 1, score_limited) < 0)
            return -2;

        j++;
    }

    return 0;
}
