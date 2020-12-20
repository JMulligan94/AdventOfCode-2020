// 17-ConwayCubes.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

const char* c_inputFile = "Input.txt";
const char* c_testFile = "Test.txt";

const int c_numCycles = 6;


/////////////////////////////////////////
// CellCoords
/////////////////////////////////////////
struct CellCoords
{
	CellCoords() = default;

	CellCoords(int x, int y, int z, int w = 0)
		: m_x(x)
		, m_y(y)
		, m_z(z)
		, m_w(w)
	{}

	int m_x;
	int m_y;
	int m_z;
	int m_w;
};


/////////////////////////////////////////
// CellChange
/////////////////////////////////////////
struct CellChange
{
	CellChange() = default;

	CellChange(const CellCoords& coords, bool newValue)
	{
		m_coords = coords;
		m_newValue = newValue;
	}

	CellCoords m_coords;
	bool m_newValue;
};


/////////////////////////////////////////
// ConwayCubeSlice
/////////////////////////////////////////
struct ConwayCubeSlice
{
	ConwayCubeSlice(int width)
	{
		m_cells.resize(width);
		for (int i = 0; i < width; ++i)
		{
			m_cells[i].resize(width);
		}
	}

	void SetCells(const vector<string>& lines)
	{
		for (int row = 0; row < lines.size(); ++row)
		{
			string line = lines[row];
			for (int cellCharIdx = 0; cellCharIdx < line.size(); ++cellCharIdx)
			{
				char cellChar = line[cellCharIdx];
				m_cells[row][cellCharIdx] = (cellChar == '#');
			}
		}
	}

	bool GetCell(int x, int y) const
	{
		if (y >= m_cells.size())
			return false;

		if (x >= m_cells[y].size())
			return false;

		return m_cells[y][x];
	}

	void SetCell(int x, int y, bool value)
	{
		m_cells[y][x] = value;
	}

	void PrintSlice() const
	{
		for (int row = 0; row < m_cells.size(); ++row)
		{
			for (int col = 0; col < m_cells[row].size(); ++col)
			{
				char cellChar = GetCell(col, row) ? '#' : '.';
				cout << cellChar;
			}
			cout << endl;
		}
	}

	void GrowSlice()
	{
		int currentWidth = (int)m_cells.size();
		int newWidth = currentWidth + 2;
		// Grow all existing rows
		for (int y = 0; y < m_cells.size(); ++y)
		{
			m_cells[y].insert(m_cells[y].begin(), false);
			m_cells[y].push_back(false);
		}

		// Add new row to top and bottom
		m_cells.insert(m_cells.begin(), vector<bool>(newWidth));
		m_cells.push_back(vector<bool>(newWidth));
	}

	int GetNumberActiveCells() const
	{
		int numActive = 0;
		for (int y = 0; y < m_cells.size(); ++y)
		{
			for (int x = 0; x < m_cells[y].size(); ++x)
			{
				numActive += m_cells[y][x] ? 1 : 0;
			}
		}
		return numActive;
	}

	vector<vector<bool>> m_cells;
};


/////////////////////////////////////////
// ConwayCube
/////////////////////////////////////////
class ConwayCube
{
	friend class ConwayCubeArray;
public:
	ConwayCube(int width)
	{
		m_width = width;

		for (int i = 0; i < width; ++i)
		{
			m_slices.push_back(new ConwayCubeSlice(width));
		}

	}

	~ConwayCube()
	{
		for (int i = 0; i < m_slices.size(); ++i)
		{
			delete m_slices[i];
			m_slices[i] = nullptr;
		}
		m_slices.clear();
	}

private:
	bool GetCell(int x, int y, int z) const
	{
		for (int i = 0; i < m_slices.size(); ++i)
		{
			if (i == z)
			{
				return m_slices[i]->GetCell(x, y);
			}
		}

		return false;
	}

	void SetCell(CellCoords coords, bool value)
	{
		SetCell(coords.m_x, coords.m_y, coords.m_z, value);
	}

	void SetCell(int x, int y, int z, bool value)
	{
		for (int i = 0; i < m_slices.size(); ++i)
		{
			if (i == z)
			{
				m_slices[i]->SetCell(x, y, value);
				break;
			}
		}
	}

	int GetWidth() const
	{
		return m_width;
	}

