#pragma once
#include <vector>

struct Road;

struct Junction
{
	std::vector<float> offsets;
	std::vector<std::vector<int>> priority;
	std::vector <Road*> roads;
	int status; //0 - priority e-w, 1 - red light, 2 - bi-derectional lights e-w green, 3, bi-dir n-s green
	Junction(std::vector<float> d, std::vector<std::vector<int>> x, std::vector <Road*> r) : offsets(d), priority(x), roads(r) {
		status = 0;		
	}
};

struct Road
{
	float startX, startY, endX, endY;
	int lanes;
	int dir; //1 is east-west, 2 is north-south
	std::vector <Junction*> junctions;
	Road(float a, float b, float c, float d, int l, int di) : startX(a), startY(b), endX(c), endY(d), lanes(l), dir(di) {}
	inline float length() {
		return sqrt((endX-startX)*(endX - startX) + (endY - startY)*(endY - startY));
	}
};