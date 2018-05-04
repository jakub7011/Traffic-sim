#pragma once
#include "RoadSystem.h"
#include <vector>
#include <iostream>
#include <cmath>
#define SQ(x) ((x)*(x))

RoadSystem::RoadSystem(Graphics* g) : gfx(g) {}

void RoadSystem::addRoad(float startX, float startY, float endX, float endY, int lanes, int di)
{
	Road* road = new Road(startX, startY, endX, endY, lanes, di);
	roads.push_back(road);
}

void RoadSystem::addJunction(std::vector<float> d, std::vector<std::vector<int>> x, std::vector <Road*> r)
{
	Junction* junc = new Junction(d, x, r);
	for (auto rd : r) {
		rd->junctions.push_back(junc);
	}
}

void RoadSystem::drawRoads()
{

	gfx->BeginDraw();
	gfx->ClearScreen(0.9f, 0.9f, 0.9f);
	for (auto rd : roads)
	{
		float dist = calcDist(rd->startX, rd->startY, rd->endX, rd->endY);
		for (float it = 0.8; it < rd->lanes; ++it) {

			gfx->DrawLine(rd->startX + 11.0*((float)it)*(rd->endY - rd->startY) / dist,
				rd->startY + 11.0*((float)it)*(rd->endX - rd->startX) / dist,
				rd->endX + 11.0*((float)it)*(rd->endY - rd->startY) / dist,
				rd->endY + 11.0*((float)it)*(rd->endX - rd->startX) / dist,
				0.2f, 0.2f, 0.2f);
			gfx->DrawLine(rd->startX - 11.0*((float)it)*(rd->endY - rd->startY) / dist,
				rd->startY - 11.0*((float)it)*(rd->endX - rd->startX) / dist,
				rd->endX - 11.0*((float)it)*(rd->endY - rd->startY) / dist,
				rd->endY - 11.0*((float)it)*(rd->endX - rd->startX) / dist,
				0.2f, 0.2f, 0.2f);
		}
		for (auto jt : rd->junctions)
		{
			if (jt->roads[0] == rd) {
				if (jt->status==1 || jt->status==3) {
					gfx->DrawCircle(calcX(rd->startX, rd->startY, rd->endX, rd->endY, jt->offsets[0]), calcY(rd->startX, rd->startY, rd->endX, rd->endY, jt->offsets[0]), 8.0f, 1.0f, 0.1f, 0.1f);
				}
				else if (jt->status == 0 || jt->status == 2) {
					gfx->DrawCircle(calcX(rd->startX, rd->startY, rd->endX, rd->endY, jt->offsets[0]), calcY(rd->startX, rd->startY, rd->endX, rd->endY, jt->offsets[0]), 8.0f, 0.1f, 1.0f, 0.1f);

				}
				else {
					gfx->DrawCircle(calcX(rd->startX, rd->startY, rd->endX, rd->endY, jt->offsets[0]), calcY(rd->startX, rd->startY, rd->endX, rd->endY, jt->offsets[0]), 8.0f, 1.0f, 1.0f, 0.1f);

				}
			}
		}
	}

	gfx->EndDraw();
}

Road* RoadSystem::getRoad(int index)
{
	if (index >= roads.size())
	{
		return NULL;
	}
	return roads[index];
}

void RoadSystem::addVehicle(Road* road, int lane, float x, int dir, std::vector<Junction *> j, std::vector<int> turns)
{
	Vehicle* v = new Vehicle (x, road, lane, dir, j, turns, getFirstVehicleSpeed(lane, road, dir));
	v->travelTime = travelTime;
	vehicles.push_back(v);
}

