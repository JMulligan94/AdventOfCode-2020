// 22-CrabCombat.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <string>
#include <sstream>
#include <stdint.h>

//#define DEBUG_PRINT 1

using namespace std;

class Player
{
public:
	Player() = default;

	Player(const deque<int>& cardData)
	{
		m_deck = cardData;
	}

	int DrawCard()
	{
		int value = m_deck.front();
		m_deck.pop_front();
		return value;
	}

	void PlaceCard(int card)
	{
		m_deck.push_back(card);
	}

	int GetCardCount() const
	{
		return (int)m_deck.size();
	}

	bool IsDeckEmpty() const
	{
		return m_deck.empty();
	}

	void GetDeckCopy(deque<int>& deckCopy, int amount)
	{
		deckCopy.clear();
		for (auto iter = m_deck.begin(); iter != m_deck.end(); ++iter)
		{
			if (amount == 0)
				return;

			deckCopy.push_back(*iter);
			amount--;
		}
	}

	uint64_t GetScore() const
	{
		uint64_t score = 0;
		int scoreMultiplier = m_deck.size();
		for (auto iter = m_deck.begin(); iter != m_deck.end(); ++iter)
		{
			score += (*iter) * scoreMultiplier;
			scoreMultiplier--;
		}
		return score;
	}

	string GetDeckString() const
	{
		string deckStr;
		for (auto iter = m_deck.begin(); iter != m_deck.end(); ++iter)
		{
			deckStr.append(to_string(*iter));
			deckStr.append(", ");
		}
		deckStr = deckStr.substr(0, deckStr.length() - 2);
		return deckStr;
	}

private:
	deque<int> m_deck;
};

int s_gameNumber = 0;

class CrabCombatGame
{
public:
	CrabCombatGame() = default;

	CrabCombatGame(bool recursive, const deque<int>& playerOneCards, const deque<int>& playerTwoCards)
	{
		m_recursive = recursive;
		m_playerOne = Player(playerOneCards);
		m_playerTwo = Player(playerTwoCards);
		m_gameNumber = ++s_gameNumber;
	}

	bool HaveDecksBeenSeenBefore() const
	{
		// Check if P1 deck has been seen before
		bool foundP1Deck = false;
		string currentP1Deck = m_playerOne.GetDeckString();
		for (string hand : m_prevPlayerOneHands)
		{
			if (currentP1Deck == hand)
			{
				foundP1Deck = true;
				break;
			}
		}

		if (!foundP1Deck)
			return false;

		// Check if P2 deck has been seen too
		string currentP2Deck = m_playerTwo.GetDeckString();
		for (string hand : m_prevPlayerTwoHands)
		{
			if (currentP2Deck == hand)
				return true;
		}
		
		return false;
	}

