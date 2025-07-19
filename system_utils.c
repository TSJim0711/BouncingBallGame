#include <stdio.h>
#include <time.h> // for clock_gettime

struct timespec cur_time, prev_time;
long long delta_time_temp_ns;
double delta_time_temp_sec, delta_time=0;
double game_time=0;
struct timespec private_prev_time[5];//


void update_frame_timer()//Get time diff between frame.
{
    clock_gettime(CLOCK_MONOTONIC, &cur_time);
    long long delta_time_temp = (long long)(cur_time.tv_sec - prev_time.tv_sec) * 1000000000LL +(cur_time.tv_nsec - prev_time.tv_nsec);//nanosec
    double delta_time_temp_sec = (double)delta_time_temp / 1000000000.0; //second
    if(delta_time_temp_sec>0 && delta_time_temp_sec<5)
        delta_time=delta_time_temp_sec;//fuck thoese system clock jump backwards, but now we have monotonic clock. I kept this here for any unexpected.
    prev_time=cur_time;
    game_time+=delta_time;
}

double get_frame_delta_time()
{
    return delta_time;
}
double get_frame_game_time()
{
    return game_time;
}