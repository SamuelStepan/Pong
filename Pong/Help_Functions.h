#pragma once
#include <random>
#include <ctime>
#include <cmath>
#include "dynamic_obj.h"
#include "olcPixelGameEngine.h"

class dynamic_obj;

const float pi = 3.14159265358979323846f;

namespace h_f {
	//returns random -1 or 1
	int random_sign() {
		srand(time(0));
		int rnd = rand() % 2;
		rnd = (rnd != 0) ? rnd : -1;
		return rnd;
	}
	int get_sign(float number) {
		if (number >= 0) return 1;
		else return -1;
	}
	//function that returns k element from list
	dynamic_obj* get_k_elem(std::list<dynamic_obj*> extraction_list, int k) {
		std::list<dynamic_obj*>::iterator it = extraction_list.begin();
		for (int i = 0; i < k; i++) {
			++it;
		}
		return *it;
	}
	//rotates point around origin given direction(by cos and sin)
	olc::vf2d rotate_vector(olc::vf2d point, olc::vf2d &origin, float& cos_angle, float& sin_angle) {
		point = point - origin;
		point = { point.x * cos_angle - point.y * sin_angle, point.x * sin_angle + point.y * cos_angle };		
		return point + origin;
	}
	//draws rotated rectangle
	void draw_rotated_rect(olc::PixelGameEngine* pong_engine, dynamic_obj* rect) {
		float cos_angle = cos(rect->angle);
		float sin_angle = sin(rect->angle);
		std::vector<olc::vf2d> rect_points = { {rect->position.x, rect->position.y}, {rect->position.x + rect->size.x, rect->position.y}, {rect->position.x + rect->size.x, rect->position.y + rect->size.y} , { rect->position.x, rect->position.y + rect->size.y } };
		olc::vf2d pointA;
		olc::vf2d pointB;
		olc::vf2d origin = (rect_points[0] + rect_points[2]) / 2.0f;
		
		for (int i = 0; i < 4; i++) {
			pointA = rect_points[i];
			pointA = rotate_vector(pointA, origin, cos_angle, sin_angle);
			/*if (i == 0)pong_engine->DrawCircle(pointA, 1.0f, olc::RED);*/
			if (i != 3) pointB = rect_points[i + 1];
			else pointB = rect_points[0];
			pointB = rotate_vector(pointB, origin, cos_angle, sin_angle);
			pong_engine->DrawLine(pointA, pointB, olc::BLUE);
		}
	}
	//checks for collision with rotated paddle, solves it if needed
	bool ball_vs_paddle(dynamic_obj* ball, dynamic_obj* paddle, olc::vf2d potential_position, bool dyn_collision) {
		olc::vf2d nearest_p, vector_to_rect;
		float over_lap;
		float cos_minus = cos(-(paddle->angle));
		float sin_minus = sin(-(paddle->angle)); 
		float cos_plus = cos((paddle->angle));
		float sin_plus = sin((paddle->angle));
		olc::vf2d zero_p = { 0.0f, 0.0f };
		olc::vf2d origin = { (2.0f * paddle->position.x + paddle->size.x) / 2.0f, (2.0f * paddle->position.y + paddle->size.y) / 2.0f };
		//if we rotate ball in counterangle to paddle, we can sort collision very similary to standart ball vs rectangle collision
		olc::vf2d rot_ball_pos = rotate_vector(ball->position, origin, cos_minus, sin_minus);
		olc::vf2d rot_ball_speed = rotate_vector(ball->speed, zero_p, cos_minus, sin_minus);
		olc::vf2d rot_potential_position = rotate_vector(potential_position, origin, cos_minus, sin_minus);
		olc::vf2d rot_paddle_speed = rotate_vector(paddle->speed, zero_p, cos_minus, sin_minus);
		
		//clamping, calculates the closest point at rectangle to ball
		nearest_p.x = std::max(paddle->position.x, std::min(rot_potential_position.x, paddle->position.x + paddle->size.x));
		nearest_p.y = std::max(paddle->position.y, std::min(rot_potential_position.y, paddle->position.y + paddle->size.y));
		olc::vf2d rot_nearest_p = rotate_vector(nearest_p, origin, cos_plus, sin_plus);

		vector_to_rect = nearest_p - rot_potential_position;
		over_lap = ball->radius - vector_to_rect.mag();
		if (std::isnan(over_lap)) over_lap = 0;
		//collision happened
		if (over_lap > 0) {
			rot_potential_position = rot_potential_position - vector_to_rect.norm() * over_lap;		//moves ball back, it touches the rectangle
			rot_ball_pos = rot_potential_position;
			if (dyn_collision) {
				if (nearest_p.x > paddle->position.x and nearest_p.x < paddle->position.x + paddle->size.x) {	//hit from upper or bottom side
					rot_ball_speed.y *= -1;
					rot_ball_speed.x += rot_paddle_speed.x;
					rot_ball_speed.y += rot_paddle_speed.y;
				}
				else /*if (nearest_p.y > paddle->position.y and nearest_p.y < paddle->position.y + paddle->size.y)*/ {	//hit from left or right side
					float rounded_angle = paddle->angle;	//angle between -pi and pi
					int k = rounded_angle / (2.0f * pi);
					rounded_angle -= k * 2.0f * pi;
					if (rounded_angle < 0.0f) rounded_angle = 2.0f * pi + rounded_angle;
					bool upper_collision = (rounded_angle <= pi / 2.0f or rounded_angle >= 3.0f * pi / 2.0f) ? ((nearest_p.y - origin.y) <= 0.0f) : ((nearest_p.y - origin.y) >= 0.0f);		//upper collision in screen space
					bool left_collision = (rounded_angle <= pi / 2.0f or rounded_angle >= 3.0f * pi / 2.0f) ? ((nearest_p.x - origin.x) <= 0.0f) : ((nearest_p.x - origin.x) >= 0.0f);		//left collision in screen space
					float arm = abs(nearest_p.y - origin.y);
					float rotation_speed = paddle->rot_speed * (arm);
					float passed_rot_speed = 0.0f;
					if ((left_collision) and rotation_speed < 0 and upper_collision) passed_rot_speed = rotation_speed;	//hit from left side in upper side and counterclockwise rotation speed
					else if ((left_collision) and rotation_speed > 0 and !upper_collision) passed_rot_speed = -rotation_speed;	//hit from left side in lower side and clockwise rotation speed
					else if ((!left_collision) and rotation_speed < 0 and !upper_collision) passed_rot_speed = -rotation_speed;	//hit from right side in upper side and counterclockwise rotation speed
					else if ((!left_collision) and rotation_speed > 0 and upper_collision) passed_rot_speed = rotation_speed;	//hit from right side in lower side and clockwise rotation speed
					if (!(rounded_angle <= pi / 2.0f or rounded_angle >= 3.0f * pi / 2.0f)) passed_rot_speed *= -1;	//if angle is between pi/2 and 3pi/2, given rotation speed will be opposite because we operate in rotated axis
					rot_ball_speed.x *= -1;
					rot_ball_speed.x += rot_paddle_speed.x + passed_rot_speed;
					rot_ball_speed.y += rot_paddle_speed.y;
				}
				paddle->rot_speed /= 3.0f;
			}
			//rotates back ball after solved collision
			ball->position = rotate_vector(rot_ball_pos, origin, cos_plus, sin_plus);
			ball->speed = rotate_vector(rot_ball_speed, zero_p, cos_plus, sin_plus);
			return true;
		}
		return false;
	}
	//function that solves out if collision between ball and rectangle happened
	bool ball_vs_rect(dynamic_obj *ball, std::list<dynamic_obj*> dyn_objs, olc::vf2d potential_position, bool dyn_collision = true) {
		olc::vf2d nearest_p; //nearest point between ball and rectangle
		olc::vf2d vector_to_rect; //vector from middle of ball to rectangle(closest range)
		float over_lap;
		for (auto& obj : dyn_objs) {
			if (ball->get_id() != obj->get_id() and obj->rectangle) {
				if (obj->paddle and obj->angle != 0) {
					if (ball_vs_paddle(ball, obj, potential_position, dyn_collision)) return false;
				}
				else {
					//clamping, calculates the closest point at rectangle to ball
					nearest_p.x = std::max(obj->position.x, std::min(potential_position.x, obj->position.x + obj->size.x));
					nearest_p.y = std::max(obj->position.y, std::min(potential_position.y, obj->position.y + obj->size.y));

					vector_to_rect = nearest_p - potential_position;
					over_lap = ball->radius - vector_to_rect.mag();
					if (std::isnan(over_lap)) over_lap = 0;
					//collision happened
					if (over_lap > 0) {
						potential_position = potential_position - ball->speed.norm() * over_lap;		//moves ball back, it touches the rectangle
						ball->position = potential_position;
						if (dyn_collision) {
							if (nearest_p.x > obj->position.x and nearest_p.x < obj->position.x + obj->size.x) {	//hit from upper or bottom side
								ball->speed.y *= -1;
								ball->speed.y += obj->speed.y;
							}
							else /*if (nearest_p.y > obj->position.y and nearest_p.y < obj->position.y + obj->size.y)*/ {	//hit from left or right side
								ball->speed.x *= -1;
								ball->speed.y += obj->speed.y;
							}
						}
						return false;
					}
				}				
			}
		}
		return true;
	}
	//returns true given rectangles collide
	bool rect_vs_rect_collision(dynamic_obj* r1, dynamic_obj* r2, olc::vf2d potential_position) {
		if ((int)potential_position.x < (int)r2->position.x + (int)r2->size.x and
			(int)potential_position.x + (int)r1->size.x >(int)r2->position.x and
			(int)potential_position.y < (int)r2->position.y + (int)r2->size.y and
			(int)potential_position.y + (int)r1->size.y >(int)r2->position.y) return true;
		return false;
	}
	//returns true if given rectangle collide with any other rectangle and solves the collision
	bool rect_vs_rect(dynamic_obj* rect, std::list<dynamic_obj*> dyn_objs, olc::vf2d potential_position, bool only_obstacle = true) {
		for (auto obj : dyn_objs) {
			if (only_obstacle and obj->paddle) continue;	//since map is designed in way obstacles can not hit paddles, we will skip paddle objects
			if (obj->rectangle and (obj->get_id() != rect->get_id())) {
				if (rect->speed.x != 0 or rect->speed.y != 0) {
					if (rect_vs_rect_collision(rect, obj, potential_position)) {
						if (rect->speed.x > 0) rect->position.x = obj->position.x - rect->size.x;
						else if(rect->speed.x < 0)rect->position.x = obj->position.x + obj->size.x;
						rect->speed.x *= -1;
						if (rect->speed.y > 0) rect->position.y = obj->position.y - rect->size.y;
						else if (rect->speed.y < 0)rect->position.y = obj->position.y + obj->size.y;
						rect->speed.y *= -1;
						return true;
					}
				}				
			}
		}
		return false;
	}
	
}
