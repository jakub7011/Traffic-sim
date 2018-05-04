#pragma comment(lib, "d2d1")
#pragma once
#include <Windows.h>
#include "RoadSystem.h"
#include <iostream>
#include <random>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <time.h>  

Graphics* graphics;
RoadSystem* rs;
std::default_random_engine gener;
std::uniform_int_distribution<int> distr(0, 10000);

//Windows process handler
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_DESTROY) {
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

//Windows main function
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmd, int nCmdShow) {
	WNDCLASSEX windowclass;
	ZeroMemory(&windowclass, sizeof(WNDCLASSEX));
	windowclass.cbSize = sizeof(WNDCLASSEX);
	windowclass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	windowclass.hInstance = hInstance;
	windowclass.lpfnWndProc = WindowProc;
	windowclass.lpszClassName = "MainWindow";
	windowclass.style = CS_HREDRAW | CS_VREDRAW;
	srand(time(NULL));

	RegisterClassEx(&windowclass);
	RECT rect = { 50,50,1500,1300 };
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);
	HWND windowhandle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, "MainWindow", "Simulator", WS_OVERLAPPEDWINDOW, 100, 100,
		rect.right-rect.left, rect.bottom-rect.top, NULL, NULL, hInstance, 0);

	if (!windowhandle)
	{
		return -1;
	}

	graphics = new Graphics();
	if (!graphics->Init(windowhandle)) {
		delete graphics;
		return -1;
	}

	ShowWindow(windowhandle, nCmdShow);

		//
		//
		//	SAMPLE SCENARIO: MOTORWAY
		//
		//
	
	
	
	rs = new RoadSystem(graphics);


	// adding roads 
	rs->addRoad(200.0f, 700.0f, 1200.0f, 700.0f, 3, 1);


	//draw road system
	rs->drawRoads();

	MSG message;
	message.message = WM_NULL;
	long long timer = -100; //simulation timer, allow 100 time steps to popuate the simualtion
	long long juncTimer = 0;
	while (message.message != WM_QUIT)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&message);
		}
		else
		{
			//
			// update the simulation
			//

			rs->updateVehicles();

			++timer;
			++juncTimer;

			std::vector<Junction*> juncs;


			int carAddFreq = 5;
			//add vehicles to each lane
			for (int x = 0; x < rs->getRoad(0)->lanes; ++x) {
				if (timer%carAddFreq == distr(gener) % carAddFreq) {
					int val = 30; //pick buffer distance to prevent cars bunching up at the edge


					//adding vehicles to road
					if (rs->getFirstVehicleCoord(0, rs->getRoad(x), 1) > val) {
						rs->addVehicle(rs->getRoad(0), x, 0.0f, 1, juncs, std::vector<int>({ 1 }));
					}
				}

				int simulationLength = 72000;
				if (timer >= simulationLength) {
					break;
				}
			}

			// draw vehicles
			// remove to increase speed of simulation
			rs->drawVehicles();
		}
	}

	

		//
		//
		//	SAMPLE SCENARIO 2: SIGNALISED INTERSECTION
		//
		//
	/*

	rs = new RoadSystem(graphics);


	// adding roads 
	rs->addRoad(200.0f, 700.0f, 1200.0f, 700.0f, 1, 1);
	rs->addRoad(700.0f, 200.0f, 700.0f, 1200.0f, 1, 2);


	// adding intersections
	std::vector<float> floats = {500.0f, 500.0f};
	//singalised intersection for e-w
	std::vector<std::vector<int>> prio = {
		{ 0, 0, 0, 0, 0, 0 , 0 ,0 ,0 ,0,0,0 },
		{ 0, 0, 0, 0, 0, 0 , 0 ,0 ,0 ,0,0,0 },
		{ 0, 0, 0, 0, 0, 0 , 0 ,0 ,0 ,0,0,0 },
		{ 0, 0, 0, 0, 0, 0 , 0 ,0 ,0 ,0,0,2 },
		{ 0, 0, 0, 0, 0, 0 , 0 ,0 ,0 ,0,0,2 },
		{ 0, 0, 0, 0, 0, 0 , 0 ,0 ,0 ,1,1,0 },
		{ 0, 0, 0, 0, 0, 0 , 0 ,0 ,0 ,0,0,0 },
		{ 0, 0, 0, 0, 0, 0 , 0 ,0 ,0 ,0,0,0 },
		{ 0, 0, 0, 0, 0, 0 , 0 ,0 ,0 ,0,0,0 },
		{ 0, 0, 0, 0, 0, 2 , 0 ,0 ,0 ,0,0,0 },
		{ 0, 0, 0, 0, 0, 2 , 0 ,0 ,0 ,0,0,0 },
		{ 0, 0, 0, 1, 1, 0 , 0 ,0 ,0 ,0,0,0 }
	};
	//singalised intersection for n-s
	std::vector<std::vector<int>> prio2 = {
		{ 0, 0, 0, 0, 0, 0, 0, 0, 2, 0 ,0 ,0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 2, 0 ,0 ,0 },
		{ 0, 0, 0, 0, 0, 0, 1, 1, 0, 0 ,0 ,0 },
		{ 0, 0, 0, 0, 0, 0, 0 ,0 ,0 ,0 ,0 ,0 },
		{ 0, 0, 0, 0, 0, 0, 0 ,0 ,0 ,0 ,0 ,0 },
		{ 0, 0, 0, 0, 0, 0, 0 ,0 ,0 ,0 ,0 ,0 },
		{ 0, 0, 2, 0, 0, 0, 0 ,0 ,0 ,0 ,0 ,0 },
		{ 0, 0, 2, 0, 0, 0, 0 ,0 ,0 ,0 ,0 ,0 },
		{ 1, 1, 0, 0, 0, 0, 0 ,0 ,0 ,0 ,0 ,0 },
		{ 0, 0, 0, 0, 0, 0, 0 ,0 ,0 ,0 ,0 ,0 },
		{ 0, 0, 0, 0, 0, 0, 0 ,0 ,0 ,0 ,0 ,0 },
		{ 0, 0, 0, 0, 0, 0, 0 ,0 ,0 ,0 ,0 ,0 }
	};
	std::vector<Road*> rds = { rs->getRoad(0), rs->getRoad(1) };
	rs->addJunction(floats, prio, rds);
	rs->getJunction(rs->getRoad(0), 0)->status = 1;


	//draw road system
	rs->drawRoads();

	MSG message;
	message.message = WM_NULL;
	long long timer = -100; //simulation timer, allow 100 time steps to popuate the simualtion
	long long juncTimer = 0;
	while (message.message != WM_QUIT)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&message);
		}
		else
		{
			//
			// update the simulation
			//

			rs->updateVehicles();

			++timer;
			++juncTimer;

			std::vector<Junction*> juncs = { rs->getRoad(0)->junctions[0] };

			
			int carAddFreq = 25;
			if (timer%carAddFreq == distr(gener)% carAddFreq) {
				int dir = (distr(gener) % 2)*2 - 1; //pick 1 or -1 for direction

				int rand = distr(gener) % 10;
				int turn = rand < 8 ? 1 : rand == 8 ? 0 : 2; //pick a direction of turning, 80% straight, 10% for left and right each
				int val = 30; //pick buffer distance to prevent cars bunching up at the edge


				//adding vehicles to road e-w
				if (rs->getFirstVehicleCoord(0, rs->getRoad(0), dir) > val) {
					rs->addVehicle(rs->getRoad(0), 0, dir == 1 ? 0.0f : 1000.0f, dir, juncs, std::vector<int>({ turn , 2 }));
				}

				//repeat for road n-s
				dir = (distr(gener) % 2) * 2 - 1;
				int rand = distr(gener) % 10;
				int turn = rand < 8 ? 1 : rand == 8 ? 0 : 2; //pick a direction of turning, 80% straight, 10% for left and right each
				//rs->addVehicle(rs->getRoad(rand()%2), 0, (dir == 1 ? 0.0f : 600.0f), dir, juncs, std::vector<int>({ rand()%3 , 2 }));
				if (rs->getFirstVehicleCoord(0, rs->getRoad(1), dir) > val) {
					rs->addVehicle(rs->getRoad(1), 0, dir == 1 ? 0.0f : 1000.0f, dir, juncs, std::vector<int>({ turn , 2 }));
				}
			}

			int simulationLength = 72000;
			if (timer >= simulationLength) {
				break;
			}

			//
			// signalisation control
			//

			if (juncTimer >= 20) {
				rs->getRoad(0)->junctions[0]->status = 2;
			}

			if (juncTimer >= 120) {
				rs->getRoad(0)->junctions[0]->status = 1;

				rs->getJunction(rs->getRoad(0), 0)->priority = prio2;
			}
			if (juncTimer >= 140) {
				rs->getRoad(0)->junctions[0]->status = 3;
			}
			if (juncTimer >= 240) {
				rs->getRoad(0)->junctions[0]->status = 1;
				rs->getJunction(rs->getRoad(0), 0)->priority = prio;
				juncTimer = 0;
			}

			// draw vehicles
			// remove to increase speed of simulation
			rs->drawVehicles();
		}
		*/


		//
		//
		//	SAMPLE SCENARIO 3: UNSIGNALISED INTERSECTION
		//
		//

	/*
	rs = new RoadSystem(graphics);	

		// adding roads 
	rs->addRoad(200.0f, 700.0f, 1200.0f, 700.0f, 1, 1);
	rs->addRoad(700.0f, 200.0f, 700.0f, 1200.0f, 1, 2);


	// adding intersections
	std::vector<float> floats = { 500.0f, 500.0f };
	//e-w priority intersection matrix
	std::vector<std::vector<int>> prio = {
	{ 0, 0, 0, 0, 1, 0 , 0 ,2 ,2 ,0,0,0 },
	{ 0, 0, 0, 0, 1, 1 , 0 ,2 ,2 ,1,1,1 },
	{ 0, 0, 0, 0, 1, 1 , 1 ,0 ,0 ,0,1,1 },
	{ 0, 0, 0, 0, 0, 0 , 2 ,0 ,0 ,0,0,2 },
	{ 2, 2, 2, 0, 0, 0 , 2 ,2 ,2 ,0,0,2 },
	{ 0, 2, 2, 0, 0, 0 , 2 ,2 ,2 ,1,1,0 },
	{ 0, 0, 2, 0, 0, 0 , 0 ,0 ,0 ,0,1,0 },
	{ 0, 0, 2, 1, 1, 1 , 0 ,0 ,0 ,0,1,1 },
	{ 1, 1, 0, 0, 1, 1 , 0 ,0 ,0 ,0,1,1 },
	{ 0, 2, 0, 0, 0, 2 , 0 ,0 ,0 ,0,0,0 },
	{ 0, 2, 2, 0, 0, 2 , 2 ,2 ,2 ,0,0,0 },
	{ 0, 2, 2, 1, 1, 0 , 0 ,2 ,2 ,0,0,0 }
	};

	std::vector<Road*> rds = { rs->getRoad(0), rs->getRoad(1) };
	rs->addJunction(floats, prio, rds);
	rs->getJunction(rs->getRoad(0), 0)->status = 0;


	//draw road system
	rs->drawRoads();

	MSG message;
	message.message = WM_NULL;
	long long timer = -100; //simulation timer, allow 100 time steps to popuate the simualtion
	long long juncTimer = 0;
	while (message.message != WM_QUIT)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&message);
		}
		else
		{
			//
			// update the simulation
			//

			rs->updateVehicles();

			++timer;
			++juncTimer;

			std::vector<Junction*> juncs = { rs->getRoad(0)->junctions[0] };

			int carAddFreq = 25;
			if (timer%carAddFreq == distr(gener) % carAddFreq) {
				int dir = (distr(gener) % 2) * 2 - 1; //pick 1 or -1 for direction

				int rand = distr(gener) % 10;
				int turn = rand < 8 ? 1 : rand == 8 ? 0 : 2; //pick a direction of turning, 80% straight, 10% for left and right each
				int val = 30; //pick buffer distance to prevent cars bunching up at the edge


							  //adding vehicles to road e-w
				if (rs->getFirstVehicleCoord(0, rs->getRoad(0), dir) > val) {
					rs->addVehicle(rs->getRoad(0), 0, dir == 1 ? 0.0f : 1000.0f, dir, juncs, std::vector<int>({ turn , 2 }));
				}

				//repeat for road n-s
				dir = (distr(gener) % 2) * 2 - 1;
				rand = distr(gener) % 5;
				turn = rand < 3 ? 1 : rand == 8 ? 0 : 2;
				//rs->addVehicle(rs->getRoad(rand()%2), 0, (dir == 1 ? 0.0f : 600.0f), dir, juncs, std::vector<int>({ rand()%3 , 2 }));
				if (rs->getFirstVehicleCoord(0, rs->getRoad(1), dir) > val) {
					rs->addVehicle(rs->getRoad(1), 0, dir == 1 ? 0.0f : 1000.0f, dir, juncs, std::vector<int>({ turn , 2 }));
				}
			}

			int simulationLength = 72000;
			if (timer >= simulationLength) {
				break;
			}

			// draw vehicles
			// remove to increase speed of simulation
			rs->drawVehicles();

		}
	}
	*/

	char buffer[16];
	snprintf(buffer, sizeof(buffer), "%g", rs->totalTravelTime);
	OutputDebugString("\nTotal travel time: "); OutputDebugString(buffer);
	snprintf(buffer, sizeof(buffer), "%g", (float)rs->vehs);
	OutputDebugString("\nTotal flow: "); OutputDebugString(buffer);
	snprintf(buffer, sizeof(buffer), "%g", (float)rs->travelTime);
	OutputDebugString("\ntTime elapsed: "); OutputDebugString(buffer);
	delete graphics;

	return 0;
}