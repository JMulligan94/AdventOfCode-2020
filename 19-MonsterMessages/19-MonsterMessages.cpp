// 19-MonsterMessages.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stdint.h>
#include <sstream>

using namespace std;

const char* c_inputFile = "Input.txt";
const char* c_testFile = "Test.txt";

class Rule;
vector<Rule*> m_rules;
vector<string> m_messages;

class Rule
{
public:
	Rule(string line)
	{
		int colonIndex = (int)line.find(':');
		m_id = stoi(line.substr(0, colonIndex));

		string token;
		istringstream stream(line.substr(colonIndex + 2));

		pair<int, int> idPair;
		idPair.first = -1;
		idPair.second = -1;
		m_IdA = idPair;
		m_IdB = idPair;
		m_messageChar = '#';

		bool fillCritA = true;

		while (getline(stream, token, ' '))
		{
			if (token[0] == '\"')
			{
				m_messageChar = token[1];
				continue;
			}

			if (token == "|")
			{
				m_IdA = idPair;
				idPair.first = -1;
				idPair.second = -1;
				fillCritA = false;
				continue;
			}

			if (idPair.first == -1)
			{
				idPair.first = stoi(token);
			}
			else if (idPair.second == -1)
			{
				idPair.second = stoi(token);
			}
		}

		if (fillCritA)
			m_IdA = idPair;
		else
			m_IdB = idPair;
	}

	void Fixup()
	{
		// Rule is just a char - no need to fixup
		if (m_messageChar != '#')
		{
			return;
		}

		if (m_IdA.first != -1)
			m_criteriaA.first = m_rules[m_IdA.first];
			
		if (m_IdA.second != -1)
			m_criteriaA.second = m_rules[m_IdA.second];


		if (m_IdB.first != -1)
			m_criteriaB.first = m_rules[m_IdB.first];

		if (m_IdB.second != -1)
			m_criteriaB.second = m_rules[m_IdB.second];
	}

	void BuildStrings()
	{
		if (!m_cachedValidMsgs.empty())
			return;

		if (m_messageChar != '#')
		{
			m_cachedValidMsgs.push_back(string(1, m_messageChar));
			return;
		}

		{
			vector<string> firstMsgs;
			if (m_criteriaA.first != nullptr)
			{
				m_criteriaA.first->BuildStrings();
				firstMsgs = m_criteriaA.first->m_cachedValidMsgs;
			}

			vector<string> secondMsgs;
			if (m_criteriaA.second != nullptr)
			{
				m_criteriaA.second->BuildStrings();
				secondMsgs = m_criteriaA.second->m_cachedValidMsgs;
			}

			for (string first : firstMsgs)
			{
				string msg = first;
				if (secondMsgs.empty())
					m_cachedValidMsgs.push_back(msg);

				for (string second : secondMsgs)
				{
					msg.append(second);
					m_cachedValidMsgs.push_back(msg);
				}
			}
		}

		{
			vector<string> firstMsgs;
			if (m_criteriaB.first != nullptr)
			{
				m_criteriaB.first->BuildStrings();
				firstMsgs = m_criteriaB.first->m_cachedValidMsgs;
			}

			vector<string> secondMsgs;
			if (m_criteriaB.second != nullptr)
			{
				m_criteriaB.second->BuildStrings();
				secondMsgs = m_criteriaB.second->m_cachedValidMsgs;
			}

			for (string first : firstMsgs)
			{
				string msg = first;
				if (secondMsgs.empty())
					m_cachedValidMsgs.push_back(msg);

				for (string second : secondMsgs)
				{
					msg.append(second);
					m_cachedValidMsgs.push_back(msg);
				}
			}
		}
	}

	bool IsMessageValid(string message)
	{
		BuildStrings();

		for (string validMsg : m_cachedValidMsgs)
		{
			if (validMsg == message)
				return true;
		}
		
		return false;
	}

	int GetID() const { return m_id; }

private:
	int m_id;

	char m_messageChar;

	pair<int, int> m_IdA;
	pair<Rule*, Rule*> m_criteriaA;

	pair<int, int> m_IdB;
	pair<Rule*, Rule*> m_criteriaB;

	vector<string> m_cachedValidMsgs;

};

int main()
{
	ifstream inputStream(c_inputFile);
	if (inputStream.is_open())
	{
		string line;

		// Get rules
		while (getline(inputStream, line))
		{
			if (line.empty())
				break;

			Rule* newRule = new Rule(line);
			if (newRule->GetID() >= m_rules.size())
			{
				m_rules.resize(newRule->GetID() + 1);
			}
			m_rules[newRule->GetID()] = newRule;
		}

		// Get messages
		while (getline(inputStream, line))
		{
			m_messages.push_back(line);
		}

		// Fixup all rules
		for (Rule* rule : m_rules)
		{
			rule->Fixup();
		}

		for (string message : m_messages)
		{
			bool messageValid = m_rules[0]->IsMessageValid(message);
			if (messageValid)
				cout << "MATCH: ";
			cout << "Checking: " << message << endl;
		}
	}
}