void RoadSystem::removeVehicle(Vehicle* v) {
	totalTravelTime += (travelTime-v->travelTime);
	vehs++;
	for (int x = 0; x < vehicles.size(); x++) {
		if (vehicles[x] == v) {
			vehicles.erase(vehicles.begin() + x);
		}
	}
}
void RoadSystem::drawVehicles()
{
	drawRoads();
	gfx->BeginDraw();
	for (auto v : vehicles)
	{
		Road * rd = v->road;
		float dist = calcDist(rd->startX, rd->startY, rd->endX, rd->endY);
		float x = calcX(v->road->startX, v->road->startY, v->road->endX, v->road->endY, v->x) - v->dir*11.0*((float)v->lane + 0.8)*(rd->endY - rd->startY) / dist;
		float y = calcY(v->road->startX, v->road->startY, v->road->endX, v->road->endY, v->x) + v->dir*11.0*((float)v->lane + 0.8)*(rd->endX - rd->startX) / dist;
		gfx->DrawCircle(x, y, 5.0f, 1.0f, 0.0f, 0.0f);
	}
	gfx->EndDraw();
}

void RoadSystem::updateVehicles()
{
	travelTime += 1;
	for (auto v : vehicles) {
		//MOBIL handling
		if (switchLane(v, -1)) {
			v->switchLane(-1);
		}
		else if (switchLane(v, 1)) {
			v->switchLane(1);
		}
		//vehicle removal from simulation
		if ((v->x > 1000.0 && v->dir == 1) || (v->x < 0.0 && v->dir == -1)) {
			removeVehicle(v);
		}

		v->isLeader = isLeader(v);

		//inersectino handling
		if (v->juncs.size() > 0 && v->approaching && isBlocked(v)) {

			int index;
			if (v->juncs[0]->roads[0] == v->road) {
				index = 0;
			}
			else {
				index = 1;
			}
			v->update(Obstacle(v->dir*(v->juncs[0]->offsets[index]-v->x)-5.0f, 0.0f, 0.0));

		}
		else {
			v->update(nextObstacle(v));
		}
	}
	return;
}

Vehicle* RoadSystem::preceededVehicle(Vehicle* v) {
	return preceededVehicle(v->road, v->lane, v->dir, v->x);
}
Vehicle* RoadSystem::preceededVehicle(Road* r, int lane, int dir, float pos) {
	Vehicle* preceededV = NULL;
	for (auto v : vehicles) {
		if (v->dir == dir && v->road == r && v->lane == lane && dir*(pos-v->x) >0 && dir*(pos - v->x) < 1000.0f && (preceededV == NULL || dir*( v->x- preceededV->x)>0 )) {
			preceededV = v;
		}
	}
	return preceededV;

}

Obstacle RoadSystem::nextObstacle(Vehicle* vehicle) {
	return nextObstaclePos(vehicle->road, vehicle->lane, vehicle->x, vehicle->dir);
}

//impleentation of MOBIL
bool RoadSystem::switchLane(Vehicle* vehicle, int move) {
	//-1 is left, 1 is right
	if (vehicle->lane+move < 0 || vehicle->lane + move >= vehicle->road->lanes) {
		return false;
	}
	bool safety;
	Vehicle* preceededVCh = preceededVehicle(vehicle->road, vehicle->lane + move, vehicle->dir, vehicle->x);
	float accBCh;
	float accChBCh;
	Obstacle frontMy = nextObstaclePos(vehicle->road, vehicle->lane, vehicle->x, vehicle->dir);
	Obstacle frontMyCh = nextObstaclePos(vehicle->road, vehicle->lane + move, vehicle->x, vehicle->dir);
	if (preceededVCh == NULL) {
		safety = true;
		accBCh = 0.0;
		accChBCh = 0.0;
	}
	else {
		Obstacle eq = nextObstacle(preceededVCh);
		accChBCh = preceededVCh->calcChSpeed(Obstacle(eq.dist < vehicle->dir*(vehicle->x - preceededVCh->x) - vehicle->length ? eq.dist : vehicle->dir*(vehicle->x - preceededVCh->x) - vehicle->length
			, eq.dist < vehicle->dir*(vehicle->x - preceededVCh->x) - vehicle->length ? 0.0f : vehicle->speed,0.0));
		eq = nextObstacle(preceededVCh);
		safety = (accChBCh > -vehicle->safetyBreak) &&
			(abs(vehicle->x - preceededVCh->x) > vehicle->length + vehicle->desDistance) &&
			frontMyCh.dist > vehicle->length + vehicle->desDistance &&
			abs(preceededVCh->x+vehicle->dir*eq.dist - vehicle->x) > vehicle->length + vehicle->desDistance;

		accBCh = preceededVCh->calcChSpeed(nextObstacle(preceededVCh));
	}

	float accChMyCh = vehicle->calcChSpeed(frontMyCh);

	float accMy = vehicle->calcChSpeed(frontMy);
	
	Vehicle* preceededV = preceededVehicle(vehicle);
	float accB;
	float accChB;
	if (preceededV == NULL) {
		accB = 0.0;
		accChB = 0.0;
	}
	else {
		Obstacle eq1 = nextObstacle(preceededV);
		accB = preceededV->calcChSpeed(nextObstacle(preceededV));

		accChB = preceededV->calcChSpeed(Obstacle(((eq1.dist < frontMy.dist + vehicle->dir*(vehicle->x - preceededV->x)) ?
			eq1.dist : (frontMy.dist + vehicle->dir*(vehicle->x - preceededV->x))),
			(eq1.dist < vehicle->dir*(vehicle->x - preceededV->x) - vehicle->length) ? 0.0f : frontMy.speed, 0.0));
	}

	float benefit = accChMyCh - accMy;
	float cost = vehicle->politeness*(accB + accBCh - accChB - accChBCh) + RoadSystem::laneThr ;
	if (move == 1) {
		benefit += RoadSystem::biasRight;
	}
	if (cost < benefit && safety) {
		return true;
	}
	return false;
}

