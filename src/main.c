#include <pebble.h>
#include "minutes_framework.h"
#include "hours_framework.h"
#include "data_framework.h"
#include "main.h"
#include "settings_link.h"
	
int get_animation_ms(){
	newrand:;
	int ms = rand() % 150;
	if(ms < 25){
		goto newrand;
	}
	return ms;
}

int get_animation_setting(){
	return settings.animation_setting;
}
	
void stopped(Animation *anim, bool finished, void *context){
    property_animation_destroy((PropertyAnimation*) anim);
}
 
void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay){
    PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);
     
    animation_set_duration((Animation*) anim, duration);
    animation_set_delay((Animation*) anim, delay);
     
    AnimationHandlers handlers = {
        .stopped = (AnimationStoppedHandler) stopped
    };
    animation_set_handlers((Animation*) anim, handlers, NULL);
     
    animation_schedule((Animation*) anim);
}

static TextLayer* text_layer_init(GRect location, bool is_date)
{
	TextLayer *layer = text_layer_create(location);
	text_layer_set_text_color(layer, GColorWhite);
	text_layer_set_background_color(layer, GColorClear);
	text_layer_set_text_alignment(layer, GTextAlignmentCenter);
	text_layer_set_font(layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMPACT_32)));
	if(is_date){
		text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	}
	return layer;
}

void refresh_settings(bool boot){
	layer_set_hidden(inverter_layer_get_layer(custom_t), settings.theme);
	minutes_set_hidden(settings.circles);
	hours_set_hidden(settings.squares);
	layer_mark_dirty(rect_layer);
	if(!boot){
		vibes_double_pulse();
	}
}

void process_tuple(Tuple *t)
{
	int key = t->key;
	int value = t->value->int32;
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Loaded key num: %d with value %d", key, value);
  switch (key) {
	  case 0:
	  	settings.btdisalert = value;
	  	break;
	  case 1:
	  	settings.btrealert = value;
	  	break;
	  case 2:
	  	settings.theme = value;
	  	break;
	  case 3:
	 	settings.batterybar = value;
	  	break;
	  case 4:
	  	settings.animations = value;
	  	break;
	  case 5:
	  	settings.circles = value;
	  	break;
	  case 6:
	  	settings.squares = value;
	  	break;
	  case 7:
	  	settings.hourlyvibe = value;
	  	break;
	  case 8:
	  	settings.animation_setting = value;
	  	break;
  }
}

void tuples_processed(){
	refresh_settings(false);
}

static void in_received_handler(DictionaryIterator *iter, void *context) 
{
	refresh_timer = app_timer_register(1000, tuples_processed, NULL);
	Tuple *t = dict_read_first(iter);
	//APP_LOG(APP_LOG_LEVEL_INFO, "got data");
	if(t)
	{
		process_tuple(t);
	}
	while(t != NULL)
	{
		t = dict_read_next(iter);
		if(t)
		{
			process_tuple(t);
		}
	}
}

void tick_handler(struct tm *t, TimeUnits unit){
	if(!booted_m && settings.animations){
		booted_m = true;
		m_timer = app_timer_register(1500, tick_handler_fire, NULL);
		return;
	}
	static char buffer[] = "00 00";
	if(clock_is_24h_style()){
		strftime(buffer, sizeof(buffer), "%H:%M", t);
   	}
   	else{
		strftime(buffer,sizeof(buffer), "%I:%M", t);
   	}
	
	static char d_buffer[] = "Fri. xx.xx.'xx.";
	strftime(d_buffer, sizeof(d_buffer), "%a %d.%m.'%y", t);
	
	text_layer_set_text(time_layer, buffer);
	text_layer_set_text(date_layer, d_buffer);
	
	set_t_dfw(t);
	update_minutes_layer();
	
	if(t->tm_min == 0 && booted_m && settings.hourlyvibe){
		vibes_short_pulse();
	}
}

void tick_handler_fire(){
	struct tm *t;
  	time_t temp;        
  	temp = time(NULL);        
  	t = localtime(&temp);
	tick_handler(t, MINUTE_UNIT);
}

void rect_proc(Layer *l, GContext *ctx){
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_rect(ctx, GRect(31, 48, 83, 57), 4, GCornersAll);
	if(settings.batterybar){
		graphics_context_set_fill_color(ctx, GColorWhite);
		graphics_fill_rect(ctx, GRect(36, 105, 73, 6), 0, GCornerNone);
		graphics_draw_rect(ctx, GRect(36, 105, 73, 6));

		graphics_context_set_fill_color(ctx, GColorBlack);
		for(int i = battery_percent/10; i > 0; i--){
			int fix = 38+((i-1)*7);
			graphics_fill_rect(ctx, GRect(fix, 107, 6, 2), 0, GCornerNone);
		}
	}
}

