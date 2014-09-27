#pragma once
void tick_handler_fire();

Window *window;
BitmapLayer *background_layer;
TextLayer *time_layer, *date_layer, *update_at_a_glance;
GBitmap *background;
Layer *rect_layer;
int battery_percent = 100;
AppTimer *m_timer, *refresh_timer;
bool booted_m = false;
InverterLayer *custom_t;
AppTimer *pulse_timer_m;
bool changed_constanim = false;

typedef struct Persist {
	bool theme;
	bool btdisalert;
	bool btrealert;
	bool batterybar;
	bool animations;
	bool circles;
	bool squares;
	bool hourlyvibe;
	int animation_setting;
	int constant_animation;
	bool boot_glance;
}Persist;

Persist settings;