inline float RoadSystem::calcX(float startX, float startY, float endX, float endY, float offset)
{
	return startX + (endX - startX)*offset / sqrt((endX - startX)*(endX - startX) + (endY - startY)*(endY - startY));
}

inline float RoadSystem::calcY(float startX, float startY, float endX, float endY, float offset)
{
	return startY + (endY - startY)*offset / sqrt((endX - startX)*(endX - startX) + (endY - startY)*(endY - startY));
}

inline float RoadSystem::calcDist(float x1, float x2, float y1, float y2) {
	return std::sqrt(SQ(y2 - y1) + SQ(x2 - x1));
}

Obstacle RoadSystem::nextObstaclePos(Road* r, int lane, float offset, int dir) {
	float minOffset = dir*100000.0;
	for (auto j : r->junctions) {
		if (dir == 1) {
			for (int it = 0; it < j->roads.size(); it++) {
				if ((j->status == 1 || (j->status == 2 && r->dir == 2) || (j->status == 3 && r->dir == 1))
					&& j->roads[it] == r && j->offsets[it] >= offset && ( j->offsets[it] <= minOffset || abs(minOffset) == 100000.0)) {
					minOffset = j->offsets[it];
				}
			}
		}
		else {
			for (int it = 0; it < j->roads.size(); it++) {
				if ((j->status == 1 || (j->status == 2 && r->dir == 2) || (j->status == 3 && r->dir == 1))
					&& j->roads[it] == r && j->offsets[it] <= offset &&  (j->offsets[it] >= minOffset || abs(minOffset) == 100000.0)) {
					minOffset = j->offsets[it];
				}
			}
		}
	}
	Obstacle out = Obstacle(dir*(minOffset - offset), 0, 0.0);
	for (auto v : vehicles) {
		if (dir == 1) {
			if (v->road == r && v->lane == lane && v->dir == 1 && v->x != offset) {
				if (v->x > offset && (v->x < minOffset || minOffset == 100000.0)) {
					minOffset = v->x;
					out.dist = minOffset - offset;
					out.speed = v->speed;
					out.acc = v->acc;
				}
			}
		}
		else {
			if (v->road == r && v->lane == lane && v->dir == -1 && v->x != offset) {
				if (v->x<offset &&( v->x > minOffset || minOffset == -100000.0)) {
					minOffset = v->x;
					out.dist =offset - minOffset;
					out.speed = v->speed;
					out.acc = v->acc;
				}
			}
		}
		
	}

	if ( abs(minOffset) > 1000.0) {
		out.dist = 1000.0;
	}
	return out;
}

