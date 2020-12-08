// 08-HandheldHalting.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

enum OperatorType
{
	JUMP,
	ACCUMULATE,
	NOTHING
};

OperatorType GetOperatorType(string str)
{
	if (strcmp(str.c_str(), "acc") == 0)
		return OperatorType::ACCUMULATE;
	else if (strcmp(str.c_str(), "jmp") == 0)
		return OperatorType::JUMP;

	return OperatorType::NOTHING;
}

int main()
{
	vector<pair<OperatorType, int>> instructions;
	// Read input.txt and grab the relevant info
	ifstream inputStream("input.txt");
	if (inputStream.is_open())
	{
		string line;
		while (getline(inputStream, line))
		{
			int index = (int)line.find(' ');
			string operatorStr = line.substr(0, index);
			
			string operandStr = line.substr(index + 1);
			instructions.push_back(pair<OperatorType, int>(GetOperatorType(operatorStr), stoi(operandStr)));
		}
	}

	{
		// Part One
		cout << "=== Part 1 ===" << endl;
		int accumulator = 0;
		int currentLine = 0;
		vector<bool> linesAccessed;
		linesAccessed.resize(instructions.size());
		while (!linesAccessed[currentLine])
		{
			OperatorType type = instructions[currentLine].first;
			int operand = instructions[currentLine].second;

			linesAccessed[currentLine] = true;

			if (type == OperatorType::JUMP)
			{
				currentLine += operand;
			}
			else
			{
				currentLine++;
				if (type == OperatorType::ACCUMULATE)
				{
					accumulator += operand;
				}
			}
		}
		cout << "The accumulator has a value of: " << accumulator << endl;
	}

	{
		// Part Two
		cout << "=== Part 2 ===" << endl;
		bool fixed = false;
		int iterationCount = 0;
		int accumulator = 0;
		int lineSwapped = -1;
		while (!fixed)
		{
			accumulator = 0;
			int currentLine = 0;
			vector<bool> linesAccessed;
			linesAccessed.resize(instructions.size());
			int nopJmpCount = 0;

			while (currentLine < (int)instructions.size()  // We're looking at a line in the instruction set
				&& !linesAccessed[currentLine]) // We've not checked this line yet
			{
				OperatorType type = instructions[currentLine].first;
				int operand = instructions[currentLine].second;

				linesAccessed[currentLine] = true;

				//Check if JMP or NOP
				if (type == OperatorType::JUMP)
				{
					if (nopJmpCount == iterationCount)
					{
						type = OperatorType::NOTHING;
						cout << "Attempt " << iterationCount << " - Swapped JMP to NOP on line " << currentLine << endl;
						lineSwapped = currentLine;
					}
					nopJmpCount++;
				}
				else if (type == OperatorType::NOTHING)
				{
					if (nopJmpCount == iterationCount)
					{
						type = OperatorType::JUMP;
						cout << "Attempt " << iterationCount << " - Swapped NOP to JMP on line " << currentLine << endl; 
						lineSwapped = currentLine;
					}
					nopJmpCount++;
				}

				// Perform instruction
				if (type == OperatorType::JUMP)
				{
					currentLine += operand;
				}
				else
				{
					currentLine++;
					if (type == OperatorType::ACCUMULATE)
					{
						accumulator += operand;
					}
				}
			}
			fixed = currentLine == (int)instructions.size();
			iterationCount++;
		}

		if (fixed)
			cout << "Swapped line " << lineSwapped << ", accumulator = "<< accumulator << endl;
	}

}