#ifndef SYSTEM_UTILS_H
#define SYSTEM_UTILS_H

void update_frame_timer();
double get_frame_delta_time();
double get_frame_game_time();
void initTermios();
void term_print(int on_flag);
int get_random(int from, int to);
#endif