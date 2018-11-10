#include "info_struct.h"

void init_info_exchange(info_exchange *init_var, int argc, char **argv)
{
    // --global program arguments--
    init_var->argc = argc;
    init_var->argv = argv;

    // -- screen state --
    init_var->orientation_change = 0;
    init_var->scaling_mode = 0;
    //init_var->display;
    //init_var->road_size;
    //init_var->car_size;
    //init_var->clock_texture_char_size;

    // -- game statistics and info --
    init_var->date_at_launch = NULL;
    init_uint16_linked( &(init_var->hit_times) );
    init_uint16_linked( &(init_var->refuel_times) );
    init_uint16_linked( &(init_var->auto_refuel_times) );
    init_uint16_linked( &(init_var->show_times) );
    init_uint16_linked( &(init_var->void_times) );
    init_var->score = 0;
    for(int i = 0; i < HIST_SCORES; i++)
    {
        init_var->score_hist[i][0] = '\0'; // lowest value possible
    }

    // -- game settings --
    //init_var->show_fuel_duration_ms;
    init_handedness( init_var );
    //init_var->max_fuel;
    //init_var->rel_speed;
    //init_var->time_total;
    //init_var->difficulty;
    //init_var->display_numeric_clock;
    //init_var-crash_penalty;
    //init_var-avoid_reward;
    //init_var-refuel_reward;

    // -- game state --
    //init_var->time_left;
    while ( (init_var->numeric_clock = malloc( 6 * sizeof(char) ) ) == NULL );
    //init_var->scroll_state;
    init_var->collision = SDL_FALSE;
    init_var->pause = SDL_FALSE;
    init_var->quit = SDL_FALSE;
    init_var->end = SDL_FALSE;
    init_var->player_hit = SDL_FALSE;
    init_var->refueling = SDL_FALSE;
    //init_var->fuel;
    init_var->fuel_pointer_position = 120;
    init_var->time_game_start = SDL_GetTicks();
    init_var->time_game_end = 0;
    init_var->time_last_second_tick = init_var->time_game_start;
    //init_var->time_last_check_tick;
    init_var->time_last_show_fuel_tick = 0;
    //init_var->play_area;
    //init_var->menu_area;

    // -- positions --
    init_var->player_car_pos_logical = 1;
    init_var->player_car_pos_renderer = 0;
    // we don't generate the first opposite car line here anymore
    //init_var->opp_cars;
    init_var->opp_cars = NULL;

    // -- events --
    //init_var->e_touch;
    init_var->e_touch_processed = SDL_TRUE;
    //init_var->e_accel_pos; //unused
    //init_var->e_screen_rot; //unused

    //init_var->debug_messages = (string_linked*) malloc( sizeof(string_linked) );
    (init_var->debug_messages).first = NULL;
    (init_var->debug_messages).last  = NULL;

}

void free_info_exchange(info_exchange *free_var)
{
    // -- game statistics and info --
    free(free_var->date_at_launch);
    free(free_var->hit_times);
    free(free_var->refuel_times);
    free(free_var->auto_refuel_times);
    free(free_var->show_times);
    free(free_var->void_times);

    // -- game state --
    free(free_var->numeric_clock);

    free_string_linked( &(free_var->debug_messages) );
}

void init_handedness( info_exchange *init_var )
{
    init_var->hand = NO_HANDED;
    if ( strchr( init_var->argv[ARGV_HAND], ARGV_HAND_L ) != NULL )
        init_var->hand |= LEFT_HANDED;
    if ( strchr( init_var->argv[ARGV_HAND], ARGV_HAND_R ) != NULL )
        init_var->hand |= RIGHT_HANDED;
}

void calc_scaling(info_exchange *state)
{
    if (state->display.h > state->display.w)
        state->scaling_mode = (uint8_t) floor(state->display.w / 360);
    else
        state->scaling_mode = (uint8_t) floor(state->display.h / 600);
}

// UTILS

void check_bounds8(
        uint8_t *val, string_linked *err_msg, const int min, const int max, const int std )
{
    if ( *val <= min || max <= *val )
    {
        *val = (uint8_t) std;
        push_string_linked( err_msg, "Value out of range !" );
    }
}

