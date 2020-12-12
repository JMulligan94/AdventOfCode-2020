// 11-SeatingSystem.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

const char c_vacantChar = 'L';
const char c_occupiedChar = '#';
const char c_emptyChar = '.';

vector<string> m_seatLayout;

// struct to hold info for a change of seat 
struct SeatChangeInfo
{
	SeatChangeInfo(int row, int col, char newSeatChar)
		: m_row(row)
		, m_col(col)
		, m_newSeatChar(newSeatChar)
	{ }

	int m_row;
	int m_col;
	char m_newSeatChar;
};

char GetSeatInfo(int row, int col)
{
	return m_seatLayout[row][col];
}

// Is this seat co-ord within range of the grid
bool IsValidSeat(int row, int col)
{
	if (row < 0 || row >= (int)m_seatLayout.size())
		return false;

	if (col < 0 || col >= (int)m_seatLayout.front().size())
		return false;

	return true;
}

void SetOccupied(int row, int col)
{
	m_seatLayout[row][col] = c_occupiedChar;
}

void SetVacant(int row, int col)
{
	m_seatLayout[row][col] = c_vacantChar;
}

// Check seat directly next to given seat. If '#' then occupied.
bool IsImmediatelyOccupiedInDirection(int row, int col, int deltaRow, int deltaCol)
{
	int newRow = row + deltaRow;
	int newCol = col + deltaCol;

	if (!IsValidSeat(newRow, newCol))
		return false;

	return GetSeatInfo(newRow, newCol) == c_occupiedChar;
}

// Check seat directly next to given seat in all compass points
int GetNumImmediateAdjacentOccupied(int row, int col)
{
	int occupied = 0;

	// Check up
	occupied += IsImmediatelyOccupiedInDirection(row, col, -1, 0) ? 1 : 0;

	// Check up-left
	occupied += IsImmediatelyOccupiedInDirection(row, col, -1, -1) ? 1 : 0;

	// Check up-right
	occupied += IsImmediatelyOccupiedInDirection(row, col, -1, 1) ? 1 : 0;

	// Check down
	occupied += IsImmediatelyOccupiedInDirection(row, col, 1, 0) ? 1 : 0;

	// Check down-left
	occupied += IsImmediatelyOccupiedInDirection(row, col, 1, -1) ? 1 : 0;

	// Check down-right
	occupied += IsImmediatelyOccupiedInDirection(row, col, 1, 1) ? 1 : 0;

	// Check left
	occupied += IsImmediatelyOccupiedInDirection(row, col, 0, -1) ? 1 : 0;

	// Check right
	occupied += IsImmediatelyOccupiedInDirection(row, col, 0, 1) ? 1 : 0;

	return occupied;
}

// Check the seats in the direction from the given seat until either a vacant or occupied seat is hit.
bool IsOccupiedInDirection(int row, int col, int deltaRow, int deltaCol)
{
	int currentRow = row + deltaRow;
	int currentCol = col + deltaCol;
	while (true)
	{
		if (!IsValidSeat(currentRow, currentCol)) // Hit sides of grid
			return false;

		char seat =	GetSeatInfo(currentRow, currentCol);
		if (seat == c_occupiedChar) // Seat within view is occupied
			return true;
		else if (seat == c_vacantChar) // Seat within view is vacant
			return false;

		currentRow = currentRow + deltaRow;
		currentCol = currentCol + deltaCol;
	}
}

int GetNumAdjacentOccupied(int row, int col)
{
	int occupied = 0;

	// Check up
	occupied += IsOccupiedInDirection(row, col, -1, 0) ? 1 : 0;

	// Check up-left
	occupied += IsOccupiedInDirection(row, col, -1, -1) ? 1 : 0;

	// Check up-right
	occupied += IsOccupiedInDirection(row, col, -1, 1) ? 1 : 0;

	// Check down
	occupied += IsOccupiedInDirection(row, col, 1, 0) ? 1 : 0;

	// Check down-left
	occupied += IsOccupiedInDirection(row, col, 1, -1) ? 1 : 0;

	// Check down-right
	occupied += IsOccupiedInDirection(row, col, 1, 1) ? 1 : 0;
	
	// Check left
	occupied += IsOccupiedInDirection(row, col, 0, -1) ? 1 : 0;

	// Check right
	occupied += IsOccupiedInDirection(row, col, 0, 1) ? 1 : 0;

	return occupied;
}

