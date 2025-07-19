#include <stdio.h>
#include "testing_world.h"

int worldSizeY=20;
int worldSizeX=45;
struct pixel_info hitbox[20][45]={0};

void initialize_world()
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
}
    
void print_world()//build(print) the world
{
    for (int printY=0;printY<worldSizeY; printY++)
    {
        for (int printX=0;printX<worldSizeX; printX++)
                printf(" ");//print air
        printf("\n");
    }
}

struct pixel_info get_pixel_info(int x,int y)
{
    return hitbox[y][x];
}