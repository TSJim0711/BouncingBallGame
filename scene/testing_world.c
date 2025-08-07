//World Code 0//
#include <stdio.h>
#include "world.h"
#include "world_manager.h"
#include "../system_utils.h"

double curX=10, curY=10;
int test_world_worldSizeX=45, test_world_worldSizeY=20;
char test_world_midground[20][46][20]={0};
char test_world_background[20][45]={0};
struct pixel_info hitbox[20][45]={0};
struct position starting_pos={.x=14, .y=6};

struct position initialize_test_world()
{
    for(int i=0;i<test_world_worldSizeY;i++)
    {
        hitbox[i][0].is_wall=1;
        hitbox[i][0].angle=90;
        hitbox[i][0].force_absorb_rate=90;
        strcpy(test_world_midground[i][0],"│");//right wall
        hitbox[i][test_world_worldSizeX-1].is_wall=1;//0 = air, 1= wall, 2 = star
        hitbox[i][test_world_worldSizeX-1].angle=90;
        hitbox[i][test_world_worldSizeX-1].force_absorb_rate=90;
        strcpy(test_world_midground[i][test_world_worldSizeX-1],"│");//left wall
    }
    for(int i=0;i<test_world_worldSizeX;i++)
    {
        hitbox[0][i].is_wall=1;
        hitbox[0][i].angle=0;
        hitbox[0][i].force_absorb_rate=90;
        strcpy(test_world_midground[0][i],"─");//top roof
        hitbox[test_world_worldSizeY-1][i].is_wall=1;
        hitbox[test_world_worldSizeY-1][i].angle=0;
        hitbox[test_world_worldSizeY-1][i].force_absorb_rate=90;
        strcpy(test_world_midground[test_world_worldSizeY-1][i],"─");//ground
        if(i>15 && i<test_world_worldSizeX)
        {
            hitbox[4][i].is_wall=1;//platform on the air
            hitbox[4][i].angle=0;
            hitbox[4][i].force_absorb_rate=90;
            strcpy(test_world_midground[4][i],"═");
        }
    }
    strcpy(test_world_midground[test_world_worldSizeY-1][0],"┌");
    strcpy(test_world_midground[test_world_worldSizeY-1][test_world_worldSizeX-1],"┐");
    strcpy(test_world_midground[0][0],"└");
    strcpy(test_world_midground[0][test_world_worldSizeX-1],"┘");
    strcpy(test_world_midground[4][test_world_worldSizeX-1],"╡");

    //ascii art in background
    strcpy(test_world_background[16], "  ooooooooooooo                        .   ");
    strcpy(test_world_background[15], "  8'   888   `8                      .o8   ");
    strcpy(test_world_background[14], "       888       .oorooo.   .oooo.o .o888oo ");
    strcpy(test_world_background[13], "       888      d88' `88b d88(  \"8   888   ");
    strcpy(test_world_background[12], "       888      888ooo888 `\"Y88b.    888   ");
    strcpy(test_world_background[11],"       888      888    .o o.  )88b   888 . ");
    strcpy(test_world_background[10],"      o888o     `Y8bod8P' 8\"\"888P'   \"888\" ");
    return starting_pos;
}
    
void print_test_world()//build(print) the world
{
    printf("\n\n");
    for(int y=test_world_worldSizeY-1;y>=0;y--)
    {
        for(int x=0;x<test_world_worldSizeX;x++)
        {
            if(test_world_midground[y][x][0]!=0)//if midground have somthing, then print midground, else print test_world_background, else print space
                printf("%s",test_world_midground[y][x]);
            else if(test_world_background[y][x]!=0)
                printf("\x1b[90m%c\x1b[0m",test_world_background[y][x]);
            else
                printf(" ");
        }
        printf("\n");
    }    test_world_place_star();
}

struct pixel_info get_test_world_pixel_info(int x,int y)
{
    return hitbox[y][x];
}

int test_world_recycle_star(int pos_x, int pos_y)//when star is hit
{
    hitbox[pos_y][pos_x].is_wall=0;
    hitbox[pos_y][pos_x].angle=0;
    hitbox[pos_y][pos_y].force_absorb_rate=0;
    test_world_midground[pos_y][pos_x][0]=0;
    recover_background(pos_x,pos_y);
    return 10;//score
}

struct position test_world_place_star()
{
    struct position star_pos;
    while(1)
    {
        star_pos.x =get_random(2,test_world_worldSizeX-2);//random a position, not in wall
        star_pos.y =get_random(0,test_world_worldSizeY-10);
        if (hitbox[star_pos.y][star_pos.x].is_wall==0)
        {
            hitbox[star_pos.y][star_pos.x].is_wall=2;//set star
            hitbox[star_pos.y][star_pos.x].angle=0;
            hitbox[star_pos.y][star_pos.x].force_absorb_rate=0;
            strcpy(test_world_midground[star_pos.y][star_pos.x],"\x1b[33m*\x1b[0m");
            recover_background(star_pos.x,star_pos.y);
            return star_pos;
        }
    }
}