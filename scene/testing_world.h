#ifndef WORLD_H
#define WORLD_H

extern int worldSizeY;
extern int worldSizeX;

struct pixel_info 
{
    int is_wall;
    int angle;
    int force_absorb_rate;
};

extern struct pixel_info hitbox[20][45];//const here but not in world.c, edit it with function!

// Function to draw the map
void initialize_world();
void print_world();

#endif