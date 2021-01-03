// 20-JurassicJigsaw.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <stdint.h>
#include <cassert>

using namespace std;

enum Direction
{
	Top = 1 << 0,
	Bottom = 1 << 1,
	Left = 1 << 2,
	Right = 1 << 3
};

class PuzzlePiece
{
public:

	struct Edge
	{
		Edge() = default;

		Edge(const Edge& other)
		{
			m_data = other.m_data;
			m_connectedTo = other.m_connectedTo;
		}

		string m_data;
		PuzzlePiece* m_connectedTo;
	};

	PuzzlePiece(vector<string> lines)
	{
		string idLine = lines.front();
		m_id = stoi(idLine.substr(5));

		for (int i = 1; i < (int)lines.size(); ++i)
		{
			m_pieceData.push_back(lines[i]);
		}
		CacheEdges();
	}

	PuzzlePiece(const PuzzlePiece& other)
	{
		m_id = other.m_id;
		m_pieceData = other.m_pieceData;

		m_top = new Edge(*other.m_top);
		m_left = new Edge(*other.m_left);
		m_bottom = new Edge(*other.m_bottom);
		m_right = new Edge(*other.m_right);
	}

	~PuzzlePiece()
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

	// Create new edge data from full piece data
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
		m_top->m_data = m_pieceData.front();

		// Store bottom edge
		m_bottom->m_data = m_pieceData.back();

		m_left->m_data.clear();
		m_right->m_data.clear();
		for (string line : m_pieceData)
		{
			// Store left edge
			m_left->m_data.push_back(line.front());

			// Store right edge
			m_right->m_data.push_back(line.back());
		}
	}

	// Flip piece data horizontally
	void FlipPiece()
	{
		for (int i = m_pieceData.size() - 1; i >= 0; --i)
		{
			for (int j = 0; j < (int)m_pieceData[i].length() / 2; j++)
			{
				swap(m_pieceData[i][j], m_pieceData[i][m_pieceData[i].length() - j - 1]);
			}
		}

		CacheEdges();

		// Move found neighbour states
		auto tempState = m_left->m_connectedTo;
		m_left->m_connectedTo = m_right->m_connectedTo;
		m_right->m_connectedTo = tempState;
	}

	// Rotate piece 90 degrees clockwise
	void RotatePiece()
	{
		vector<string> oldData = m_pieceData;
		for (int i = 0; i < (int)oldData.size(); ++i)
		{
			string oldLine = oldData[i];
			for (int j = 0; j < (int)oldLine.length(); ++j)
			{
				m_pieceData[j][oldLine.length() - i - 1] = oldLine[j];
			}
		}
		CacheEdges();

		// Move found neighbour states
		auto tempState = m_top->m_connectedTo;
		m_top->m_connectedTo = m_left->m_connectedTo;
		m_left->m_connectedTo = m_bottom->m_connectedTo;
		m_bottom->m_connectedTo = m_right->m_connectedTo;
		m_right->m_connectedTo = tempState;
	}

	// Attempt to connect with piece in specified direction if compatible
	void TryConnectInDirection(PuzzlePiece* otherPiece, Direction direction)
	{
		switch (direction)
		{
			case Direction::Top:
			{
				if (m_top->m_data == otherPiece->m_bottom->m_data)
					m_top->m_connectedTo = otherPiece;
			}
			break;
			case Direction::Bottom:
			{
				if (m_bottom->m_data == otherPiece->m_top->m_data)
					m_bottom->m_connectedTo = otherPiece;
			}
			break;
			case Direction::Left:
			{
				if (m_left->m_data == otherPiece->m_right->m_data)
					m_left->m_connectedTo = otherPiece;
			}
			break;
			case Direction::Right:
			{
				if (m_right->m_data == otherPiece->m_left->m_data)
					m_right->m_connectedTo = otherPiece;
			}
			break;
		}
	}

	int GetOuterEdgeCount()
	{
		// Get amount of edges with no neighbours
		int outerEdges = 0;
		outerEdges += m_top->m_connectedTo == nullptr ? 1 : 0;
		outerEdges += m_bottom->m_connectedTo == nullptr ? 1 : 0;
		outerEdges += m_left->m_connectedTo == nullptr ? 1 : 0;
		outerEdges += m_right->m_connectedTo == nullptr ? 1 : 0;
		return outerEdges;
	}

	void GetBorderlessTileData(vector<string>& borderlessData) const
	{
		borderlessData = m_pieceData;

		// Remove top and bottom border
		borderlessData.erase(borderlessData.begin());
		borderlessData.erase(borderlessData.begin() + borderlessData.size()-1);

		for (int i = 0; i < (int)borderlessData.size(); ++i)
		{
			// Remove left border
			borderlessData[i].erase(borderlessData[i].begin());

			// Remove right border
			borderlessData[i].erase(borderlessData[i].begin() + borderlessData[i].size()-1);
		}
	}

	// Return piece connected to this piece in a certain direction
	PuzzlePiece* GetConnectedPiece(Direction dir)
	{
		switch (dir)
		{
			case Direction::Top: return m_top->m_connectedTo;
			case Direction::Bottom: return m_bottom->m_connectedTo;
			case Direction::Left: return m_left->m_connectedTo;
			case Direction::Right: return m_right->m_connectedTo;
		}
		return nullptr;
	}

	int GetID() const { return m_id; }

