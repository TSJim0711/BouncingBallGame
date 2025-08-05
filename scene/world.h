#ifndef WORLD_H
#define WORLD_H
#include <string.h>

extern int test_world_worldSizeY;
extern int test_world_worldSizeX;
extern char test_world_midground[20][46][20];
extern char test_world_background[20][45];

//testing_world
struct position initialize_test_world();
void print_test_world();
struct pixel_info get_test_world_pixel_info(int x,int y);
int test_world_recycle_star(int pos_x, int pos_y);
struct position test_world_place_star();

#endif