// Count the number of occupied seats
int GetNumOccupiedSeats()
{
	int occupiedCount = 0;
	for (int row = 0; row < (int)m_seatLayout.size(); ++row)
	{
		for (int col = 0; col < (int)m_seatLayout[row].size(); ++col)
		{
			if (GetSeatInfo(row, col) == c_occupiedChar)
				occupiedCount++;
		}
	}

	return occupiedCount;
}

// Do a round of changing seat layout
// return: bool to determine if any changes were made to the seats
bool DoRound(bool checkImmediateAdjacent, int occupiedToleranceVal)
{
	// Cache all changes first and then apply, since they happen simultaneously
	vector<SeatChangeInfo> changesToMake;

	// Iterate over all seats
	for (int row = 0; row < (int)m_seatLayout.size(); ++row)
	{
		for (int col = 0; col < (int)m_seatLayout[row].size(); ++col)
		{
			char currentSeat = GetSeatInfo(row, col);
			if (currentSeat == c_emptyChar)
				continue;

			// How many occupied seats are next to this seat?
			int numOccupied = 0;
			if (checkImmediateAdjacent)
				numOccupied = GetNumImmediateAdjacentOccupied(row, col);
			else
				numOccupied = GetNumAdjacentOccupied(row, col);

			// If the seat is vacant and there are no occupied seats around, the seat will be taken
			if (currentSeat == c_vacantChar)
			{
				if (numOccupied == 0)
				{
					changesToMake.push_back(SeatChangeInfo(row, col, c_occupiedChar));
				}
			}
			// Else if the seat is occupied but there are more than the tolerated number of occupied seats around it
			// the seat will become vacant
			else if (currentSeat == c_occupiedChar)
			{
				if (numOccupied >= occupiedToleranceVal)
				{
					changesToMake.push_back(SeatChangeInfo(row, col, c_vacantChar));
				}
			}
		}
	}

	// Make all the changes now
	for (SeatChangeInfo change : changesToMake)
	{
		if (change.m_newSeatChar == c_occupiedChar)
			SetOccupied(change.m_row, change.m_col);

		if (change.m_newSeatChar == c_vacantChar)
			SetVacant(change.m_row, change.m_col);
	}

	return !changesToMake.empty();
}

// Debug function for printing current state of seats
void PrintSeats(int roundNum)
{
	cout << "=== ROUND " << roundNum << " ===" << endl;
	for (int row = 0; row < (int)m_seatLayout.size(); ++row)
	{
		cout << m_seatLayout[row] << endl;
	}
	cout << endl;
}

int main()
{
	ifstream inputStream("input.txt");
	//ifstream inputStream("test.txt");
	if (inputStream.is_open())
	{
		string line;
		while (getline(inputStream, line))
		{
			m_seatLayout.push_back(line);
		}
	}
	vector<string> originalLayout = m_seatLayout;

	{
		// Part One
		cout << "=== Part 1 ===" << endl;

		//PrintSeats(0);

		int numRounds = 0;
		bool changesMade = true;
		while (changesMade)
		{
			changesMade = DoRound(true, 4);

			numRounds++;
			//PrintSeats(numRounds);
		}

		cout << "Stabilised at round " << numRounds << ", with " << GetNumOccupiedSeats() << " occupied seats" << endl << endl;
	}

	// Reset layout
	m_seatLayout = originalLayout;

	{
		// Part Two
		cout << "=== Part 2 ===" << endl;

		//PrintSeats(0);

		int numRounds = 0;
		bool changesMade = true;
		while (changesMade)
		{
			changesMade = DoRound(false, 5);

			numRounds++;
			//PrintSeats(numRounds);
		}

		cout << "Stabilised at round " << numRounds << ", with " << GetNumOccupiedSeats() << " occupied seats" << endl;
	}

}