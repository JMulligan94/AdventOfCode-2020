// 13-ShuttleSearch.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <stdint.h>

using namespace std;

const char* c_inputFile = "Input.txt";
const char* c_testFile = "Test.txt";

struct Schedule
{
	Schedule(string line)
	{
		m_scheduleStr = line;

		string busIntervalsStr = line;

		int delimiterIndex = (int)busIntervalsStr.find(',');
		string currentInterval;
		while (delimiterIndex != -1)
		{
			currentInterval = busIntervalsStr.substr(0, delimiterIndex);
			if (currentInterval[0] == 'x')
				m_buses.push_back(-1);
			else
				m_buses.push_back(stoi(currentInterval));
			busIntervalsStr = busIntervalsStr.substr(delimiterIndex + 1);
			delimiterIndex = (int)busIntervalsStr.find(',');
		}

		if (busIntervalsStr[0] == 'x')
			m_buses.push_back(-1);
		else
			m_buses.push_back(stoi(busIntervalsStr));
	}

	string m_scheduleStr;
	vector<int> m_buses;
};

uint64_t GetConsecutiveBusTime(const vector<int>& schedule)
{
	uint64_t currentTime = 0; // Current time we're checking
	uint64_t runStartTime = 0; // Time the current run started
	uint64_t skipFactor = schedule.front(); // How much time we should skip to check the next run

	int currentIndex = 0; // How many buses have we found consecutively on this run?
	int highestConsecutiveIndexFound = 0; // How many buses have we got in our chain so far (i.e. how many are currently contributing to the skip factor
	
	// Iterate until we've found all buses in the schedule consecutively
	while (currentIndex != schedule.size())
	{
		// Get the bus ID we're currently searching for
		int busID = schedule[currentIndex];
		if (busID == -1) 
		{
			// i.e. if busID is 'x', it's an automatic pass, and onto the next time
			currentIndex++;
			currentTime++;
			continue;
		}

		// If currentIndex is 0, we must be on the start of a new run
		if (currentIndex == 0)
			runStartTime = currentTime;

		// Does this bus arrive at this current time?
		bool busArrives = (currentTime % busID) == 0;
		if (busArrives)
		{
			// Is this the furthest along the chain we've ever been?
			if (currentIndex > highestConsecutiveIndexFound)
			{
				cout << "Discovered bus index " << currentIndex << " (" << busID << ") at time: " << currentTime << endl;

				// Now have a new highest index
				highestConsecutiveIndexFound = currentIndex;

				// This bus ID should contribute to the skipFactor from now on
				// There's no way this pattern of bus IDs can happen again, 
				// until another * busID times since we're assuming we're dealing with
				// prime numbers
				skipFactor *= busID;
				
				cout << "Skip factor now = " << skipFactor << endl;
			}

			// Onto the next bus to check if the chain continues!
			currentIndex++;
			currentTime++;
			continue;
		}

		// Bus did not arrive! The chain is broken!
		// Reset and check from index 0 again
		currentIndex = 0;
		currentTime = runStartTime + skipFactor;
		continue;
	}

	return runStartTime;
}

int main()
{
	// Get information from input file
	int arrivalTime = 0;
	vector<Schedule> busSchedules;
	ifstream inputStream(c_inputFile);
	if (inputStream.is_open())
	{
		string line;
		getline(inputStream, line);
		arrivalTime = stoi(line);

		while (getline(inputStream, line))
		{
			busSchedules.push_back(Schedule(line));
		}
	}

	{
		// Part One
		cout << "=== Part 1 ===" << endl;

		for (Schedule schedule : busSchedules)
		{
			// Determine shortest wait time and therefore bus ID
			// from mod of arrival time with the bus ID (i.e. how often the bus arrives)
			int shortestWaitTime = INT_MAX;
			int shortestWaitBusID = INT_MAX;
			for (int busID : schedule.m_buses)
			{
				// i.e. bus ID is 'x'
				if (busID == -1)
					continue;

				int waitTime = busID - (arrivalTime % busID);
				// Is this the shortest wait time so far?
				if (waitTime < shortestWaitTime)
				{
					shortestWaitTime = waitTime;
					shortestWaitBusID = busID;
				}
			}

			cout << "For bus schedule: " << schedule.m_scheduleStr << endl;
			cout << "The first bus to arrive is: " << shortestWaitBusID << endl;
			cout << "The time to wait is: " << shortestWaitTime << endl;
			cout << "The answer is: " << (shortestWaitBusID * shortestWaitTime) << endl << endl;
		}
	}

	{
		// Part Two
		cout << "=== Part 2 ===" << endl;

		for (int i = 0; i < busSchedules.size(); ++i)
		{
			uint64_t time = GetConsecutiveBusTime(busSchedules[i].m_buses);
			cout << endl << "For bus schedule: " << busSchedules[i].m_scheduleStr << endl;
			cout << "Earliest timestamp: " <<time << endl << endl;
		}
	}
}
