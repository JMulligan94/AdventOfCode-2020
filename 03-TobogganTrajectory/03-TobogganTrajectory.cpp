// 03-TobogganTrajectory.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

vector<string> m_lines;

bool DescendLevel(int deltaX, int deltaY, int& x, int& y)
{
	int currentX = x;
	int currentY = y;

	int newX = x + deltaX;
	int newY = y + deltaY;

	if (newY >= (int)m_lines.size())
		return false;

	string level = m_lines[newY];
	int index = newX % (int)level.size();
	char token = level.at(index);
	
	x = newX;
	y = newY;

	// '.' = open square
	// '#' = tree!
	return token == '#';
}

int DescendSlope(int deltaX, int deltaY)
{
	int currentX = 0;
	int currentY = 0;

	int numTrees = 0;

	for (int y = 0; y < (int)m_lines.size(); ++y)
	{
		if (DescendLevel(deltaX, deltaY, currentX, currentY))
			numTrees++;
	}

	return numTrees;
}

int main()
{
	ifstream inputStream("input.txt");
	if (inputStream.is_open())
	{
		string line;
		while (getline(inputStream, line))
		{
			m_lines.push_back(line);
		}
	}

	
	{
		// Part One
		cout << "=== Part 1 ===" << endl;

		int numTrees = DescendSlope(3, 1);

		cout << "The number of trees hit: " << numTrees << endl << endl;
	}


	{
		// Part Two
		cout << "=== Part 2 ===" << endl;

		int deltaXs[5] = { 1,3,5,7,1 };
		int deltaYs[5] = { 1,1,1,1,2 };

		unsigned int treeProduct = 0;
		treeProduct = DescendSlope(deltaXs[0], deltaYs[0]);

		for (int i = 1; i < 5; ++i)
		{
			treeProduct *= DescendSlope(deltaXs[i], deltaYs[i]);
		}

		cout << "The product of trees hit: " << treeProduct << endl << endl;
	}
}