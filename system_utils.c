#include <stdio.h>
#include <time.h> // for clock_gettime
#include <stdlib.h>
#include <termios.h>
#include <unistd.h> // For read()

struct timespec cur_time, prev_time;
long long delta_time_temp_ns;
double delta_time_temp_sec, delta_time=0;
double game_time=0;
struct timespec private_prev_time[5];//
static struct termios term_orig, term_no_print;//store terminal config

void update_frame_timer()//Get time diff between frame.
{
    clock_gettime(CLOCK_MONOTONIC, &cur_time);
    long long delta_time_temp = (long long)(cur_time.tv_sec - prev_time.tv_sec) * 1000000000LL +(cur_time.tv_nsec - prev_time.tv_nsec);//nanosec
    double delta_time_temp_sec = (double)delta_time_temp / 1000000000.0; //second
    if(delta_time_temp_sec>0 && delta_time_temp_sec<5)
        delta_time=delta_time_temp_sec;//fuck thoese system clock jump backwards, but now we have monotonic clock. I kept this here for any unexpected.
    prev_time=cur_time;
    game_time+=delta_time;
}

double get_frame_delta_time()
{
    return delta_time;
}
double get_frame_game_time()
{
    return game_time;
}

struct func_call_timetable
{
    void (*func)();// the function in invoke
    double awake_time;
    struct func_call_timetable *next;
};

struct func_call_timetable *invoke_list_head=NULL;

void invoke(void (*func)(), float delay_sec)//just like unity
{
    struct func_call_timetable *new_pending=(struct func_call_timetable*)malloc(sizeof(struct func_call_timetable));
    if(new_pending==NULL)
    {
        printf("Fail malloc in invoke.");
        return;
    }
    new_pending->func=func;//load data to struct
    new_pending->awake_time=get_frame_game_time()+delay_sec;

    // hang new invoke to the chain list
    if(invoke_list_head==NULL)
        invoke_list_head=new_pending;
    else//list not empty, just make itself be the head ^w^
    {
        new_pending->next=invoke_list_head;
        invoke_list_head=new_pending;
    }
}

void invoke_handler()
{
    struct func_call_timetable *cursor = invoke_list_head;
    struct func_call_timetable *cursor_assist=cursor;
    while(cursor!=NULL)//go through the entire list
    {
        if(get_frame_game_time()>cursor->awake_time)
        {
            cursor->func();//run the function
            if(invoke_list_head==cursor)//delete the node, is the head
            {
                invoke_list_head=cursor->next;
                free(cursor);
                cursor=invoke_list_head;
            }else//delete node, in middle or end of the list
            {
                cursor_assist->next=cursor->next;
                free(cursor);
                cursor=cursor_assist->next;
            }
        }
        else
        {
            cursor_assist=cursor;// move cursor to next node, cursor assit alway behind cursor
            cursor=cursor->next;
        }

    }
}

void initTermios() 
{
    tcgetattr(STDIN_FILENO, &term_orig);
    term_no_print = term_orig;
    term_no_print.c_lflag &= ~ICANON; //set to immediate input, no need press ENTER
    term_no_print.c_lflag &= ~ECHO;//set term_no_print config -> terminal no echo
}

void term_print(int on_flag)
{
    if (on_flag)
        tcsetattr(STDIN_FILENO, TCSANOW, &term_orig);//terminal echos
    else
        tcsetattr(STDIN_FILENO, TCSANOW, &term_no_print);//terminal no echo
}

int get_random(int from, int to)
{
    return rand() % (to - from) + from;
}