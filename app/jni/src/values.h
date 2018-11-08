#ifndef VALUES_H

    #define VALUES_H

    #include "info_struct.h"

    #define MIN_FUEL 0

    #define LEFT_LANE 0
    #define MIDDLE_LANE 1
    #define RIGHT_LANE 2

    // Comment out the following line to not display the debug messages/info
    //#define DISPLAY_DEBUG_MSG
    //#define DISPLAY_DEBUG_INFO
    #define DEBUG_MSG_TIMEOUT 5000

    #define GAME_OVER_MAX_TRANSPARENCY 210

    #define EXPECTED_STAT_ITEMS 18

    #define HIST_SCORES 5
    #define HIST_SCORES_LEN 9

    /* Score norm :
     * avoid cars  +33 points
     * crash      -100 points
     * refuel     +300 points
     *
     * Note: MIN_SCORE and MAX_SCORE were selected because we want a 5-digit score
     */
    #define MIN_SCORE -9999
    #define MAX_SCORE 99999
    #define SCORE_AVOID( state )  state->score += state->avoid_reward; \
                                  /*push_string_linked( &(state->debug_messages), "Avoided!" );*/ \
                                  if (MAX_SCORE < state->score) { state->score = MAX_SCORE; }
    #define SCORE_REFUEL( state ) state->score += state->refuel_reward; \
                                  /*push_string_linked( &(state->debug_messages), "Refueled!" );*/ \
                                  if (MAX_SCORE < state->score) { state->score = MAX_SCORE; }
    #define SCORE_CRASH( state )  state->score -= state->crash_penalty; \
                                  /*push_string_linked( &(state->debug_messages), "Crashed!" );*/ \
                                  if (state->score < MIN_SCORE) { state->score = MIN_SCORE; }
    #define SCORE_NOFUEL( state ) state->score -= state->nofuel_penalty; \
                                  /*push_string_linked( &(state->debug_messages), "No Fuel!" );*/ \
                                  if (state->score < MIN_SCORE) { state->score = MIN_SCORE; }

    /* Argument norm :
     * 0: Useless
     * 1: Integer, ID/Code
     * 2: Integer, Age
     * 3: String, Sex
     * 4: String, Handedness
     * 5: String, file location for stat database
     * 6: String, file location for game config
     */
    #define ARGV_IDCODE   1
    #define ARGV_AGE      2
    #define ARGV_SEX      3
    #define ARGV_HAND     4
    #define ARGV_STATFILE 5
    #define ARGV_CONFFILE 6

    #define ARGV_HAND_L  'L'
    #define ARGV_HAND_R  'R'

    /* Configuration norm :
     * 0: uint8_t, Game duration [s]
     * 1: uint8_t, Fuel tank duration [s]
     * 2: uint8_t, Speed [%]
     * 3: uint16_t, Show Fuel duration [ms]
     * 4: uint16_t, Display numeric clock [boolean = {0,1}]
     * 5: uint16_t, Avoid crash reward [pts]
     * 6: uint16_t, Refuel reward [pts]
     * 7: uint16_t, Crash penalty [pts]
     * 8: uint16_t, Out of Fuel penalty [pts]
     */
    #define CONF_GAME_DURATION  0
    #define CONF_FUEL_DURATION  1
    #define CONF_REL_SPEED      2
    #define CONF_SHOW_DURATION  3
    #define CONF_DSPLY_NUM_CLK  4
    #define CONF_AVOID_REWARD   5
    #define CONF_REFUEL_REWARD  6
    #define CONF_CRASH_PENALTY  7
    #define CONF_NOFUEL_PENALTY 8
    #define CONF_EXPECTED_NB_ITEMS 9

    /* Norm :
     *  19 < CONF_GAME_DURATION  <  601
     *  19 < CONF_FUEL_DURATION  <  301
     *  49 < CONF_REL_SPEED      <  201
     * 999 < CONF_SHOW_DURATION  < 5001
     *  -1 < CONF_AVOID_REWARD   < 1001
     *  -1 < CONF_REFUEL_REWARD  < 3001
     *  -1 < CONF_CRASH_PENALTY  < 1001
     *  -1 < CONF_NOFUEL_PENALTY < 1001
     */
    #define CONF_GAME_DURATION_MIN   19
    #define CONF_GAME_DURATION_MAX  601
    #define CONF_GAME_DURATION_STD  120
    #define CONF_FUEL_DURATION_MIN   19
    #define CONF_FUEL_DURATION_MAX  301
    #define CONF_FUEL_DURATION_STD   40
    #define CONF_REL_SPEED_MIN       49
    #define CONF_REL_SPEED_MAX      201
    #define CONF_REL_SPEED_STD      100
    #define CONF_SHOW_DURATION_MIN  999
    #define CONF_SHOW_DURATION_MAX 5001
    #define CONF_SHOW_DURATION_STD 2000
    #define CONF_AVOID_REWARD_MIN    -1
    #define CONF_AVOID_REWARD_MAX  1001
    #define CONF_AVOID_REWARD_STD    33
    #define CONF_REFUEL_REWARD_MIN   -1
    #define CONF_REFUEL_REWARD_MAX 3001
    #define CONF_REFUEL_REWARD_STD  300
    #define CONF_CRASH_PENALTY_MIN   -1
    #define CONF_CRASH_PENALTY_MAX 1001
    #define CONF_CRASH_PENALTY_STD  100
    #define CONF_NOFUEL_PENALTY_MIN   -1
    #define CONF_NOFUEL_PENALTY_MAX 1001
    #define CONF_NOFUEL_PENALTY_STD  100

#endif