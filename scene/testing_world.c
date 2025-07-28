//World Code 0//
#include <stdio.h>
#include "world.h"
#include "world_manager.h"

double curX=10, curY=10;
int worldSizeX=45, worldSizeY=20;
struct pixel_info hitbox[20][45]={0};
struct position starting_pos={.x=10, .y=10};

struct position initialize_test_world()
{
    for(int i=0;i<worldSizeY;i++)
    {
        hitbox[i][0].is_wall=1;
        hitbox[i][0].angle=90;
        hitbox[i][0].force_absorb_rate=90;
        hitbox[i][worldSizeX-1].is_wall=1;
        hitbox[i][worldSizeX-1].angle=90;
        hitbox[i][worldSizeX-1].force_absorb_rate=90;
    }
    for(int i=0;i<worldSizeX;i++)
    {
        hitbox[0][i].is_wall=1;
        hitbox[0][i].angle=0;
        hitbox[0][i].force_absorb_rate=90;
        hitbox[worldSizeY-1][i].is_wall=1;
        hitbox[worldSizeY-1][i].angle=0;
        hitbox[worldSizeY-1][i].force_absorb_rate=90;
    }
    return starting_pos;
}
    
void print_test_world()//build(print) the world
{
    for (int printY=0;printY<worldSizeY; printY++)
    {
        for (int printX=0;printX<worldSizeX; printX++)
                printf(" ");//print air
        printf("\n");
    }
}

struct pixel_info get_test_world_pixel_info(int x,int y)
{
    return hitbox[y][x];
}