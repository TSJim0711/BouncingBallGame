#ifndef WORLD_MANAGER_H
#define WORLD_MANAGER_H
#include "../system_utils.h"
struct pixel_info 
{
    int is_wall;
    int angle;
    int force_absorb_rate;
};

void initialize_world(int world_code);
struct pixel_info get_pixel_info(int world_code, int x,int y);
void recover_background(int x,int y);
void recover_background_batch(struct zone area_to_recover);
struct position hit_star(int x,  int y);
extern int score;

#endif