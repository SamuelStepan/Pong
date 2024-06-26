#pragma once
#include "olcPixelGameEngine.h"
#include <vector>
#include <list>


class dynamic_obj
{
public:
	static int maxid;
	bool rectangle = false;
	bool paddle = false;
	bool ball = false;
	bool solid = false;
	float radius = 0;
	float angle = 0;
	float rot_speed = 0;	//omega
	olc::vf2d position;
	olc::vf2d speed;
	olc::vf2d size;
protected:
	int id;
public:
	dynamic_obj() {}
	int get_id() { return id; }
	virtual void draw_self(olc::PixelGameEngine* pong_engine) {};
	//returns true if change of direction happened
	virtual bool move_self(float elapsed_t, olc::PixelGameEngine* pong_engine, std::list<dynamic_obj*> dyn_objs, std::vector<int>& score) { return false; };
	
};
int dynamic_obj::maxid = 0;

