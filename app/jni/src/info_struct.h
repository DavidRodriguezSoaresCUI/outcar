#ifndef INFOSTRUCT
#define INFOSTRUCT
//#define DISPLAY_DEBUG_MSG

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include <time.h>
#include <string.h>
#include "values.h"
//#include "utils.h" // Cross-include issues

// enumeration for screen orientation state (is it necessary?)
/*typedef enum screen_orient
{
    portrait,
    landscape
} screen_orient;*/

// structure that defines a single line of cars
typedef struct opp_car_list
{
    uint8_t gap;
    double pos;
    uint8_t car_design[2];
    struct opp_car_list *next_el;
} opp_car_list;

typedef struct uint16_lt
{
    uint16_t value;
    struct uint16_lt *next;
} uint16_lt;

typedef struct uint16_linked
{
    struct uint16_lt *first;
    struct uint16_lt *last;
} uint16_linked;

typedef struct uint32_lt
{
    uint32_t value;
    struct uint32_lt *next;
} uint32_lt;

typedef struct uint32_linked
{
    struct uint32_lt *first;
    struct uint32_lt *last;
} uint32_linked;

typedef struct string_lt
{
    char *str;
    uint32_t timestamp;
    struct string_lt *next;
} string_lt;

typedef struct string_linked
{
    struct string_lt *first;
    struct string_lt *last;
} string_linked;

typedef struct int_lt
{
    int val;
    uint32_t timestamp;
    struct int_lt *next;
} int_lt;

typedef struct int_linked
{
    struct int_lt *first;
    struct int_lt *last;
} int_linked;

typedef enum texture_fx_e
{
    FX_CRASH    = 0,
    FX_NO_FUEL  = 1,
    FX_REFUEL   = 2,
    FX_CTDWN_GO = 3,
    FX_CTDWN_1  = 4,
    FX_CTDWN_2  = 5,
    FX_CTDWN_3  = 6,
    FX_ELSE     = 7,
    FX_NONE     = 8 } texture_fx_e;

typedef enum sound_fx_e
{
    SFX_CRASH    = 0,
    SFX_NO_FUEL  = 1,
    SFX_REFUEL   = 2,
    SFX_NONE     = 3 } sound_fx_e;


typedef struct timed_texture_fx
{
    texture_fx_e texture;
    uint32_t end_timestamp;
} timed_texture_fx;

typedef enum handedness
{
    LEFT_HANDED,
    RIGHT_HANDED,
} handedness_e;

/**
 * info exchange structure for communcation between different parts
 * of the code
 */
