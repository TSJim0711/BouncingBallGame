#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include <float.h>

double degree_2_radius (double degree)
{
    double radius=degree*(3.1415926/180);
    return radius;
}

int main()
{
    system("clear");
    printf("Creating the world...\033[?25l");//Hide the cursor
    float gravity = 6.8, air_friction_strength=0.02;
    int worldSizeX=45, worldSizeY=20;
    int hitbox[worldSizeY][worldSizeX][3];
    for(int i=0;i<worldSizeY;i++)
        for(int j=0;j<45;j++)
            hitbox[i][j][0]=0;
    for(int i=0;i<worldSizeY;i++)
    {
        hitbox[i][0][0]=1;
        hitbox[i][0][1]=90;
        hitbox[i][0][2]=8;
        hitbox[i][worldSizeX-1][0]=1;
        hitbox[i][worldSizeX-1][1]=90;
        hitbox[i][worldSizeX-1][2]=8;
    }
    for(int i=0;i<worldSizeX;i++)
    {
        hitbox[0][i][0]=1;
        hitbox[0][i][1]=0;
        hitbox[0][i][2]=8;
        hitbox[worldSizeY-1][i][0]=1;
        hitbox[worldSizeY-1][i][1]=0;
        hitbox[worldSizeY-1][i][2]=8;
    }
    double curX=10, curY=10, prevX=10, prevY=10, curAccX, curAccY;
    double delta_time,sim_time=0;
    int fixedX=0,fixedY=0;
    int defaultAngl=45;
    float defaultAcc=2;
    struct timeval delta_time_gravity_prev,delta_time_gravity_cur;
    int dummy;
    double radius;

    double dda_distX,dda_distY,dda_total_dist,dda_next_pixel_distX,dda_next_pixel_distY,dda_deltaX,dda_deltaY,dda_traveled_dist;
    int dda_cursorX,dda_cursorY,dda_step_dirX,dda_step_dirY;

    
    FILE *logp=fopen("hit.log","w");
    fprintf(logp,"Start:\n");
    
    curAccX=cos(degree_2_radius(defaultAngl))*defaultAcc;
    curAccY=sin(degree_2_radius(defaultAngl))*defaultAcc;   
    
    //build(print) the world
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
        if(dda_step_dirY==-1 && hitbox[(int)floor(curY)][(int)floor(curX)][0]==1 && hitbox[(int)floor(curY)][(int)floor(curX)][1]==0)//if ball not on platform and not yet going up, then provide downward speed from gravity
            curAccY=0;
        else
            curAccY-=gravity*delta_time;


        //Air Friction to Acc
        curAccX-=curAccX*(air_friction_strength*delta_time);
        curAccY-=curAccY*(air_friction_strength*delta_time);
        
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

        //Ray-Grid Traversal Algorithm, detect if pass hitbox from prev to cur position
        dda_distX=curX-prevX;//distant betweeen prev and cur on a axis
        dda_distY=curY-prevY;
        dda_total_dist=sqrt(pow(dda_distX,2)+pow(dda_distY,2));//actual distance

        dda_cursorX=floor(prevX);//starting point of dda algorithm
        dda_cursorY=floor(prevY);

        dda_step_dirX=(dda_distX>0)?1:-1;//dir for each step
        dda_step_dirY=(dda_distY>0)?1:-1;

        dda_deltaX = (dda_distX==0)?DBL_MAX :fabs(1.0/dda_distX);//distant needed for cursor to next pixel, now is ratio
        dda_deltaY = (dda_distY==0)?DBL_MAX :fabs(1.0/dda_distY);

        if (dda_distX > 0)//distant for current virtual cursor to next pixel, now is ratio
            dda_next_pixel_distX = (floor(prevX) + 1.0 - prevX) * dda_deltaX;
        else
            dda_next_pixel_distX = (prevX - floor(prevX)) * dda_deltaX;
        if (dda_distY > 0)
            dda_next_pixel_distY = (floor(prevY) + 1.0 - prevY) * dda_deltaY;
        else
            dda_next_pixel_distY = (prevY - floor(prevY)) * dda_deltaY;
        
        if(dda_total_dist>0)//turn ratio to actual dist
        {
            dda_deltaX*=dda_total_dist;
            dda_deltaY*=dda_total_dist;
            dda_next_pixel_distX*=dda_total_dist;
            dda_next_pixel_distY*=dda_total_dist;
        }

        dda_traveled_dist=0;//the virtual cursor of this algorithm traveled distant
        fprintf(logp,"Frame:%-4d From[%5.2f %5.2f] To [%5.2f %5.2f] ACC{%5.2f,%5.2f}: ",i,prevX,prevY,curX,curY,curAccX,curAccY);
        while (dda_traveled_dist < dda_total_dist) 
        {
            if (dda_next_pixel_distX < dda_next_pixel_distY) {//to the nearest pixel, x or y axis
                dda_cursorX += dda_step_dirX;//move virtual cursor 1 pixel forward
                dda_traveled_dist = dda_next_pixel_distX;//add the dist to total dist
                dda_next_pixel_distX += dda_deltaX;//add traveled distant to next_pixel_dist, reduce weight re-enter the if
            } else {//same as above
                dda_cursorY += dda_step_dirY;
                dda_traveled_dist = dda_next_pixel_distY;
                dda_next_pixel_distY += dda_deltaY;
            }
            fprintf(logp," [%2d %2d]",dda_cursorX,dda_cursorY);
            if(hitbox[dda_cursorY][dda_cursorX][0]==1)
            {
                fprintf(logp," HIT! DIRX:%d, DIRY:%d, Angle:{%d %d %d}",dda_step_dirX,dda_step_dirY,hitbox[dda_cursorY][dda_cursorX][0],hitbox[dda_cursorY][dda_cursorX][1],hitbox[dda_cursorY][dda_cursorX][2]);
                curX=dda_cursorX;//the ball set to cursor
                curY=dda_cursorY;
                if(hitbox[dda_cursorY][dda_cursorX][1]>=0&&hitbox[dda_cursorY][dda_cursorX][1]<89)//ground to slope
                    if(dda_step_dirY==1)
                        curY=dda_cursorY-1;//ball upward, touch ceil add a little to prevent stuck into ceil
                    else
                        curY=dda_cursorY+1;//ball falling downward
                if(hitbox[dda_cursorY][dda_cursorX][1]>1&&hitbox[dda_cursorY][dda_cursorX][1]<=90)//slope to wall
                    if(dda_step_dirX==1)
                        curX=dda_cursorX-1;//ball going right 
                    else
                        curX=dda_cursorX+1;//ball going left

                radius=degree_2_radius(2*hitbox[dda_cursorY][dda_cursorX][1]);//re calculate the speed of ball
                curAccX=curAccX*cos(radius)+curAccY*sin(radius)*(0.95);//vx​=v0x​cos(2θ)+v0y​sin(2θ)*material absorb force
                curAccY=curAccX*sin(radius)-curAccY*cos(radius)*(0.95);//vy​=v0x​sin(2θ)−v0y​cos(2θ)
                fprintf(logp," [%d %d] {%.2f,%.2f}",(int)curX,(int)curY,curAccX,curAccY);
                break;//the ball settle here
            }
        }
        fprintf(logp," \n");
        fflush(logp);

        //print frame
        printf("\033[1;1H");
        printf("FRAME %d SIMed: %.2fms, [%.2f,%.2f] {%.2f,%.2f}    ",i,sim_time,curX,curY,curAccX,curAccY);
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