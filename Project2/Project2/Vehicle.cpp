#pragma once
#include "Vehicle.h"
#include <algorithm>  
#include <windows.h>
#include <string>
#include <cmath>  
#include <..\um\debugapi.h>
#include <iostream>
#include <random>
#include <stdlib.h>
#include <time.h>  
#undef max

std::default_random_engine generator;
std::normal_distribution<float> distribution(0.0, 1.0);

Obstacle::Obstacle(float a, float b, float c) : dist(a), speed(b) {}

Vehicle::Vehicle(float x, Road* r, int lane, int dir, std::vector<Junction*> jun, std::vector<int> tur, float sp)
	: x(x), road(r), lane(lane), dir(dir), juncs(jun), turns(tur) {
	speed = sp;
	desDistance = 7.0f; //2.5 m + 4.5m length
	ds = desSpeed;
	maxAcceleration = 0.305f;
	maxDecceleration = 0.40f;
	length = 4.5f;
	safetyBreak = 3.0f;
	critical = 0.05f;
	approaching = false;
	if (juncs.size() > 0) {
		for (int x = 0; x < juncs[0]->roads.size(); ++x) {
			if (juncs[0]->roads[x] == road) {
				juncOffset = juncs[0]->offsets[x];
			}
		}
	}
	acc = 0;
	card = (road->dir + (dir - 1) + 4) % 4;
	//
	// Human driven
	//

	float speedrand = distribution(generator);
	float timerand = distribution(generator);
	float polirand = distribution(generator);
	desSpeed = 11.2f +((speedrand > -2.0 && speedrand < 2.0) ? speedrand : 0.0); //approximating truncated normal distribution, speed unit is m per time step, i. e. 1/2 m/s
	desTime = 1.5f +0.35f*(timerand > -2.0 && timerand < 2.0 ? timerand : 0.0);
	politeness = 0.3f + 0.1f*(polirand > -2.0 && polirand < 2.0 ? polirand : 0.0);
	//
	// Autnomous
	//
	/*
	desSpeed = 11.2f;
	desTime = 0.5f;
	politeness = 0.2f;
	*/
	//
	// CAV
	//
	/*
	desSpeed = 11.2f;
	desTime = 0.5f;
	politeness = 0.8f;
	*/
}

void Vehicle::update(Obstacle obst) {
	// calculate according to IDM
	acc = calcChSpeed(obst);
	speed += acc;
	if (speed < 0) {
		speed = 0;
	}
	x += (speed*dir);
	if (juncs.size() > 0) {
		if (abs(x - juncOffset) < desSpeed*10.0 && isLeader && dir*(juncOffset - x)>0) {
			approaching = true;
			if (turns[0] != 1) {
				desSpeed = 2.0f;
			}
		}
		if (approaching && abs(x - juncOffset) < speed + maxAcceleration && (juncs[0]->status == 0 || (juncs[0]->status == 2 && road->dir == 1) || (juncs[0]->status == 3 && road->dir == 2))){
			turn();
		}
	}
}

void Vehicle::switchLane(int dr) {
	lane += dr;
}

void Vehicle::turn() {

	Road* thisRoad = road;
	int thatRoad;
	if (juncs[0]->roads[0] == road) {
		thatRoad = 1;
	}

	else {
		thatRoad = 0;
	}

	card = (card + 4 + turns[0] - 1) % 4;
	if (card == 1 || card == 2) {
		dir = 1;
	}
	else {
		dir = -1;
	}

	if (turns[0] != 1) {
		road = juncs[0]->roads[thatRoad];
		x = juncs[0]->offsets[thatRoad] + dir;
	}
	desSpeed = ds;
	approaching = false;
	isLeader = false;
	juncs.erase(juncs.begin());

	turns.erase(turns.begin());
	return;
}

float Vehicle::calcChSpeed(Obstacle o) {
	// Human driven and autnomous
	float dynGap = desDistance + std::max(0.0f, (speed*desTime + critical*pow(speed, 2) / maxDecceleration + (speed*(speed - o.speed)) / (2 * sqrt(maxAcceleration*maxDecceleration))));
	//CAV:
	//float dynGap = desDistance + std::max(0.0f, (speed*desTime + critical*pow(speed, 2) / maxDecceleration + (speed*(speed - o.speed - o.acc )) / (2 * sqrt(maxAcceleration*maxDecceleration))));
	float chSpeed = maxAcceleration*(1 - pow(speed / desSpeed, 4) - abs(dynGap / (o.dist))*abs(dynGap / o.dist));
	return chSpeed;
}