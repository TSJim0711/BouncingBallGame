#include <stdio.h>
#include <float.h>
#include <stdlib.h>
#include <math.h>
#include "system_utils.h"
#include "scene/world_manager.h"

FILE *logp;
struct ball_pos
{
    double position_x;
    double position_y;
};
float gravity = 6.8, air_friction_strength=0.2;
double curSpeedX=0, curSpeedY=0, radius;
struct ball_pos cur_pos={.position_x=1.0, .position_y=1.0};
struct ball_pos prev_pos={.position_x=1.0, .position_y=1.0};
struct pixel_info current_pixel_info_Up, current_pixel_info_Dwn, current_pixel_info_Rgh, current_pixel_info_Lft;


int fixed_x=0,fixed_y=0;
int hit_flag=0;

double dda_route_dist_x,dda_route_dist_y, dda_total_dist, dda_traveled_dist;
double dda_step_x,dda_step_y;
struct ball_pos dda_cursor, dda_prev_cursor;


int hit_angle=-1;
int launch_angle=45;
double launch_speed=2;

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
}

struct ball_pos get_ball_pos()
{
    //Gravity to Speed
    if(dda_route_dist_y>0 || current_pixel_info_Dwn.is_wall!=1 || current_pixel_info_Dwn.angle!=0)//if ball not on platform and not yet going up, then provide downward speed from gravity
        curSpeedY-=gravity*get_frame_delta_time();

    //Air Friction to Speed
    curSpeedX-=curSpeedX*(air_friction_strength*get_frame_delta_time());
    curSpeedY-=curSpeedY*(air_friction_strength*get_frame_delta_time());

    //Calculate new position
    if(!fixed_x)
        cur_pos.position_x=cur_pos.position_x+curSpeedX;
    if(!fixed_y)
        cur_pos.position_y=cur_pos.position_y+curSpeedY;

    //Line Traversal Alg, Simple Step Based
    dda_route_dist_x=cur_pos.position_x-prev_pos.position_x;//distant betweeen prev and cur on a axis
    dda_route_dist_y=cur_pos.position_y-prev_pos.position_y;
    dda_total_dist=sqrt(pow(dda_route_dist_x,2)+pow(dda_route_dist_y,2));
    dda_step_x=0.5*(dda_route_dist_x/dda_total_dist);//0.5 can be config, in the future, I guess '~'
    dda_step_y=0.5*(dda_route_dist_y/dda_total_dist);
    dda_traveled_dist=0;

    fprintf(logp,"From[%5.2f %5.2f] To [%5.2f %5.2f] Speed{%5.2f,%5.2f}: ",prev_pos.position_x,prev_pos.position_y,cur_pos.position_x,cur_pos.position_y,curSpeedX,curSpeedY);
    dda_cursor.position_x=prev_pos.position_x;//set dda_cursor pos to current pos
    dda_cursor.position_y=prev_pos.position_y;
    while (dda_traveled_dist < dda_total_dist) 
    {
        //cal next location with step
        dda_cursor.position_x+=dda_step_x;
        dda_cursor.position_y+=dda_step_y;
        dda_traveled_dist+=0.5;
        if(dda_cursor.position_x==dda_prev_cursor.position_x && dda_cursor.position_y==dda_prev_cursor.position_y)//repeat pixel, skip
            continue;
        dda_prev_cursor.position_x=dda_cursor.position_x;
        dda_prev_cursor.position_y=dda_cursor.position_y;
        
        fprintf(logp," [%2.0f %2.0f]",dda_cursor.position_x,dda_cursor.position_y);
        current_pixel_info_Up=get_pixel_info(0,(int)floor(dda_cursor.position_x),(int)floor(dda_cursor.position_y)+1);
        current_pixel_info_Dwn=get_pixel_info(0,(int)floor(dda_cursor.position_x),(int)floor(dda_cursor.position_y)-1);
        current_pixel_info_Rgh=get_pixel_info(0,(int)floor(dda_cursor.position_x)+1,(int)floor(dda_cursor.position_y));
        current_pixel_info_Lft=get_pixel_info(0,(int)floor(dda_cursor.position_x)-1,(int)floor(dda_cursor.position_y));
        
        if(dda_route_dist_x>0  && current_pixel_info_Up.is_wall==1  && !fixed_y)//check if is a hit
            hit_flag=hit_flag|0b1000; 
        if(dda_route_dist_x<=0 && current_pixel_info_Dwn.is_wall==1)
            if(!fixed_y)
                hit_flag=hit_flag|0b0100;
            else
                if(curSpeedX>1)//if fixed Y and on grond, slow. Bouncing is wonderful but sliding is bored.
                    curSpeedX*=0.625;
                else
                    curSpeedX*=0.9375;
        if(dda_route_dist_y>0  && current_pixel_info_Rgh.is_wall==1 && !fixed_x)
            hit_flag=hit_flag|0b0010;
        if(dda_route_dist_y<=0 && current_pixel_info_Lft.is_wall==1 && !fixed_x)
            hit_flag=hit_flag|0b0001;
        
        if(hit_flag>0)//Due with hit
        {
            fprintf(logp," HIT!");
            cur_pos.position_x=dda_cursor.position_x;//the ball set to cursor
            cur_pos.position_y=dda_cursor.position_y;
            if(hit_flag&0b1000)
                hit_angle=current_pixel_info_Up.angle;//ball upward,
            else if(hit_flag&0b0100)
                hit_angle=current_pixel_info_Dwn.angle;//ball falling downward
            if(hit_flag&0b0010)
                if (hit_angle==-1)//no up or down hit this frame
                    hit_angle=current_pixel_info_Rgh.angle;
                else
                    hit_angle=(hit_angle+current_pixel_info_Rgh.angle)/2;
            else if(hit_flag&0b0001)
                if (hit_angle==-1)//hit left
                    hit_angle=current_pixel_info_Lft.angle;
                else
                    hit_angle=(hit_angle+current_pixel_info_Lft.angle)/2;

            radius=degree_2_radius(2*hit_angle);//re calculate the speed of ball
            curSpeedX=curSpeedX*cos(radius)+curSpeedY*sin(radius)*(0.8);//vx​=v0x​cos(2θ)+v0y​sin(2θ)*material absorb force
            curSpeedY=curSpeedX*sin(radius)-curSpeedY*cos(radius)*(0.8);//vy​=v0x​sin(2θ)−v0y​cos(2θ)
            fprintf(logp," [%d %d] {%.2f,%.2f}",(int)cur_pos.position_x,(int)cur_pos.position_y,curSpeedX,curSpeedY);
            hit_angle=-1;//set used back to defualt
            hit_flag=0;
            break;//the ball settle here
        }
    }
            //Speed too low == stop. As Speed can never be 0 in this programme
    if(curSpeedX<0.5 && curSpeedX>-0.5)
    {
            cur_pos.position_x=round(cur_pos.position_x);
            fixed_x=1;
            curSpeedX=0;
    }
    if(curSpeedY<0.5 && curSpeedY>-0.5 && cur_pos.position_y>=1 && cur_pos.position_y<2)//stop when up Speed too low
    {
            cur_pos.position_y=1;
            fixed_y=1;
            curSpeedY=0;
    }
    fprintf(logp," \n");
    fflush(logp);

    prev_pos=cur_pos;
    return cur_pos;
}

void launch_ball()
{
    curSpeedX=cos(degree_2_radius(launch_angle))*launch_speed;
    curSpeedY=sin(degree_2_radius(launch_angle))*launch_speed;   
    fixed_x=0;
    fixed_y=0;
}

int get_ball_stable_status()
{
    return fixed_x+fixed_y;
}