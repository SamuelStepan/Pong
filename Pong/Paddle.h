#pragma once
#include "dynamic_obj.h"
#include "olcPixelGameEngine.h"
#include "Help_Functions.h"

class Paddle :
    public dynamic_obj
{
private:
    float acc = 40.0f;
    float rot_acc = 4.0f;
    bool pressed_rot_btn = false;   //for buttons that deal with rotation movement speed
    bool pressed_vert_btn = false;  //for buttons that deal with movement in vertical direction
public:
    bool computer = false;
public:
    Paddle(olc::vf2d position) {
        this->size = { 10.0f, 70.0f };
        this->position = position;
        this->speed = { 0.0f, 0.0f };
        this->rectangle = true;
        this->solid = true;
        this->id = maxid++;
        this->paddle = true;
    }
    //uploads movement speed in up direction by acceleration, if it is used as break, triples acceleration 
    void speed_up(float elapsed_t) { speed.y -= (speed.y < 0) ? acc * elapsed_t : 3.0f * acc * elapsed_t; pressed_vert_btn = true;}
    //same but in down direction
    void speed_down(float elapsed_t) { speed.y += (speed.y > 0) ? acc * elapsed_t : 3.0f * acc * elapsed_t; pressed_vert_btn = true;}
    void turn_right(float elapsed_t) { 
        rot_speed += (rot_speed > 0) ? rot_acc * elapsed_t : 3.0f * rot_acc * elapsed_t; 
        if (abs(rot_speed) > 3.0f) rot_speed = h_f::get_sign(rot_speed) * 3.0f;
        pressed_rot_btn = true;
    }
    void turn_left(float elapsed_t) { 
        rot_speed -= (rot_speed < 0) ? rot_acc * elapsed_t : 3.0f * rot_acc * elapsed_t; 
        if (abs(rot_speed) > 3.0f) rot_speed = h_f::get_sign(rot_speed) * 3.0f;
        pressed_rot_btn = true;
    }
    void draw_self(olc::PixelGameEngine* pong_engine) override { h_f::draw_rotated_rect(pong_engine, this); }
    bool move_self(float elapsed_t, olc::PixelGameEngine* pong_engine, std::list<dynamic_obj*> dyn_objs, std::vector<int>& score) override {
        olc::vf2d next_position = position + speed * elapsed_t;
        angle += rot_speed * elapsed_t;
        /*if (0.0f <= next_position.x and (float)pong_engine->ScreenWidth() >= next_position.x + size.x) position.x = next_position.x;
        else speed.y = 0.0f;*/
        if (1.0f <= next_position.y and (float)pong_engine->ScreenHeight() - 1.0f >= next_position.y + size.y)position.y = next_position.y;
        else speed.y = 0.0f;
        if (!pressed_rot_btn and (abs(rot_speed)>0)) {
            rot_speed -= h_f::get_sign(rot_speed) * 4.0f * elapsed_t;
            if (abs(rot_speed) < 0.5f) rot_speed = 0.0f;
        }
        if (!pressed_vert_btn and (abs(speed.y) > 0)) {
            speed.y -= h_f::get_sign(speed.y) * 20.0f * elapsed_t;
            if (abs(speed.y) < 0.5f) speed.y = 0.0f;
        }
        pressed_rot_btn = false;
        pressed_vert_btn = false;
        return false;
    }
    float get_rot_speed() { return rot_speed; }
};

