// 23-CrabCups.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <stdint.h>
#include <cassert>

//#define DEBUG_PRINT
//#define DEBUG_PRINT_CUPS

using namespace std;

class Cup
{
public:
	int m_label;
	Cup* m_next;
};

class CupGame
{
public:
	CupGame(string input)
	{
		m_numCups = (int)input.size();
		m_lowestLabel = INT_MAX;
		m_highestLabel = INT_MIN;
		vector<Cup*> cupsAdded;

		for (int i = 0; i < m_numCups; ++i)
		{
			int label = input[i] - '0';
			Cup* cup = new Cup();
			cup->m_label = label;

			if (label >= (int)m_allCups.size())
				m_allCups.resize(label + 1);

			m_allCups[label] = cup;
			cupsAdded.push_back(cup);

			m_lowestLabel = min(m_lowestLabel, label);
			m_highestLabel = max(m_highestLabel, label);
		}

		for (int i = 0; i < (int)cupsAdded.size() - 1; ++i)
		{
			cupsAdded[i]->m_next = cupsAdded[i+1];
		}

		m_firstCup = cupsAdded.front();
		m_lastCup = cupsAdded.back();
		m_lastCup->m_next = m_firstCup;
	}

	~CupGame()
	{
		for (int i = 0; i < (int)m_allCups.size(); ++i)
		{
			if (m_allCups[i] == nullptr)
				continue;
			delete m_allCups[i];
			m_allCups[i] = nullptr;
		}
		m_allCups.clear();
	}

	void PrintCups(Cup* startCup)
	{
#ifdef DEBUG_PRINT_CUPS
		cout << "cups:  (" << startCup->m_label << ")  ";
		Cup* cup = startCup->m_next;
		while (cup != startCup)
		{
			cout << cup->m_label << "  ";
			cup = cup->m_next;
		}
#endif
	}

	void DoMove(Cup* currentCup)
	{
		PrintCups(currentCup);

		// The crab picks up the three cups that are immediately clockwise of the current cup.
		// They are removed from the circle; cup spacing is adjusted as necessary to maintain the circle.
		Cup* cup1 = currentCup->m_next;
		Cup* cup2 = cup1->m_next;
		Cup* cup3 = cup2->m_next;

		vector<Cup*> pickUps;
		pickUps.push_back(cup1);
		pickUps.push_back(cup2);
		pickUps.push_back(cup3);

		// Fill in gap where chain of cups has been removed
		currentCup->m_next = cup3->m_next;

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
		
		// If at any point in this process the value goes below the lowest value on any cup's label, 
		// it wraps around to the highest value on any cup's label instead.
		if (destValue < m_lowestLabel)
			destValue = m_highestLabel;

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

		Cup* destinationCup = m_allCups[destValue];
#ifdef DEBUG_PRINT
		cout << endl << "destination:\t" << destinationCup->m_label << endl << endl;
#endif

		// The crab places the cups it just picked up so that they are immediately clockwise of the destination cup. 
		// They keep the same order as when they were picked up.
		pickUps[2]->m_next = destinationCup->m_next;
		destinationCup->m_next = pickUps[0];
	}

	void PlayGame(int maxMoves)
	{
		Cup* currentCup = m_firstCup;
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
		return m_allCups[label];
	}

	void ExpandToCupCount(int cupCount) 
	{
		while (m_numCups < cupCount)
		{
			Cup* newCup = new Cup();
			m_highestLabel++;
			newCup->m_label = m_highestLabel;

			m_lastCup->m_next = newCup;
			m_allCups.push_back(newCup);
			m_lastCup = newCup;
			m_numCups++;
		}

		m_lastCup->m_next = m_firstCup;
	}

	bool IsValidLoop(int& finalLabel) const 
	{
		int cupCount = 0;
		Cup* cup = m_firstCup;
		while (cupCount <= m_numCups)
		{
			if (cup == m_firstCup && cupCount == m_numCups)
			{
				finalLabel = cup->m_label;
				return true;
			}
			cup = cup->m_next;
			cupCount++;
		}
		finalLabel = cup->m_label;
		return false;
	}

private:
	vector<Cup*> m_allCups; // Use label as index, to find cup in O(1) time
	Cup* m_firstCup;
	Cup* m_lastCup;

	int m_numCups;
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
	//string cupInfo = c_testInput;
	string cupInfo = c_input;

	{
		// Part One
		cout << endl << "=== Part 1 ===" << endl;

		CupGame game(cupInfo);
		const int maxMoves = 100;
		game.PlayGame(maxMoves);

		cout << "The labels on the cups after cup 1 is: " << GetAnswerString(game.FindCupByLabel(1)) << endl;

		int finalLabel;
		bool isValid = game.IsValidLoop(finalLabel);
		cout << "Valid loop: " << isValid << " - " << finalLabel << endl;
	}

	{
		// Part Two
		cout << endl << "=== Part 2 ===" << endl;

		CupGame game(cupInfo);
		const int cupCount = 1000000; // One million cups!
		game.ExpandToCupCount(cupCount);
		const int maxMoves = 10000000; // Ten million moves!
		game.PlayGame(maxMoves);

		Cup* cup1 = game.FindCupByLabel(1);
		uint64_t neighbourOneLabel = cup1->m_next->m_label;
		uint64_t neighbourTwoLabel = cup1->m_next->m_next->m_label;
		cout << "The immediate cup labels are: " << neighbourOneLabel << ", " << neighbourTwoLabel << endl;
		uint64_t neighbourProduct = (neighbourOneLabel * neighbourTwoLabel);
		cout << "The product is: " << neighbourProduct << endl;

		int finalLabel;
		bool isValid = game.IsValidLoop(finalLabel);
		cout << "Valid loop: " << isValid << " - " << finalLabel << endl;
	}

}