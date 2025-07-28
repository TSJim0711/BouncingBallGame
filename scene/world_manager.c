#include <stdio.h>
#include "world_manager.h"
#include "world.h"
#include "../physic.h"

void initialize_world(int world_code)
{
    struct position starting_pos;
    switch (world_code)
    {
    case 0://Test World
        starting_pos=initialize_test_world();
        print_test_world();
        init_ball_status(starting_pos.x,starting_pos.y,0,0);
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