void check_bounds16(
        uint16_t *val, string_linked *err_msg, const int min, const int max, const int std )
{
    if ( *val <= min || max <= *val )
    {
        *val = (uint16_t) std;
        push_string_linked( err_msg, "Value out of range !" );
    }
}

void strip_char( char *str, const char c )
{
    char *buffer = NULL;
    size_t len = (strlen(str)+1);
    while( ( buffer = (char *) malloc( len * sizeof(char) ) ) == NULL );
    uint16_t i = 0, j = 0;
    for( i = 0; i < len; i++ )
    {
        if ( str[i] != c )
            buffer[j++] = str[i];
    }
    buffer[j] = '\0';
    snprintf( str, len, "%s", buffer );
    free(buffer);
}

void strip_comments( char *str )
{
    char *ptr = NULL;
    if ( (ptr = strstr(str, "//")) == NULL )
        return;

    size_t len = ptr - str + 1;
    snprintf( str, len, "%s", str );

}

uint32_t count_char_instances( const char *str, const char c )
{
    uint32_t count = 0;
    size_t len = strlen(str);

    for( int i = 0; i <= len; i++)
    {
        if ( str[i] == c )
            count++;
    }

    return count;
}

uint32_t count_items_CSV_line( const char *str )
{
    uint32_t nb_items = 1;
    size_t len = strlen(str);
    char c = 0, inbound = 0;
    for( int i = 0; i < len; i++ )
    {
        c = str[i];
        if( !inbound && c == ',' )
        {
            nb_items++;
            continue;
        }
        if( !inbound && c == '\"' )
        {
            inbound = 1;
            continue;
        }
        else if( inbound && c == '\"' )
        {
            inbound = 0;
            continue;
        }
        if ( inbound && c == '\\' && str[i+1] == '\"' )
            i++;
    }
    return nb_items;
}

char** split_CSV_line( char *str, uint32_t *nb_items )
{
    *nb_items = count_items_CSV_line( str );
    char **items = (char**) malloc( (size_t) *nb_items * sizeof(char*) );
    ASSERT_NOT_NULL( items )

    uint32_t item_nber = 0;
    size_t len = strlen(str), lenItem = 0;
    char c = 0, inbound = 0;
    char *startItem = str;

    for( int i = 0; i < len; i++ )
    {
        c = str[i];
        if( !inbound && c == ',' )
        {
            lenItem = &str[i] - startItem + 1;

            char *item = (char*) malloc( (size_t) lenItem * sizeof(char) );
            ASSERT_NOT_NULL( item )
            snprintf( item, lenItem, "%s", startItem );
            items[item_nber++] = item;

            startItem = &str[i+1];
            continue;
        }
        if( !inbound && c == '\"' )
        {
            inbound = 1;
            continue;
        }
        else if( inbound && c == '\"' )
        {
            inbound = 0;
            continue;
        }
        if ( inbound && c == '\\' && str[i+1] == '\"' )
            i++;
    }

    // We still have to add the last item (the item after the last comma
    lenItem = len - (startItem - str) + 1;
    char *item = (char*) malloc( (size_t) lenItem * sizeof(char) );
    ASSERT_NOT_NULL( item )
    snprintf( item, lenItem, "%s", startItem );
    items[item_nber++] = item;

    return items;
}

char** split_str_lines( char *str, uint32_t *nb_lines )
{
    *nb_lines = count_char_instances( str, '\n' ) + 1;
    char **lines = (char**) malloc( (size_t) *nb_lines * sizeof(char*) );
    ASSERT_NOT_NULL( lines )

    char *token_line = strtok (str,"\n");
    uint32_t curr_line = 0, actual_lines = 0;

    while (token_line != NULL)
    {
        size_t len_line = strlen(token_line) + 1;
        char *line = (char*) malloc( len_line + 1 );
        ASSERT_NOT_NULL( line )

        snprintf( line, len_line, "%s", token_line );
        lines[curr_line++] = line;
        actual_lines++;

        token_line = strtok (NULL, "\n");
    }
    // As the amount of actual lines (non-empty lines) can be lower than the expected
    // amount of lines (determined as the number of 'end of line character' + 1), we
    // correct this value
    *nb_lines = actual_lines;

    return lines;
}

