#ifndef SYSTEM_UTILS_H
#define SYSTEM_UTILS_H

struct position
{
    int x;
    int y;
};

struct dbl_position
{
    double x;
    double y;
};

struct zone
{
    struct position up_left;
    struct position down_right;
};

void update_frame_timer();
double get_frame_delta_time();
double get_frame_game_time();

void invoke(void (*func)(), float delay_sec);
void invoke_handler();

void initTermios();
void term_print(int on_flag);

int get_random(int from, int to);
#endif