private:
	int m_id;

	vector<string> m_pieceData;

	Edge* m_top;
	Edge* m_bottom;
	Edge* m_left;
	Edge* m_right;
};

// Class to represent an image assembled out of puzzle pieces
class Image
{
public:
	Image(const vector<vector<PuzzlePiece*>>& attachedPieces)
	{
		for (int pieceRow = 0; pieceRow < (int)attachedPieces.size(); ++pieceRow)
		{
			for (int pieceCol = 0; pieceCol < (int)attachedPieces[pieceRow].size(); ++pieceCol)
			{
				PuzzlePiece* tile = attachedPieces[pieceRow][pieceCol];

				// Assemble with removed borders
				vector<string> borderlessData;
				tile->GetBorderlessTileData(borderlessData);

				for (int i = 0; i < (int)borderlessData.size(); ++i)
				{
					if (pieceCol == 0)
					{
						m_imageData.push_back(borderlessData[i]);
					}
					else
					{
						int dataRow = pieceRow * borderlessData.size() + i;
						m_imageData[dataRow].append(borderlessData[i]);
					}
				}
			}
		}
	}

	Image(const Image& otherImage)
	{
		m_imageData = otherImage.m_imageData;
	}

	void MarkSeaMonsters(vector<pair<int, int>>& seaMonsterOffsets)
	{
		// Find all instances of sea monsters in the image and mark with 'O'
		int seaMonsterWidth = c_seaMonsterData.front().size();
		int seaMonsterHeight = c_seaMonsterData.size();

		for (int pieceRow = 0; pieceRow < (int)m_imageData.size(); ++pieceRow)
		{
			for (int pieceCol = 0; pieceCol < (int)m_imageData[pieceRow].size(); ++pieceCol)
			{
				if (pieceCol + seaMonsterWidth > (int)m_imageData[pieceRow].size())
					continue;

				if (pieceRow + seaMonsterHeight > (int)m_imageData.size())
					continue;

				int currentMonsterRow = 0;
				int currentMonsterCol = 0;

				bool foundMonster = false;
				while (!foundMonster)
				{
					if (c_seaMonsterData[currentMonsterRow][currentMonsterCol] == '#')
					{
						// Must be a '#' so it must match to a '#' in the image data
						if (m_imageData[pieceRow + currentMonsterRow][pieceCol + currentMonsterCol] != '#')
						{
							break;
						}
					}

					// Advance current monster row and col
					currentMonsterCol++;
					if (currentMonsterCol == seaMonsterWidth)
					{
						currentMonsterCol = 0;
						currentMonsterRow++;
					}

					if (currentMonsterRow == seaMonsterHeight)
					{
						foundMonster = true;
						break;
					}
				}

				if (foundMonster)
				{
					seaMonsterOffsets.push_back(pair<int, int>(pieceRow, pieceCol));
				}
			}
		}

		if (!seaMonsterOffsets.empty())
		{
			for (auto& offset : seaMonsterOffsets)
			{
				int startRow = offset.first;
				int startCol = offset.second;

				int currentMonsterRow = 0;
				int currentMonsterCol = 0;

				bool foundMonster = false;
				while (!foundMonster)
				{
					if (c_seaMonsterData[currentMonsterRow][currentMonsterCol] == '#')
					{
						// Must be a '#' so it must match to a '#' in the image data
						if (m_imageData[startRow + currentMonsterRow][startCol + currentMonsterCol] == '#')
							m_imageData[startRow + currentMonsterRow][startCol + currentMonsterCol] = 'O';
					}

					// Advance current monster row and col
					currentMonsterCol++;
					if (currentMonsterCol == seaMonsterWidth)
					{
						currentMonsterCol = 0;
						currentMonsterRow++;
					}

					if (currentMonsterRow == seaMonsterHeight)
					{
						break;
					}
				}
			}
		}
	}

