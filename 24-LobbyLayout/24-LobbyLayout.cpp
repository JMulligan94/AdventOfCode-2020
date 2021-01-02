// 24-LobbyLayout.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <sstream>
#include <stdint.h>

using namespace std;

#define DEBUG_PRINT

// Tiles are:
//		 /  \
//	nw /      \ ne
//    |        |
//  w |        | e
//    |        |
//  sw \      / se
//       \  /


enum Direction
{
	E,
	SE,
	SW,
	W,
	NW,
	NE
};

map<Direction, string> dirLabels = {
	{Direction::E, "East"},
	{Direction::NE, "North East"},
	{Direction::NW, "North West"},
	{Direction::W, "West"},
	{Direction::SE, "South East"},
	{Direction::SW, "South West"}
};

struct Coords
{
	Coords() = default;
	Coords(int row, int col) { m_row = row; m_col = col; }

	int m_row;
	int m_col;
};

// Tiles stored as:
//	0,0		0,1		0,2		0,3
//		1,0		1,1		1,2		1,3
//	2,0		2,1		2,2		2,3
//		3,0		3,1		3,2		3,3
//	4,0		4,1		4,2		4,3

vector<string> m_tiles;
Coords m_referenceTile;
int m_tileGridWidth = 1;
vector<bool> m_offsetRows;

const char c_blackTile = 'B';
const char c_whiteTile = 'W';

void FlipTile(const Coords& coords)
{
	int row = coords.m_row;
	int col = coords.m_col;
#ifdef DEBUG_PRINT
	cout << "FLIPPING: " << row << "," << col << endl;
#endif
	char currentValue = m_tiles[row][col];
	m_tiles[row][col] = (currentValue == c_whiteTile) ? c_blackTile : c_whiteTile;
}

void GrowNorth()
{
	string newTiles;
	newTiles.resize(m_tileGridWidth, 'W');
	m_tiles.insert(m_tiles.begin(), newTiles);
	m_referenceTile.m_row++; // Reference tile shunts down a row each time
	m_offsetRows.insert(m_offsetRows.begin(), !m_offsetRows.front());
}

void GrowSouth()
{
	string newTiles;
	newTiles.resize(m_tileGridWidth, 'W');
	m_tiles.push_back(newTiles);
	m_offsetRows.push_back(!m_offsetRows.back());
}

void GrowWest()
{
	for (int i = 0; i < (int)m_tiles.size(); ++i)
	{
		m_tiles[i].insert(m_tiles[i].begin(), 'W');
	}
	m_referenceTile.m_col++; // Reference tile shunts across a column each time
	m_tileGridWidth++;
}

void GrowEast()
{
	for (int i = 0; i < (int)m_tiles.size(); ++i)
	{
		m_tiles[i].push_back('W');
	}
	m_tileGridWidth++;
}

void FlipTileAtOffset(const Coords& offset)
{
#ifdef DEBUG_PRINT
	cout << "Flipping tile at offset: " << offset.m_row << "," << offset.m_col << endl;
#endif
	int row = offset.m_row;
	int col = offset.m_col;
#ifdef DEBUG_PRINT
	cout << "Ref tile: " << m_referenceTile.m_row << "," << m_referenceTile.m_col << endl;
#endif

	// Check if we need to grow the bounds of our tile structure
	if (row < 0)
	{
		// Tile is out of bounds to the north of the current bounds
		while (row < 0)
		{
			GrowNorth();
			row++;
		}
	}
	else if (row >= (int)m_tiles.size())
	{
		// Tile is out of bounds to the south of the current bounds
		while (row >= (int)m_tiles.size())
		{
			GrowSouth();
		}
	}

	if (col < 0)
	{
		// Tile is out of bounds to the west of the current bounds
		while (col < 0)
		{
			GrowWest();
			col++;
		}
	}
	else if (col >= m_tileGridWidth)
	{
		// Tile is out of bounds to the east of the current bounds
		while (col >= m_tileGridWidth)
		{
			GrowEast();
		}
	}
	FlipTile(Coords(row, col));
}

