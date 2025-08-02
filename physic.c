#include <stdio.h>
#include <float.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "system_utils.h"
#include "scene/world_manager.h"

FILE *log_pred, *log_real;
struct ball_pos
{
    double position_x;
    double position_y;
};
float gravity = 0.5, air_friction_strength=0.02;
double curSpeedX=0, curSpeedY=0, radius, pred_curSpeedX=0, pred_curSpeedY;
struct ball_pos cur_pos={.position_x=1.0, .position_y=1.0};
struct ball_pos prev_pos={.position_x=1.0, .position_y=1.0};
struct ball_pos pred_cur_pos;
struct ball_pos pred_prev_pos;
struct pixel_info current_pixel_info_Up, current_pixel_info_Dwn, current_pixel_info_Rgh, current_pixel_info_Lft,current_pixel_info;


int fixed_x=0,fixed_y=0;
int hit_flag=0, star_flag=0;

double dda_route_dist_x,dda_route_dist_y, dda_total_dist, dda_traveled_dist;
double dda_step_x,dda_step_y;
struct ball_pos dda_cursor, dda_prev_cursor;

struct ball_pos pred_route[24];
int pred_cursor=0;

int hit_angle=-1;
int launch_angle=45;
double launch_speed=6;

double degree_2_radius (double degree)
{
    double radius=degree*(3.1415926/180);
    return radius;
}

void init_ball_status(int start_position_x, int start_position_y, int starting_angle, int starting_speed)
{
    log_real=fopen("hit.log","w");
    fprintf(log_real,"Start:\n");
    log_pred=fopen("pred.log","w");
    fprintf(log_pred,"Start:\n");
    cur_pos.position_x=start_position_x;
    cur_pos.position_y=start_position_y;
    prev_pos=cur_pos;
    prev_pos=cur_pos;
}

struct ball_pos get_ball_pos(float predict_dist)//predict_dist=0:real move, else prediting
{
    if(dda_route_dist_y>0 || current_pixel_info_Dwn.is_wall!=1 || current_pixel_info_Dwn.angle!=0)//if ball not on platform and not yet going up, then provide downward speed from gravity
        curSpeedY-=gravity;

    //Air Friction to Speed
    curSpeedX-=curSpeedX*(air_friction_strength);
    curSpeedY-=curSpeedY*(air_friction_strength);

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
    if(predict_dist != 0 && dda_total_dist > predict_dist)//end pred earlier when it require less distance than provided
        dda_total_dist = predict_dist;
        
    if(predict_dist==0)
        fprintf(log_real,"From[%5.2f %5.2f] To [%5.2f %5.2f] Speed{%5.2f,%5.2f} Pred:%f ",prev_pos.position_x,prev_pos.position_y,cur_pos.position_x,cur_pos.position_y,curSpeedX,curSpeedY,predict_dist);
    else
        fprintf(log_pred,"From[%5.2f %5.2f] To [%5.2f %5.2f] Speed{%5.2f,%5.2f} Pred:%f ",prev_pos.position_x,prev_pos.position_y,cur_pos.position_x,cur_pos.position_y,curSpeedX,curSpeedY,predict_dist);
    dda_cursor.position_x=prev_pos.position_x;//set dda_cursor pos to current pos
    dda_cursor.position_y=prev_pos.position_y;
    while (dda_traveled_dist < dda_total_dist) 
    {
        //cal next location with step
        dda_cursor.position_x+=dda_step_x;
        dda_cursor.position_y+=dda_step_y;
        dda_traveled_dist+=0.5;
        if(floor(dda_cursor.position_x)==floor(dda_prev_cursor.position_x) && floor(dda_cursor.position_y)==floor(dda_prev_cursor.position_y))//repeat pixel, skip
            continue;
        dda_prev_cursor.position_x=dda_cursor.position_x;
        dda_prev_cursor.position_y=dda_cursor.position_y;

        if(predict_dist!=0)
        {
            pred_route[pred_cursor].position_x=dda_cursor.position_x;
            pred_route[pred_cursor].position_y=dda_cursor.position_y;
            pred_cursor++;
        }

        if(predict_dist==0)
            fprintf(log_real," [%2.0f %2.0f]",dda_cursor.position_x,dda_cursor.position_y);
        else
            fprintf(log_pred," [%2.0f %2.0f]",dda_cursor.position_x,dda_cursor.position_y);
        current_pixel_info_Up=get_pixel_info(0,(int)floor(dda_cursor.position_x),(int)floor(dda_cursor.position_y)+1);
        current_pixel_info_Dwn=get_pixel_info(0,(int)floor(dda_cursor.position_x),(int)floor(dda_cursor.position_y)-1);
        current_pixel_info_Rgh=get_pixel_info(0,(int)floor(dda_cursor.position_x)+1,(int)floor(dda_cursor.position_y));
        current_pixel_info_Lft=get_pixel_info(0,(int)floor(dda_cursor.position_x)-1,(int)floor(dda_cursor.position_y));
        current_pixel_info=get_pixel_info(0,(int)floor(dda_cursor.position_x),(int)floor(dda_cursor.position_y));
        
        if(dda_route_dist_y>0  && current_pixel_info_Up.is_wall==1  && !fixed_y)//check if is a hit
            hit_flag=hit_flag|0b1000; 
        if(dda_route_dist_y<=0 && current_pixel_info_Dwn.is_wall==1)
            if(!fixed_y)
                hit_flag=hit_flag|0b0100;
            else
                if(curSpeedX>1)//if fixed Y and on grond, slow. Bouncing is wonderful but sliding is bored.
                    curSpeedX*=0.625;
                else
                    curSpeedX*=0.9375;
        if(dda_route_dist_x>0  && current_pixel_info_Rgh.is_wall==1 && !fixed_x)
            hit_flag=hit_flag|0b0010;
        if(dda_route_dist_x<=0 && current_pixel_info_Lft.is_wall==1 && !fixed_x)
            hit_flag=hit_flag|0b0001;
        
        if(hit_flag>0)//Due with hit
        {
            if(predict_dist==0)
                fprintf(log_real," HIT!");
            else
                fprintf(log_pred," HIT!");
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
            if(predict_dist==0)
                fprintf(log_real," [%d %d] {%.2f,%.2f}",(int)cur_pos.position_x,(int)cur_pos.position_y,curSpeedX,curSpeedY);
            else
                fprintf(log_pred," [%d %d] {%.2f,%.2f}",(int)cur_pos.position_x,(int)cur_pos.position_y,curSpeedX,curSpeedY);

            hit_angle=-1;//set used back to defualt
            hit_flag=0;
            break;//the ball settle here
        }

        if(current_pixel_info.is_wall==2)//check if is a hit star
        {
            hit_star(dda_cursor.position_x,dda_cursor.position_y);
        }
    }
            
