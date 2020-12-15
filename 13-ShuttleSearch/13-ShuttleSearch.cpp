// 13-ShuttleSearch.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

const char* c_inputFile = "Input.txt";
const char* c_testFile = "Test.txt";

struct MinuteSchedule
{
	vector<int> m_buses;
};

void ParseLineIntoSchedule(vector<int>& schedule, const string& line)
{
	string busIntervalsStr = line;

	int delimiterIndex = (int)busIntervalsStr.find(',');
	string currentInterval;
	while (delimiterIndex != -1)
	{
		currentInterval = busIntervalsStr.substr(0, delimiterIndex);
		if (currentInterval[0] == 'x')
			schedule.push_back(-1);
		else
			schedule.push_back(stoi(currentInterval));
		busIntervalsStr = busIntervalsStr.substr(delimiterIndex + 1);
		delimiterIndex = (int)busIntervalsStr.find(',');
	}

	if (busIntervalsStr[0] == 'x')
		schedule.push_back(-1);
	else
		schedule.push_back(stoi(busIntervalsStr));
}

long long GetConsecutiveBusTime(const vector<int>& schedule)
{
	vector<pair<long long, int>> consecutiveBuses;
	long long currentTime = 0;
	int currentIndex = 0;
	while (consecutiveBuses.size() != schedule.size())
	{
		int busID = schedule[currentIndex];
		long long mod = currentTime % busID;
		if (mod == 0 || busID == -1)
		{
			consecutiveBuses.push_back(pair<long long, int>(currentTime, busID));
			currentIndex++;
			currentTime++;
			continue;
		}

		if (!consecutiveBuses.empty())
		{
			int highestValidId = INT32_MIN;
			for (int j = 0; j < consecutiveBuses.size(); ++j)
			{
				highestValidId = max(highestValidId, consecutiveBuses[j].second);
			}
			currentTime = consecutiveBuses.front().first + highestValidId;
		}
		else
			currentTime++;
		
		currentIndex = 0;
		consecutiveBuses.clear();
	}

	return consecutiveBuses[0].first;
}

int main()
{
	{
		// Part One
		cout << "=== Part 1 ===" << endl;

		int arrivalTime = 0;
		vector<int> busIntervals;
		ifstream inputStream(c_inputFile);
		if (inputStream.is_open())
		{
			string line;

			getline(inputStream, line);
			arrivalTime = stoi(line);

			getline(inputStream, line);
			string busIntervalsStr = line;

			ParseLineIntoSchedule(busIntervals, line);
		}

		int longestInterval = *max_element(busIntervals.begin(), busIntervals.end());

		vector<MinuteSchedule> busSchedule(longestInterval);
		for (int interval : busIntervals)
		{
			if (interval == -1)
				continue;

			int mod = interval - (arrivalTime % interval);
			busSchedule[mod].m_buses.push_back(interval);
		}

		// Get first bus to arrive after the arrival time
		int firstBusID = 0;
		int waitTime = 0;
		for (int i = 0; i < busSchedule.size(); ++i)
		{
			MinuteSchedule minuteSchedule = busSchedule[i];
			if (!minuteSchedule.m_buses.empty())
			{
				firstBusID = minuteSchedule.m_buses.front();
				waitTime = i;
				break;
			}
		}

		cout << "The first bus to arrive is: " << firstBusID << endl;
		cout << "The time to wait is: " << waitTime << endl;
		cout << "The answer is: " << (firstBusID * waitTime) << endl << endl;
	}

	{
		// Part Two
		cout << "=== Part 2 ===" << endl;
		vector<vector<int>> busSchedules;
		ifstream inputStream(c_inputFile);
		if (inputStream.is_open())
		{
			string line;
			getline(inputStream, line); // Discard first line

			while (getline(inputStream, line))
			{
				vector<int> schedule;
				ParseLineIntoSchedule(schedule, line);
				busSchedules.push_back(schedule);
			}
		}

		for (int i = 0; i < busSchedules.size(); ++i)
		{
			long long time = GetConsecutiveBusTime(busSchedules[i]);
			cout << time << endl;
		}

		cout << "Finished!" << endl;
	}
}
