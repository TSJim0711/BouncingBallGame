#include <stdio.h>
#include <math.h>
#include <unistd.h>

double degree_2_radius (double degree)
{
    double radius=degree*(3.1415926/180);
    return radius;
}

int main()
{
    int gravity = 9.8;
    int worldSizeX=60, worldSizeY=20;
    int world[worldSizeY][worldSizeX];//MAP
    double curX=0, curY=0, prevX, prevY, curForceX, curForceY;
    int defaultAngl=60;
    float defaultForce=1;
    int time_delay=30000;
    int exceed_map_X_flag=0,exceed_map_Y_flag=0;//if ball go to fast, it pass through the wall, active this flag, wall won't block the ball, deactivate it when the ball return to the map.

    curForceX=cos(degree_2_radius(defaultAngl))*defaultForce;
    curForceY=sin(degree_2_radius(defaultAngl))*defaultForce;   

    //build(print) the world
    system("clear");
    printf("Creating the world...");
    for (int printY=0;printY<worldSizeY; printY++)
    {
        for (int printX=0;printX<worldSizeX; printX++)
                printf(" ");//print air
        printf("\n");
    }
    for(int i=0;i<=2000;i++)
    {
        //New position
        curX=curX+curForceX;
        curY=curY+curForceY;
        //Gravity to force
        curForceY-=gravity*(time_delay/10000000.0);
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

        //print frame
        printf("\033[1;1H");
        printf("FRAME %d, [%.2f,%.2f] {%.2f,%.2f}",i,curX,curY,curForceX,curForceY);
        //Remove ball from last frame
        printf("\033[%d;%dH", worldSizeY - (int)round(prevY), (int)round(prevY) + 1);
        printf(" ");
        //Print the ball on screen
        printf("\033[%d;%dH", worldSizeY - (int)round(curY), (int)round(curX) + 1);
        printf("*");
        
        //Backup ball location
        prevX=curX;
        prevY=curY;
        usleep(time_delay);//Delay       
    }

    return 0;
}