	void GetNeighbours(vector<CellCoords>& cells, int x, int y, int z)
	{
		for (int deltaZ = -1; deltaZ <= 1; ++deltaZ)
		{
			cells.push_back(CellCoords(x + 1, y, z + deltaZ));
			cells.push_back(CellCoords(x + 1, y + 1, z + deltaZ));
			cells.push_back(CellCoords(x, y + 1, z + deltaZ));
			cells.push_back(CellCoords(x + 1, y - 1, z + deltaZ));
			cells.push_back(CellCoords(x - 1, y, z + deltaZ));
			cells.push_back(CellCoords(x - 1, y - 1, z + deltaZ));
			cells.push_back(CellCoords(x, y - 1, z + deltaZ));
			cells.push_back(CellCoords(x - 1, y + 1, z + deltaZ));

			if (deltaZ != 0)
				cells.push_back(CellCoords(x, y, z + deltaZ));
		}
	}

	int GetNumActiveNeighbours(CellCoords coords, bool includeCoords)
	{
		int numActive = 0;

		vector<CellCoords> neighbouringCells;
		GetNeighbours(neighbouringCells, coords.m_x, coords.m_y, coords.m_z);

		if (includeCoords)
			neighbouringCells.push_back(coords);

		for (CellCoords cellCoords : neighbouringCells)
		{
			numActive += GetCell(cellCoords) ? 1 : 0;
		}
		return numActive;
	}

	void GrowCube()
	{
		m_width += 2;

		for (int i = 0; i < m_slices.size(); ++i)
		{
			m_slices[i]->GrowSlice();
		}
		m_slices.insert(m_slices.begin(), new ConwayCubeSlice(m_width));
		m_slices.push_back(new ConwayCubeSlice(m_width));
	}

public:

	void SetStartingSlice(const vector<string>& cubeLines)
	{
		int halfWidth = m_width / 2;
		m_slices[halfWidth]->SetCells(cubeLines);
	}

	bool GetCell(const CellCoords& coords)
	{
		return GetCell(coords.m_x, coords.m_y, coords.m_z);
	}

	bool GetCell(int x, int y, int z)
	{
		if (z < 0 || z >= m_slices.size())
			return false;

		return m_slices[z]->GetCell(x, y);
	}

	void DoCycle()
	{
		GrowCube();

		vector<CellChange> changesToMake;
		for (int z = 0; z < m_slices.size(); ++z)
		{
			int cubeWidth = GetWidth();
			for (int x = 0; x < cubeWidth; ++x)
			{
				for (int y = 0; y < cubeWidth; ++y)
				{
					CellCoords coords(x, y, z);
					bool cellState = GetCell(coords);
					int activeNeighbours = GetNumActiveNeighbours(coords, false);
					if (cellState)
					{
						// If a cube is active and exactly 2 or 3 of its neighbors are also active, the cube remains active. Otherwise, the cube becomes inactive.
						if (activeNeighbours != 2 && activeNeighbours != 3)
							changesToMake.push_back(CellChange(coords, false));
					}
					else
					{
						// If a cube is inactive but exactly 3 of its neighbors are active, the cube becomes active. Otherwise, the cube remains inactive.
						if (activeNeighbours == 3)
							changesToMake.push_back(CellChange(coords, true));
					}
				}
			}
		}

		// During a cycle, all cubes simultaneously change their state according to the following rules :
		for (CellChange change : changesToMake)
		{
			SetCell(change.m_coords, change.m_newValue);
		}
	}

	void PrintCube() const
	{
		int halfWidth = m_width / 2;
		for (int i = 0; i < m_slices.size(); ++i)
		{
			cout << endl << "z=" << i - halfWidth << endl;
			m_slices[i]->PrintSlice();
		}
	}

	int GetNumberActiveCells() const
	{
		int cellCount = 0;
		for (ConwayCubeSlice* slice : m_slices)
		{
			cellCount += slice->GetNumberActiveCells();
		}
		return cellCount;
	}

private:
	vector<ConwayCubeSlice*> m_slices;
	int m_width;
};


/////////////////////////////////////////
// ConwayCubeArray
/////////////////////////////////////////
class ConwayCubeArray
{
public:
	ConwayCubeArray(int width)
	{
		m_width = width;
		for (int i = 0; i < width; ++i)
		{
			m_cubes.push_back(new ConwayCube(width));
		}
	}

	~ConwayCubeArray()
	{
		for (int i = 0; i < m_cubes.size(); ++i)
		{
			delete m_cubes[i];
			m_cubes[i] = nullptr;
		}
	}

	void GrowCubeArray()
	{
		for (int i = 0; i < m_width; ++i)
		{
			m_cubes[i]->GrowCube();
		}
		m_width += 2;
		m_cubes.insert(m_cubes.begin(), new ConwayCube(m_width));
		m_cubes.push_back(new ConwayCube(m_width));
	}

