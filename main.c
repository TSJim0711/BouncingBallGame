#include <stdio.h>
#include <math.h>//round
#include <unistd.h>//usleep
#include <fcntl.h>//read no break
#include "scene/world_manager.h"
#include "system_utils.h"
#include "physic.h"

int bg_clr=0;//just for trail invoke()
void rst_bg_clr()
{
    bg_clr=0;
}

void dismiss_dialog()
{
    recover_background_batch((struct zone){(struct position){20,5},(struct position){45,16}});//the ascii art
    recover_background_batch((struct zone){(struct position){1,1},(struct position){45,5}});//the dialog box
}

int main()
{
    initTermios();
    system("clear");
    int read_flag = fcntl(STDIN_FILENO, F_GETFL, 0);//non-blocking read
    fcntl(STDIN_FILENO, F_SETFL, read_flag | O_NONBLOCK);
    if(read_flag==-1)
        perror("fcntl F_GETFL");
    //printf("Creating the world...\033[?25l");//Hide the cursor

    initialize_world(0);
    struct dbl_position cur_pos={.x=1.0, .y=1.0};
    struct dbl_position prev_pos={.x=1.0, .y=1.0};

    char key_press=0;
    int prev_score=0;
    int dummy;
    term_print(0);
    for(int i=0;i<=10000;i++)
    {
        update_frame_timer();
        prev_pos=cur_pos;
        cur_pos=get_ball_pos(0);
    
        if(score!=prev_score)//just for invoke trail ()
        {
            prev_score=score;
            bg_clr=43;
            invoke(rst_bg_clr,0.2);// delay 0.2 and run that function
            if (score==10)
            {
                const char *ascii_art =//23*18
                "          ⠀⠀⣀⠤⠐⠂⠀⠐⠪⡝⣄⠀   \n"
                "⠀⠀⠀⠀⠀⠀⠀⠀⢀⡿⡫⠂⠀⠀⠀⠀⠀⠀⠀⠱⠑⢄⠀⠀⠀\n"
                "⠀⠀⠀⠀⠀⠀⠀⢠⠂⢠⠀⠀⠀⠀⠀⠀⠐⠀⠀⣰⣲⣮⣦⡀⠀\n"
                "⠀⠖⡀⡀⠀⠀⠀⡆⡄⡸⠀⠀⣆⢠⠀⠀⠀⡇⠁⢻⣺⣿⣿⠃⠀\n"
                "⢀⠸⠀⠀⢆⠀⠀⡗⣶⣷⡥⢸⢤⣾⣔⢅⢀⡇⠀⠘⢛⠚⠻⡀⠀\n"
                "⠀⠈⠐⠢⡀⢣⣼⠀⠇⡹⠀⠀⠈⢠⢹⣿⢢⠁⡠⡑⢸⠘⠀⡇⠀\n"
                "⠀⠀⠀⠀⠘⣾⢳⢾⠈⢀⣀⡀⠀⠘⠺⠃⡬⠊⠔⠀⠸⠀⠃⢸⠀\n"
                "⠀⠀⠀⠀⠀⢿⠀⢊⠂⠘⣀⢀⠁⠀⠀⡴⠊⠀⠀⠀⡂⠆⠀⠀⡆\n"
                "⠀⠀⠀⠀⠀⢸⡀⠈⠑⢼⣶⣤⠤⠤⠐⠀⠀⠀⢀⠨⠁⠀⢰⢸⢰\n"
                "⠀⠀⠀⠀⠀⠀⠣⡀⢻⣿⡟⡀⠀⠀⠃⢀⠀⢂⠕⠁⠁⢀⠮⡈⢠\n"
                "⠀⠀⠀⠀⠀⠀⠀⠀⣺⠿⣿⣤⡒⣀⢠⣧⣶⣿⣷⡀⡴⣿⢼⠧⠃\n"
                "⠀⠀⠀⠀⠀⠀⠀⠀⣿⡖⣿⡙⢿⣿⣿⣿⣿⣿⣿⣿⠈⠀⠀⠀⠀\n"
                "⠀⠀⠀⠀⠀⠀⠀⢀⣿⢳⣿⢻⣦⣿⣿⣿⣿⣿⣿⣿⡆⠀⠀⠀⠀\n"
                "⠀⠀⠀⠀⠀⠀⠀⢸⣿⢸⣿⢸⣿⣿⣿⣿⣿⣿⣿⣿⣇⠀⠀⠀⠀\n";
                int y=8;
                printf("\033[%d;%dH", y,20);
                for(int cursor=0;ascii_art[cursor]!='\0';cursor++)
                {
                    if (ascii_art[cursor]=='\n')
                    {
                        printf("\033[%d;%dH",++y,20);
                        fflush(stdout);
                    }
                    else
                        printf("%c",ascii_art[cursor]);
                }
                
                for (int y=18; y<22;y++)
                {
                    printf("\033[%d;%dH", y,2);
                    for(int x=2; x<45;x++)
                    {
                        if (y==18 && x==2)//corners
                            printf("╭");
                        else if (y==18 && x==44)
                            printf("╮");
                        else if (y==21 && x==2)
                            printf("╰");
                        else if (y==21 && x==44)
                            printf("╯");
                        else if ((y==18||y==21)&& (x>1&&x<45))//edges
                            printf("─");
                        else if(x==2||x==44)
                            printf("│");
                        else
                            printf(" ");//content space
                    }
                }
                printf("\033[%d;%dHName Here", 18,5);//title or name
                printf("\033[%d;%dHWOW, you get 10 scores!",19,5);//content
                printf("\033[%d;%dHSomething Somthing", 20,5);//content
            }
            invoke(dismiss_dialog,5);
        }
        //print frame
        printf("\033[1;1H");
        printf("FRAME %d SIMed: %.2fms, \x1b[0%dmScore:%3d\x1b[0m [%.2f,%.2f]    ",i,get_frame_game_time(),bg_clr,score,cur_pos.x,cur_pos.y);
        //Remove ball from last frame
        recover_background((int)round(prev_pos.x),(int)round(prev_pos.y));
        //Print the ball on screen
        printf("\033[%d;%dH", 20 - (int)round(cur_pos.y)+2, (int)round(cur_pos.x) + 1);
        printf("O");
        fflush(stdout);//ALL THE LETTERS, GET OUT THE BUFFER AND GO TO THE PLAYGROUND! NOW!!!
        if(get_ball_stable_status(0)==2)//only trigger when ball is stable AKA return <0
        {
            //Launch the ball when user press space bar
            printf("\033[%d;%dH ", 23,0);
            printf("Dir <+S %03d D-> ↿SPACE↾ <+W %05.2f S-> Eng",launch_angle,launch_speed);
            fflush(stdout);                
            key_press=0;
            if(read(STDIN_FILENO, &key_press, 1)>0)//read 1 charactor from KB
            {
                read_flag=0;//reset flag
                while ((dummy = getchar()) != '\n' && dummy != EOF) {};//clear the buffer
                action(key_press,1,10.0);
            }
        }else
        {
            printf("\033[%d;%dH ", 23,0);
            printf("              >Bouncing<                                              ");
            fflush(stdout);
        }
        usleep(30000);//Delay
        invoke_handler();       
    }
    printf("\033[?25h");//Show the cursor
    return 0;
}
