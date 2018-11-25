#ifndef GAMELOGIC
	#define GAMELOGIC

	#include <stdlib.h>
	#include <SDL.h>
	#include <math.h>
	#include "renderer.h"
	#include "info_struct.h"
	#include "audio_sfx.h"
	#include "values.h"
	#include "sdl2_func.h"

	int game_logic(
        info_exchange* state,
        double* partial_scroll_state);

	void touch_event_handler(
        info_exchange *state,
        int lane_relative_width);

	void calc_fuel_pointer_position(info_exchange *state);
    void free_slots(opp_car_list *in);
	void player_collision(info_exchange *state);
	uint16_t get_timer(info_exchange *state);
	void out_of_fuel(info_exchange * state);
	void manual_refuel(info_exchange *state);
	void show_fuel(info_exchange *state);
	void generate_new_opp_car_line(info_exchange *state);

#endif
