// 20-JurassicJigsaw.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stdint.h>

using namespace std;

enum Neighbour
{
	Top,
	Bottom,
	Left,
	Right
};

class Tile;
vector<Tile*> m_tiles;

class Tile
{
public:
	Tile(vector<string> lines)
	{
		string idLine = lines.front();
		m_id = stoi(idLine.substr(5));

		for (int i = 1; i < (int)lines.size(); ++i)
		{
			m_tileData.push_back(lines[i]);
		}
		CacheEdges();
	}

	void GetNeighbours(Neighbour neighbour, vector<Tile*>& neighbourTiles) const
	{
		string tileString = GetEdgeString(neighbour);

		for (Tile* tile : m_tiles)
		{
			if (tile == this)
				continue;

			string oppositeString = tile->GetOppositeEdgeString(neighbour);

			if (tileString == oppositeString)
				neighbourTiles.push_back(tile);
		}
	}

	string GetEdgeString(Neighbour neighbour) const
	{
		switch (neighbour)
		{
			case Top:		return m_top;
			case Bottom:	return m_bottom;
			case Left:		return m_left;
			default:		return m_right;
		}
	}

	string GetOppositeEdgeString(Neighbour neighbour) const
	{
		switch (neighbour)
		{
			case Top:		return m_bottom;
			case Bottom:	return m_top;
			case Left:		return m_right;
			default:		return m_left;
		}
	}

	void CacheEdges()
	{
		// Store top edge
		m_top = m_tileData.front();

		// Store bottom edge
		m_bottom = m_tileData.back();

		for (string line : m_tileData)
		{
			// Store left edge
			m_left.push_back(line.front());

			// Store right edge
			m_right.push_back(line.back());
		}
	}

	void FlipHorizontally()
	{
		for (int i = m_tileData.size() - 1; i >= 0; --i)
		{
			for (int j = 0; j < (int)m_tileData[i].length() / 2; j++)
			{
				swap(m_tileData[i][j], m_tileData[i][m_tileData[i].length() - j - 1]);
			}
		}

		CacheEdges();
	}

	void FlipVertically()
	{
		for (int i = 0; i < (int)m_tileData.size() / 2; i++)
		{
			swap(m_tileData[i], m_tileData[m_tileData.size() - i - 1]);
		}
		CacheEdges();
	}

	void Rotate90()
	{
		vector<string> oldData = m_tileData;
		for (int i = 0; i < oldData.size(); ++i)
		{
			string oldLine = oldData[i];
			for (int j = 0; j < oldLine.length(); ++j)
			{
				m_tileData[j][oldLine.length() - i - 1] = oldLine[j];
			}
		}
		CacheEdges();
	}

	void GetNumberValidNeighbours()
	{
		vector<Tile*> leftNeighbours;
		GetNeighbours(Neighbour::Left, leftNeighbours);

		vector<Tile*> rightNeighbours;
		GetNeighbours(Neighbour::Right, rightNeighbours);

		vector<Tile*> topNeighbours;
		GetNeighbours(Neighbour::Top, topNeighbours);

		vector<Tile*> bottomNeighbours;
		GetNeighbours(Neighbour::Bottom, bottomNeighbours);

		int i = 0;
	}

	void PrintTile()
	{
		cout << endl << "Tile " << m_id << ": " << endl;
		for (string data : m_tileData)
		{
			cout << data << endl;
		}
	}

private:
	int m_id;

	vector<string> m_tileData;

	string m_top;
	string m_bottom;
	string m_left;
	string m_right;
};

const char* c_inputFile = "Input.txt";
const char* c_testFile = "Test.txt";

int main()
{
	ifstream inputStream(c_inputFile);
	if (inputStream.is_open())
	{
		string line;
		vector<string> lines;

		// Get fields
		while (getline(inputStream, line))
		{
			if (line.empty())
			{
				m_tiles.push_back(new Tile(lines));
				lines.clear();
			}
			else
			{
				lines.push_back(line);
			}
		}
		m_tiles.push_back(new Tile(lines));
	}

	m_tiles.front()->PrintTile();
	m_tiles.front()->FlipVertically();
	m_tiles.front()->PrintTile();
	m_tiles.front()->FlipHorizontally();
	m_tiles.front()->PrintTile();
	m_tiles.front()->Rotate90();
	m_tiles.front()->PrintTile();

	m_tiles.front()->GetNumberValidNeighbours();


	// Delete any tiles
	for (int i = 0; i < (int)m_tiles.size(); ++i)
	{
		delete m_tiles[i];
		m_tiles[i] = nullptr;
	}
	m_tiles.clear();
}