//return true iff the next obsacle is a junction without a red light
bool RoadSystem::isLeader(Vehicle* vehicle) {
	float minOffset = vehicle->dir *100000.0;
	bool isLeader = false;
	for (auto j : vehicle->road->junctions) {
		if (vehicle->dir == 1) {
			for (int it = 0; it < j->roads.size(); it++) {
				if (j->roads[it] == vehicle->road && j->offsets[it] >= vehicle->x && (j->offsets[it] <= minOffset || abs(minOffset) == 100000.0)) {
					minOffset = j->offsets[it];
					isLeader = true;
				}
			}
		}
		else {
			for (int it = 0; it < j->roads.size(); it++) {
				if (j->roads[it] == vehicle->road && j->offsets[it] <= vehicle->x && (j->offsets[it] >= minOffset || abs(minOffset) == 100000.0)) {
					minOffset = j->offsets[it];
					isLeader = true;
				}
			}
		}
	}
	for (auto v : vehicles) {
		if (vehicle->dir == 1) {
			if (v->road == vehicle->road && v->lane == vehicle->lane && v->dir == 1 && v->x != vehicle->x) {
				if (v->x > vehicle->x && (v->x < minOffset || minOffset == 100000.0)) {
					return false;
				}
			}
		}
		else {
			if (v->road == vehicle->road && v->lane == vehicle->lane && v->dir == -1 && v->x != vehicle->x) {
				if (v->x < vehicle->x && (v->x > minOffset || minOffset == -100000.0)) {
					return false;
				}
			}
		}
	}
	return isLeader;
}

Junction* RoadSystem::getJunction(Road* r, int index) {
	return r->junctions[index];
}

//return true iff the junction the vehicle is approaching is uncrossable due to vehicle with right of way
bool RoadSystem::isBlocked(Vehicle* vehicle) {
	int thisIndex = returnJunctionIndex(vehicle, vehicle->juncs[0]);
	for (auto v : vehicles) {
		if (v->approaching) {
			int thatIndex = returnJunctionIndex(v, v->juncs[0]);
			if (v->juncs.size() > 0 && v->juncs[0] == vehicle->juncs[0] && v->juncs[0]->priority[thisIndex][thatIndex] == 1) {
				return true;
			}
		}
	}
	return false;
}

int RoadSystem::returnJunctionIndex(Vehicle* vehicle, Junction* junction) {
	int index = -1;
	int currDir;
	if (vehicle->road->dir == 1) { //goes east-west
		if (vehicle->dir == 1) {
			currDir = 9;
		}
		else {
			currDir = 3;
		}
	}
	else {//goes north-south
		if (vehicle->dir == 1) {
			currDir = 0;
		}
		else {
			currDir = 6;
		}
	}
	int turn = vehicle->turns[0] == 0 ? 2 : vehicle->turns[0] == 1 ? 1 : 0;
	return currDir+turn;
}

float RoadSystem::getFirstVehicleCoord(int lane, Road* r, int dir) {
	if (dir == 1) {
		float min = 1000;
		for (auto v : vehicles) {
			if (v->x < min && v->lane == lane && v->road == r && v->dir == dir) {
				min = v->x;
			}
		}
		return min;
	}
	float max = -1;
	for (auto v : vehicles) {
		if (v->x > max && v->lane == lane && v->road == r && v->dir == dir) {
			max = v->x;
		}
	}
	return max == -1 ? 1000 : max;
}

float RoadSystem::getFirstVehicleSpeed(int lane, Road* r, int dir) {
	float speed = 10.0f;
	if (dir == 1) {
		float min = 1000;
		for (auto v : vehicles) {
			if (v->x < min && v->lane == lane && v->road == r && v->dir == dir) {
				min = v->x;
				speed = v->speed;
			}
		}
		return speed;
	}
	float max = 0;
	for (auto v : vehicles) {
		if (v->x > max && v->lane == lane && v->road == r && v->dir == dir) {
			max = v->x;
			speed = v->speed;
		}
	}
	return speed;
}