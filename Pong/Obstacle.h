#pragma once
#include "dynamic_obj.h"
#include "olcPixelGameEngine.h"
#include "Help_Functions.h"

class Obstacle :
    public dynamic_obj
{
private:
    float time_since_change;
public:
    Obstacle(olc::vf2d position, olc::vf2d size, olc::vf2d speed) {
        this->size = size;
        this->position = position;
        this->speed = speed;
        this->rectangle = true;
        this->solid = true;
        this->id = maxid++;
        this->time_since_change = rand() % 4;
    }
    void draw_self(olc::PixelGameEngine* pong_engine) override { pong_engine->DrawRect(position, size); }
    bool move_self(float elapsed_t, olc::PixelGameEngine* pong_engine, std::list<dynamic_obj*> dyn_objs, std::vector<int>& score) override {
        olc::vf2d next_position = position + speed * elapsed_t;
        //obstacle did not collide
        if (!h_f::rect_vs_rect(this, dyn_objs, next_position)) {
            if (((float)pong_engine->ScreenWidth() / 5.0f - 4.0f <= next_position.x) and (4.0f * (float)pong_engine->ScreenWidth() / 5.0f >= (next_position.x + size.x))) position.x = next_position.x;
            else speed.x *= -1.0f;
            if (0.0f <= next_position.y and (float)pong_engine->ScreenHeight() >= next_position.y + size.y)position.y = next_position.y;
            else speed.y *= -1.0f;
        }
        //every 5s obstacle will change it´s speed
        time_since_change += elapsed_t;
        if (time_since_change > 5.0f) {
            time_since_change = 0.0f;
            if (h_f::random_sign() == 1)speed.x *= (rand() % 3 + 1);
            else speed.x /= (rand() % 3 + 1);
            if (h_f::random_sign() == 1)speed.y *= (rand() % 3 + 1);
            else speed.y /= (rand() % 3 + 1);
            if (abs(speed.x) > 20) speed.x = h_f::get_sign(speed.x) * 20;
            if (abs(speed.y) > 20) speed.y = h_f::get_sign(speed.y) * 20;
            if (speed.x != 0 and abs(speed.x) < 5) speed.x = h_f::get_sign(speed.x) * 5;
            if (speed.y != 0 and abs(speed.y) < 5) speed.y = h_f::get_sign(speed.y) * 5;
        }
        return false;
    }
};

