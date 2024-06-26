#pragma once
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "dynamic_obj.h"
#include "Paddle.h"
#include "Ball.h"
#include "Obstacle.h"
#include "AI.h"

// Override base class with your custom functionality
class Pong_Engine : public olc::PixelGameEngine
{
private:
	void get_inputs(float elapsed_t) {
		if (GetKey(olc::Key::I).bPressed) {
			if (!paddles[0]->computer) {
				paddles[0]->computer = true;
				AIs[0] = new AI(paddles[0], dyn_objs, this);
			}
			else {
				paddles[0]->computer = false;
				delete(AIs[0]);
				AIs[0] = nullptr;
			}
		}
		if (GetKey(olc::Key::O).bPressed) {
			if (!paddles[1]->computer) {
				paddles[1]->computer = true;
				AIs[1] = new AI(paddles[1], dyn_objs, this);
			}
			else {
				paddles[1]->computer = false;
				delete(AIs[1]);
				AIs[1] = nullptr;
			}
		}
		if (!paddles[0]->computer) {
			if (GetKey(olc::Key::S).bHeld or GetKey(olc::Key::S).bPressed) paddles[0]->speed_down(elapsed_t);
			if (GetKey(olc::Key::W).bHeld or GetKey(olc::Key::W).bPressed) paddles[0]->speed_up(elapsed_t);
			if (GetKey(olc::Key::D).bHeld or GetKey(olc::Key::D).bPressed) paddles[0]->turn_right(elapsed_t);
			if (GetKey(olc::Key::A).bHeld or GetKey(olc::Key::A).bPressed) paddles[0]->turn_left(elapsed_t);
		}
		if (!paddles[1]->computer) {
			if (GetKey(olc::Key::DOWN).bHeld or GetKey(olc::Key::DOWN).bPressed) paddles[1]->speed_down(elapsed_t);
			if (GetKey(olc::Key::UP).bHeld or GetKey(olc::Key::UP).bPressed) paddles[1]->speed_up(elapsed_t);
			if (GetKey(olc::Key::RIGHT).bHeld or GetKey(olc::Key::RIGHT).bPressed) paddles[1]->turn_right(elapsed_t);
			if (GetKey(olc::Key::LEFT).bHeld or GetKey(olc::Key::LEFT).bPressed) paddles[1]->turn_left(elapsed_t);
		}
		if (GetKey(olc::Key::R).bPressed) {
			for (auto &obj : dyn_objs) if (obj->ball) {
				obj->position = { ScreenWidth() / 2.0f, ScreenHeight() / 2.0f };
				obj->speed = { float(h_f::random_sign() * 75.0f), float(h_f::random_sign() * 5.0f) };
				for (auto AI : AIs) if (AI != nullptr) AI->calculate_position();
				break;
			}
		}
	}
public:
	std::vector<AI*> AIs = {nullptr, nullptr};
	std::vector<int> score = { 0,0 };
	olc::vf2d screen_size;
	olc::vf2d screen_origin = { 0.0f, 0.0f };
	std::vector<Paddle*> paddles = { nullptr, nullptr };
	std::list<dynamic_obj*> dyn_objs;
	Pong_Engine()
	{
		sAppName = "Pong";
	}

public:
	bool OnUserCreate() override
	{
		screen_size = { (float)ScreenWidth(),(float)ScreenHeight() };
		//create pads for each player, create map
		dyn_objs.push_back(new Ball(5.0f, { screen_size.x / 2.0f , screen_size.y / 2.0f }));
		paddles[0] = (new Paddle({ 10.0f, screen_size.y / 2.0f - 35.0f }));
		dyn_objs.push_back(paddles[0]);
		paddles[1] = (new Paddle({ screen_size.x - 20.0f, screen_size.y / 2.0f - 35.0f }));
		dyn_objs.push_back(paddles[1]);
		dyn_objs.push_back(new Obstacle({ (float)(rand() % (int)(screen_size.x / 5.0f)) + screen_size.x / 5.0f, (screen_size.y / 3.0f) - 5.0f}, { 80.0f, 15.0f }, { -10.0f, 0.0f }));
		dyn_objs.push_back(new Obstacle({ (float)(rand() % (int)(screen_size.x / 5.0f)) + screen_size.x / 5.0f, (2.0f * screen_size.y / 3.0f) - 5.0f }, { 80.0f, 15.0f }, { 10.0f, 0.0f }));
		/*dyn_objs.push_back(new Obstacle({screen_size.x / 3.0f, (float)(rand() % (int)(screen_size.y / 2.0f) + 30.0f) }, { 15.0f, 100.0f }, { 0.0f, 20.0f }));*/
		/*dyn_objs.push_back(new Obstacle({ 2.0f * screen_size.x / 3.0f , (float)(rand() % (int)(screen_size.y / 3.0f) + 30.0f) }, { 15.0f, 50.0f }, { 0.0f, 20.0f }));*/
		return true;
	}

	bool OnUserUpdate(float elapsed_t) override
	{
		Clear(olc::BLACK);
		DrawString({ ScreenWidth() - 80, 10}, "Score:" + std::to_string(score[1]), olc::RED);
		DrawString({ 20, 10 }, "Score:" + std::to_string(score[0]), olc::RED);
		//search for inputs to move paddels
		get_inputs(elapsed_t);
		for (auto AI : AIs) if (AI != nullptr)AI->AI_inputs(elapsed_t);
		// move balls, pads and dynamic obstacles
		for (auto d : dyn_objs) {
			if (d->move_self(elapsed_t, this, dyn_objs, score)) for (auto AI : AIs) if (AI != nullptr) AI->calculate_position();	//if hit happened and there is some AI paddle, calculate new final posiiton
			d->draw_self(this);
		}
		/*DrawString({ 100,100 }, std::to_string(paddles[0]->speed.y));*/
		return true;
	}
};



