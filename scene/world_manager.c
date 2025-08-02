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

void hit_star(int x,  int y)
{
    switch (current_world)
    {
    case 0://Test World
        score+=test_world_recycle_star(x,y);
        test_world_place_star();
        break;
    
    default:
        {}//If it even possible?
    }
}