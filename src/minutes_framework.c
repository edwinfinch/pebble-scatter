#include <pebble.h>
#include "minutes_framework.h"
#include "hours_framework.h"
#include "data_framework.h"
#include "settings_link.h"
	
Layer *minutes_layer;
int circle_coords[2][60];	
int circle_coords_b[2][60];
AppTimer *circletimer;
int c_toshow = 10, c_enabled = 0;
int anim_speed_m = 100;
	
AppTimer *pulse_timer_m;
uint8_t c_stage = 0;
uint8_t ci_value = 0;
bool pulse_enabled_c = false;
uint8_t switch_c = 0;

void switch_minutes(){
	switch_c++;
	bool is_res, is_valid;
	is_res = false;
	is_valid = false;
	int x, y;
	int fix_pos[2];
	clear_reserved_coordinate(circle_coords_b[0][switch_c], circle_coords_b[1][switch_c], false);
	loop_fix:
	x = get_random_x(true);
	y = get_random_y(false);				
	is_valid = is_valid_coordinate(x, y, false);
	is_res = is_coordinate_reserved(x, y, false);
	if(!is_valid || is_res){
		goto loop_fix;
	}
	reserve_coordinate(x, y);
	fix_pos[0] = (x*8)-4;
	fix_pos[1] = (y*8)-5;
		
	circle_coords[0][switch_c] = fix_pos[0];
	circle_coords[1][switch_c] = fix_pos[1];
	
	if(switch_c > c_toshow){
		switch_c = 0;
	}
	layer_mark_dirty(minutes_layer);
}

void pulse_callback_c(){
	c_stage++;
	if(c_stage < 5){
		ci_value += 1;
	}
	else{
		ci_value -= 1;
	}
		
	if(c_stage < 8){
		pulse_timer_m = app_timer_register(100, pulse_callback_c, NULL);
	}
	else{
		c_stage = 0;
		ci_value = 3;
	}
	layer_mark_dirty(minutes_layer);
}

void pulse_minutes(){
	pulse_enabled_c = true;
	c_stage = 0;
	pulse_timer_m = app_timer_register(100, pulse_callback_c, NULL);
}

bool minutes_loaded(){
	if(c_toshow == c_enabled){
		return true;
	}
	return false;
}

void update_anim_ms_circles(){
	switch(get_animation_setting()){
		case 0:
			anim_speed_m = 50;
			break;
		case 1:
			anim_speed_m = 100;
			break;
		case 2:
			anim_speed_m = get_animation_ms();
			break;
		case 3:
			anim_speed_m = 15;
			break;
	}
}

void minutes_set_hidden(bool hidden){
	layer_set_hidden(minutes_layer, hidden);
}

void show_circles(){
	layer_mark_dirty(minutes_layer);
	if(c_enabled == c_toshow){
		update_hours_layer();
		return;
	}
	c_enabled++;
	update_anim_ms_circles();
	circletimer = app_timer_register(anim_speed_m, show_circles, NULL);
}

void fire_circle_animation(){
	clear_squares();
	int i = get_time(1);
	clear_all_reserved();
	c_toshow = i;
	c_enabled = 0;
	while(i > 0){
		bool is_res, is_valid;
		is_res = false;
		is_valid = false;
		int x, y;
		int fix_pos[2];
		
		//for some fucking shit reason my while loop was being ignored
		//so now we use goto
		
		loop_fix:
		x = get_random_x(true);
		y = get_random_y(false);
						
		is_valid = is_valid_coordinate(x, y, false);
		is_res = is_coordinate_reserved(x, y, false);
		if(!is_valid || is_res){
			goto loop_fix;
		}
		
		//APP_LOG(APP_LOG_LEVEL_INFO, "reserving coordinate %d, %d", pos[0], pos[1]);
		reserve_coordinate(x, y);
		fix_pos[0] = (x*8)-4;
		fix_pos[1] = (y*8)-5;
		
		circle_coords[0][i] = fix_pos[0];
		circle_coords[1][i] = fix_pos[1];
		circle_coords_b[0][i] = x;
		circle_coords_b[1][i] = y;
		
		//APP_LOG(APP_LOG_LEVEL_INFO, "X: %d, Y: %d (%d, %d)", fix_pos[0], fix_pos[1], x, y);
		
		i--;
	}
	circletimer = app_timer_register(50, show_circles, NULL);
}

void minutes_proc(Layer *layer, GContext *ctx){
	for(int j = c_enabled; j > 0; j--){
		if(pulse_enabled_c){
			graphics_context_set_stroke_color(ctx, GColorWhite);
			graphics_fill_circle(ctx, GPoint(circle_coords[0][j], circle_coords[1][j]), ci_value);
			graphics_draw_circle(ctx, GPoint(circle_coords[0][j], circle_coords[1][j]), ci_value+1);
		}
		else{
			graphics_fill_circle(ctx, GPoint(circle_coords[0][j], circle_coords[1][j]), 3);
		}
	}
}

void update_minutes_layer(){
	fire_circle_animation();
}

void minutes_layer_init(Window *window){
	minutes_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(minutes_layer, minutes_proc);
	layer_add_child(window_get_root_layer(window), minutes_layer);
}

void minutes_layer_deinit(){
	layer_destroy(minutes_layer);
}