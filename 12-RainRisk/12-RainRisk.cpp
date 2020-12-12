// 12-RainRisk.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

struct NavAction
{
	NavAction(char direction, int magnitude)
		: m_direction(direction)
	, m_magnitude(magnitude)
	{ }

	char m_direction;
	int m_magnitude;
};

// Rotate a point around origin by the given amount of degrees
void RotatePointAroundOrigin(int& x, int& y, float degrees)
{
	float rads = degrees * (float)M_PI / 180.0f;
	float cs = cos(rads);
	float sn = sin(rads);

	float newX = ((x * cs) - (y * sn));
	float newY = ((x * sn) + (y * cs));

	x = (int)round(newX);
	y = (int)round(newY);
}

int main()
{
	vector<NavAction> instructions;
	ifstream inputStream("input.txt");
	//ifstream inputStream("test.txt");
	if (inputStream.is_open())
	{
		string line;
		while (getline(inputStream, line))
		{
			instructions.push_back(NavAction(line[0], stoi(line.substr(1))));
		}
	}

	{
		// Part One
		cout << "=== Part 1 ===" << endl;
		int longitudeDist = 0; // North/South movement
		int latitudeDist = 0; // East/West movement

		// Ship starts facing east
		int currentHeading = 90;

		for (NavAction action : instructions)
		{
			switch (action.m_direction)
			{
				case 'N':
				{
					longitudeDist += action.m_magnitude;
				}
				break;
				case 'S':
				{
					longitudeDist -= action.m_magnitude;
				}
				break;
				case 'E':
				{
					latitudeDist += action.m_magnitude;
				}
				break;
				case 'W':
				{
					latitudeDist -= action.m_magnitude;
				}
				break;
				case 'L':
				{
					// Turn left by given amount
					currentHeading -= action.m_magnitude;
				}
				break;
				case 'R':
				{
					// Turn right by given amount
					currentHeading += action.m_magnitude;
				}
				break;
				case 'F':
				{
					if (currentHeading == 0)
						longitudeDist += action.m_magnitude;
					else if (currentHeading == 180)
						longitudeDist -= action.m_magnitude;
					else if (currentHeading == 90)
						latitudeDist += action.m_magnitude;
					else if (currentHeading == 270)
						latitudeDist -= action.m_magnitude;
				}
				break;
			}

			while (currentHeading >= 360)
				currentHeading -= 360;

			while (currentHeading < 0)
				currentHeading += 360;
		}

		int longDistManhattan = abs(longitudeDist);
		int latDistManhattan = abs(latitudeDist);
		cout << "Manhattan distance N/S: " << longDistManhattan << endl;
		cout << "Manhattan distance E/W: " << latDistManhattan << endl;
		cout << "Total Manhattan distance: " << (longDistManhattan + latDistManhattan) << endl << endl;
	}

	{
		// Part Two
		cout << "=== Part 2 ===" << endl;

		// Waypoint starts 10 units east, 1 unit north of ship
		int waypointX = 10;
		int waypointY = 1;

		int longitudeDist = 0; // North/South movement
		int latitudeDist = 0; // East/West movement

		for (NavAction action : instructions)
		{
			switch (action.m_direction)
			{
				case 'N':
				{
					waypointY += action.m_magnitude;
				}
				break;
				case 'S':
				{
					waypointY -= action.m_magnitude;
				}
				break;
				case 'E':
				{
					waypointX += action.m_magnitude;
				}
				break;
				case 'W':
				{
					waypointX -= action.m_magnitude;
				}
				break;
				case 'L':
				{
					// Rotate waypoint around origin by given degrees
					RotatePointAroundOrigin(waypointX, waypointY, (float)action.m_magnitude);
				}
				break;
				case 'R':
				{
					// Rotate waypoint around origin by given degrees
					RotatePointAroundOrigin(waypointX, waypointY, (float)-action.m_magnitude);
				}
				break;
				case 'F':
				{
					// Move in direction of waypoint by factor of magnitude
					longitudeDist += (waypointY * action.m_magnitude);
					latitudeDist += (waypointX * action.m_magnitude);
				}
				break;
			}
		}

		int longDistManhattan = abs(longitudeDist);
		int latDistManhattan = abs(latitudeDist);
		cout << "Manhattan distance N/S: " << longDistManhattan << endl;
		cout << "Manhattan distance E/W: " << latDistManhattan << endl;
		cout << "Total Manhattan distance: " << (longDistManhattan + latDistManhattan) << endl << endl;
	}
}
