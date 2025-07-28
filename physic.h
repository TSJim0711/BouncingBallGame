#ifndef PHYSIC_H
#define PHYSIC_H

struct ball_pos
{
    double position_x;
    double position_y;
};

extern int launch_angle;
extern double launch_speed;

void init_ball_status(int start_position_x, int start_position_y, int starting_angle, int starting_speed);
struct ball_pos get_ball_pos();
int get_ball_stable_status();

void launch_ball();
#endif