void battery_handler(BatteryChargeState state){
	battery_percent = state.charge_percent;
	layer_mark_dirty(rect_layer);
}

void bt_handler(bool connected){
	if(settings.btdisalert && !connected){
		vibes_long_pulse();
	}
	if(settings.btrealert && connected){
		vibes_double_pulse();
	}
}

void window_load(Window *w){
	Layer *window_layer = window_get_root_layer(w);
	background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
	
	background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	bitmap_layer_set_bitmap(background_layer, background);
	layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));
	
	rect_layer = layer_create(GRect(0, -300, 144, 168));
	layer_set_update_proc(rect_layer, rect_proc);
	layer_add_child(window_layer, rect_layer);
	
	time_layer = text_layer_init(GRect(33, -300, 80, 38), false);
	text_layer_set_text(time_layer, "00 00");
	layer_add_child(window_layer, text_layer_get_layer(time_layer));
	
	date_layer = text_layer_init(GRect(33, -300, 80, 38), true);
	text_layer_set_text(date_layer, "Fri, 22.08.'14");
	layer_add_child(window_layer, text_layer_get_layer(date_layer));
// ghhgftyghghgty hey whats up 10010101010100101101001011011100011011010111010
	
	minutes_layer_init(w);
	hours_layer_init(w);
	
	custom_t = inverter_layer_create(GRect(0, 0, 144, 168));
	layer_add_child(window_layer, inverter_layer_get_layer(custom_t));
	
	struct tm *t;
  	time_t temp;        
  	temp = time(NULL);        
  	t = localtime(&temp);
	static char buffer[] = "00 00";
	if(clock_is_24h_style()){
		strftime(buffer, sizeof(buffer), "%H:%M", t);
   	}
   	else{
		strftime(buffer,sizeof(buffer), "%I:%M", t);
   	}
	/*
	We don't call tick_handler on load because it causes this small flashy
	effect that can't happen or else the watchface will be not perfect
	*/
	
	static char d_buffer[] = "Fri. xx.xx.'xx.";
	strftime(d_buffer, sizeof(d_buffer), "%a %d.%m.'%y", t);
	
	text_layer_set_text(time_layer, buffer);
	text_layer_set_text(date_layer, d_buffer);
	
	refresh_settings(true);
	
	if(settings.animations){
		animate_layer(bitmap_layer_get_layer(background_layer), &GRect(0, 0, 144, 0), &GRect(0, 0, 144, 168), 700, 0);
		animate_layer(rect_layer, &GRect(0, 300, 144, 168), &GRect(0, 0, 144, 168), 1000, 700);
		animate_layer(text_layer_get_layer(time_layer), &GRect(33, -200, 80, 38), &GRect(33, 50, 80, 38), 700, 1200);
		animate_layer(text_layer_get_layer(date_layer), &GRect(33, 300, 80, 38), &GRect(33, 83, 80, 38), 700, 1200);
    }
	else{
		layer_set_frame(bitmap_layer_get_layer(background_layer), GRect(0, 0, 144, 168));
		layer_set_frame(rect_layer, GRect(0, 0, 144, 168));
		layer_set_frame(text_layer_get_layer(time_layer), GRect(33, 50, 80, 38));
		layer_set_frame(text_layer_get_layer(date_layer), GRect(33, 83, 80, 38));
	}
}

void window_unload(Window *w){
	gbitmap_destroy(background);
	bitmap_layer_destroy(background_layer);
	text_layer_destroy(date_layer);
	text_layer_destroy(time_layer);
	minutes_layer_deinit();
	hours_layer_deinit();
	layer_destroy(rect_layer);
}

void init(){
	window = window_create();
	window_set_background_color(window, GColorBlack);
	window_set_window_handlers(window, (WindowHandlers){
		.load = window_load,
		.unload = window_unload,
	});
	battery_state_service_subscribe(battery_handler);
	BatteryChargeState state = battery_state_service_peek();
	battery_percent = state.charge_percent;
	
	bluetooth_connection_service_subscribe(bt_handler);
	
	app_message_register_inbox_received(in_received_handler);
	app_message_open(512, 512);
	
	int value = persist_read_data(0, &settings, sizeof(settings));
	APP_LOG(APP_LOG_LEVEL_INFO, "%d bytes read", value);
	
	tick_timer_service_subscribe(MINUTE_UNIT, &tick_handler);
	srand(time(NULL));
	window_stack_push(window, true);
}

void deinit(){
	int value = persist_write_data(0, &settings, sizeof(settings));
	APP_LOG(APP_LOG_LEVEL_INFO, "%d bytes written", value);
	battery_state_service_unsubscribe();
	window_destroy(window);
}
	
int main(){
	init();
	app_event_loop();
	deinit();
}