bool IsOffsetRow(int x)
{
	// Works for negative integers
	bool oldValue = (x & 1) != 0;
	bool newValue = false;
	if (x >= 0 && x < (int)m_offsetRows.size())
	{
		newValue = m_offsetRows[x];
	}
	else if (x < 0)
	{
		int diff = abs(x);
		newValue = diff % 2 == 0 ? m_offsetRows.front() : !m_offsetRows.front();
	}
	else
	{
		int diff = x - ((int)m_offsetRows.size() - 1);
		newValue = diff % 2 == 0 ? m_offsetRows.back() : !m_offsetRows.back();
	}
	return newValue;
	
}

void PrintTiles()
{
#ifdef DEBUG_PRINT
	cout << endl;
	for (int i = 0; i < (int)m_tiles.size(); ++i)
	{
		if (IsOffsetRow(i))
			cout << " ";

		for (int j = 0; j < (int)m_tiles[i].size(); ++j)
		{
			if (i == m_referenceTile.m_row && j == m_referenceTile.m_col)
				cout << (char)(m_tiles[i][j] + ('a'-'A')) << " ";
			else
				cout << m_tiles[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
#endif
}

Coords GetNeighbourTileCoords(const Coords& coords, Direction direction)
{
	int row = coords.m_row;
	int col = coords.m_col;

#ifdef DEBUG_PRINT
	//cout << "Finding neighbour tile for " << row << "," << col << " in direction: " << dirLabels[direction] << endl;
#endif

	int newRow = row;
	int newCol = col;
	switch (direction)
	{
		case E:
		{
			// row, col + 1
			newCol++;
		}
		break;
		case SE:
		{
			// if row even, row + 1, col
			// if row odd, row + 1, col + 1
			newRow++;
			if (IsOffsetRow(row))
				newCol++;
		}
		break;
		case SW:
		{
			// if row even, row + 1, col - 1
			// if row odd, row + 1, col
			newRow++;
			if (!IsOffsetRow(row))
				newCol--;
		}
		break;
		case W:
		{
			// row, col - 1
			newCol--;
		}
		break;
		case NW:
		{
			// if row even, row - 1, col - 1 
			// if row odd, row - 1, col
			newRow--;
			if (!IsOffsetRow(row))
				newCol--;
		}
		break;
		case NE:
		{
			// if row even, row - 1, col
			// if row odd, row - 1, col + 1
			newRow--;
			if (IsOffsetRow(row))
				newCol++;
		}
		break;
	}

	return Coords(newRow, newCol);
}

void ParseLineIntoDirections(const string& line, vector<Direction>& directions)
{
	for (int i = 0; i < (int)line.size(); ++i)
	{
		char token = line[i];
		if (token == 'w')
		{
			directions.push_back(Direction::W);
		}
		else if (token == 'e')
		{
			directions.push_back(Direction::E);
		}
		else if (token == 's')
		{
			// South must have a second direction combined with it
			// Check next char
			char nextToken = line[i+1];
			if (nextToken == 'e')
				directions.push_back(Direction::SE);
			else if (nextToken == 'w')
				directions.push_back(Direction::SW);

			i++;
		}
		else if (token == 'n')
		{
			// South must have a second direction combined with it
			// Check next char
			char nextToken = line[i + 1];
			if (nextToken == 'e')
				directions.push_back(Direction::NE);
			else if (nextToken == 'w')
				directions.push_back(Direction::NW);

			i++;
		}
	}
}

int GetBlackTileCount()
{
	int count = 0;
	for (string tileRow : m_tiles)
	{
		for (char tile : tileRow)
		{
			count += (tile == c_blackTile) ? 1 : 0;
		}
	}
	return count;
}

bool GetIsBlackTile(const Coords& coords)
{
	if (coords.m_row < 0 || coords.m_row >= (int)m_tiles.size())
		return false;

	if (coords.m_col < 0 || coords.m_col >= m_tileGridWidth)
		return false;

	return m_tiles[coords.m_row][coords.m_col] == c_blackTile;
}

void DoExhibitRules()
{
	// Do we need to expand the tile grid before doing the exhibit rules?
	// Check if there are any black tiles on the northern-most row
	for (int col = 0; col < (int)m_tiles.front().size(); ++col)
	{
		if (GetIsBlackTile(Coords(0, col)))
		{
			GrowNorth();
			break;
		}
	}

	// Check if there are any black tiles on the southern-most row
	for (int col = 0; col < (int)m_tiles.back().size(); ++col)
	{
		if (GetIsBlackTile(Coords(m_tiles.size()-1, col)))
		{
			GrowSouth();
			break;
		}
	}

	// Check if there are any black tiles on the western-most columns
	for (int row = 0; row < (int)m_tiles.size(); ++row)
	{
		if (GetIsBlackTile(Coords(row, 0)))
		{
			GrowWest();
			break;
		}
	}

	// Check if there are any black tiles on the eastern-most columns
	for (int row = 0; row < (int)m_tiles.size(); ++row)
	{
		if (GetIsBlackTile(Coords(row, m_tileGridWidth - 1)))
		{
			GrowEast();
			break;
		}
	}

	PrintTiles();

	vector<Coords> tilesToFlip;
	for (int row = 0; row < (int)m_tiles.size(); ++row)
	{
		for (int col = 0; col < (int)m_tiles[row].size(); ++col)
		{
			Coords currentCoords = Coords(row, col);
			bool isBlackTile = GetIsBlackTile(currentCoords);

			int numBlackNeighbours = 0;
			numBlackNeighbours += GetIsBlackTile(GetNeighbourTileCoords(currentCoords, Direction::E)) ? 1 : 0;
			numBlackNeighbours += GetIsBlackTile(GetNeighbourTileCoords(currentCoords, Direction::W)) ? 1 : 0;
			numBlackNeighbours += GetIsBlackTile(GetNeighbourTileCoords(currentCoords, Direction::SE)) ? 1 : 0;
			numBlackNeighbours += GetIsBlackTile(GetNeighbourTileCoords(currentCoords, Direction::SW)) ? 1 : 0;
			numBlackNeighbours += GetIsBlackTile(GetNeighbourTileCoords(currentCoords, Direction::NE)) ? 1 : 0;
			numBlackNeighbours += GetIsBlackTile(GetNeighbourTileCoords(currentCoords, Direction::NW)) ? 1 : 0;

			if (isBlackTile)
			{
				// Any black tile with zero or more than 2 black tiles immediately adjacent to it is flipped to white.
				if (numBlackNeighbours == 0 || numBlackNeighbours > 2)
				{
					tilesToFlip.push_back(currentCoords);
				}
			}
			else
			{
				// Any white tile with exactly 2 black tiles immediately adjacent to it is flipped to black.
				if (numBlackNeighbours == 2)
				{
					tilesToFlip.push_back(currentCoords);
				}
			}
		}
	}

	for (Coords coordToFlip : tilesToFlip)
	{
		FlipTile(coordToFlip);
	}
}

const char* c_inputFile = "Input.txt";
const char* c_testFile = "Test.txt";

int main()
{
	vector<string> directionStrings;
	vector<vector<Direction>> directionList;
	ifstream inputStream(c_testFile);
	if (inputStream.is_open())
	{
		string line;
		// Get fields
		while (getline(inputStream, line))
		{
			vector<Direction> directions;
			ParseLineIntoDirections(line, directions);
			directionList.push_back(directions);
			directionStrings.push_back(line);
		}
	}

	m_referenceTile = Coords(0, 0);
	m_tiles.push_back(string(1, c_whiteTile));
	m_offsetRows.push_back(false);

	{
		// Part One
		cout << "=== Part 1 ===" << endl;
		int i = 0;
		for (auto directions : directionList)
		{
#ifdef DEBUG_PRINT
			cout << endl << directionStrings[i] << endl;
#endif
			i++;
			// Start at reference tile
			Coords offset = m_referenceTile;
			for (auto dir : directions)
			{
				offset = GetNeighbourTileCoords(offset, dir);
			}

#ifdef DEBUG_PRINT
			cout << endl;
#endif

			// Found destination tile from directions - flip it
			FlipTileAtOffset(offset);

#ifdef DEBUG_PRINT
			PrintTiles();
#endif

			cout << "The number of black tiles is: " << GetBlackTileCount() << endl;
		}

		cout << "The number of black tiles is: " << GetBlackTileCount() << endl;
	}

	{
		// Part Two
		cout << endl << "=== Part 2 ===" << endl;
		const int c_numberOfDays = 10;
		int dayCount = 1;
		while (dayCount <= c_numberOfDays)
		{
			cout << endl << "=== Day " << dayCount << " ===" << endl;
			DoExhibitRules();
			cout << "Day " << dayCount << ": " << GetBlackTileCount() << endl;
			dayCount++;
		}
	}

}