	// Play rounds until winner is found
	// Returns winner
	int PlayUntilWinner()
	{
		int roundNumber = 1;
		while (!m_playerOne.IsDeckEmpty() && !m_playerTwo.IsDeckEmpty())
		{
			// Do Round
#ifdef DEBUG_PRINT
			cout << endl << "-- Round " << roundNumber << " (Game " << m_gameNumber << ") --" << endl;

			cout << "Player 1's deck: " << m_playerOne.GetDeckString() << endl;
			cout << "Player 2's deck: " << m_playerTwo.GetDeckString() << endl;
#endif

			// Before either player deals a card, if there was a previous round in this game that had exactly 
			// the same cards in the same order in the same players' decks, the game instantly ends in a win for player 1.
			if (HaveDecksBeenSeenBefore())
			{
#ifdef DEBUG_PRINT
				cout << "This round has occurred before!" << endl;
#endif
				return 1;
			}
			else
			{
				// Store info for this round, to avoid infinite recursion
				m_prevPlayerOneHands.push_back(m_playerOne.GetDeckString());
				m_prevPlayerTwoHands.push_back(m_playerTwo.GetDeckString());
			}

			// Draw cards
			int playerOneCard = m_playerOne.DrawCard();
			int playerTwoCard = m_playerTwo.DrawCard();

#ifdef DEBUG_PRINT
				cout << "Player 1 plays: " << playerOneCard << endl;
				cout << "Player 2 plays: " << playerTwoCard << endl;
#endif

			// If recursion is allowed, check if the criteria is met for a sub-game to be played
			if (m_recursive)
			{
				// RECURSIVE ONLY
				// If both players have at least as many cards remaining in their deck as the value of the card they just drew, 
				// the winner of the round is determined by playing a new game of Recursive Combat
				if (playerOneCard <= m_playerOne.GetCardCount()
					&& playerTwoCard <= m_playerTwo.GetCardCount())
				{
					deque<int> playerOneCopy;
					m_playerOne.GetDeckCopy(playerOneCopy, playerOneCard);
					deque<int> playerTwoCopy;
					m_playerTwo.GetDeckCopy(playerTwoCopy, playerTwoCard);

#ifdef DEBUG_PRINT
					cout << "Playing a sub-game to determine the winner..." << endl;
#endif
					CrabCombatGame subGame = CrabCombatGame(true, playerOneCopy, playerTwoCopy);

					// Winner of the round is the winner of the subgame!
					int roundWinner = subGame.Play();

					if (roundWinner == 1)
					{
						// Player 1 wins the round!
						m_playerOne.PlaceCard(playerOneCard);
						m_playerOne.PlaceCard(playerTwoCard);
					}
					else if (roundWinner == 2)
					{
						// Player 2 wins the round!
						m_playerTwo.PlaceCard(playerTwoCard);
						m_playerTwo.PlaceCard(playerOneCard);
					}
#ifdef DEBUG_PRINT
					cout << "Player " << roundWinner << " wins subgame and therefore round " << roundNumber << " of game " << m_gameNumber << endl;
#endif
					// Next round!
					continue;
				}
			}

			// By default, winner is player with higher-value card
			if (playerOneCard > playerTwoCard)
			{
				// Player 1 wins the round!
				m_playerOne.PlaceCard(playerOneCard);
				m_playerOne.PlaceCard(playerTwoCard);

#ifdef DEBUG_PRINT
				cout << "Player 1 wins round " << roundNumber << " of game " << m_gameNumber << endl;
#endif
			}
			else
			{
				// Player 2 wins the round!
				m_playerTwo.PlaceCard(playerTwoCard);
				m_playerTwo.PlaceCard(playerOneCard);

#ifdef DEBUG_PRINT
				cout << "Player 2 wins round " << roundNumber << " of game " << m_gameNumber << endl;
#endif
			}

			roundNumber++;
		}

		return (m_playerOne.GetCardCount() > 0) ? 1 : 2;
	}

	int Play()
	{
		int gameWinner = PlayUntilWinner();

#ifdef DEBUG_PRINT
		cout << endl << "== Post-game results ==" << endl;
		if (gameWinner == 1)
		{
			cout << "The winner of game " << m_gameNumber << " is player 1!" << endl;
		}
		else if (gameWinner == 2)
		{
			cout << "The winner of game " << m_gameNumber << " is player 2!" << endl;
		}
		cout << "Player 1's deck: " << m_playerOne.GetDeckString() << endl;
		cout << "Player 2's deck: " << m_playerTwo.GetDeckString() << endl;
#endif

		return gameWinner;
	}

	uint64_t GetScore(int playerIdx) const
	{
		if (playerIdx == 1)
			return m_playerOne.GetScore();
		else if (playerIdx == 2)
			return m_playerTwo.GetScore();
		
		return 0;
	}

private:
	Player m_playerOne;
	Player m_playerTwo;

	vector<string> m_prevPlayerOneHands;
	vector<string> m_prevPlayerTwoHands;

	bool m_recursive;
	int m_gameNumber;
};

const char* c_inputFile = "Input.txt";
const char* c_testFile = "Test.txt";

int main()
{
	deque<int> playerOneData;
	deque<int> playerTwoData;

	ifstream inputStream(c_inputFile);
	if (inputStream.is_open())
	{
		string line;
		deque<int> playerData;
		// Get fields
		while (getline(inputStream, line))
		{
			if (line.empty())
			{
				playerOneData = playerData;
				playerData.clear();
			}
			else if (line.find("Player") == -1)
			{
				playerData.push_back(stoi(line));
			}
		}
		playerTwoData = playerData;
	}

	{
		// Part One
		cout << endl << "=== Part 1 ===" << endl;

		CrabCombatGame game = CrabCombatGame(false, playerOneData, playerTwoData);
		int winner = game.Play();

		cout << "The winner's score is: " << game.GetScore(winner) << endl << endl;
	}

	{
		// Part Two
		cout << endl << "=== Part 2 ===" << endl;

		s_gameNumber = 0;
		CrabCombatGame game = CrabCombatGame(true, playerOneData, playerTwoData);
		int winner = game.Play();

		cout << "The winner's score is: " << game.GetScore(winner) << endl << endl;
	}
}
