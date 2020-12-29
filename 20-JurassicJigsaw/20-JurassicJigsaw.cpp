// 20-JurassicJigsaw.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stdint.h>

using namespace std;

enum Direction
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

	enum FlipType
	{
		Original = 0,
		FlipHoriz = 1,
		FlipVert = 2
	};

	struct Edge
	{
		string m_data;
		bool m_foundNeighbour;
	};

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

	~Tile()
	{
		delete m_top;
		delete m_bottom;
		delete m_left;
		delete m_right;

		m_top = nullptr;
		m_bottom = nullptr;
		m_left = nullptr;
		m_right = nullptr;
	}

	bool HasValidNeighbour(Direction neighbour) const
	{
		Edge& edge = GetEdge(neighbour);
		string tileString = edge.m_data;

		for (Tile* tile : m_tiles)
		{
			if (tile == this)
				continue;

			string oppositeString = tile->GetOppositeEdge(neighbour).m_data;

			if (tileString == oppositeString)
			{
				edge.m_foundNeighbour = true;
				return true;
			}
		}
		return false;
	}

	void GetNeighbours(Direction neighbour, vector<Tile*>& neighbourTiles) const
	{
		Edge& edge = GetEdge(neighbour);
		string tileString = edge.m_data;

		for (Tile* tile : m_tiles)
		{
			if (tile == this)
				continue;

			string oppositeString = tile->GetOppositeEdge(neighbour).m_data;

			if (tileString == oppositeString)
				neighbourTiles.push_back(tile);
		}
	}

	Edge& GetEdge(Direction neighbour) const
	{
		switch (neighbour)
		{
			case Top:		return *m_top;
			case Bottom:	return *m_bottom;
			case Left:		return *m_left;
			default:		return *m_right;
		}
	}

	Edge& GetOppositeEdge(Direction neighbour) const
	{
		switch (neighbour)
		{
			case Top:		return *m_bottom;
			case Bottom:	return *m_top;
			case Left:		return *m_right;
			default:		return *m_left;
		}
	}

	void CacheEdges()
	{
		if (m_top == nullptr)
			m_top = new Edge();

		if (m_bottom == nullptr)
			m_bottom = new Edge();

		if (m_left == nullptr)
			m_left = new Edge();

		if (m_right == nullptr)
			m_right = new Edge();

		// Store top edge
		m_top->m_data = m_tileData.front();

		// Store bottom edge
		m_bottom->m_data = m_tileData.back();

		m_left->m_data.clear();
		m_right->m_data.clear();
		for (string line : m_tileData)
		{
			// Store left edge
			m_left->m_data.push_back(line.front());

			// Store right edge
			m_right->m_data.push_back(line.back());
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

		// Move found neighbour states
		bool tempState = m_left->m_foundNeighbour;
		m_left->m_foundNeighbour = m_right->m_foundNeighbour;
		m_right->m_foundNeighbour = tempState;
	}

	void FlipVertically()
	{
		for (int i = 0; i < (int)m_tileData.size() / 2; i++)
		{
			swap(m_tileData[i], m_tileData[m_tileData.size() - i - 1]);
		}

		CacheEdges();

		// Move found neighbour states
		bool tempState = m_top->m_foundNeighbour;
		m_top->m_foundNeighbour = m_bottom->m_foundNeighbour;
		m_bottom->m_foundNeighbour = tempState;
	}

	void Rotate90()
	{
		vector<string> oldData = m_tileData;
		for (int i = 0; i < (int)oldData.size(); ++i)
		{
			string oldLine = oldData[i];
			for (int j = 0; j < (int)oldLine.length(); ++j)
			{
				m_tileData[j][oldLine.length() - i - 1] = oldLine[j];
			}
		}
		CacheEdges();

		// Move found neighbour states
		bool tempState = m_top->m_foundNeighbour;
		m_top->m_foundNeighbour = m_left->m_foundNeighbour;
		m_left->m_foundNeighbour = m_bottom->m_foundNeighbour;
		m_bottom->m_foundNeighbour = m_right->m_foundNeighbour;
		m_right->m_foundNeighbour = tempState;
	}

	void SetFlipState(int flipState)
	{
		if (m_flipState == flipState)
			return;

		if (m_flipState & FlipHoriz)
			FlipHorizontally();

		if (m_flipState & FlipVert)
			FlipVertically();

		if (flipState & FlipHoriz)
			FlipHorizontally();

		if (flipState & FlipVert)
			FlipVertically();

		m_flipState = flipState;
	}

	int GetNeighbourlessEdgeCount()
	{
		int neighbourlessEdges = 4;
		for (int i = 0; i < 4; ++i)
		{
			switch (i)
			{
				case 0:
				break;
				case 1:
				{
					SetFlipState(FlipHoriz);
				}
				break;
				case 2:
				{
					SetFlipState(FlipVert);
				}
				break;
				case 3:
				{
					SetFlipState(FlipHoriz | FlipVert);
				}
				break;
			}

			for (int rotation = 0; rotation < 4; ++rotation)
			{
				HasValidNeighbour(Direction::Left) ? 1 : 0;
				HasValidNeighbour(Direction::Right) ? 1 : 0;
				HasValidNeighbour(Direction::Top) ? 1 : 0;
				HasValidNeighbour(Direction::Bottom) ? 1 : 0;

				Rotate90();
			}
		}
		SetFlipState(Original);

		neighbourlessEdges -= m_top->m_foundNeighbour ? 1 : 0;
		neighbourlessEdges -= m_bottom->m_foundNeighbour ? 1 : 0;
		neighbourlessEdges -= m_left->m_foundNeighbour ? 1 : 0;
		neighbourlessEdges -= m_right->m_foundNeighbour ? 1 : 0;
		return neighbourlessEdges;
	}

	void PrintTile()
	{
		cout << endl << "Tile " << m_id << ": " << endl;
		for (string data : m_tileData)
		{
			cout << data << endl;
		}
	}

	int GetID() const { return m_id; }

private:
	int m_id;

	vector<string> m_tileData;

	Edge* m_top;
	Edge* m_bottom;
	Edge* m_left;
	Edge* m_right;

	int m_flipState;
};

