#pragma once
#include "dynamic_obj.h"
#include "Ball.h"
#include "Paddle.h"
#include "olcPixelGameEngine.h"

class Ball;
class Paddle;
class dynamic_obj;

const olc::vf2d vert_dir = { 0.0f, 1.0f };

struct Line {
	float x1;
	float y1;
	float x2;
	float y2;
	Line(olc::vf2d p1, olc::vf2d p2) {
		x1 = p1.x;
		y1 = p1.y;
		x2 = p2.x;
		y2 = p2.y;
	}
};
class AI
{
private:
	olc::PixelGameEngine* pge;
	Line* vertical;
	Paddle* paddle;
	Ball* ball;
	std::list<dynamic_obj*> dyn_objs;
	float final_y;
	float final_angle;
	bool upper_hit;
	float time_to_hit = 100000.0f;
	float current_time = 0.0f;
	bool rotate = false;
public:
	AI(Paddle* paddle, std::list<dynamic_obj*> dyn_objs, olc::PixelGameEngine* pge) {
		this->paddle = paddle; 
		this->dyn_objs = dyn_objs;
		this->pge = pge;
		for (auto& obj : dyn_objs) if (obj->ball) {
			ball = (Ball*)obj;
			break;
		}
		olc::vf2d closest_p;
		if (paddle->position.x < pge->ScreenWidth() / 2) closest_p = { paddle->position.x + paddle->size.x + ball->radius, paddle->position.y };
		else closest_p = { paddle->position.x - ball->radius, paddle->position.y };
		vertical = new Line(closest_p, closest_p + vert_dir);
		if(!calculate_position()) final_y = paddle->position.y;
	}
	~AI(){}
	//calculates position of expected collision with 
	bool calculate_position() {
		current_time = 0.0f;
		time_to_hit = 100000.0f;
		rotate = false;
		if (paddle->position.x > pge->ScreenWidth() / 2.0f and ball->speed.x < 0.0f) return false;
		if (paddle->position.x < pge->ScreenWidth() / 2.0f and ball->speed.x > 0.0f) return false;
		Line ball_line(ball->position, ball->position + ball->speed);
		float determinant = (vertical->x1 - vertical->x2) * (ball_line.y1 - ball_line.y2) - (vertical->y1 - vertical->y2) * (ball_line.x1 - ball_line.x2);
		if (determinant == 0.0f) return false;	//lines are paralel, there is no interception
		final_y = ((vertical->x1 * vertical->y2 - vertical->y1 * vertical->x2) * (ball_line.y1 - ball_line.y2) - (vertical->y1 - vertical->y2) * (ball_line.x1 * ball_line.y2 - ball_line.y1 * ball_line.x2)) / determinant;
		time_to_hit = abs((final_y - ball->position.y) / ball->speed.y);
		//if the y position is not in screen, we can calculate where in screen ball will end because of swaping from top to bottom or in reverse
		while (final_y < 0.0f) final_y += pge->ScreenHeight();
		while (final_y > pge->ScreenHeight()) final_y -= pge->ScreenHeight();
		//paddle will try to move in way that ball will hit it in 1/4 of its height
		if (final_y > 2.0f * paddle->size.y / 5.0f or final_y < (pge->ScreenHeight() - 3.0f * paddle->size.y / 5.0f)) {
			if (final_y < paddle->position.y + paddle->size.y / 2.0f) {
				final_y -= 2.0f * paddle->size.y / 5.0f;	//paddle will try to perform hit in upper part
				time_to_hit -= (paddle->size.y / ball->speed.mag())/2.0f;
				upper_hit = true;
			}
			else {
				final_y -= 3.0f * paddle->size.y / 5.0f;	//paddle will try to perform hit in lower part
				time_to_hit -= (paddle->size.y / ball->speed.mag())/2.0f;
				upper_hit = false;
			}
		}
		//ball will end in close to top or bottom of screen, we will just move paddle to the top or bottom of the screen
		else if (final_y <= paddle->size.y / 4.0f) {
			final_y = 1.0f;
			time_to_hit -= (paddle->size.y / ball->speed.mag())/3.0f;
			if (final_y < paddle->size.y / 2.0f) upper_hit = true;
			else upper_hit = false;
			
		}
		else {
			final_y = pge->ScreenHeight() - 1.0f - paddle->size.y;
			time_to_hit -= (paddle->size.y / ball->speed.mag())/3.0f;
			if (final_y < pge->ScreenHeight() - paddle->size.y / 2.0f) upper_hit = true;
			else upper_hit = false;
		}
		return true;
	}
	void AI_inputs(float elapsed_t) {
		if (paddle->position.y < final_y) paddle->speed_down(elapsed_t);
		else if (paddle->position.y > final_y) paddle->speed_up(elapsed_t);
		current_time += elapsed_t;
		if (current_time > time_to_hit) {
			rotate = true;
			time_to_hit = 100000.0f;
			current_time = 0.0f;
		}
		if (rotate and abs(paddle->speed.y) < 10.0f) {
			if (upper_hit and paddle->position.x < pge->ScreenWidth() / 2) paddle->turn_right(elapsed_t);
			else if (upper_hit and paddle->position.x > pge->ScreenWidth() / 2) paddle->turn_left(elapsed_t);
			else if (!upper_hit and paddle->position.x < pge->ScreenWidth() / 2) paddle->turn_left(elapsed_t);
			else if (!upper_hit and paddle->position.x > pge->ScreenWidth() / 2) paddle->turn_right(elapsed_t);
		}
		else {
			if (paddle->angle > 0.01) paddle->turn_left(elapsed_t);
			else if (paddle->angle < 0.01) paddle->turn_right(elapsed_t);
		}
	}

};

