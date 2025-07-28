#ifndef WORLD_H
#define WORLD_H

extern int worldSizeY;
extern int worldSizeX;

//testing_world
struct position initialize_test_world();
void print_test_world();
struct pixel_info get_test_world_pixel_info(int x,int y);

#endif