#include <stdio.h>
#include <math.h>//round
#include <unistd.h>//usleep
#include "scene/testing_world.h"
#include "system_utils.h"
#include "physic.h"

int main()
{
    system("clear");
    printf("Creating the world...\033[?25l");//Hide the cursor

    double curX=10, curY=10;
    int defaultAngl=45;
    float defaultSpeed=2;
    struct ball_pos cur_pos={.position_x=1.0, .position_y=1.0};
    struct ball_pos prev_pos={.position_x=1.0, .position_y=1.0};

    initialize_world();
    init_ball_status(curX,curY,defaultAngl,defaultSpeed);
    print_world();
    
    //build(print) the world
    for (int printY=0;printY<worldSizeY; printY++)
    {
        for (int printX=0;printX<worldSizeX; printX++)
                printf(" ");//print air
        printf("\n");
    }

    for(int i=0;i<=2000;i++)
    {
        update_frame_timer();
        prev_pos=cur_pos;
        cur_pos=get_ball_pos();
        
        //print frame
        printf("\033[1;1H");
        printf("FRAME %d SIMed: %.2fms, [%.2f,%.2f]    ",i,get_frame_game_time(),cur_pos.position_x,cur_pos.position_y);
        //Remove ball from last frame
        printf("\033[%d;%dH ", worldSizeY - (int)round(prev_pos.position_y), (int)round(prev_pos.position_x) + 1);
        printf(" ");
        //Print the ball on screen
        printf("\033[%d;%dH", worldSizeY - (int)round(cur_pos.position_y), (int)round(cur_pos.position_x) + 1);
        printf("*");
        fflush(stdout);//ALL THE LETTERS, GET OUT THE BUFFER AND GO TO THE PLAYGROUND! NOW!!!
        
        usleep(30000);//Delay       
    }
    printf("\033[?25h");//Show the cursor
    return 0;
}