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

class PuzzlePiece;
vector<PuzzlePiece*> m_allPieces;
map<int, vector<PuzzlePiece*>> m_pieceConfigMap;

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

	bool ConnectsTop(PuzzlePiece* otherPiece)
	{
		if (m_top->m_data == otherPiece->m_bottom->m_data)
		{
			m_top->m_connectedTo = otherPiece;
			return true;
		}
		return false;
	}

	bool ConnectsBottom(PuzzlePiece* otherPiece)
	{
		if (m_bottom->m_data == otherPiece->m_top->m_data)
		{
			m_bottom->m_connectedTo = otherPiece;
			return true;
		}
		return false;
	}

	bool ConnectsLeft(PuzzlePiece* otherPiece)
	{
		if (m_left->m_data == otherPiece->m_right->m_data)
		{
			m_left->m_connectedTo = otherPiece;
			return true;
		}
		return false;
	}

	bool ConnectsRight(PuzzlePiece* otherPiece)
	{
		if (m_right->m_data == otherPiece->m_left->m_data)
		{
			m_right->m_connectedTo = otherPiece;
			return true;
		}
		return false;
	}

	int GetOuterEdgeCount()
	{
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

	void PrintTile()
	{
		cout << endl << "Piece " << m_id << ": " << endl;
		for (string data : m_pieceData)
		{
			cout << data << endl;
		}
	}

	void PrintNeighbours()
	{
		cout << endl << "=== Tile " << m_id << " neighbours ===" << endl;
		
		cout << "Left: ";
		if (m_left->m_connectedTo != nullptr)
			cout << m_left->m_connectedTo->GetID();
		else
			cout << "-";
		cout << endl;

		cout << "Right: ";
		if (m_right->m_connectedTo != nullptr)
			cout << m_right->m_connectedTo->GetID();
		else
			cout << "-";
		cout << endl;

		cout << "Top: ";
		if (m_top->m_connectedTo != nullptr)
			cout << m_top->m_connectedTo->GetID();
		else
			cout << "-";
		cout << endl;

		cout << "Bottom: ";
		if (m_bottom->m_connectedTo != nullptr)
			cout << m_bottom->m_connectedTo->GetID();
		else
			cout << "-";
		cout << endl << endl;
	}

	int GetID() const { return m_id; }

	PuzzlePiece* GetNeighbourPiece(Direction dir)
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

private:
	int m_id;

	vector<string> m_pieceData;

	Edge* m_top;
	Edge* m_bottom;
	Edge* m_left;
	Edge* m_right;
};

