#ifndef WORLD_MANAGER_H
#define WORLD_MANAGER_H

struct pixel_info 
{
    int is_wall;
    int angle;
    int force_absorb_rate;
};

struct position
{
    int x;
    int y;
};

void initialize_world(int world_code);
struct pixel_info get_pixel_info(int world_code, int x,int y);
void hit_star(int x,  int y);
extern int score;

#endif