	int GetWaterRoughness() const
	{
		// Water roughness = amount of '#' left after sea monsters have been marked
		int waterRoughness = 0;
		for (auto& row : m_imageData)
		{
			for (auto& data : row)
			{
				if (data == '#')
					waterRoughness++;
			}
		}
		return waterRoughness;
	}

	void PrintImage() const
	{
		cout << endl;
		for (auto line : m_imageData)
		{
			cout << line << endl;
		}
		cout << endl;
	}

	void RotateImage()
	{
		// Rotate image 90 degrees clockwise
		vector<string> oldData = m_imageData;
		for (int i = 0; i < (int)oldData.size(); ++i)
		{
			string oldLine = oldData[i];
			for (int j = 0; j < (int)oldLine.length(); ++j)
			{
				m_imageData[j][oldLine.length() - i - 1] = oldLine[j];
			}
		}
	}

	void FlipImage()
	{
		// Flip image horizontally
		for (int i = m_imageData.size() - 1; i >= 0; --i)
		{
			for (int j = 0; j < (int)m_imageData[i].length() / 2; j++)
			{
				swap(m_imageData[i][j], m_imageData[i][m_imageData[i].length() - j - 1]);
			}
		}
	}

private:
	vector<string> m_imageData;

	const vector<string> c_seaMonsterData =
	{
		"                  # ",
		"#    ##    ##    ###",
		" #  #  #  #  #  #   "
	};
};

// All puzzle pieces, including rotated and flipped variants
vector<PuzzlePiece*> m_allPieces;

void ConnectPiecesToNeighbours()
{
	for (auto& piece : m_allPieces)
	{
		int pieceID = piece->GetID();

		for (auto& pieceToCheck : m_allPieces)
		{
			if (pieceToCheck->GetID() == pieceID)
				continue;

			// Try to connect in all directions to this other piece
			piece->TryConnectInDirection(pieceToCheck, Direction::Top);
			piece->TryConnectInDirection(pieceToCheck, Direction::Bottom);
			piece->TryConnectInDirection(pieceToCheck, Direction::Left);
			piece->TryConnectInDirection(pieceToCheck, Direction::Right);
		}
	}
}

void GetEdgeAndCornerPieces(vector<PuzzlePiece*>& edgePieces, vector<PuzzlePiece*>& cornerPieces)
{
	// Corner pieces will have 2 outer sides
	// Edge pieces will have 1 outer side
	for (PuzzlePiece* piece : m_allPieces)
	{
		int outerEdgeCount = piece->GetOuterEdgeCount();
		if (outerEdgeCount == 2)
			cornerPieces.push_back(piece);
		else if (outerEdgeCount == 1)
			edgePieces.push_back(piece);
	}
}

// Get top-left piece to start off the jigsaw
PuzzlePiece* GetStartingPiece(int pieceID = -1)
{
	vector<PuzzlePiece*> matchingPieces;
	for (auto& piece : m_allPieces)
	{
		if (piece->GetID() != pieceID)
			continue;

		bool hasBottomPiece = piece->GetConnectedPiece(Direction::Bottom) != nullptr;
		bool hasRightPiece = piece->GetConnectedPiece(Direction::Right) != nullptr;

		if (hasBottomPiece && hasRightPiece)
			matchingPieces.push_back(piece);
	}

	// Return first matching piece
	return !matchingPieces.empty() ? matchingPieces.front() : nullptr;
}

const char* c_inputFile = "Input.txt";
const char* c_testFile = "Test.txt";

