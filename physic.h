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
struct ball_pos get_ball_pos(float predict_dist);
int get_ball_stable_status();
void predict_ball_route(char key, int bounce_times, float dist);
void clear_predict_print();

void phy_back_up_data();
void phy_extract_data();


void launch_ball();
#endif