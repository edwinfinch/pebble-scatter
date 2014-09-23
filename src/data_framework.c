#include <pebble.h>
#include "data_framework.h"
	
bool dot_taken[1000];
struct tm *t_dfw;
	
bool x_is_blackzone(int x, bool isSquare){
	int limit_1 = 3, limit_2 = 16;
	if(isSquare){
		limit_1 = 2;
		limit_2 = 15;
	}
	
	if(x > limit_1 && x < limit_2){
		return true;
	}
	if(isSquare){
		if((x*8)+1 < 10 || (x*8)+1 > 145){
			return true;
		}
	}
	else{
		if((x*8)-4 < 10 || (x*8)-4 > 145){
			return true;
		}
	}
	return false;
}

bool y_is_blackzone(int y, bool isSquare){
	int limit_1 = 6, limit_2 = 15;
	if(isSquare){
		limit_1 = 5;
		limit_2 = 14;
	}
	
	if(y > limit_1 && y < limit_2){
		return true;
	}
	if(isSquare){
		if((y*8) < 10 || (y*8) > 176){
			return true;
		}
	}
	else{
		if((y*8)-5 < 10 || (y*8)-5 > 176){
			return true;
		}
	}
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Returning Y: %d", y);
	return false;
}

void clear_all_reserved(){
	for(int i = 270; i > -1; i--){
		dot_taken[i] = false;
	}
}

void reserve_coordinate(int x, int y){
	int num = x*y;
	//APP_LOG(APP_LOG_LEVEL_INFO, "%d, %d, %d", num, coords[0], coords[1]);
	dot_taken[num] = num;
}

bool is_coordinate_reserved(int x, int y, bool isSquareRequest){
	if(isSquareRequest){
		x++;
		y++;
	}
	int num = x*y;
	bool reserved = dot_taken[num];
	return reserved;
}

bool is_valid_coordinate(int x, int y, bool isSquare){
	if(x_is_blackzone(x, isSquare) && y_is_blackzone(y, isSquare)){
		return false;
	}
	return true;
}

int get_random_x(bool is_circle){
	int x = rand() % 18;
	if(is_circle && x == 0){
		x++;
	}
	
	return x;
}

int get_random_y(bool is_circle){
	int y = rand() % 21;
	
	if(y == 0 && is_circle){
		y++;
	}
	
	return y;
}

//time stuff
void set_t_dfw(struct tm *t){
	t_dfw = t;
}

int get_time(int time){
	switch(time){
		case 0:
			return t_dfw->tm_hour;
			break;
		case 1:
			return t_dfw->tm_min;
			break;
		case 2:
			return t_dfw->tm_sec;
			break;
	}
	return 0;
}