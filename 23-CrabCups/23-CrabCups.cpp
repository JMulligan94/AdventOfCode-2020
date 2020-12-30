// 23-CrabCups.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <stdint.h>
#include <cassert>

//#define DEBUG_PRINT 1

using namespace std;

class Cup
{
public:
	int m_label;

	Cup* m_prev;
	Cup* m_next;
};

class CupGame
{
public:
	CupGame(string input)
	{
		const int cupCount = (int)input.size();
		m_lowestLabel = INT_MAX;
		m_highestLabel = INT_MIN;

		for (int i = 0; i < cupCount; ++i)
		{
			int label = input[i] - '0';
			Cup* cup = new Cup();
			cup->m_label = label;
			m_allCups.push_back(cup);

			m_lowestLabel = min(m_lowestLabel, label);
			m_highestLabel = max(m_highestLabel, label);
		}

		for (int i = 0; i < cupCount - 1; ++i)
		{
			ConnectCup(m_allCups[i], m_allCups[i + 1]);
		}
		ConnectCup(m_allCups.back(), m_allCups.front());
	}

	~CupGame()
	{
		for (int i = 0; i < (int)m_allCups.size(); ++i)
		{
			delete m_allCups[i];
			m_allCups[i] = nullptr;
		}
		m_allCups.clear();
	}

	void PrintCups(Cup* startCup)
	{
#ifdef DEBUG_PRINT
		cout << "cups:  (" << startCup->m_label << ")  ";
		Cup* cup = startCup->m_next;
		while (cup != startCup)
		{
			cout << cup->m_label << "  ";
			cup = cup->m_next;
		}
#endif
	}

	void ConnectCup(Cup* cupA, Cup* cupB)
	{
		cupA->m_next = cupB;
		cupB->m_prev = cupA;
	}

	void InsertCup(Cup* insertionCup, Cup* prev)
	{
		assert(insertionCup->m_next == nullptr 
			&& insertionCup->m_prev == nullptr);

		Cup* nextCup = prev->m_next;
		
		prev->m_next = insertionCup;
		insertionCup->m_prev = prev;

		insertionCup->m_next = nextCup;
		nextCup->m_prev = insertionCup;
	}

	void RemoveCup(Cup* cupToRemove)
	{
		assert(cupToRemove->m_next != nullptr
			&& cupToRemove->m_prev != nullptr);

		Cup* nextCup = cupToRemove->m_next;
		Cup* prevCup = cupToRemove->m_prev;

		ConnectCup(prevCup, nextCup);

		cupToRemove->m_next = nullptr;
		cupToRemove->m_prev = nullptr;
	}

	void DoMove(Cup* currentCup)
	{
		PrintCups(currentCup);

		// The crab picks up the three cups that are immediately clockwise of the current cup.
		// They are removed from the circle; cup spacing is adjusted as necessary to maintain the circle.
		vector<Cup*> pickUps;
		const int c_numPickupCups = 3;
		for (int i = 0; i < c_numPickupCups; ++i)
		{
			Cup* nextCup = currentCup->m_next;
			pickUps.push_back(nextCup);
			RemoveCup(nextCup);
		}

#ifdef DEBUG_PRINT
		cout << endl << "pick up:  ";
		for (int i = 0; i < 3; ++i)
		{
			cout << pickUps[i]->m_label << "  ";
		}
#endif

		// The crab selects a destination cup: the cup with a label equal to the current cup's label minus one.
		// If this would select one of the cups that was just picked up, the crab will keep subtracting one until 
		// it finds a cup that wasn't just picked up.
		int destValue = currentCup->m_label - 1;
		bool foundDestValue = false;
		while (!foundDestValue)
		{
			for (int i = 0; i < 3; ++i)
			{
				// If at any point in this process the value goes below the lowest value on any cup's label, 
				// it wraps around to the highest value on any cup's label instead.
				if (destValue < m_lowestLabel)
					destValue = m_highestLabel;

				if (destValue == pickUps[i]->m_label)
				{
					destValue--;
					break;
				}

				if (i == 2)
					foundDestValue = true;
			}
		}

		Cup* destinationCup = nullptr;
		Cup* checkedCup = currentCup->m_next;
		while (destinationCup == nullptr)
		{
			if (checkedCup->m_label == destValue)
			{
				destinationCup = checkedCup;
			}
			checkedCup = checkedCup->m_next;
		}

#ifdef DEBUG_PRINT
		cout << endl << "destination:\t" << destinationCup->m_label << endl << endl;
#endif

		// The crab places the cups it just picked up so that they are immediately clockwise of the destination cup. 
		// They keep the same order as when they were picked up.
		InsertCup(pickUps[0], destinationCup);
		InsertCup(pickUps[1], pickUps[0]);
		InsertCup(pickUps[2], pickUps[1]);
	}

	void PlayGame(int maxMoves)
	{
		Cup* currentCup = m_allCups.front();
		int moveNumber = 1;
		while (moveNumber <= maxMoves)
		{
#ifdef DEBUG_PRINT
			cout << "-- move " << moveNumber << " --" << endl;
#endif
			DoMove(currentCup);

			// The crab selects a new current cup: the cup which is immediately clockwise of the current cup.
			currentCup = currentCup->m_next;

			moveNumber++;
		}
		cout << "-- final --" << endl;
		PrintCups(currentCup);
		cout << endl;
	}

	Cup* FindCupByLabel(int label)
	{
		for (Cup* cup : m_allCups)
		{
			if (cup->m_label == label)
				return cup;
		}
		return nullptr;
	}

	void ExpandToCupCount(int cupCount) 
	{
		cupCount -= m_allCups.size();
		while (cupCount > 0)
		{
			Cup* newCup = new Cup();
			m_highestLabel++;
			newCup->m_label = m_highestLabel;

			InsertCup(newCup, m_allCups.back());
			m_allCups.push_back(newCup);

			cupCount--;
		}
	}

private:
	vector<Cup*> m_allCups;

	int m_highestLabel;
	int m_lowestLabel;
};


const char* c_testInput = "389125467";
const char* c_input = "685974213";

string GetAnswerString(Cup* fromCup)
{
	string answer;
	Cup* currentCup = fromCup->m_next;
	while (currentCup != fromCup)
	{
		answer.append(to_string(currentCup->m_label));
		currentCup = currentCup->m_next;
	}
	return answer;
}

int main()
{
	string cupInfo = c_testInput;
	//string cupInfo = c_input;

	{
		// Part One
		cout << endl << "=== Part 1 ===" << endl;

		CupGame game(cupInfo);
		const int maxMoves = 100;
		game.PlayGame(maxMoves);

		cout << "The labels on the cups after cup 1 is: " << GetAnswerString(game.FindCupByLabel(1)) << endl;
	}

	{
		// Part Two
		cout << endl << "=== Part 2 ===" << endl;

		CupGame game(cupInfo);
		const int maxMoves = 10000000; // Ten million moves!
		const int cupCount = 1000000; // One million cups!
		game.ExpandToCupCount(cupCount);
		game.PlayGame(maxMoves);

		int i = 0;
	}

}