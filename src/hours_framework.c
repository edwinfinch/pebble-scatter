#include <pebble.h>
#include "hours_framework.h"
#include "data_framework.h"
#include "settings_link.h"
	
Layer *hours_layer;
int square_coords[2][24];	
int square_coords_b[2][24];
AppTimer *squaretimer;
AppTimer *pulse_timer_h;
int s_toshow = 10, s_enabled = 0;
int anim_speed = 100;
uint8_t s_stage = 0;
int sq_value = 0;
int sq_pos_change = 0;
bool pulse_enabled_s = false;
uint8_t switch_s = 0;
bool pulse_s_shrink = 0;

void switch_hours(){
	switch_s++;
	bool is_res, is_valid;
	is_res = false;
	is_valid = false;
	int x, y;
	int fix_pos[2];
	clear_reserved_coordinate(square_coords_b[0][switch_s], square_coords_b[1][switch_s], true);
	loop_fix:
	x = get_random_x(false);
	y = get_random_y(false);
						
	is_valid = is_valid_coordinate(x, y, true);
	is_res = is_coordinate_reserved(x, y, true);
	if(!is_valid || is_res){
		goto loop_fix;
	}
	reserve_coordinate(x, y);
	fix_pos[0] = (x*8)+1;
	fix_pos[1] = (y*8);
		
	square_coords[0][switch_s] = fix_pos[0];
	square_coords[1][switch_s] = fix_pos[1];
	
	if(switch_s > s_toshow){
		switch_s = 0;
	}
	layer_mark_dirty(hours_layer);
}

void pulse_callback(){
	s_stage++;
	if(pulse_s_shrink){
		if(s_stage < 5){
			sq_value -= 2;
			sq_pos_change--;
		}
		else{
			sq_value += 2;
			sq_pos_change++;
		}
	}
	else{
		if(s_stage < 5){
			sq_value += 2;
			sq_pos_change++;
		}
		else{
			sq_value -= 2;
			sq_pos_change--;
		}
	}
		
	if(s_stage < 8){
		pulse_timer_h = app_timer_register(100, pulse_callback, NULL);
	}
	else{
		s_stage = 0;
		sq_value = 7;
		sq_pos_change = 0;
	}
	layer_mark_dirty(hours_layer);
}

void pulse_hours(bool shrink){
	pulse_enabled_s = true;
	s_stage = 0;
	pulse_s_shrink = shrink;
	pulse_timer_h = app_timer_register(100, pulse_callback, NULL);
}

bool hours_loaded(){
	if(s_toshow == s_enabled){
		return true;
	}
	return false;
}

void update_anim_ms_squares(){
	switch(get_animation_setting()){
		case 0:
			anim_speed = 100;
			break;
		case 1:
			anim_speed = 50;
			break;
		case 2:
			anim_speed = get_animation_ms();
			break;
		case 3:
			anim_speed = 15;
			break;
	}
}

void hours_set_hidden(bool hidden){
	layer_set_hidden(hours_layer, hidden);
}

void clear_squares(){
	s_toshow = 0;
	s_enabled = 0;
	layer_mark_dirty(hours_layer);
}

void show_squares(){
	layer_mark_dirty(hours_layer);
	if(s_enabled == s_toshow){
		return;
	}
	s_enabled++;
	update_anim_ms_squares();
	squaretimer = app_timer_register(anim_speed, show_squares, NULL);
}

void fire_square_animation(){
	int i = get_time(0);
	s_toshow = i;
	s_enabled = 0;
	while(i > 0){
		bool is_res, is_valid;
		is_res = false;
		is_valid = false;
		int x, y;
		int fix_pos[2];
		
		//for some fucking shit reason my while loop was being ignored
		//so now we use goto
		
		loop_fix:
		x = get_random_x(false);
		y = get_random_y(false);
						
		is_valid = is_valid_coordinate(x, y, true); //yes
		is_res = is_coordinate_reserved(x, y, true);
		if(!is_valid || is_res){
			goto loop_fix;
		}
		reserve_coordinate(x, y);
		fix_pos[0] = (x*8)+1;
		fix_pos[1] = (y*8);
		
		square_coords[0][i] = fix_pos[0];
		square_coords[1][i] = fix_pos[1];
		square_coords_b[0][i] = x;
		square_coords_b[1][i] = y;
		
		//APP_LOG(APP_LOG_LEVEL_INFO, "X: %d, Y: %d (%d, %d)", fix_pos[0], fix_pos[1], x, y);
		
		i--;
	}
	squaretimer = app_timer_register(50, show_squares, NULL);
}

void hours_proc(Layer *layer, GContext *ctx){
	graphics_context_set_fill_color(ctx, GColorWhite);
	//APP_LOG(APP_LOG_LEVEL_INFO, "sqposc == %d, sq_value == %d, s_stage == %d", sq_pos_change, sq_value, s_stage);
	for(int j = s_enabled; j > 0; j--){
		if(pulse_enabled_s){
			graphics_fill_rect(ctx, GRect(square_coords[0][j]-sq_pos_change, square_coords[1][j]-sq_pos_change, sq_value, sq_value), 0, GCornerNone);
			graphics_draw_rect(ctx, GRect(square_coords[0][j]-sq_pos_change, square_coords[1][j]-sq_pos_change, sq_value, sq_value));
		}
		else{
			graphics_fill_rect(ctx, GRect(square_coords[0][j], square_coords[1][j], 7, 7), 0, GCornerNone);
			graphics_draw_rect(ctx, GRect(square_coords[0][j], square_coords[1][j], 7, 7));
		}
	}
}

void update_hours_layer(){
	fire_square_animation();
}

void hours_layer_init(Window *window){
	hours_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(hours_layer, hours_proc);
	layer_add_child(window_get_root_layer(window), hours_layer);
}

void hours_layer_deinit(){
	layer_destroy(hours_layer);
}