const char* c_inputFile = "Input.txt";
const char* c_testFile = "Test.txt";


void GetEdgesAndCorners(vector<Tile*>& edgeTiles, vector<Tile*>& cornerTiles)
{
	// Edge pieces will have one side with no neighbours
	// Corner pieces will have two sides with no neighbours

	// Check normal, flip horiz, flip vert, flip horiz and vert
	for (Tile* tile : m_tiles)
	{
		int neighbourlessCount = tile->GetNeighbourlessEdgeCount();
		cout << "Tile " << tile->GetID() << " has " << neighbourlessCount << " empty edges" << endl;
		if (neighbourlessCount == 2)
			cornerTiles.push_back(tile);
		if (neighbourlessCount == 1)
			edgeTiles.push_back(tile);
	}
}

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

	vector<Tile*> edges;
	vector<Tile*> corners;
	GetEdgesAndCorners(edges, corners);

	if (corners.size() != 4)
		cout << "ERROR: Did not find 4 corner tiles" << endl;

	uint64_t cornerProduct = 0;
	if (!corners.empty())
		cornerProduct = corners.front()->GetID();

	for (int i = 1; i < (int)corners.size(); ++i)
	{
		cornerProduct *= corners[i]->GetID();;
	}

	for (int i = 0; i < (int)corners.size(); ++i)
	{
		cout << "Corner [" << i << "] = " << corners[i]->GetID() << endl;
	}

	cout << "Corner tile product is: " << cornerProduct << endl;

	// Delete any tiles
	for (int i = 0; i < (int)m_tiles.size(); ++i)
	{
		delete m_tiles[i];
		m_tiles[i] = nullptr;
	}
	m_tiles.clear();
}
