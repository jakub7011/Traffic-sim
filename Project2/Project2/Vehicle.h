#pragma once
#include "Road.h"
#undef max

struct Obstacle {
	float dist;
	float speed;
	float acc;
	Obstacle(float a, float b, float c);
};

class Vehicle
{
public:
	float x;
	float speed;
	float acc;
	float desDistance;
	float desTime;
	float desSpeed;
	float ds;
	float maxAcceleration;
	float maxDecceleration;
	float length;
	float safetyBreak;
	float politeness;
	float critical;
	bool approaching;
	bool isLeader;
	std::vector<Junction *> juncs;
	Road* road;
	int lane;
	float juncOffset;
	int dir;
	int card;
	float travelTime;
	float expTime;
	std::vector<int> turns; //0 right, 1 straight, 2 left
	Vehicle(float x, Road* r, int lane, int dir, std::vector<Junction*> jun, std::vector<int> tur, float speed);

	void update(Obstacle obst);

	void switchLane(int dr);

	void turn();

	float calcChSpeed(Obstacle o);
};