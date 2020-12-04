// 01-ReportRepair.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

bool FindTwoEntries(const vector<int>& entries, int& entryA, int& entryB)
{
	entryA = -1;
	entryB = -1;

	for (int i = 0; i < (int)entries.size() - 1; ++i)
	{
		entryA = entries[i];
		for (int j = i + 1; j < (int)entries.size(); ++j)
		{
			entryB = entries[j];
			int sum = entryA + entryB;
			if (sum == 2020)
			{
				return true;
			}
		}
	}
	return false;
}

bool FindThreeEntries(const vector<int>& entries, int& entryA, int& entryB, int& entryC)
{
	entryA = -1;
	entryB = -1;
	entryC = -1;

	for (int i = 0; i < (int)entries.size() - 2; ++i)
	{
		entryA = entries[i];
		for (int j = i + 1; j < (int)entries.size() - 1; ++j)
		{
			entryB = entries[j];
			for (int l = j + 1; l < (int)entries.size(); ++l)
			{
				entryC = entries[l];
				int sum = entryA + entryB + entryC;
				if (sum == 2020)
				{
					return true;
				}
			}
		}
	}
	return false;
}

int main()
{
	vector<int> entries;

	ifstream inputStream("input.txt");
	if (inputStream.is_open())
	{
		string line;
		while (getline(inputStream, line))
		{
			entries.push_back(std::stoi(line));
		}
	}	

	// Part One
	{
		int entryA, entryB = -1;

		if (FindTwoEntries(entries, entryA, entryB))
		{
			int answer = entryA * entryB;
			cout << "=== Part 1 ===" << endl;
			cout << "The answer is : " << entryA << " * " << entryB << " = " << answer << endl << endl;
		}
	}

	// Part Two
	{
		int entryA, entryB, entryC = -1;

		if (FindThreeEntries(entries, entryA, entryB, entryC))
		{
			int answer = entryA * entryB * entryC;
			cout << "=== Part 2 ===" << endl;
			cout << "The answer is : " << entryA << " * " << entryB << " * " << entryC << " = " << answer << endl << endl;
		}
	}
}
