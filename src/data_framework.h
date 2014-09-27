#pragma once
	
void set_t_dfw(struct tm *t);
int get_time(int time);
void clear_all_reserved();
void reserve_coordinate(int x, int y);
bool is_valid_coordinate(int x, int y, bool isSquare);
bool is_coordinate_reserved(int x, int y, bool isSquareRequest);
void clear_reserved_coordinate(int x, int y, bool isSquare);
int get_random_x(bool is_circle);
int get_random_y(bool is_circle);