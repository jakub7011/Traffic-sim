#pragma once
#include <vector>
#include "Vehicle.h"
#include "Graphics.h"

class RoadSystem
{
	std::vector < Road* > roads;
	std::vector < Vehicle* > vehicles;
	Graphics* gfx;

public:
	static float constexpr biasRight = 0.075;
	static float constexpr laneThr = 0.05; 
	float travelTime = 0.0f;
	float totalTravelTime = 0.0f;
	int vehs = 0;
	RoadSystem(Graphics*);
	void addRoad(float startX, float startY, float endX, float endY, int lanes, int di);
	void addJunction(std::vector<float>, std::vector<std::vector<int>>, std::vector <Road*>);
	void addVehicle(Road* r, int lane, float x, int dir, std::vector<Junction*> j, std::vector<int> turns); 
	void drawRoads(); 
	void drawVehicles();
	void removeVehicle(Vehicle*);
	void updateVehicles();
	Vehicle* preceededVehicle(Vehicle*); //return vehicle behind anoter vehicle
	Vehicle* preceededVehicle(Road*, int lane, int dir, float pos);//return vehicle behind anoter vehicle by coordiantes
	bool switchLane(Vehicle*, int); //checks is switch lane possible
	Obstacle nextObstacle(Vehicle*); //returns next object in front
	bool isLeader(Vehicle*); //returns if next car to reach intersection
	bool isBlocked(Vehicle*); //returns if cannot cross the intersecion due to another car with right of way
	Obstacle nextObstaclePos(Road*, int, float, int); //next obstacle in front by coorinate
	float calcX(float startX, float startY, float endX, float endY, float offset); //calculcate x coordiante on a given road by length from start
	float calcY(float startX, float startY, float endX, float endY, float offset); //calculcate y coordiante on a given road by length from start
	float calcDist(float x1, float x2, float y1, float y2);  //calculcate distance between two point
	Road* getRoad(int index);
	Junction* getJunction(Road* r, int index);
	int returnJunctionIndex(Vehicle* vehicle, Junction* junction);
	float getFirstVehicleCoord(int lane, Road* r, int dir); //return distance to first vehicle at the edge of simulation
	float getFirstVehicleSpeed(int lane, Road* r, int dir);//return speed of first vehicle at the edge of simulation
};