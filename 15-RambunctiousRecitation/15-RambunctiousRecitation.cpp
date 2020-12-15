// 15-RambunctiousRecitation.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <stdint.h>

using namespace std;

const char* c_inputFile = "Input.txt";
const char* c_testFile = "Test.txt";

const int c_maxTurnsPartOne = 2020;
const int c_maxTurnsPartTwo = 30000000;

// struct to hold information for a single game
struct Game
{
	// Construct with the string of start numbers
	Game(string start)
	{
		m_start = start;
		ResetGame();
	}

	void AddSpokenNumber(int turn, int spokenNumber)
	{
		// Record a spoken number into the list
		if (spokenNumber >= m_turnNumbers.size())
			m_turnNumbers.resize(m_turnNumbers.size() * 2);

		m_turnNumbers[spokenNumber] = turn;
	}

	void ResetGame()
	{
		// Clear list
		m_turnNumbers.clear();
		m_turnNumbers.resize(100);

		// Reset turn count and last spoken
		m_turn = 1;
		m_lastSpoken = 0;

		// Parse string into list
		string start = m_start;
		int delimiter = (int)start.find(',');
		while (delimiter != -1)
		{
			m_lastSpoken = (stoi(start.substr(0, delimiter)));
			AddSpokenNumber(m_turn, m_lastSpoken);
			m_turn++;

			start = start.substr(delimiter + 1);
			delimiter = (int)start.find(',');
		}
		m_lastSpoken = (stoi(start));
		m_turn++;
	}

	int GetLastSpokenNumber() const
	{
		return m_lastSpoken;
	}

	int FindLastTurn(int spokenNumber) const
	{
		// Can't have been spoken before if list isn't big enough for it
		if (spokenNumber >= m_turnNumbers.size())
			return -1;

		// Return the turn for the number - if turn is 0, not been spoken before
		int lastTurn = m_turnNumbers[spokenNumber];
		return lastTurn != 0 ? lastTurn : -1;
	}

	void PlayGame(int endTurn)
	{
		cout << "Starting game: " << m_start << endl;
		while (m_turn <= endTurn)
		{	
			int turnLastSpoken = FindLastTurn(m_lastSpoken);
			AddSpokenNumber(m_turn - 1, m_lastSpoken); // Add last turn's num to the list

			int newNumber = 0;

			// If last turn's number has been spoken before
			// This turn's number = how long since last spoken
			if (turnLastSpoken != -1)
			{
				newNumber = (m_turn - 1) - turnLastSpoken;
			}
			// Else, the new number is 0
			m_lastSpoken = newNumber;

			m_turn++;
		}
	}

	string m_start;
	int m_lastSpoken = 0;
	int m_turn = 1;
	vector<int> m_turnNumbers;
};

int main()
{
	vector<Game*> games;
	ifstream inputStream(c_inputFile);
	if (inputStream.is_open())
	{
		string line;
		while (getline(inputStream, line))
		{
			games.push_back(new Game(line));
		}
	}

	{
		// Part One
		cout << "=== Part 1 ===" << endl;
		for (Game* game : games)
		{
			game->PlayGame(c_maxTurnsPartOne);
			cout << "Number at turn " << c_maxTurnsPartOne << ": " << game->GetLastSpokenNumber() << endl;
		}
		cout << endl;
	}

	// Reset all the games
	for (Game* game : games)
	{
		game->ResetGame();
	}

	{
		// Part Two
		cout << "=== Part 2 ===" << endl;
		for (Game* game : games)
		{
			game->PlayGame(c_maxTurnsPartTwo);
			cout << "Number at turn " << c_maxTurnsPartTwo << ": " << game->GetLastSpokenNumber() << endl << endl;
		}
	}

	for (int i = 0; i < games.size(); ++i)
	{
		delete games[i];
		games[i] = nullptr;
	}
	games.clear();
}