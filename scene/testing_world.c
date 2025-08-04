//World Code 0//
#include <stdio.h>
#include "world.h"
#include "world_manager.h"
#include "../system_utils.h"

double curX=10, curY=10;
int worldSizeX=45, worldSizeY=20;
char midground[20][45]={0};
char background[20][45]={0};
struct pixel_info hitbox[20][45]={0};
struct position starting_pos={.x=10, .y=10};

struct position initialize_test_world()
{
    for(int i=0;i<worldSizeY;i++)
    {
        hitbox[i][0].is_wall=1;
        hitbox[i][0].angle=90;
        hitbox[i][0].force_absorb_rate=90;
        midground[i][0]='|';
        hitbox[i][worldSizeX-1].is_wall=1;//0 = air, 1= wall, 2 = star
        hitbox[i][worldSizeX-1].angle=90;
        hitbox[i][worldSizeX-1].force_absorb_rate=90;
        midground[i][worldSizeX-1]='|';
    }
    for(int i=0;i<worldSizeX;i++)
    {
        hitbox[0][i].is_wall=1;
        hitbox[0][i].angle=0;
        hitbox[0][i].force_absorb_rate=90;
        midground[worldSizeY-1][i]='-';
        hitbox[worldSizeY-1][i].is_wall=1;
        hitbox[worldSizeY-1][i].angle=0;
        hitbox[worldSizeY-1][i].force_absorb_rate=90;
        midground[0][i]='-';
        if(i>5 && i<40)
        {
            hitbox[6][i].is_wall=1;//platform on the air
            hitbox[6][i].angle=0;
            hitbox[6][i].force_absorb_rate=90;
            midground[worldSizeY-7][i]='_';
        }
    }

    strcpy(background[5], "  ooooooooooooo                        .   ");
    strcpy(background[6], "  8'   888   `8                      .o8   ");
    strcpy(background[7], "       888       .ooooo.   .oooo.o .o888oo ");
    strcpy(background[8], "       888      d88' `88b d88(  \"8   888   ");
    strcpy(background[9], "       888      888ooo888 `\"Y88b.    888   ");
    strcpy(background[10],"       888      888    .o o.  )88b   888 . ");
    strcpy(background[11],"      o888o     `Y8bod8P' 8\"\"888P'   \"888\" ");
    return starting_pos;
}
    
void print_test_world()//build(print) the world
{
    for(int y=0;y<worldSizeY;y++)
    {
        for(int x=0;x<worldSizeX;x++)
        {
            if(midground[y][x]!=0)//if midground have somthing, then print midground, else print background, else print space
                printf("%c",midground[y][x]);
            else if(background[y][x]!=0)
                printf("\x1b[90m%c\x1b[0m",background[y][x]);
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
    printf("\033[%d;%dH ", 20 - pos_y,pos_x + 1);//remove star
    printf(" ");
    return 10;//score
}

void test_world_place_star()
{
    struct position star_pos;
    while(1)
    {
        star_pos.x =get_random(2,worldSizeX-3);//random a position, not in wall
        star_pos.y =get_random(2,worldSizeY-3);
        if (hitbox[star_pos.y][star_pos.x].is_wall==0)
        {
            hitbox[star_pos.y][star_pos.x].is_wall=2;//set star
            hitbox[star_pos.y][star_pos.x].angle=0;
            hitbox[star_pos.y][star_pos.x].force_absorb_rate=0;
            printf("\033[%d;%dH ", 20 - star_pos.y,star_pos.x + 1);//print the star on the position
            printf("%s*%s","\x1b[33m","\x1b[0m");
            fflush(stdout);//flush the stdout, the star symbol will be printed
            return;
        }
    }
}