    if(curSpeedX<0.5 && curSpeedX>-0.5)//Speed too low == stop. As Speed can never be 0 in this programme
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
    if(predict_dist==0)
    {
        fprintf(log_real," \n");
        fflush(log_real);
    }else
    {
        fprintf(log_pred," \n");
        fflush(log_pred);        
    }


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

void predict_ball_route(int bounce_times, float dist)
{
    curSpeedX=cos(degree_2_radius(launch_angle))*launch_speed;
    curSpeedY=sin(degree_2_radius(launch_angle))*launch_speed;   
    fixed_x=0;
    fixed_y=0;
    pred_cursor=0;
    for(int i=0;i<bounce_times|| dist<0;i++)
    {   
        get_ball_pos(dist);
        for(int pred_cursor=1;pred_cursor<24 && (pred_route[pred_cursor].position_x!=0 && pred_route[pred_cursor].position_y!=0) ;pred_cursor++)//read route from array
        {
            printf("\033[%d;%dH", 20 - (int)round(pred_route[pred_cursor].position_y), (int)round(pred_route[pred_cursor].position_x) + 1);
            printf(".");
        }
        dist-=(dda_total_dist-( dda_total_dist));
    }
    fflush(stdout);
}

void clear_predict_print()//why don't embedded in predict_ball_route you would ask, cz we don't want new print when clear it
{
    for(int pred_cursor=1;pred_cursor<24 && (pred_route[pred_cursor].position_x!=0 && pred_route[pred_cursor].position_y!=0) ;pred_cursor++)//read route from array
    {
        printf("\033[%d;%dH", 20 - (int)round(pred_route[pred_cursor].position_y), (int)round(pred_route[pred_cursor].position_x) + 1);
        printf(" ");
    }
    bzero(pred_route,sizeof(pred_route));//clean the route histrt
}

void phy_back_up_data()//move all data to cache, some func need get_ball_pos, and we don't want affect the main data
{
    pred_curSpeedX=curSpeedX;
    pred_curSpeedY=curSpeedY;
    pred_cur_pos.position_x=cur_pos.position_x;
    pred_cur_pos.position_y=cur_pos.position_y;
    pred_prev_pos.position_x=prev_pos.position_x;
    pred_prev_pos.position_y=prev_pos.position_y;
}

void phy_extract_data()
{
    curSpeedX=pred_curSpeedX;
    curSpeedY=pred_curSpeedY;
    cur_pos.position_x=pred_cur_pos.position_x;
    cur_pos.position_y=pred_cur_pos.position_y;
    prev_pos.position_x=pred_prev_pos.position_x;
    prev_pos.position_y=pred_prev_pos.position_y;
}

int get_ball_stable_status()
{
    return fixed_x+fixed_y;
}