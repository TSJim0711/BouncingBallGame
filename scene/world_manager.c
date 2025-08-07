#include <stdio.h>
#include "world_manager.h"
#include "world.h"
#include "../physic.h"

int current_world=0;
int score=0;

void initialize_world(int world_code)
{
    struct position starting_pos;
    switch (world_code)
    {
    case 0://Test World
        starting_pos=initialize_test_world();
        print_test_world();
        init_ball_status(starting_pos.x,starting_pos.y,0,0);
        
        current_world=0;
        break;
    
    default:
        printf("World Not Listed.");
    }
}

struct pixel_info get_pixel_info(int world_code, int x,int y)
{
    switch (world_code)
    {
    case 0://Test World
        return get_test_world_pixel_info(x,y);
        break;
    
    default:
        printf("World Not Listed.");
    }
}

struct position hit_star(int x,  int y)
{
    switch (current_world)
    {
    case 0://Test World
        score+=test_world_recycle_star(x,y);
        return (test_world_place_star());
        break;
    
    default:
        {}//If it even possible?
    }
}

void recover_background(int x,int y)//recover pixel after something cover the pixel
{
    printf("\033[%d;%dH", 22-y, x+1);
    switch (current_world)
    {
        case 0://Test World
            if(test_world_midground[y][x][0]!=0)//if midground have somthing, then print midground, else print background, else print space
                printf("%s",test_world_midground[y][x]);
            else if(test_world_background[y][x]!=0)
                printf("\x1b[90m%c\x1b[0m",test_world_background[y][x]);
            else
                printf(" ");
        break;
    
    default:
        break;
    }
}

void recover_background_batch(struct zone area_to_recover)
{
    for(int y=area_to_recover.up_left.y;y<area_to_recover.down_right.y;y++)
        for(int x=area_to_recover.up_left.x;x<area_to_recover.down_right.x;x++)
            recover_background(x,y);
}