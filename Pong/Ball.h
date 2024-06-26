#pragma once
#include "dynamic_obj.h"
#include "olcPixelGameEngine.h"
#include "Help_Functions.h"

class Ball :
    public dynamic_obj
{
private:
    bool swap = false; //varibale that signals if ball has been swaped from top corner to botttom or in reverse
    float time_since_swap = 0.0f;
    bool hit = false;
    float time_since_hit = 0.0f;
public:
    Ball(float radius, olc::vf2d position) {
        this->radius = radius;
        int rnd_int = h_f::random_sign() * 20 * (rand() % 3 + 2);
        this->speed.x =  rnd_int;
        rnd_int = h_f::random_sign() * 25 * (rand() % 1 + 1);
        this->speed.y = rnd_int;
        this->position = position;
        this->ball = true;
        this->id = maxid++;
    }
    void draw_self(olc::PixelGameEngine* pong_engine) override { pong_engine->DrawCircle(position, this->radius); }
    bool move_self(float elapsed_t, olc::PixelGameEngine* pong_engine, std::list<dynamic_obj*> dyn_objs, std::vector<int>& score) override {
        h_f::ball_vs_rect(this, dyn_objs, position, false); //because since last move of ball obstacles and paddles could move, we must first solve static collision and then we can look for dynamic collision
        olc::vf2d next_position = position + speed * elapsed_t;
        if (h_f::ball_vs_rect(this, dyn_objs, next_position, !hit)) position = next_position;  //solves ball vs rectangle collision if happened, if not, just moves the ball
        else hit = true;
        if (hit) time_since_hit += elapsed_t;
        if (time_since_hit > 0.01f) {
            hit = false;
            time_since_hit = 0.0f;
        }
        if (swap) time_since_swap += elapsed_t;
        if (time_since_swap > 1.0f) {
            time_since_swap = 0.0f;
            swap = false;
        }
        if (position.y + radius < 0.0f and !swap) {     //ball has moved up from screen
            position.y = pong_engine->ScreenHeight() + radius;
            swap = true;
        }
        if (position.y - radius > pong_engine->ScreenHeight() and !swap) {  //ball has moved down from screen
            position.y = 0.0f - radius;
            swap = true;
        }
        if (position.x - radius > pong_engine->ScreenWidth()) { //player 1 scores
            position = { pong_engine->ScreenWidth() / 2.0f, pong_engine->ScreenHeight() / 2.0f };
            int rnd_int = h_f::random_sign() * 25 * (rand() % 3 + 1);
            this->speed.x = rnd_int;
            rnd_int = h_f::random_sign() * 25 * (rand() % 1 + 1);
            this->speed.y = rnd_int;
            score[0] += 1;
            return true;
            /*pong_engine->DrawString({ (int)(pong_engine->ScreenWidth() / 2.0f), (int)(pong_engine->ScreenHeight() / 6.0f) }, "Score!!!", olc::RED);*/
        }
        if (position.x + radius < 0) {  //player 2 scores
            position = { pong_engine->ScreenWidth() / 2.0f, pong_engine->ScreenHeight() / 2.0f };
            int rnd_int = h_f::random_sign() * 25 * (rand() % 3 + 1);
            this->speed.x = rnd_int;
            rnd_int = h_f::random_sign() * 25 * (rand() % 1 + 1);
            this->speed.y = rnd_int;
            score[1] += 1;
            return true;
            /*pong_engine->DrawString({ (int)(pong_engine->ScreenWidth() / 2.0f), (int)(pong_engine->ScreenHeight() / 6.0f) }, "Score!!!", olc::RED);*/
        }
        if (hit) return true;
        else return false;
    }
};