// Reads the content of a file to a string, using SDL methods
char* read_file_SDL( const char *file_path ) {
    // Code form SDL_RWread documentation, used to read a whole file in a buffer
    SDL_RWops *rw = SDL_RWFromFile( file_path, "r" );
    if ( rw == NULL )
    {
        // Error
        return NULL;
    }

    size_t file_size = (size_t) SDL_RWsize(rw);
    char *buffer = (char*) malloc(file_size+1);
    ASSERT_NOT_NULL( buffer )

    size_t nb_read_total = 0, nb_read = 1;
    while (nb_read_total < file_size && nb_read != 0) {
        nb_read = SDL_RWread( rw, &buffer[nb_read_total],
                              1, (file_size - nb_read_total) );
        nb_read_total += nb_read;
    }
    SDL_RWclose(rw);

    // Data size mismatch -> abort
    if ( 1 < abs( nb_read_total - file_size ) ) {
        free(buffer);
        return NULL;
    }

    buffer[file_size] = '\0';
    return buffer;
}

void init_uint16_linked( uint16_linked **ptr )
{
    if ( (*ptr = (uint16_linked *) malloc(sizeof(uint16_linked))) == NULL )
        exit( EXIT_FAILURE );

    (*ptr)->first = NULL;
    (*ptr)->last = NULL;
}

void push_uint16_linked( uint16_linked *chain, uint16_t val )
{
    // We allocate a new uint16_lt ..
    uint16_lt *new = NULL;
    if ( (new = (uint16_lt *) malloc( sizeof(uint16_lt) )) == NULL )
        exit( EXIT_FAILURE );
    // .. initialize it ..
    new->value = val;
    new->next = NULL;

    if ( chain->first == NULL )
    {
        // The chain contains no element
        chain->first = new; // .. and put it as the first element ..
    }
    else
    {
        // The chain contains at least one element
        (chain->last)->next = new; // .. and put it after the last element
    }

    chain->last = new; // .. and put it as the last element
}

char * uint16_linked_toString( uint16_linked *chain )
{
    // For debug only ! Not overflow safe !
    char *buffer;
    if ( (buffer = malloc(150)) == NULL )
        exit( EXIT_FAILURE );
    uint16_lt *curr = chain->first;
    int i = 0;
    i += sprintf( &buffer[i], "\"");
    while (curr != NULL && i < 140)
    {
        i += sprintf( &buffer[i], "%d,", curr->value);
        curr = curr->next;
    }
    if ( 1 < i ) i--;
    sprintf( &buffer[i], "\"");
    return buffer;
}

uint16_t uint16_linked_count( uint16_linked *chain )
{
    // Counts how many values are stored in the linked list
    uint16_t count = 0;
    uint16_lt *curr = (*chain).first;
    while (curr != NULL)
    {
        count++;
        curr = curr->next;
    }
    return count;
}

void push_string_linked( string_linked *chain, char *str )
{
    // We allocate a new uint16_lt ..
    string_lt *newLink = NULL;
    char *newStr = NULL;
    size_t len = (strlen(str)+1) * sizeof(char);
    if ( (newLink = (string_lt *) malloc( sizeof(string_lt) )) == NULL ||
         (newStr  = (char *)      malloc( len )) == NULL )
        exit( EXIT_FAILURE );

    // .. initialize it ..
    snprintf( newStr, len, "%s", str );
    newLink->str       = newStr;
    newLink->timestamp = SDL_GetTicks();
    newLink->next      = NULL;

    if ( chain->first == NULL )
    {
        // The chain contains no element
        chain->first = newLink; // .. and put it as the first element ..
    }
    else
    {
        // The chain contains at least one element
        (chain->last)->next = newLink; // .. and put it after the last element
    }

    chain->last = newLink; // .. and put it as the last element
}

void free_string_linked( string_linked *chain )
{
    // We allocate a new uint16_lt ..
    string_lt *currLink = NULL, *nextLink = NULL;
    currLink = chain->first;

    while( currLink != NULL )
    {
        nextLink = currLink->next;
        free( currLink->str );
        free( currLink );
        currLink = nextLink;
    }
}


