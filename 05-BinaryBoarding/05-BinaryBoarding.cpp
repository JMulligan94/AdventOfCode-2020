// 05-BinaryBoarding.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>

using namespace std;

int m_plane[128][8];
vector<string> m_lines;

int CalculateSeatID(int row, int col)
{
	return (row * 8) + col;
}

int FindSeatID(string pass, int& row, int& col)
{
	// Find Row
	int minRow = 0;
	int maxRow = 127;
	for (int i = 0; i < 7; ++i)
	{
		int diff = maxRow - minRow;
		char current = pass.at(i);
		if (current == 'F')
		{
			// Front
			maxRow = minRow + (diff / 2);
		}
		else
		{
			// Back
			minRow = minRow + (diff / 2);
		}
	}

	row = maxRow;

	// Find Column
	int minCol = 0;
	int maxCol = 7;
	for (int i = 7; i < 10; ++i)
	{
		int diff = maxCol - minCol;
		char current = pass.at(i);
		if (current == 'L')
		{
			// Left
			maxCol = minCol + (diff / 2);
		}
		else
		{
			// Right
			minCol = minCol + (diff / 2);
		}
	}

	col = maxCol;

	return CalculateSeatID(row, col);
}

bool FindEmptyMiddleSeat(int& row, int& col)
{
	bool foundFilledSeat = false;
	for (int r = 0; r < 128; ++r)
	{
		for (int c = 0; c < 8; ++c)
		{
			if (m_plane[r][c] == 0)
			{
				if (foundFilledSeat)
				{
					row = r;
					col = c;
					return true;
				}
			}
			else
			{
				foundFilledSeat = true;
			}
		}
	}
	return false;
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

	int highestSeatID = 0;
	for (int i = 0; i < (int)m_lines.size(); ++i)
	{
		int row, col;
		int seatID = FindSeatID(m_lines[i], row, col);
		if (seatID > highestSeatID)
			highestSeatID = seatID;

		m_plane[row][col] = seatID;
	}

	{
		// Part One
		cout << "=== Part 1 ===" << endl;
		cout << "The highest seat ID is: " << highestSeatID << endl;
	}

	{
		// Part Two
		cout << "=== Part 2 ===" << endl;
		int vacantRow;
		int vacantCol;
		FindEmptyMiddleSeat(vacantRow, vacantCol);
		
		int vacantSeatID = CalculateSeatID(vacantRow, vacantCol);
		cout << "The vacant seat ID is: " << vacantSeatID << endl;
	}
}
