#include <stdio.h>
#include <math.h>//round
#include <unistd.h>//usleep
#include "scene/world_manager.h"
#include "system_utils.h"
#include "physic.h"

int main()
{
    initTermios();
    system("clear");
    printf("Creating the world...\033[?25l");//Hide the cursor

    initialize_world(0);
    
    struct ball_pos cur_pos={.position_x=1.0, .position_y=1.0};
    struct ball_pos prev_pos={.position_x=1.0, .position_y=1.0};

    char key_press='\0';
    term_print(0);
    for(int i=0;i<=10000;i++)
    {
        update_frame_timer();
        prev_pos=cur_pos;
        cur_pos=get_ball_pos();
        
        //print frame
        printf("\033[1;1H");
        printf("FRAME %d SIMed: %.2fms, [%.2f,%.2f]    ",i,get_frame_game_time(),cur_pos.position_x,cur_pos.position_y);
        //Remove ball from last frame
        printf("\033[%d;%dH ", 20 - (int)round(prev_pos.position_y), (int)round(prev_pos.position_x) + 1);
        printf(" ");
        //Print the ball on screen
        printf("\033[%d;%dH", 20 - (int)round(cur_pos.position_y), (int)round(cur_pos.position_x) + 1);
        printf("O");
        fflush(stdout);//ALL THE LETTERS, GET OUT THE BUFFER AND GO TO THE PLAYGROUND! NOW!!!
        if(get_ball_stable_status()==2)//only trigger when ball is stable AKA return <0
        {
            do      //Launch the ball when user press space bar
            {
                printf("\033[%d;%dH ", 21,0);
                printf("Angle <+S %03d D->    [SPACE TO BOUNCE]   <+K %05.2f L-> Force",launch_angle,launch_speed);
                fflush(stdout);
                key_press=0;
                read(STDIN_FILENO, &key_press, 1); //read 1 charactor from KB
                switch (key_press)
                {
                case 's':
                    launch_angle+=5;
                    break;
                case 'd':
                    launch_angle-=5;
                    break;
                case'k':
                    launch_speed+=0.5;
                    break;
                case'l':
                    launch_speed-=0.5;
                default:
                    break;
                }
            } while (key_press!=' ');
            launch_ball();
            update_frame_timer();//drop delta time when input
        }else
        {
            printf("\033[%d;%dH ", 21,0);
            printf("                      >Bouncing<                                                  ");
            fflush(stdout);
        }
        usleep(30000);//Delay       
    }
    printf("\033[?25h");//Show the cursor
    return 0;
}