	int GetNumActiveNeighbours(CellCoords coords) const
	{
		int numActiveNeighbours = 0;
		int halfWidth = m_width / 2;
		for (int deltaW = -1; deltaW <= 1; ++deltaW)
		{
			int wCoord = coords.m_w + deltaW;
			if (wCoord < 0 || wCoord >= m_cubes.size())
				continue;

			numActiveNeighbours += m_cubes[wCoord]->GetNumActiveNeighbours(coords, deltaW != 0);
		}
		return numActiveNeighbours;
	}

	bool GetCell(CellCoords coords) const
	{
		return m_cubes[coords.m_w]->GetCell(coords);
	}

	void SetCell(CellCoords coords, bool newValue)
	{
		m_cubes[coords.m_w]->SetCell(coords, newValue);
	}

	void DoCycle()
	{
		GrowCubeArray();

		vector<CellChange> changesToMake;

		int cubeWidth = m_width;
		for (int w = 0; w < m_cubes.size(); ++w)
		{
			for (int z = 0; z < cubeWidth; ++z)
			{
				for (int x = 0; x < cubeWidth; ++x)
				{
					for (int y = 0; y < cubeWidth; ++y)
					{
						CellCoords coords(x, y, z, w);
						bool cellState = GetCell(coords);
						int activeNeighbours = GetNumActiveNeighbours(coords);
						if (cellState)
						{
							// If a cube is active and exactly 2 or 3 of its neighbors are also active, the cube remains active. Otherwise, the cube becomes inactive.
							if (activeNeighbours != 2 && activeNeighbours != 3)
								changesToMake.push_back(CellChange(coords, false));
						}
						else
						{
							// If a cube is inactive but exactly 3 of its neighbors are active, the cube becomes active. Otherwise, the cube remains inactive.
							if (activeNeighbours == 3)
								changesToMake.push_back(CellChange(coords, true));
						}
					}
				}
			}
		}

		int halfWidth = m_width / 2;
		// During a cycle, all cubes simultaneously change their state according to the following rules :
		for (CellChange change : changesToMake)
		{
			SetCell(change.m_coords, change.m_newValue);
		}
	}

	void SetStartingSlice(const vector<string>& lines)
	{
		int halfWidth = m_width / 2;
		m_cubes[halfWidth]->SetStartingSlice(lines);
	}

	void PrintCubeArray()
	{
		int halfWidth = m_width / 2;
		for (int i = 0; i < m_width; ++i)
		{
			cout << endl << "=== w=" << i - halfWidth << " ===" << endl;
			m_cubes[i]->PrintCube();
		}
	}

	int GetNumberActiveCells() const
	{
		int cellCount = 0;
		for (ConwayCube* cube : m_cubes)
		{
			cellCount += cube->GetNumberActiveCells();
		}
		return cellCount;
	}

private:
	int m_width;
	vector<ConwayCube*> m_cubes;
};



int main()
{
	vector<string> lines;
	ifstream inputStream(c_inputFile);
	if (inputStream.is_open())
	{
		string line;

		// Get fields
		while (getline(inputStream, line))
		{
			lines.push_back(line);
		}
	}

	{
		// Part One
		cout << endl << "=== Part 1 ===" << endl;

		ConwayCube* cube = new ConwayCube((int)lines.size());
		cube->SetStartingSlice(lines);

		//cout << "Before any cycles: " << endl;
		//cube->PrintCube();

		for (int i = 1; i <= c_numCycles; ++i)
		{
			cube->DoCycle();

			//cout << endl << endl << "After " << i << " cycle/s:" << endl;
			//cube->PrintCube();
		}

		int numActive = cube->GetNumberActiveCells();
		cout << "The number of active cells after " << c_numCycles << " cycles is: " << numActive << endl << endl;

		delete cube;
		cube = nullptr;
	}

	{
		// Part Two
		cout << endl << "=== Part 2 ===" << endl;

		ConwayCubeArray* conwayCubeArray = new ConwayCubeArray((int)lines.size());
		conwayCubeArray->SetStartingSlice(lines);

		//conwayCubeArray->PrintCubeArray();
		
		for (int i = 1; i <= c_numCycles; ++i)
		{
			conwayCubeArray->DoCycle();

			//cout << endl << endl << "After " << i << " cycle/s:" << endl;
			//conwayCubeArray->PrintCubeArray();
		}

		int numActive = conwayCubeArray->GetNumberActiveCells();
		cout << "The number of active cells after " << c_numCycles << " cycles is: " << numActive << endl << endl;

		delete conwayCubeArray;
		conwayCubeArray = nullptr;
	}
}