// 10-AdapterArray.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

const int c_startJoltage = 0;
vector<int> m_orderedAdapters;
size_t m_adaptersSize;

void GetNumArrangements(long long& numArrangements, int targetAdapter, int startIndex, int startAdapter)
{
	if (startAdapter == targetAdapter)
	{
		numArrangements++;
		return;
	}

	for (int i = 1; i <= 3; ++i)
	{
		int indexToCheck = startIndex + i;
		if (indexToCheck < m_adaptersSize)
		{
			int nextAdapter = m_orderedAdapters[indexToCheck];
			int diff = nextAdapter - startAdapter;
			if (diff <= 3)
				GetNumArrangements(numArrangements, targetAdapter, indexToCheck, nextAdapter);
			else
				return;
		}
	}
}

void GetNumArrangementsForRange(long long& numArrangements, int start, int end)
{
	cout << "=== Arrangements for range " << start << " (" << m_orderedAdapters[start] << ") - " 
		<< end << " (" << m_orderedAdapters[end] << ") ===" << endl;
	int startAdapter = m_orderedAdapters[start];
	int targetAdapter = m_orderedAdapters[end];
	GetNumArrangements(numArrangements, targetAdapter, start, startAdapter);
	cout << numArrangements << endl;
}

int compare(const void* a, const void* b)
{
	return (*(int*)a - *(int*)b);
}

int main()
{
	vector<int> adapters;
	//ifstream inputStream("input.txt");
	ifstream inputStream("test.txt");
	if (inputStream.is_open())
	{
		string line;
		while (getline(inputStream, line))
		{
			adapters.push_back(stoi(line));
		}
	}
	
	m_orderedAdapters = adapters;
	m_orderedAdapters.push_back(0);
	std::qsort(&m_orderedAdapters[0], m_orderedAdapters.size(), sizeof(int), compare);
	m_adaptersSize = m_orderedAdapters.size();

	for (int i = 0; i < m_orderedAdapters.size(); ++i)
	{
		cout << i << " - " << m_orderedAdapters[i] << endl;
	}

	vector<pair<int, int>> nonThreeGapRanges;

	{
		// Part One
		cout << "=== Part 1 ===" << endl;

		int startRange = 0;
		bool prevWasThree = false;
		int num1JoltDiff = 0;
		int num3JoltDiff = 1; // Device built-in is always +3 to final
		for (int i = 0; i < m_orderedAdapters.size()-1; ++i)
		{
			int currentAdapter = m_orderedAdapters[i];
			int nextAdapter = m_orderedAdapters[i+1];

			int diffJoltage = nextAdapter - currentAdapter;

			if (diffJoltage == 3)
			{
				if (!prevWasThree)
					nonThreeGapRanges.push_back(pair<int, int>(startRange, i));
				num3JoltDiff++;
				prevWasThree = true;
			}
			else
			{
				if (prevWasThree)
					startRange = i;
				
				prevWasThree = false;
				if (diffJoltage == 1)
					num1JoltDiff++;
			}
		}

		cout << "Number of 1 joltage differences: " << num1JoltDiff << endl;
		cout << "Number of 3 joltage differences: " << num3JoltDiff << endl;
		cout << "Answer is: " << (num1JoltDiff * num3JoltDiff) << endl << endl;
	}

	{
		// Part Two
		cout << "=== Part 2 ===" << endl;
		vector<long long> rangeArrangements;
		for (int i = 0; i < nonThreeGapRanges.size(); ++i)
		{
			int startIndex = nonThreeGapRanges[i].first;
			int endIndex = nonThreeGapRanges[i].second;

			long long numArrangements = 0;
			GetNumArrangementsForRange(numArrangements, startIndex, endIndex);
			rangeArrangements.push_back(numArrangements);
		}

		unsigned long long total = rangeArrangements[0];
		for (int i = 1; i < rangeArrangements.size(); ++i)
		{
			total *= rangeArrangements[i];
			cout << total << endl;
		}

		int i = 0;
		//cout << "The number of possible adapter arrangements is: " << numberArrangements << endl;
	}
}

