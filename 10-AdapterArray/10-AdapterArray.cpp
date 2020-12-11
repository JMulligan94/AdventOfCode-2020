// 10-AdapterArray.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

const int c_startJoltage = 0;
const int c_maxRange = 3;
vector<int> m_orderedAdapters;

// Get the number of arrangements for a given number of adapters
long long GetNumArrangements(int num, int currentIndex = 0)
{
	if (currentIndex == num)
	{
		return 1;
	}

	long long numArrangements = 0;

	for (int i = 1; i <= c_maxRange; ++i)
	{
		int indexToCheck = currentIndex + i;
		if (indexToCheck <= num)
		{
			numArrangements += GetNumArrangements(num, indexToCheck);
		}
	}
	return numArrangements;
}

int compare(const void* a, const void* b)
{
	return (*(int*)a - *(int*)b);
}

int main()
{
	vector<int> adapters;
	ifstream inputStream("input.txt");
	//ifstream inputStream("test.txt");
	//ifstream inputStream("test2.txt");
	if (inputStream.is_open())
	{
		string line;
		while (getline(inputStream, line))
		{
			adapters.push_back(stoi(line));
		}
	}
	
	// Order the adapters by joltage
	m_orderedAdapters = adapters;
	m_orderedAdapters.push_back(0);
	std::qsort(&m_orderedAdapters[0], m_orderedAdapters.size(), sizeof(int), compare);
	m_orderedAdapters.push_back(m_orderedAdapters.back() + 3);

	// Keep track of the running differences in joltage between consecutive adapters
	vector<int> runningDiffs;
	int joltDiff1 = 0;
	int joltDiff3 = 0;
	for (int i = 0; i < (int)m_orderedAdapters.size() - 1; ++i)
	{
		int currentAdapter = m_orderedAdapters[i];
		int nextAdapter = m_orderedAdapters[i + 1];

		int diffJoltage = nextAdapter - currentAdapter;

		runningDiffs.push_back(diffJoltage);

		if (diffJoltage == 1)
			joltDiff1++;
		else if (diffJoltage == 3)
			joltDiff3++;
	}

	{
		// Part One
		cout << "=== Part 1 ===" << endl;
		cout << "Number of 1 joltage differences: " << joltDiff1 << endl;
		cout << "Number of 3 joltage differences: " << joltDiff3 << endl;
		cout << "Answer is: " << (joltDiff1 * joltDiff3) << endl << endl;
	}


	{
		// Part Two
		cout << "=== Part 2 ===" << endl;

		// Calculate the different numbers of arrangements for each quantity of consecutive adapters
		vector<long long> arrangementSequences;
		for (int i = 0; i < 15; ++i)
		{
			long long numArrangements = GetNumArrangements(i);
			arrangementSequences.push_back(numArrangements);
			cout << " Arrangements for " << i << " = " << numArrangements << endl;
		}

		// Run through the ordered adapters, counting the amounts of consecutive 1s
		long long totalArrangements = 0;
		int running1s = 0;
		for (int i = 0; i < (int)runningDiffs.size(); ++i)
		{
			if (runningDiffs[i] == 1)
				running1s++;
			else if (runningDiffs[i] == 3 && running1s > 0)
			{
				// We've hit a 3 and this is the end of a string of 1s!
				// Use the cached arrangement amounts and multiply onto the amount of arrangements found so far
				if (totalArrangements == 0)
					totalArrangements = arrangementSequences[running1s];
				else
					totalArrangements *= arrangementSequences[running1s];
				running1s = 0;
			}
		}

		cout << "The number of possible adapter arrangements is: " << totalArrangements << endl << endl;
	}
}