class Image
{
public:
	Image(const vector<vector<PuzzlePiece*>>& attachedPieces)
	{
		cout << "Attached pieces:" << endl;
		for (auto& attachedPieceRow : attachedPieces)
		{
			for (auto& piece : attachedPieceRow)
			{
				piece->PrintTile();
			}
		}
		cout << endl;

		cout << "Attached borderless pieces:" << endl;
		for (int pieceRow = 0; pieceRow < (int)attachedPieces.size(); ++pieceRow)
		{
			for (int pieceCol = 0; pieceCol < (int)attachedPieces[pieceRow].size(); ++pieceCol)
			{
				PuzzlePiece* tile = attachedPieces[pieceRow][pieceCol];
				vector<string> borderlessData;
				tile->GetBorderlessTileData(borderlessData);
				for (auto& piece : borderlessData)
				{
					cout << piece << endl;
				}
				cout << endl;

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

	void SetImageData(const vector<string>& data)
	{
		m_imageData = data;
	}

	void MarkSeaMonsters(vector<pair<int, int>>& seaMonsterOffsets)
	{
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

void ConnectPiecesToNeighbours()
{
	for (auto& piece : m_allPieces)
	{
		int pieceID = piece->GetID();

		for (auto& pieceToCheck : m_allPieces)
		{
			if (pieceToCheck->GetID() == pieceID)
				continue;

			piece->ConnectsTop(pieceToCheck);
			piece->ConnectsBottom(pieceToCheck);
			piece->ConnectsLeft(pieceToCheck);
			piece->ConnectsRight(pieceToCheck);
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

PuzzlePiece* SearchPieces(vector<PuzzlePiece*> bag, const vector<Direction>& neighbourDirections, const vector<int>& neighbourIDs, const vector<Direction>& outerEdgeDirections, int pieceID = -1)
{
	vector<PuzzlePiece*> matchingPieces;
	for (auto& piece : bag)
	{
		if (pieceID >= 0 && piece->GetID() != pieceID)
			continue;

		bool isValidPiece = true;

		for (int i = 0; i < (int)neighbourDirections.size(); ++i)
		{
			Direction dir = neighbourDirections[i];
			PuzzlePiece* neighbourPiece = nullptr;
			switch (dir)
			{
				case Direction::Top:
				{
					neighbourPiece = piece->GetNeighbourPiece(Direction::Top);
				}
				break;
				case Direction::Bottom:
				{
					neighbourPiece = piece->GetNeighbourPiece(Direction::Bottom);
				}
				break;
				case Direction::Left:
				{
					neighbourPiece = piece->GetNeighbourPiece(Direction::Left);
				}
				break;
				case Direction::Right:
				{
					neighbourPiece = piece->GetNeighbourPiece(Direction::Right);
				}
				break;
			}

			if (neighbourPiece == nullptr)
			{
				isValidPiece = false;
				continue;
			}

			if (neighbourIDs[i] >= 0 && neighbourPiece->GetID() != neighbourIDs[i])
			{
				isValidPiece = false;
				continue;
			}
		}

		if (isValidPiece)
			matchingPieces.push_back(piece);
	}

	for (int i = 0; i < (int)matchingPieces.size(); ++i)
	{
		bool isValidPiece = true;
		for (auto& outerEdgeDir : outerEdgeDirections)
		{
			PuzzlePiece* outerPiece = nullptr;
			switch (outerEdgeDir)
			{
				case Direction::Top:
				{
					outerPiece = matchingPieces[i]->GetNeighbourPiece(Direction::Top);
				}
				break;
				case Direction::Bottom:
				{
					outerPiece = matchingPieces[i]->GetNeighbourPiece(Direction::Bottom);
				}
				break;
				case Direction::Left:
				{
					outerPiece = matchingPieces[i]->GetNeighbourPiece(Direction::Left);
				}
				break;
				case Direction::Right:
				{
					outerPiece = matchingPieces[i]->GetNeighbourPiece(Direction::Right);
				}
				break;
			}

			if (outerPiece != nullptr)
			{
				isValidPiece = false;
				break;
			}
		}

		if (!isValidPiece)
		{
			matchingPieces.erase(matchingPieces.begin() + i);
			--i;
		}
	}

	// Return first matching piece
	return !matchingPieces.empty() ? matchingPieces.front() : nullptr;
}

void RemovePiecesFromBag(vector<PuzzlePiece*>& bag, int id)
{
	for (int i = 0; i < (int)bag.size(); ++i)
	{
		if (bag[i]->GetID() == id)
		{
			bag.erase(bag.begin() + i);
			--i;
		}
	}
}

const char* c_inputFile = "Input.txt";
const char* c_testFile = "Test.txt";
const char* c_seaMonsterTestFile = "SeaMonsterTest.txt";

int main()
{
	ifstream inputStream(c_testFile);
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
		vector<PuzzlePiece*> pieceConfigs;
		// Original
		pieceConfigs.push_back(piece);

		// All rotations
		PuzzlePiece* rotatedPiece = new PuzzlePiece(*piece);
		for (int i = 0; i < 3; ++i)
		{
			rotatedPiece->RotatePiece();
			pieceConfigs.push_back(rotatedPiece);
			rotatedPiece = new PuzzlePiece(*rotatedPiece);
		}
		rotatedPiece->RotatePiece();

		// Original rotation but flipped
		PuzzlePiece* flippedPiece = rotatedPiece;
		flippedPiece->FlipPiece();
		pieceConfigs.push_back(flippedPiece);

		// Add all rotations after flip
		rotatedPiece = new PuzzlePiece(*flippedPiece);
		for (int i = 0; i < 3; ++i)
		{
			rotatedPiece->RotatePiece();
			pieceConfigs.push_back(rotatedPiece);
			rotatedPiece = new PuzzlePiece(*rotatedPiece);
		}

		m_pieceConfigMap.emplace(piece->GetID(), pieceConfigs);
		for (auto p : pieceConfigs)
		{
			m_allPieces.push_back(p);
		}
	}

	ConnectPiecesToNeighbours();

	vector<PuzzlePiece*> cornerPieces;
	vector<PuzzlePiece*> edgePieces;
	GetEdgeAndCornerPieces(edgePieces, cornerPieces);

	// Start assembling the jigsaw
	vector<PuzzlePiece*> piecesBag = m_allPieces;
	int jigsawWidth = (edgePieces.size() / (8 * 4)) + 2;
	vector<vector<PuzzlePiece*>> jigsaw;
	jigsaw.resize(jigsawWidth);
	for (auto& pieceRow : jigsaw)
	{
		pieceRow.resize(jigsawWidth);
	}

	int row = 0;
	int col = 0;

	// Get top-left corner piece - any corner with a right & down neighbour
	int startingID = cornerPieces.front()->GetID();
	{
		vector<Direction> neighbourDirections = { Direction::Right, Direction::Bottom };
		vector<int> neighbourIDs = { -1, -1 };
		vector<Direction> outerDirections = { Direction::Left, Direction::Top };
		jigsaw[row][col] = SearchPieces(piecesBag, neighbourDirections, neighbourIDs, outerDirections, startingID);
		RemovePiecesFromBag(piecesBag, jigsaw[row][col]->GetID()); // Remove from bag of pieces
	}
	col++;

	// ASSEMBLE EDGES
	// Top edge
	int lastPieceID = startingID;
	while (col < jigsawWidth)
	{
		if (col < jigsawWidth - 1)
		{
			// Get next edge piece, must connect on left edge to last piece and have no connections on top edge
			vector<Direction> neighbourDirections = { Direction::Left, Direction::Bottom, Direction::Right };
			vector<int> neighbourIDs = { lastPieceID, -1, -1 };
			vector<Direction> outerDirections = { Direction::Top };

			PuzzlePiece* newPiece = SearchPieces(piecesBag, neighbourDirections, neighbourIDs, outerDirections, -1);
			jigsaw[0][col] = newPiece;
			lastPieceID = newPiece->GetID();
			RemovePiecesFromBag(piecesBag, lastPieceID); // Remove from bag of pieces
		}
		else
		{
			// If last in row, must be the top-right corner piece
			vector<Direction> neighbourDirections = { Direction::Left, Direction::Bottom }; // Has left and bottom neighbours
			vector<int> neighbourIDs = { lastPieceID, -1 };	// Left must be prev piece but bottom can be anything
			vector<Direction> outerDirections = { Direction::Top, Direction::Right };

			PuzzlePiece* newPiece = SearchPieces(piecesBag, neighbourDirections, neighbourIDs, outerDirections, -1);
			jigsaw[0][col] = newPiece;
			lastPieceID = newPiece->GetID();
			RemovePiecesFromBag(piecesBag, lastPieceID); // Remove from bag of pieces
		}

		col++;
	}

	// Left edge
	col = 0;
	row = 1;
	lastPieceID = startingID;
	while (row < jigsawWidth)
	{
		if (row < jigsawWidth - 1)
		{
			// Get next edge piece, must connect on top edge to last piece and have no connections on left edge
			vector<Direction> neighbourDirections = { Direction::Top, Direction::Right, Direction::Bottom };
			vector<int> neighbourIDs = { lastPieceID, -1, -1 };
			vector<Direction> outerDirections = { Direction::Left };

			PuzzlePiece* newPiece = SearchPieces(piecesBag, neighbourDirections, neighbourIDs, outerDirections, -1);
			jigsaw[row][0] = newPiece;
			lastPieceID = newPiece->GetID();
			RemovePiecesFromBag(piecesBag, lastPieceID); // Remove from bag of pieces
		}
		else
		{
			// Bottom-left corner piece
			vector<Direction> neighbourDirections = { Direction::Top, Direction::Right };
			vector<int> neighbourIDs = { lastPieceID, -1 };
			vector<Direction> outerDirections = { Direction::Left, Direction::Bottom };

			PuzzlePiece* newPiece = SearchPieces(piecesBag, neighbourDirections, neighbourIDs, outerDirections, -1);
			jigsaw[row][0] = newPiece;
			lastPieceID = newPiece->GetID();
			RemovePiecesFromBag(piecesBag, lastPieceID); // Remove from bag of pieces
		}
		row++;
	}

	// Bottom edge
	col = 1;
	row = jigsawWidth - 1;
	while (col < jigsawWidth)
	{
		if (col < jigsawWidth - 1)
		{
			// Get next edge piece, must connect on left edge to last piece and have no connections on bottom edge
			vector<Direction> neighbourDirections = { Direction::Left, Direction::Top, Direction::Right };
			vector<int> neighbourIDs = { lastPieceID, -1, -1 };
			vector<Direction> outerDirections = { Direction::Bottom };

			PuzzlePiece* newPiece = SearchPieces(piecesBag, neighbourDirections, neighbourIDs, outerDirections, -1);
			jigsaw[row][col] = newPiece;
			lastPieceID = newPiece->GetID();
			RemovePiecesFromBag(piecesBag, lastPieceID); // Remove from bag of pieces
		}
		else
		{
			// If last in row, must be the bottom-right corner piece
			vector<Direction> neighbourDirections = { Direction::Left, Direction::Top }; // Has left and bottom neighbours
			vector<int> neighbourIDs = { lastPieceID, -1 };	// Left must be prev piece but bottom can be anything
			vector<Direction> outerDirections = { Direction::Bottom, Direction::Right };

			PuzzlePiece* newPiece = SearchPieces(piecesBag, neighbourDirections, neighbourIDs, outerDirections, -1);
			jigsaw[row][col] = newPiece;
			lastPieceID = newPiece->GetID();
			RemovePiecesFromBag(piecesBag, lastPieceID); // Remove from bag of pieces
		}

		col++;
	}

	// Right edge
	col = jigsawWidth - 1;
	row = 1;
	lastPieceID = jigsaw[0][col]->GetID();
	while (row < jigsawWidth - 1)
	{
		if (row < jigsawWidth - 2)
		{
			// Get next edge piece, must connect on top edge to last piece and have no connections on right edge
			vector<Direction> neighbourDirections = { Direction::Top, Direction::Left, Direction::Bottom };
			vector<int> neighbourIDs = { lastPieceID, -1, -1 };
			vector<Direction> outerDirections = { Direction::Right };

			PuzzlePiece* newPiece = SearchPieces(piecesBag, neighbourDirections, neighbourIDs, outerDirections, -1);
			jigsaw[row][col] = newPiece;
			lastPieceID = newPiece->GetID();
			RemovePiecesFromBag(piecesBag, lastPieceID); // Remove from bag of pieces
		}
		else
		{
			// Piece above bottom-left corner piece - must fit
			vector<Direction> neighbourDirections = { Direction::Top, Direction::Left, Direction::Bottom };
			vector<int> neighbourIDs = { lastPieceID, -1, jigsaw[jigsawWidth-1][col]->GetID() };
			vector<Direction> outerDirections = { Direction::Right };

			PuzzlePiece* newPiece = SearchPieces(piecesBag, neighbourDirections, neighbourIDs, outerDirections, -1);
			jigsaw[row][col] = newPiece;
			lastPieceID = newPiece->GetID();
			RemovePiecesFromBag(piecesBag, lastPieceID); // Remove from bag of pieces
		}
		row++;
	}

	cout << endl;
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

	// Assemble middle pieces
	row = 1;
	col = 1;
	while (row < jigsawWidth - 1)
	{
		while (col < jigsawWidth - 1)
		{
			cout << "Filling in piece " << row << ", " << col << endl;
			// Get next edge piece, must connect on top edge to last piece and have no connections on right edge
			vector<Direction> neighbourDirections = { Direction::Top, Direction::Left, Direction::Bottom, Direction::Right };
			vector<int> neighbourIDs = { jigsaw[row-1][col]->GetID(), jigsaw[row][col-1]->GetID(), -1, -1 };
			vector<Direction> outerDirections = {}; // No outer edges

			PuzzlePiece* newPiece = SearchPieces(piecesBag, neighbourDirections, neighbourIDs, outerDirections, -1);
			jigsaw[row][col] = newPiece;
			lastPieceID = newPiece->GetID();
			RemovePiecesFromBag(piecesBag, lastPieceID); // Remove from bag of pieces

			col++;
		}
		col = 1;
		row++;
	}

	cout << endl;
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

			Image* rotatedImage = new Image(*image);
			for (int i = 0; i < 3; ++i)
			{
				rotatedImage->RotateImage();
				images.push_back(rotatedImage);
				rotatedImage = new Image(*rotatedImage);
			}

			Image* flippedImage = new Image(*image);
			flippedImage->FlipImage();
			images.push_back(flippedImage);

			rotatedImage = new Image(*flippedImage);
			for (int i = 0; i < 3; ++i)
			{
				rotatedImage->RotateImage();
				images.push_back(rotatedImage);
				rotatedImage = new Image(*rotatedImage);
			}
		}

		for (Image* img : images)
		{
			vector<pair<int, int>> offsets;
			img->MarkSeaMonsters(offsets);

			cout << endl;
			img->PrintImage();
			cout << endl;

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
	m_pieceConfigMap.clear();
}
