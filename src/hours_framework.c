#include <pebble.h>
#include "hours_framework.h"
#include "data_framework.h"
#include "settings_link.h"
	
Layer *hours_layer;
int square_coords[2][24];	
AppTimer *squaretimer;
int s_toshow = 10, s_enabled = 0;
int anim_speed = 100;
	
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
						
		is_valid = is_valid_coordinate(x, y, true);
		is_res = is_coordinate_reserved(x, y, true);
		if(!is_valid || is_res){
			goto loop_fix;
		}
		reserve_coordinate(x, y);
		fix_pos[0] = (x*8)+1;
		fix_pos[1] = (y*8);
		
		square_coords[0][i] = fix_pos[0];
		square_coords[1][i] = fix_pos[1];
		
		//APP_LOG(APP_LOG_LEVEL_INFO, "X: %d, Y: %d (%d, %d)", fix_pos[0], fix_pos[1], x, y);
		
		i--;
	}
	squaretimer = app_timer_register(50, show_squares, NULL);
}

void hours_proc(Layer *layer, GContext *ctx){
	graphics_context_set_fill_color(ctx, GColorWhite);
	for(int j = s_enabled; j > 0; j--){
		graphics_fill_rect(ctx, GRect(square_coords[0][j], square_coords[1][j], 7, 7), 0, GCornerNone);
		graphics_draw_rect(ctx, GRect(square_coords[0][j], square_coords[1][j], 7, 7));
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