int main()
{
	ifstream inputStream(c_inputFile);
	vector<PuzzlePiece*> initialPieces;
	if (inputStream.is_open())
	{
		string line;
		vector<string> lines;

		// Get pieces
		while (getline(inputStream, line))
		{
			if (line.empty())
			{
				initialPieces.push_back(new PuzzlePiece(lines));
				lines.clear();
			}
			else
			{
				lines.push_back(line);
			}
		}
		initialPieces.push_back(new PuzzlePiece(lines));
	}

	// Store all 8 configurations of each piece
	for (PuzzlePiece* piece : initialPieces)
	{
		// Original
		m_allPieces.push_back(piece);

		// All rotations
		PuzzlePiece* rotatedPiece = new PuzzlePiece(*piece);
		for (int i = 0; i < 3; ++i)
		{
			rotatedPiece->RotatePiece();
			m_allPieces.push_back(rotatedPiece);
			rotatedPiece = new PuzzlePiece(*rotatedPiece);
		}
		rotatedPiece->RotatePiece();

		// Flipped
		PuzzlePiece* flippedPiece = rotatedPiece;
		flippedPiece->FlipPiece();
		m_allPieces.push_back(flippedPiece);

		// All flipped rotations
		rotatedPiece = new PuzzlePiece(*flippedPiece);
		for (int i = 0; i < 3; ++i)
		{
			rotatedPiece->RotatePiece();
			m_allPieces.push_back(rotatedPiece);
			rotatedPiece = new PuzzlePiece(*rotatedPiece);
		}
	}

	// Assumes theres only one unique other piece that matches each piece
	// Connects the pieces together
	ConnectPiecesToNeighbours();

	// Get edge pieces to determine size of jigsaw
	// And corner pieces to start off the jigsaw
	vector<PuzzlePiece*> cornerPieces;
	vector<PuzzlePiece*> edgePieces;
	GetEdgeAndCornerPieces(edgePieces, cornerPieces);

	// Start assembling the jigsaw!
	vector<PuzzlePiece*> piecesBag = m_allPieces;
	int jigsawWidth = (edgePieces.size() / (8 * 4)) + 2;
	vector<vector<PuzzlePiece*>> jigsaw;
	jigsaw.resize(jigsawWidth);
	for (auto& pieceRow : jigsaw)
	{
		pieceRow.resize(jigsawWidth);
	}

	// ASSEMBLE!
	int row = 0;
	int col = 0;
	while (row < jigsawWidth)
	{
		while (col < jigsawWidth)
		{
			if (row == 0 && col == 0)
			{
				// Get top-left corner piece
				jigsaw[row][col] = GetStartingPiece(cornerPieces[0]->GetID());
			}
			else if (row == 0)
			{
				// Top row, fill from left of last one
				jigsaw[row][col] = jigsaw[row][col - 1]->GetConnectedPiece(Direction::Right);
			}
			else
			{
				// Not top row, fill in from bottom of piece above
				jigsaw[row][col] = jigsaw[row-1][col]->GetConnectedPiece(Direction::Bottom);
			}

			col++;
		}
		row++;
		col = 0;
	}

	// Print assembled jigsaw
	cout << endl << "Assembled jigsaw: " << endl;
	for (auto& pieceRow : jigsaw)
	{
		for (PuzzlePiece* piece : pieceRow)
		{
			if (piece != nullptr)
				cout << piece->GetID() << "   ";
			else
			{
				cout << "       ";
			}
		}
		cout << endl;
	}
	cout << endl;

	{
		// Part One
		cout << endl << "=== Part 1 ===" << endl;
		vector<int> uniqueCornerIDs =
		{
			jigsaw[0][0]->GetID(),
			jigsaw[0][jigsawWidth - 1]->GetID(),
			jigsaw[jigsawWidth - 1][0]->GetID(),
			jigsaw[jigsawWidth - 1][jigsawWidth - 1]->GetID()
		};

		uint64_t cornerProduct = 0;
		if (!uniqueCornerIDs.empty())
			cornerProduct = uniqueCornerIDs.front();

		for (int i = 1; i < (int)uniqueCornerIDs.size(); ++i)
		{
			cornerProduct *= uniqueCornerIDs[i];
		}
		cout << "Corner tile product is: " << cornerProduct << endl;
	}

	vector<Image*> images;
	{
		// Part Two
		cout << endl << "=== Part 2 ===" << endl;

		{
			// Create all combinations of flipped and rotated images from completed jigsaw
			Image* image = new Image(jigsaw);
			images.push_back(image);

			// All rotations
			Image* rotatedImage = new Image(*image);
			for (int i = 0; i < 3; ++i)
			{
				rotatedImage->RotateImage();
				images.push_back(rotatedImage);
				rotatedImage = new Image(*rotatedImage);
			}

			// Flipped
			Image* flippedImage = new Image(*image);
			flippedImage->FlipImage();
			images.push_back(flippedImage);

			// All rotations while flipped
			rotatedImage = new Image(*flippedImage);
			for (int i = 0; i < 3; ++i)
			{
				rotatedImage->RotateImage();
				images.push_back(rotatedImage);
				rotatedImage = new Image(*rotatedImage);
			}
		}

		// For each image, search for sea monsters
		for (Image* img : images)
		{
			vector<pair<int, int>> offsets;
			img->MarkSeaMonsters(offsets);

			if (!offsets.empty())
			{
				int roughness = img->GetWaterRoughness();
				cout << "Found " << offsets.size() << " sea monsters" << endl;
				cout << "Water roughness: " << roughness << endl;
			}
		}
	}

	// Delete any images
	for (int i = 0; i < (int)images.size(); ++i)
	{
		delete images[i];
		images[i] = nullptr;
	}

	// Delete any pieces
	for (int i = 0; i < (int)m_allPieces.size(); ++i)
	{
		delete m_allPieces[i];
		m_allPieces[i] = nullptr;
	}
	m_allPieces.clear();
}