typedef struct info_exchange
{
    // --global program arguments--
    int argc;
    char **argv;

    // -- screen state --
    //uint8_t orientation_change;
    uint8_t scaling_mode;
    SDL_DisplayMode display;
    int road_size;
    int car_size;
    SDL_Rect clock_texture_char_size; // initialised by rendering_init_textures!

    // -- game statistics and info --
    char*          date_at_launch;
    uint16_linked* hit_times;
    uint16_linked* refuel_times;
    uint16_linked* auto_refuel_times;
    uint16_linked* show_times;
    uint16_linked* void_times;
    uint16_linked* pause_time; // in s
    uint32_linked* pause_duration; // in ms
    int32_t        score;
    char score_hist[HIST_SCORES][HIST_SCORES_LEN];

    // -- game settings --
    uint16_t show_fuel_duration_ms; // Config
    handedness_e hand;
    uint16_t max_fuel;  // Config
    uint8_t  rel_speed; // Config
    uint16_t time_total; // Config
    //uint8_t difficulty;
    uint8_t  display_numeric_clock; // boolean: SDL_FALSE or SDL_TRUE
    uint8_t  need_to_refuel;        // boolean: SDL_FALSE or SDL_TRUE
    uint8_t  display_pause_button;  // boolean: SDL_FALSE or SDL_TRUE
    uint16_t avoid_reward;
    uint16_t refuel_reward;
    uint16_t crash_penalty;
    uint16_t nofuel_penalty;

    // -- game state --
    uint32_t time_left; // in milliseconds
    char *numeric_clock;
    uint16_t scroll_state; // between 0 and 89
    uint8_t countdown;  // boolean: SDL_FALSE or SDL_TRUE
    uint8_t pause;      // boolean: SDL_FALSE or SDL_TRUE
    uint8_t quit;       // boolean: SDL_FALSE or SDL_TRUE
    uint8_t end;        // boolean: SDL_FALSE or SDL_TRUE
    uint8_t player_hit; // boolean: SDL_FALSE or SDL_TRUE
    uint8_t refueling;  // boolean: SDL_FALSE or SDL_TRUE
    double fuel;
    uint8_t fuel_pointer_position;
    uint32_t time_game_start;
    uint32_t time_game_end;
    uint32_t time_last_second_tick;
    uint32_t time_last_check_tick;
    uint32_t time_last_show_fuel_tick;
    uint32_t time_last_car_gen;
    SDL_Rect play_area;
    SDL_Rect menu_area;
    // |->fuel up message countdown data
    int_linked new_score_points;
    timed_texture_fx current_texture_fx;

    // -- misc --
    int player_car_pos_logical; // values in {0,1,2}
    double player_car_pos_renderer;
    opp_car_list *opp_cars;

    // -- events --
    SDL_TouchFingerEvent e_touch;
    uint8_t e_touch_processed;
    //int16_t e_accel_pos[2]; //unused
    //uint8_t e_screen_rot; //unused

    // -- sfx --
    SDL_AudioDeviceID audio_device_id;
    uint32_t sfx_wav_length[SFX_NONE];
    uint8_t *sfx_wav_buffer[SFX_NONE];

#ifdef DISPLAY_DEBUG_MSG
    string_linked debug_messages;
#endif

} info_exchange;

// function initialise the contents of the info exchange structure
void init_info_exchange(info_exchange *init_var, int argc, char **argv);

void free_info_exchange(info_exchange *free_var);

void init_handedness(info_exchange *init_var);

void calc_scaling(info_exchange *state);


// UTILS

#define ASSERT_NOT_NULL(var) if ( var == NULL ) { \
           printf( "Assert fail: var was NULL (file '%s', line %d)\n", __FILE__, __LINE__ ); \
           exit( EXIT_FAILURE ); }

int check_bounds8(
        uint8_t *val,
        const char *err_msg,
        const uint8_t min,
        const uint8_t max,
        const uint8_t std);

int check_bounds16(
        uint16_t *val,
        const char *err_msg,
        const uint16_t min,
        const uint16_t max,
        const uint16_t std);

int check_bounds8_bool(
        uint8_t *val,
        const char *err_msg,
        const uint8_t std);

void strip_char(
        char *str,
        const char c);

void strip_comments(char *str);

uint32_t count_char_instances(
        const char *str,
        const char c);

uint32_t count_items_CSV_line(const char *str);

char **split_CSV_line(
        char *str,
        uint32_t *nb_items);

char **split_str_lines(
        char *str,
        uint32_t *nb_lines);

char *read_file_SDL(const char *file_path);

void init_uint16_linked(uint16_linked **ptr);

void init_uint32_linked(uint32_linked **ptr);

void push_uint16_linked(
        uint16_linked *chain,
        uint16_t val);

void push_uint32_linked(
        uint32_linked *chain,
        uint32_t val);

char *uint16_linked_toString(uint16_linked *chain);

char *uint32_linked_toString(uint32_linked *chain);

uint16_t uint16_linked_count(uint16_linked *chain);

void push_string_linked(
        string_linked *chain,
        char *str);
#ifdef DISPLAY_DEBUG_MSG
void delete_string_linked(
        string_linked *chain,
        string_lt *link);
#endif
void free_string_linked(string_linked *chain);

void push_int_linked(int_linked *chain, int val);

void delete_int_linked(int_linked *chain, int_lt *link);

void free_int_linked(int_linked *chain);

char *points_s_to_i(int val);

#endif
