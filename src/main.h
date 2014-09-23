#pragma once
void tick_handler_fire();

Window *window;
BitmapLayer *background_layer;
TextLayer *time_layer, *date_layer;
GBitmap *background;
Layer *rect_layer;
int battery_percent = 100;
AppTimer *m_timer, *refresh_timer;
bool booted_m = false;
InverterLayer *custom_t;

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
}Persist;

Persist settings;