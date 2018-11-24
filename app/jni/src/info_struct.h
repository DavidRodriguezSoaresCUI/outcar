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
        struct opp_car_list* next_el;
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

    typedef enum handedness
    {
        NO_HANDED=0b00,
        LEFT_HANDED=0b01,
        RIGHT_HANDED=0b10,
        //AMBIDEXTRUOUS=0b11
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
        char *date_at_launch;
        uint16_linked *hit_times;
        uint16_linked *refuel_times;
        uint16_linked *auto_refuel_times;
        uint16_linked *show_times;
        uint16_linked *void_times;
        int32_t score;
        char score_hist[HIST_SCORES][HIST_SCORES_LEN];

        // -- game settings --
        uint16_t show_fuel_duration_ms; // Config
        handedness_e hand;
        uint16_t max_fuel;  // Config
        uint8_t rel_speed; // Config
        uint16_t time_total; // Config
        //uint8_t difficulty;
        uint8_t display_numeric_clock; // boolean: SDL_FALSE or SDL_TRUE
        uint16_t avoid_reward;
        uint16_t refuel_reward;
        uint16_t crash_penalty;
        uint16_t nofuel_penalty;

        // -- game state --
        uint32_t time_left;
    	char *numeric_clock;
	    uint16_t scroll_state; // between 0 and 89
        uint8_t pause;      // boolean: SDL_FALSE or SDL_TRUE
        uint8_t quit;       // boolean: SDL_FALSE or SDL_TRUE
        uint8_t end;        // boolean: SDL_FALSE or SDL_TRUE
        uint8_t player_hit; // boolean: SDL_FALSE or SDL_TRUE
        uint8_t refueling;  // boolean: SDL_FALSE or SDL_TRUE
        uint16_t fuel;
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
        uint8_t fuel_countdown;
        uint32_t fuel_up_msg_time_left;

        // -- misc --
        int player_car_pos_logical; // values in {0,1,2}
        double player_car_pos_renderer;
        opp_car_list* opp_cars;

        // -- events --
        SDL_TouchFingerEvent e_touch;
        uint8_t e_touch_processed;
        //int16_t e_accel_pos[2]; //unused
        //uint8_t e_screen_rot; //unused

        // -- sfx --
        SDL_AudioDeviceID audio_device_id;
        uint32_t sfx_wav_length;
        uint8_t *sfx_wav_buffer;
#ifdef DISPLAY_DEBUG_MSG
        string_linked debug_messages;
#endif

    } info_exchange;

    // function initialise the contents of the info exchange structure
    void init_info_exchange(info_exchange *init_var, int argc, char **argv);
    void free_info_exchange(info_exchange *free_var);
    void init_handedness( info_exchange *init_var );
    void calc_scaling(info_exchange *state);

    // UTILS

    #define ASSERT_NOT_NULL( var ) if ( var == NULL ) { \
                                           printf( "Assert fail: var was NULL (file '%s', line %d)\n", \
                                                   __FILE__, __LINE__ ); \
                                           exit( EXIT_FAILURE ); }


    int
    check_bounds8(uint8_t *val, char *err_msg, uint8_t min, uint8_t max, uint8_t std);
    int check_bounds16(uint16_t *val, char *err_msg, uint16_t min, uint16_t max, uint16_t std);
    void     strip_char( char *str, const char c );
    void     strip_comments( char *str );
    uint32_t count_char_instances( const char *str, const char c );
    uint32_t count_items_CSV_line( const char *str );
    char**   split_CSV_line( char *str, uint32_t *nb_items );
    char**   split_str_lines( char *str, uint32_t *nb_lines );
    char*    read_file_SDL( const char *file_path );
    void     init_uint16_linked( uint16_linked **ptr );
    void     push_uint16_linked( uint16_linked *chain, uint16_t val );
    char *   uint16_linked_toString( uint16_linked *chain );
    uint16_t uint16_linked_count( uint16_linked *chain );
    void     push_string_linked( string_linked *chain, char *str );
    void     free_string_linked( string_linked *chain );

#endif
