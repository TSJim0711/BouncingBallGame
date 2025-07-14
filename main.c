#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>

double degree_2_radius (double degree)
{
    double radius=degree*(3.1415926/180);
    return radius;
}

int main()
{
    float gravity = 6.8, air_friction_strength=0.02, material_acc_absorb_rate=0.25;
    int worldSizeX=45, worldSizeY=20;
    int world[worldSizeY][worldSizeX];//MAP
    double curX=1, curY=1, prevX, prevY, curAccX, curAccY;
    double delta_time,sim_time=0;
    int fixedX=0,fixedY=0;
    int defaultAngl=46;
    float defaultAcc=2;
    struct timeval delta_time_gravity_prev,delta_time_gravity_cur;
    int dummy;
    
    int exceed_map_X_flag=0,exceed_map_Y_flag=0;//if ball go to fast, it pass through the wall, active this flag, wall won't block the ball, deactivate it when the ball return to the map.

    curAccX=cos(degree_2_radius(defaultAngl))*defaultAcc;
    curAccY=sin(degree_2_radius(defaultAngl))*defaultAcc;   

    //build(print) the world
    system("clear");
    printf("Creating the world...\033[?25l");//Hide the cursor
    for (int printY=0;printY<worldSizeY; printY++)
    {
        for (int printX=0;printX<worldSizeX; printX++)
                printf(" ");//print air
        printf("\n");
    }
    dummy=gettimeofday(&delta_time_gravity_prev, NULL);
    for(int i=0;i<=2000;i++)
    {
        //Get time diff between frame.
        dummy=gettimeofday(&delta_time_gravity_cur, NULL);//Get current time and compare with previous time
        delta_time=((delta_time_gravity_cur.tv_sec*1000000+delta_time_gravity_cur.tv_usec)-(delta_time_gravity_prev.tv_sec*1000000+delta_time_gravity_prev.tv_usec))/1000000.0;
        delta_time_gravity_prev=delta_time_gravity_cur;
        sim_time+=delta_time;

        //Gravity to Acc
        curAccY-=gravity*delta_time;
        //Air Friction to Acc
        curAccX-=curAccX*air_friction_strength;
        curAccY-=curAccY*air_friction_strength;
        //The ball back to the map, remove flag
        if(curX>0 && curX<worldSizeX)
            exceed_map_X_flag=0;
        if(curY>0 && curY<worldSizeY)
            exceed_map_Y_flag=0;
        //Hit to Acceleration
        if((curX<=0 || curX>=worldSizeX) && !exceed_map_X_flag)
            {
                curAccX=-(curAccX*(1-material_acc_absorb_rate));
                exceed_map_X_flag=1;
            }
        if((curY<=0 || curY>=worldSizeY) && !exceed_map_Y_flag)
            {
                curAccY=-(curAccY*(1-material_acc_absorb_rate));
                exceed_map_Y_flag=1;
            }
        //Acceleration too low == stop. As Acc can never be 0 in this programme
        if(curAccX<0.2 && curAccX>-0.2)
            {
                curX=round(curX);
                fixedX=1;
                curAccX=0;
            }
        if(curAccY<0.5 && curAccY>0 && curY<=0)//stop when up acceleration too low
            {
                curY=0;
                fixedY=1;
                curAccY=0;
            }
        //Calculate new position
        if(!fixedX)
            curX=curX+curAccX;
        if(!fixedY)
            curY=curY+curAccY;
        //print frame
        printf("\033[1;1H");
        printf("FRAME %d SIMed: %.2fms, [%.2f,%.2f] {%.2f,%.2f}, Exceed Map:%d    ",i,sim_time,curX,curY,curAccX,curAccY,exceed_map_X_flag+exceed_map_Y_flag);
        //Remove ball from last frame
        printf("\033[%d;%dH ", worldSizeY - (int)round(prevY), (int)round(prevX) + 1);
        printf(" ");
        //Print the ball on screen
        printf("\033[%d;%dH", worldSizeY - (int)round(curY), (int)round(curX) + 1);
        printf("*");
        fflush(stdout);//ALL THE LETTERS, GET OUT THE BUFFER AND GO TO THE PLAYGROUND! NOW!!!
        
        //Backup ball location
        prevX=curX;
        prevY=curY;
        usleep(30000);//Delay       
    }
    printf("\033[?25h");//Show the cursor
    return 0;
}