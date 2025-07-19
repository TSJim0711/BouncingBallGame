#include <stdio.h>
#include <float.h>
#include <stdlib.h>
#include <math.h>
#include "system_utils.h"
#include "scene/testing_world.h"

FILE *logp;
struct ball_pos
{
    double position_x;
    double position_y;
};
float gravity = 6.8, air_friction_strength=0.02;
double curSpeedX=0, curSpeedY=0, radius;
struct ball_pos cur_pos={.position_x=1.0, .position_y=1.0};
struct ball_pos prev_pos={.position_x=1.0, .position_y=1.0};


int fixed_x=0,fixed_y=0;

double dda_distX,dda_distY,dda_total_dist,dda_next_pixel_distX,dda_next_pixel_distY,dda_deltaX,dda_deltaY,dda_traveled_dist;
int dda_cursorX,dda_cursorY,dda_step_dirX,dda_step_dirY;

double degree_2_radius (double degree)
{
    double radius=degree*(3.1415926/180);
    return radius;
}

void init_ball_status(int start_position_x, int start_position_y, int starting_angle, int starting_speed)
{
    logp=fopen("hit.log","w");
    fprintf(logp,"Start:\n");
    cur_pos.position_x=start_position_x;
    cur_pos.position_y=start_position_y;
    prev_pos=cur_pos;
    prev_pos=cur_pos;
    curSpeedX=cos(degree_2_radius(starting_angle))*starting_speed;
    curSpeedY=sin(degree_2_radius(starting_angle))*starting_speed;   
}

struct ball_pos get_ball_pos()
{
    //Gravity to Speed
    if(dda_step_dirY==-1 && hitbox[(int)floor(cur_pos.position_y)][(int)floor(cur_pos.position_x)].is_wall==1 && hitbox[(int)floor(cur_pos.position_y)][(int)floor(cur_pos.position_x)].angle==0)//if ball not on platform and not yet going up, then provide downward speed from gravity
        curSpeedY=0;
    else
        curSpeedY-=gravity*get_frame_delta_time();

    //Air Friction to Speed
    curSpeedX-=curSpeedX*(air_friction_strength*get_frame_delta_time());
    curSpeedY-=curSpeedY*(air_friction_strength*get_frame_delta_time());

    //Speed too low == stop. As Speed can never be 0 in this programme
    if(curSpeedX<0.2 && curSpeedX>-0.2)
        {
            cur_pos.position_x=round(cur_pos.position_x);
            fixed_x=1;
            curSpeedX=0;
        }
    if(curSpeedY<0.2 && curSpeedY>0 && cur_pos.position_y<=0)//stop when up Speed too low
        {
            cur_pos.position_y=0;
            fixed_y=1;
            curSpeedY=0;
        }

    //Calculate new position
    if(!fixed_x)
        cur_pos.position_x=cur_pos.position_x+curSpeedX;
    if(!fixed_y)
        cur_pos.position_y=cur_pos.position_y+curSpeedY;

    //Ray-Grid Traversal Algorithm, detect if pass hitbox from prev to cur position
    dda_distX=cur_pos.position_x-prev_pos.position_x;//distant betweeen prev and cur on a axis
    dda_distY=cur_pos.position_y-prev_pos.position_y;
    dda_total_dist=sqrt(pow(dda_distX,2)+pow(dda_distY,2));//actual distance

    dda_cursorX=floor(prev_pos.position_x);//starting point of dda algorithm
    dda_cursorY=floor(prev_pos.position_y);

    dda_step_dirX=(dda_distX>0)?1:-1;//dir for each step
    dda_step_dirY=(dda_distY>0)?1:-1;

    dda_deltaX = (dda_distX==0)?DBL_MAX :fabs(1.0/dda_distX);//distant needed for cursor to next pixel, now is ratio
    dda_deltaY = (dda_distY==0)?DBL_MAX :fabs(1.0/dda_distY);

    if (dda_distX > 0)//distant for current virtual cursor to next pixel, now is ratio
        dda_next_pixel_distX = (floor(prev_pos.position_x) + 1.0 - prev_pos.position_x) * dda_deltaX;
    else
        dda_next_pixel_distX = (prev_pos.position_x - floor(prev_pos.position_x)) * dda_deltaX;
    if (dda_distY > 0)
        dda_next_pixel_distY = (floor(prev_pos.position_y) + 1.0 - prev_pos.position_y) * dda_deltaY;
    else
        dda_next_pixel_distY = (prev_pos.position_y - floor(prev_pos.position_y)) * dda_deltaY;
    
    if(dda_total_dist>0)//turn ratio to actual dist
    {
        dda_deltaX*=dda_total_dist;
        dda_deltaY*=dda_total_dist;
        dda_next_pixel_distX*=dda_total_dist;
        dda_next_pixel_distY*=dda_total_dist;
    }
    
    dda_traveled_dist=0;//the virtual cursor of this algorithm traveled distant
    fprintf(logp,"From[%5.2f %5.2f] To [%5.2f %5.2f] Speed{%5.2f,%5.2f}: ",prev_pos.position_x,prev_pos.position_y,cur_pos.position_x,cur_pos.position_x,curSpeedX,curSpeedY);
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
        if(hitbox[dda_cursorY][dda_cursorX].is_wall==1)
        {
            fprintf(logp," HIT! DIRX:%d, DIRY:%d, Angle:{%d %d %d}",dda_step_dirX,dda_step_dirY,hitbox[dda_cursorY][dda_cursorX].is_wall,hitbox[dda_cursorY][dda_cursorX].angle,hitbox[dda_cursorY][dda_cursorX].force_absorb_rate);
            cur_pos.position_x=dda_cursorX;//the ball set to cursor
            cur_pos.position_y=dda_cursorY;
            if(hitbox[dda_cursorY][dda_cursorX].angle>=0&&hitbox[dda_cursorY][dda_cursorX].angle<89)//ground to slope
                if(dda_step_dirY==1)
                    cur_pos.position_y=dda_cursorY-1;//ball upward, touch ceil add a little to prevent stuck into ceil
                else
                    cur_pos.position_y=dda_cursorY+1;//ball falling downward
            if(hitbox[dda_cursorY][dda_cursorX].angle>1&&hitbox[dda_cursorY][dda_cursorX].angle<=90)//slope to wall
                if(dda_step_dirX==1)
                    cur_pos.position_x=dda_cursorX-1;//ball going right 
                else
                    cur_pos.position_x=dda_cursorX+1;//ball going left

            radius=degree_2_radius(2*hitbox[dda_cursorY][dda_cursorX].angle);//re calculate the speed of ball
            curSpeedX=curSpeedX*cos(radius)+curSpeedY*sin(radius)*(0.95);//vx​=v0x​cos(2θ)+v0y​sin(2θ)*material absorb force
            curSpeedY=curSpeedX*sin(radius)-curSpeedY*cos(radius)*(0.95);//vy​=v0x​sin(2θ)−v0y​cos(2θ)
            fprintf(logp," [%d %d] {%.2f,%.2f}",(int)cur_pos.position_x,(int)cur_pos.position_y,curSpeedX,curSpeedY);
            break;//the ball settle here
        }
    }
    fprintf(logp," \n");
    fflush(logp);

    prev_pos=cur_pos;
    return cur_pos;
}