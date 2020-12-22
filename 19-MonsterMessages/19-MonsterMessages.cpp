// 19-MonsterMessages.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stdint.h>
#include <sstream>
#include <regex>

using namespace std;

const char* c_inputFile = "Input.txt";
const char* c_input2File = "Input2.txt";
const char* c_testFile = "TestB.txt";

const char* c_test2Before = "Test2Before.txt";
const char* c_test2After = "Test2After.txt";

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

	void BuildRegexString()
	{
		if (!m_cachedRegex.empty())
			return;

		if (m_messageChar != '#')
		{
			m_cachedRegex = m_messageChar;
			return;
		}

		string firstRegex;
		{
			if (m_criteriaA.first != nullptr)
			{
				m_criteriaA.first->BuildRegexString();
				firstRegex.append(m_criteriaA.first->GetRegex());
			}

			if (m_criteriaA.second != nullptr)
			{
				m_criteriaA.second->BuildRegexString();
				firstRegex.append(m_criteriaA.second->GetRegex());
			}
		}

		string secondRegex;
		{
			if (m_criteriaB.first != nullptr)
			{
				m_criteriaB.first->BuildRegexString();
				secondRegex.append(m_criteriaB.first->GetRegex());
			}

			if (m_criteriaB.second != nullptr)
			{
				m_criteriaB.second->BuildRegexString();
				secondRegex.append(m_criteriaB.second->GetRegex());
			}
		}

		string fullRegex;
		fullRegex.append("(");

		if (!firstRegex.empty())
			fullRegex.append(firstRegex);

		if (!firstRegex.empty() && !secondRegex.empty())
			fullRegex.append("|");

		if (!secondRegex.empty())
			fullRegex.append(secondRegex);

		fullRegex.append(")");

		m_cachedRegex = fullRegex;
	}

	bool IsMessageValid(string message)
	{
		BuildRegexString();

		const regex expression(m_cachedRegex);
		return regex_match(message, expression);
	}

	int GetID() const { return m_id; }

	string GetRegex() const { return m_cachedRegex; }

private:
	int m_id;

	char m_messageChar;

	pair<int, int> m_IdA;
	pair<Rule*, Rule*> m_criteriaA;

	pair<int, int> m_IdB;
	pair<Rule*, Rule*> m_criteriaB;

	string m_cachedRegex;

};

int main()
{
	ifstream inputStream(c_testFile);
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
			if (rule != nullptr)
				rule->Fixup();
		}

		int numMatch = 0;
		for (string message : m_messages)
		{
			bool messageValid = m_rules[0]->IsMessageValid(message);
			if (messageValid)
			{
				cout << "MATCH: ";
				numMatch++;
			}
			cout << "Checking: " << message << endl;
		}
		cout << endl << "Rule[0] regex = " << m_rules[0]->GetRegex() << endl;
		cout << "Found " << numMatch << " matches" << endl;
	}
}
