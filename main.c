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
    float gravity = 4.8, air_friction_strength=0.02;
    int worldSizeX=60, worldSizeY=20;
    int world[worldSizeY][worldSizeX];//MAP
    double curX=1, curY=1, prevX, prevY, curForceX, curForceY;
    double delta_time,sim_time=0;
    int defaultAngl=60;
    float defaultForce=3;
    struct timeval delta_time_gravity_prev,delta_time_gravity_cur;
    int dummy;
    
    int exceed_map_X_flag=0,exceed_map_Y_flag=0;//if ball go to fast, it pass through the wall, active this flag, wall won't block the ball, deactivate it when the ball return to the map.

    curForceX=cos(degree_2_radius(defaultAngl))*defaultForce;
    curForceY=sin(degree_2_radius(defaultAngl))*defaultForce;   

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

        //Gravity to force
        curForceY-=gravity*delta_time;
        //Air Friction to force
        curForceX-=curForceX*air_friction_strength;
        curForceY-=curForceY*air_friction_strength;
        //Hit to force
        if((curX<=0 || curX>=worldSizeX) && !exceed_map_X_flag)
            {
                curForceX=-curForceX;
                exceed_map_X_flag=1;
            }
        if((curY<=0 || curY>=worldSizeY) && !exceed_map_Y_flag)
            {
                curForceY=-curForceY;
                exceed_map_Y_flag=1;
            }
        //The ball back to the map, remove flag
        if(curX>0 || curX<worldSizeX)
            exceed_map_X_flag=0;
        if(curY>0 || curY<worldSizeY)
            exceed_map_Y_flag=0;
        //Force too low == stop. As force can never be 0 in this programme
        if(curForceX<0.2 && curForceX>-0.2)
            curForceX=0;
        if(curForceY<1 && curForceY>0)
            curForceY=0;
        //calculate new position
        curX=curX+curForceX;
        curY=curY+curForceY;
        //print frame
        printf("\033[1;1H");
        printf("FRAME %d SIMed: %.2fms, [%.2f,%.2f] {%.2f,%.2f}     ",i,sim_time,curX,curY,curForceX,curForceY);
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